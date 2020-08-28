#include "InputThread.hpp"
#include <QLoggingCategory>
#include <QtCore>
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelMovementMgr.hpp>
#include <gpiod.hpp>

namespace internal
{
  static const unsigned int FRONT_SIZE = sizeof("internal::GetTypeNameHelper<") - 1u;
  static const unsigned int BACK_SIZE = sizeof(">::GetTypeName") - 1u;
 
  template <typename T>
  struct GetTypeNameHelper
  {
    static const char* GetTypeName(void)
    {
      static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
      static char typeName[size] = {};
      memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);
 
      return typeName;
    }
  };
}
 
 
template <typename T>
const char* GetTypeName(void)
{
  return internal::GetTypeNameHelper<T>::GetTypeName();
}

static const gpiod::line_request LINE_REQUEST_INPUT{
  "InputThread", gpiod::line_request::EVENT_BOTH_EDGES};

InputThread::InputThread():
    stateContext{StelApp::getInstance().getCore(),
                 StelApp::getInstance().getCore()->getMovementMgr()},
    statemachine(make_sm(stateContext))
    {
  qDebug() << "Constructed InputThread";
}

void InputThread::run() {
  using namespace ControlSMEvents;
  qDebug() << "Starting input thread";

  // const auto cb_f = [&](){
  //   statemachine.process_event(ControlSMEvents::RotateTimeout{});
  // };
  // connect(&turning_timeout, &QTimer::timeout, cb_f);
  // turning_timeout.start();

  running      = true;

  auto chip    = gpiod::chip("gpiochip0");
  auto buttons = chip.get_lines({26, 19, 21});
  buttons.request(LINE_REQUEST_INPUT);

  while(running) {
    auto event_lines = buttons.event_wait(std::chrono::nanoseconds(10));
    if(event_lines) {
      for(const auto& line: event_lines) {
        if(line == buttons[0]) {
          auto event = line.event_read();
          if(event.event_type == gpiod::line_event::RISING_EDGE) {
            statemachine.process_event(RotateRight{});
            inputTimerOn = true;
            lastInputTimestamp = clock::now();
          }
        } else if(line == buttons[1]) {
          auto event = line.event_read();
          if(event.event_type == gpiod::line_event::RISING_EDGE) {
            statemachine.process_event(RotateLeft{});
            inputTimerOn = true;
            lastInputTimestamp = clock::now();
          }
        } else if(line == buttons[2]) {
          auto event = line.event_read();
          if(event.event_type == gpiod::line_event::RISING_EDGE) {
            statemachine.process_event(ButtonPress{});
            inputTimerOn = false;
          }
        }
      }
    }
    if( inputTimerOn && clock::now() - lastInputTimestamp > stopDelay){
      inputTimerOn = false;
      statemachine.process_event(ControlSMEvents::RotateTimeout{});
    }
  }
  qDebug() << "Ending thread";
}

void InputThread::stop() {
  running = false;
}
