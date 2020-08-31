#include "InputThread.hpp"
#include <QLoggingCategory>
#include <QtCore>
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelMovementMgr.hpp>
#include <gpiod.hpp>

static auto                      line_identifier = "Stellarium input thread";
static const gpiod::line_request LINE_REQUEST_RISING_INPUT{
  "Stellarium Input Thread", gpiod::line_request::EVENT_RISING_EDGE};
static const gpiod::line_request LINE_REQUEST_FALLING_INPUT{
  "Stellarium Input Thread", gpiod::line_request::EVENT_FALLING_EDGE};

InputThread::InputThread(ControlSM_t& statemachine): statemachine(statemachine) {
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

  pulse.request({line_identifier, gpiod::line_request::EVENT_RISING_EDGE});
  direction.request({line_identifier, gpiod::line_request::DIRECTION_INPUT});
  button.request({line_identifier, gpiod::line_request::EVENT_RISING_EDGE});

  while(running) {
    auto event_lines = gpiod::line_bulk({pulse, button}).event_wait(rotationStopDelay);
    if(event_lines) {
      for(const auto& line: event_lines) {
        auto event = line.event_read();
        if(line == pulse) {
          rotating = true;
          if(direction.get_value()) {
            statemachine.process_event(RotateRight{});
          } else {
            statemachine.process_event(RotateLeft{});
          }
        } else {
          if(last_press && clock::now() - *last_press < doubleClickDelay) {
            last_press.reset();
            statemachine.process_event(DoubleButtonPress{});
          } else {
            last_press = clock::now();
          }
        }
      }
    } else {
      if(last_press && clock::now() - *last_press > doubleClickDelay) {
        last_press.reset();
        statemachine.process_event(ButtonPress{});
      }
      if(rotating) {
        statemachine.process_event(RotateTimeout{});
        rotating = false;
      }
    }
  }
  qDebug() << "Ending thread";
}

void InputThread::stop() {
  running = false;
}
