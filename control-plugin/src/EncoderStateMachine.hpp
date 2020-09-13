//
// Created by Robert on 08/09/2020.
//

#ifndef CONTROL_PLUGIN_ENCODERSTATEMACHINE_HPP
#define CONTROL_PLUGIN_ENCODERSTATEMACHINE_HPP

#include "QtSMLLogger.hpp"
#include <boost/sml.hpp>
#include <chrono>
#include <cstdint>
#include <utility>
using namespace boost;

namespace EncoderSMEvents {
struct RotationPulse {
  bool left = false;
};
struct Timeout {};
struct ButtonDown {};
struct ButtonUp {};
struct Tick {};
}    // namespace EncoderSMEvents

struct EncoderActions {
  std::function<void()> left_rotation_action;
  std::function<void()> right_rotation_action;
  std::function<void()> rotation_timeout_action;
  std::function<void()> long_press_action;
  std::function<void()> short_press_action;
  std::function<void()> double_press_action;
  EncoderActions(std::function<void()> leftRotationAction,
                 std::function<void()> rightRotationAction,
                 std::function<void()> rotationTimeoutAction,
                 std::function<void()> longPressAction,
                 std::function<void()> shortPressAction,
                 std::function<void()> doublePressAction):
      left_rotation_action(std::move(leftRotationAction)),
      right_rotation_action(std::move(rightRotationAction)),
      rotation_timeout_action(std::move(rotationTimeoutAction)),
      long_press_action(std::move(longPressAction)),
      short_press_action(std::move(shortPressAction)),
      double_press_action(std::move(doublePressAction)) {}
};

struct EncoderSMContext {
  using clock = std::chrono::high_resolution_clock;
  uint8_t           deadzoneCounter;
  clock::time_point timeout_after;
  bool              timeout_active;
  EncoderActions    actions;
};

template<typename... Ts>
constexpr auto combine(Ts... ts) {
  return [ts...](EncoderSMContext& c) {
    (ts(c), ...);
  };
}

struct EncoderSM {
  auto operator()() const {
    using namespace boost::sml;
    using namespace EncoderSMEvents;
    using clock                    = std::chrono::high_resolution_clock;
    constexpr auto out_of_deadzone = [](EncoderSMContext& c) {
      c.timeout_after = EncoderSMContext::clock::now() + std::chrono::milliseconds(500);
      c.timeout_active = true;
      return ++c.deadzoneCounter > 2;
    };
    constexpr auto make_set_timeout = [](const EncoderSMContext::clock::duration& duration) {
      return [duration](EncoderSMContext& c) {
        c.timeout_after  = EncoderSMContext::clock::now() + duration;
        c.timeout_active = true;
      };
    };
    constexpr auto send_rotation = [=](const RotationPulse& event, EncoderSMContext& c) {
      if(event.left) {
        c.actions.left_rotation_action();
      } else {
        c.actions.right_rotation_action();
      }
      make_set_timeout(std::chrono::milliseconds(500))(c);
    };

    // clang-format off
    return make_transition_table(
       *"nothing"_s  + event<RotationPulse> [out_of_deadzone] = "rotating"_s,
        "nothing"_s  + event<Timeout> / [](EncoderSMContext & c){c.deadzoneCounter = 0;},
        "nothing"_s  + event<ButtonDown> = "p1"_s,
        "nothing"_s + sml::on_exit<_> / [](EncoderSMContext&c){c.timeout_active=false;},


       "rotating"_s + sml::on_entry<_> / make_set_timeout(std::chrono::milliseconds(500)),
       "rotating"_s + event<RotationPulse> / send_rotation,
       "rotating"_s + event<Timeout> = "nothing"_s,
       "rotating"_s + sml::on_exit<_> / [](EncoderSMContext&c){c.deadzoneCounter = 0; c.timeout_active=false; c.actions.rotation_timeout_action();},

       "p1"_s + sml::on_entry<_> / make_set_timeout(std::chrono::seconds(2)),
       "p1"_s + event<Timeout> / [](EncoderSMContext&c) {c.actions.long_press_action();} = "nothing"_s,
       "p1"_s + event<ButtonUp> = "p2"_s,
       "p2"_s + sml::on_exit<_> / [](EncoderSMContext&c){c.timeout_active=false;},

       "p2"_s + event<ButtonDown> / [](EncoderSMContext&c) {c.actions.double_press_action();} = "nothing"_s,
       "p2"_s + sml::on_entry<_> / make_set_timeout(std::chrono::milliseconds(300)),
       "p2"_s + event<Timeout> / [](EncoderSMContext&c) {c.actions.short_press_action();} = "nothing"_s,
       "p2"_s + sml::on_exit<_> / [](EncoderSMContext&c){c.timeout_active=false;}
    );
    // clang-format on
  };
};
using EncoderSM_t = sml::sm<EncoderSM, sml::logger<QtSMLLogger>>;

#endif    // CONTROL_PLUGIN_ENCODERSTATEMACHINE_HPP
