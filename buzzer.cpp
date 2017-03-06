#include <Arduino.h>
#include "timer_event.h"
#include "buzzer.h"

static void buzzerOn(int event, int param) {
  Serial.println("TODO: buzzer on");
}

static void buzzerOff(int event, int param) {
  Serial.println("TODO: buzzer off");
}

void setupBuzzer() {
  TimerEvent::getInstance()->addListener(TimerEvent::hardwareBuzzerOn, buzzerOn);
  TimerEvent::getInstance()->addListener(TimerEvent::hardwareBuzzerOff, buzzerOff);
}

void handleBuzzer() {
  // Nothing to do here as an event sink
}