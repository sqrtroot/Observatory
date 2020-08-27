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
    turning_timeout() {
  qDebug() << "Constructed InputThread";


}
void InputThread::init_turning_timeout() {
  qDebug() << "Making thread turning timer";
  turning_timeout = std::make_unique<QTimer>(this);

  turning_timeout->setSingleShot(true);
  turning_timeout->setInterval(50);

  connect(turning_timeout.get(), &QTimer::timeout, [&](){
    statemachine.process_event(ControlSMEvents::RotateTimeout{});
  });
}
void InputThread::run() {
  using namespace ControlSMEvents;
  qDebug() << "Starting input thread";
  init_turning_timeout();
  running      = true;
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
  qDebug() << "Ending thread";
}

void InputThread::stop() {
  running = false;
}
