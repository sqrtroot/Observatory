#include "InputThread.hpp"
#include <QLoggingCategory>
#include <QtCore>
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelMovementMgr.hpp>
#include <gpiod.hpp>

namespace internal {
static const unsigned int FRONT_SIZE = sizeof("internal::GetTypeNameHelper<") - 1u;
static const unsigned int BACK_SIZE  = sizeof(">::GetTypeName") - 1u;

template<typename T>
struct GetTypeNameHelper {
  static const char* GetTypeName(void) {
    static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
    static char         typeName[size] = {};
    memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);

    return typeName;
  }
};
}    // namespace internal

template<typename T>
const char* GetTypeName(void) {
  return internal::GetTypeNameHelper<T>::GetTypeName();
}

static const gpiod::line_request LINE_REQUEST_INPUT{
  "InputThread", gpiod::line_request::EVENT_RISING_EDGE};

InputThread::InputThread():
    stateContext{StelApp::getInstance().getCore(),
                 StelApp::getInstance().getCore()->getMovementMgr()},
    statemachine(make_sm(stateContext)) {
  qDebug() << "Constructed InputThread";
}

void InputThread::run() {
  using namespace ControlSMEvents;
  qDebug() << "Starting input thread";

  running = true;

  auto chip = gpiod::chip("gpiochip0");

  auto pulse     = chip.get_line(26);
  auto direction = chip.get_line(19);
  auto button    = chip.get_line(21);

  pulse.request(LINE_REQUEST_INPUT);
  button.request(LINE_REQUEST_INPUT);
  direction.request(LINE_REQUEST_INPUT);

  while(running) {
    auto event_lines = gpiod::line_bulk({pulse, button}).event_wait(stopDelay);
    if(event_lines) {
      for(const auto& line: event_lines) {
        if(line == pulse) {
          if(direction.get_value()) {
            statemachine.process_event(RotateRight{});
          } else {
            statemachine.process_event(RotateLeft{});
          }
        } else {
          statemachine.process_event(ButtonPress{});
        }
      }
    } else {
      statemachine.process_event(RotateTimeout{});
    }
  }
  qDebug() << "Ending thread";
}

void InputThread::stop() {
  running = false;
}
