#include "servo_wrap.h"

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <Servo.h>

static Servo servo;

void servo_init(void)
{
  servo.attach(5);
}

void servo_write(int val)
{
  servo.write(val);
}