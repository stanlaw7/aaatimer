#include <Arduino.h>
#include <RBD_Timer.h>
#include "timer_event.h"
#include "state.h"
#include "utils.h"

/*

machine state
-reset
-countdown
-sounded
-led 1
-led 2

*/

static TIMER_STATE timer_state;
static boolean review_pressed; // timer is started by sequence of 'start' after review
static RBD::Timer countdownTimer;

static void handleReviewBeforeStart(int event, int param) {
  review_pressed = true;
}

static void handleStartTrigger(int event, int param) {
  if (review_pressed) {
    timer_state = TIMER_STATE_COUNTDOWN;
    TimerEvent::getInstance()->queueHardwareEvent(TimerEvent::eventResetStopPlate, 0);
    TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventStartCountDown, 0);
    TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventActCamRecStart, 0);
    review_pressed = false; // clear the start sequence as we are done
  }
}

static void stateChangeHandleEvents(int event, int param) {
  if (timer_state == TIMER_STATE_COUNTDOWN) {
    if (event == TimerEvent::eventCountDownExpire) {
      timer_state = TIMER_STATE_TIMING;
      TimerEvent::getInstance()->queueHardwareEvent(TimerEvent::hardwareStopwatchStart, 0);
      TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventStartingSound, 0);
    }
    /*
    On countdown expire event => sounded state
    On review event => reset state
    */
  } else if (timer_state == TIMER_STATE_TIMING) {
    if (event == TimerEvent::hardwareStopPlateHit) {
      timer_state = TIMER_STATE_HIT;
      TimerEvent::getInstance()->queueHardwareEvent(TimerEvent::hardwareStopwatchRecordHit, 0);
    }
    /*
    On hit event => led 1 state
    On review event => reset state
    */
  } else if (timer_state == TIMER_STATE_HIT) {
    if (event == TimerEvent::hardwareStopPlateHit) {;
      TimerEvent::getInstance()->queueHardwareEvent(TimerEvent::hardwareStopwatchRecordHit, 0);
    }
    /*
    On hit event => led 2 state
    On reset event => reset state
    On review event => show next shot
    */
  }
}

//////////////// count down support ////////////////////////
static unsigned long lastCountdownDisplayedMillis;

static void handleStartCountDown(int event, int param) {
  int countDownMillis = random(1000, 4001);
  countdownTimer.setTimeout(countDownMillis);
  countdownTimer.restart();
  lastCountdownDisplayedMillis = 10000; // make sure we display the first countdown
}

static void handleCountDownExpire(int event, int param) {
  //TODO: state change taken care of that
}

static void loopCountDown() {
  if (countdownTimer.isExpired()) {
    countdownTimer.stop();
    TimerEvent::getInstance()->queueSoftwareEvent(TimerEvent::eventCountDownExpire, 0);
  } else if (!countdownTimer.isStopped()) {
    // timer are normally stopped except during countdown
    unsigned long currentCountDownMillis = countdownTimer.getInverseValue();
    if ((lastCountdownDisplayedMillis - currentCountDownMillis) > 50) {
      // don't overflow event queue, only update significant countdown
      displayCountdown(currentCountDownMillis);
      lastCountdownDisplayedMillis = currentCountDownMillis;
    }
  }
}
//////////////// end count down support ////////////////////////

void setupState() {
  timer_state = TIMER_STATE_RESET;
  review_pressed = false;
  countdownTimer.stop();

  // review and start will set timer into countdown
  TimerEvent::getInstance()->addListener(TimerEvent::hardwareReviewKey, handleReviewBeforeStart);
  TimerEvent::getInstance()->addListener(TimerEvent::hardwareStartKey, handleStartTrigger);

  // State changing event handler
  TimerEvent::getInstance()->addListener(TimerEvent::eventCountDownExpire, stateChangeHandleEvents);
  TimerEvent::getInstance()->addListener(TimerEvent::hardwareStopPlateHit, stateChangeHandleEvents);

  // Software event handlers
  TimerEvent::getInstance()->addListener(TimerEvent::eventStartCountDown, handleStartCountDown);
  TimerEvent::getInstance()->addListener(TimerEvent::eventCountDownExpire, handleCountDownExpire);
}

void loopState() {
  loopCountDown();
}
