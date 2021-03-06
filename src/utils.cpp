#include <Arduino.h>
#include <ArduinoLog.h>
#include "timer_event.h"

static boolean runLoop = true;

static String digitToString (unsigned long digit) {
  if (digit == 0) {
    return "00";
  }
  if (digit < 10) {
    return "0" + String(digit);
  }
  return String(digit);
}

String millisToTimeString(unsigned long elapse) {
  unsigned long minute = elapse / 1000 / 60;
  unsigned long second = (elapse % (1000 * 60)) / 1000;
  unsigned long deciSecond = (elapse % 1000) / 10;
  if ((elapse % 10) > 4) { // round to the nearest deciSecond
    deciSecond++;
    if (deciSecond == 100) {
      deciSecond = 0;
      second++;
      if (second == 60) {
        second = 0;
        minute++;
      }
    }
  }
  return digitToString(minute) + ":" + digitToString(second) + ":" + digitToString(deciSecond);
}

void stopSketchLoop() {
  Log.fatal ("stopSketchLoop() and next loop() iteration will bail out" CR);
  runLoop = false;
}

boolean isSketchLoopRun() {
  return runLoop;
}

////////// display support /////////
// Display is implemented as a sequence of events, setting up display contents,
// concluded by eventDisplayRefreshParamMode as a way to abstract different types
// of hardware implementations. Think display commands.

void displayHit(int shots, unsigned long timeMillis) {
  int shotcountParam = shots << 8 | shots;
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayClear, 0);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayShotcountParam, shotcountParam);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayStopwatchParamMillis, (int) timeMillis);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayRefreshParamMode, TimerEvent::displayModeHit);
}

void displayReview(int review, int total, unsigned long timeMillis) {
  int shotcountParam = total << 8 | review;
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayClear, 0);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayShotcountParam, shotcountParam);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayStopwatchParamMillis,  (int) timeMillis);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayRefreshParamMode, TimerEvent::displayModeReview);
}

void displayTiming() {
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayClear, 0);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayStopwatchParamMillis, 0);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayRefreshParamMode, TimerEvent::displayModeTiming);
}

void displayCountdown(unsigned long timeMillis) {
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayClear, 0);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayStopwatchParamMillis,  (int) timeMillis);
  TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventDisplayRefreshParamMode, TimerEvent::displayModeCountdown);
}
