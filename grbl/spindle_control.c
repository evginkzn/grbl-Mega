/*
  spindle_control.c - spindle control methods
  Part of Grbl
  Copyright (c) 2012-2017 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "grbl.h"
#include "servo_wrap.h"

static float pwm_gradient; // Precalulated value to speed up rpm to PWM conversions.


void spindle_init()
{    
  servo_init();

  spindle_stop();
}


uint8_t spindle_get_state()
{
	return(SPINDLE_STATE_DISABLE);
}


// Disables the spindle and sets PWM output to zero when PWM variable spindle speed is enabled.
// Called by various main program and ISR routines. Keep routine small, fast, and efficient.
// Called by spindle_init(), spindle_set_speed(), spindle_set_state(), and mc_reset().
void spindle_stop()
{
  servo_write(0);
}


// Sets spindle speed PWM output and enable pin, if configured. Called by spindle_set_state()
// and stepper ISR. Keep routine small and efficient.
void spindle_set_speed(uint16_t pwm_value)
{
  servo_write(pwm_value);
}


// Called by spindle_set_state() and step segment generator. Keep routine small and efficient.
uint16_t spindle_compute_pwm_value(float rpm) // Mega2560 PWM register is 16-bit.
{
  uint16_t pwm_value;
  rpm *= (0.010*sys.spindle_speed_ovr); // Scale by spindle speed override value.
  // Calculate PWM register value based on rpm max/min settings and programmed rpm.
  if ((settings.rpm_min >= settings.rpm_max) || (rpm >= settings.rpm_max)) {
    // No PWM range possible. Set simple on/off spindle control pin state.
    sys.spindle_speed = settings.rpm_max;
    pwm_value = SPINDLE_PWM_MAX_VALUE;
  } else if (rpm <= settings.rpm_min) {
    if (rpm == 0.0) { // S0 disables spindle
    sys.spindle_speed = 0.0;
    pwm_value = SPINDLE_PWM_OFF_VALUE;
    } else { // Set minimum PWM output
    sys.spindle_speed = settings.rpm_min;
    pwm_value = SPINDLE_PWM_MIN_VALUE;
    }
  } else { 
    // Compute intermediate PWM value with linear spindle speed model.
    // NOTE: A nonlinear model could be installed here, if required, but keep it VERY light-weight.
    sys.spindle_speed = rpm;
    pwm_value = floor((rpm-settings.rpm_min)*pwm_gradient) + SPINDLE_PWM_MIN_VALUE;
  }
  return(pwm_value);
}

// Immediately sets spindle running state with direction and spindle rpm via PWM, if enabled.
// Called by g-code parser spindle_sync(), parking retract and restore, g-code program end,
// sleep, and spindle stop override.
void spindle_set_state(uint8_t state, float rpm)
{
  if (sys.abort) { return; } // Block during abort.
  if (state == SPINDLE_DISABLE)  // Halt or set spindle direction and rpm.
  {
  
    sys.spindle_speed = 0.0;
    spindle_stop();
  
  }
  else
  {
    if (rpm > 180)
    {
      servo_write(180);
    }
    else
    {
      servo_write(rpm);
    }
    
  }
  
  sys.report_ovr_counter = 0; // Set to report change immediately
}


// G-code parser entry-point for setting spindle state. Forces a planner buffer sync and bails 
// if an abort or check-mode is active.
void spindle_sync(uint8_t state, float rpm)
{
  if (sys.state == STATE_CHECK_MODE) { return; }
  protocol_buffer_synchronize(); // Empty planner buffer to ensure spindle is set when programmed.
  spindle_set_state(state,rpm);
}