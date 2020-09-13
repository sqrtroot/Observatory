//
// Created by Robert on 13/09/2020.
//

#ifndef CONTROL_PLUGIN_CONSTANTS_HPP
#define CONTROL_PLUGIN_CONSTANTS_HPP
#include <chrono>
namespace ControlPluginConsts {
#define config_const constexpr const
using namespace std::chrono_literals;

/* Button timings */
config_const auto    rotation_timeout    = 100ms;
config_const auto    doublepress_timeout = 300ms;
config_const auto    longpress_timeout   = 2s;
config_const auto    deadzone_timeout    = 500ms;
config_const uint8_t deadzoneMax         = 2;

/* Pin offsets */
config_const unsigned int pulse_pin_offset       = 6;
config_const unsigned int direction_pin_offset   = 5;
config_const unsigned int button_pin_offset      = 13;
config_const auto         line_bulk_polling_rate = 50ms;

/* Ui settings */
config_const float zoom_reset_duration = 2.0 /*seconds*/;
}    // namespace ControlPluginConsts

#endif    // CONTROL_PLUGIN_CONSTANTS_HPP
