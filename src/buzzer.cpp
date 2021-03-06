#include <Arduino.h>
#include <RBD_Timer.h>
#include <ArduinoLog.h>
#include "board.h"
#include "timer_event.h"
#include "buzzer.h"

//Driving a passive buzzer module that sound on low signal, like those Arduino uses

static RBD::Timer buzzerTimer;

static void startingSound(int event, int param) {
  digitalWrite(BUZZER_PIN, LOW);
  Log.notice("buzzer sound" CR);
  buzzerTimer.setTimeout(1000);
  buzzerTimer.restart();
#if 0 //uncomment for round trip latency test
  TimerEvent::getInstance()->queueHardwareEvent(TimerEvent::hardwareStopPlateHit, 0); // route hardwire buzzer to stop plate event for round trip latency test
#endif
}

void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  buzzerTimer.stop();
  TimerEvent::getInstance()->addListener(TimerEvent::eventStartingSound, startingSound);
}

void loopBuzzer() {
  if (buzzerTimer.isExpired()) {
    buzzerTimer.stop();
    digitalWrite(BUZZER_PIN, HIGH);
    Log.notice("buzzer stop sound" CR);
  }
}
