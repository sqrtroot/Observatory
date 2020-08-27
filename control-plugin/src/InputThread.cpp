#include "InputThread.hpp"
#include <QLoggingCategory>
#include <QtCore>
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelMovementMgr.hpp>
#include <gpiod.hpp>

static const gpiod::line_request LINE_REQUEST_INPUT{
  "InputThread", gpiod::line_request::EVENT_BOTH_EDGES};

InputThread::InputThread():
    stateContext{StelApp::getInstance().getCore(),
                 StelApp::getInstance().getCore()->getMovementMgr()},
    statemachine(make_sm(stateContext)),
    turning_timeout(this) {
  qDebug() << "Constructed InputThread";
  turning_timeout.setSingleShot(true);
  turning_timeout.setInterval(50);
  //  const auto cb_f = [&](){
  //    statemachine.process_event(ControlSMEvents::RotateTimeout{});
  //  };
  //  connect(&turning_timeout, &QTimer::timeout, cb_f);
}
void InputThread::run() {
  using namespace ControlSMEvents;
  qDebug() << "Starting input thread";


  auto chip    = gpiod::chip("gpiochip0");
  auto buttons = chip.get_lines({26, 19, 21});
  buttons.request(LINE_REQUEST_INPUT);

  while(true) {
    auto event_lines = buttons.event_wait(std::chrono::nanoseconds(5));
    if(event_lines) {
      for(const auto& line: event_lines) {
        if(line == buttons[0]) {
          auto event = line.event_read();
          if(event.event_type == gpiod::line_event::RISING_EDGE) {
            statemachine.process_event(RotateRight{});
            turning_timeout.start();
          }
        } else if(line == buttons[1]) {
          auto event = line.event_read();
          if(event.event_type == gpiod::line_event::RISING_EDGE) {
            statemachine.process_event(RotateLeft{});
            turning_timeout.start();
          }
        } else if(line == buttons[2]) {
          auto event = line.event_read();
          if(event.event_type == gpiod::line_event::RISING_EDGE) {
            statemachine.process_event(ButtonPress{});
            turning_timeout.stop();
          }
        }
      }
    }
  }
}

