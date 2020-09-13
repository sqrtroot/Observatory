#include "InputThread.hpp"
#include <QLoggingCategory>
#include <QtCore>
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelMovementMgr.hpp>
#include <boost/sml.hpp>
#include <gpiod.hpp>

static auto                      line_identifier = "Stellarium input thread";
static const gpiod::line_request LINE_REQUEST_RISING_INPUT{
  "Stellarium Input Thread", gpiod::line_request::EVENT_RISING_EDGE};
static const gpiod::line_request LINE_REQUEST_FALLING_INPUT{
  "Stellarium Input Thread", gpiod::line_request::EVENT_FALLING_EDGE};

InputThread::InputThread(const EncoderActions& encoderActions):
    encoderSmContext{.actions = encoderActions}, encoderSm(encoderSmContext, logger) {
  qDebug() << "Constructed InputThread";
}

void InputThread::run() {
  using namespace ControlSMEvents;
  using namespace EncoderSMEvents;
  qDebug() << "Starting input thread";

  running = true;

  auto chip = gpiod::chip("gpiochip0");

  auto pulse     = chip.get_line(6);
  auto direction = chip.get_line(5);
  auto button    = chip.get_line(13);

  pulse.request({line_identifier, gpiod::line_request::EVENT_RISING_EDGE});
  direction.request({line_identifier, gpiod::line_request::DIRECTION_INPUT});
  button.request({line_identifier, gpiod::line_request::EVENT_BOTH_EDGES});

  while(running) {
    auto event_lines =
      gpiod::line_bulk({pulse, button}).event_wait(std::chrono::nanoseconds(5));
    if(event_lines) {
      for(const auto& line: event_lines) {
        auto event = line.event_read();
        if(line == pulse) {
          if(direction.get_value()) {
            encoderSm.process_event(RotationPulse{.left = false});
          } else {
            encoderSm.process_event(RotationPulse{.left = true});
          }
        } else {
          if(event.event_type == gpiod::line_event::FALLING_EDGE) {
            encoderSm.process_event(ButtonUp{});
          } else {
            encoderSm.process_event(ButtonDown{});
          }
        }
      }
    }
    if(encoderSmContext.timeout_active &&
       EncoderSMContext::clock::now() >= encoderSmContext.timeout_after) {
      encoderSmContext.timeout_active = false;
      encoderSm.process_event(Timeout{});
    }
  }
  qDebug() << "Ending thread";
}

void InputThread::stop() {
  running = false;
}
