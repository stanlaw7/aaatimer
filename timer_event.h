#ifndef TIMER_EVENT_H
#define TIMER_EVENT_H

#include <EventManager.h>

class TimerEvent {
private:
  static TimerEvent* instance;
  EventManager timerEventManager;
  // Singleton disable constructor, copy constructor and assignment
  TimerEvent(){};
  TimerEvent(TimerEvent const& copy){};
  TimerEvent& operator=(TimerEvent const& copy){};
public:
  static TimerEvent* getInstance();
  ~TimerEvent();
  enum EventType {
    // hardware events are usually fast, could process in ISR and
    // resulting in firing software events
    hardwareReviewKey = 100,
    hardwareStartKey,
    hardwareStopPlateHit,
    hardwareLedOff,
    hardwareLedOneOnLedTwoOff,
    hardwareLedOneOffLedTwoOn,
    hardwareBuzzerOn,
    hardwareBuzzerOff,

    // software events usually takes more complex processing
    softwareCountDownExpire = 200
  };
  void setupTimerEvent();
  void addListener(int, EventManager::EventListener);
  void queueHardwareEvent(TimerEvent::EventType, int);
  void queueSoftwareEvent(TimerEvent::EventType, int);
  void processEvent();
};

#endif // TIMER_EVENT_H