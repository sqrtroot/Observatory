#include "InputThread.hpp"
#include <QLoggingCategory>
#include <QtCore>
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelMovementMgr.hpp>
#include <boost/sml.hpp>
#include <gpiod.hpp>

static auto                      line_identifier = "Stellarium input thread";

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

  auto pulse     = chip.get_line(ControlPluginConsts::pulse_pin_offset);
  auto direction = chip.get_line(ControlPluginConsts::direction_pin_offset);
  auto button    = chip.get_line(ControlPluginConsts::button_pin_offset);

  pulse.request({line_identifier, gpiod::line_request::EVENT_RISING_EDGE});
  direction.request({line_identifier, gpiod::line_request::DIRECTION_INPUT});
  button.request({line_identifier, gpiod::line_request::EVENT_BOTH_EDGES});

  while(running) {
    auto event_lines =
      gpiod::line_bulk({pulse, button}).event_wait(ControlPluginConsts::line_bulk_polling_rate);
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
