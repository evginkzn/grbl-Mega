#include "buttons_wrap.h"

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <Button2.h>

#include "grbl.h"

static Button2 start_btn;
static Button2 stop_btn;
static Button2 pause_btn;

void start_button_handler(Button2& btn);
void stop_button_handler(Button2& btn);
void pause_button_handler(Button2& btn);

void buttons_init(void)
{
    start_btn.begin();
    stop_btn.begin();
    pause_btn.begin();

    start_btn->setClickHandler(start_button_handler);
    start_btn->setLongClickHandler(start_button_handler);
    
    stop_btn->setClickHandler(stop_button_handler);
    stop_btn->setLongClickHandler(stop_button_handler);

    pause_btn->setClickHandler(pause_button_handler);
    pause_btn->setLongClickHandler(pause_button_handler);
}

void buttons_run(void)
{
    start_btn.loop();
    stop_btn.loop();
    pause_btn.loop();
}

void start_button_handler(Button2& btn)
{
    printString("Start button pressed!");
}

void stop_button_handler(Button2& btn)
{
    printString("Stop button pressed!");
}

void pause_button_handler(Button2& btn)
{
    printString("Pause button pressed!");
}