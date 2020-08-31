#pragma once
#include "DateTimeGui.hpp"
#include "QtSMLLogger.hpp"
#include <StelCore.hpp>
#include <StelMovementMgr.hpp>
#include <atomic>
#include <boost/sml.hpp>
#include <functional>
#include <mutex>
#include <queue>

#define nothing \
  []() {        \
  }

namespace sml = boost::sml;

struct ControlSMContext {
  StelCore*        stelCore;
  StelMovementMgr* stelMovementMgr;
  DateGui*         dateGui;
  TimeGui*         timeGui;
};

namespace ControlSMEvents {
struct DoubleButtonPress {};
struct ButtonPress {};
struct RotateLeft {};
struct RotateRight {};
struct RotateTimeout {};
struct ReturnFromSub {};
}    // namespace ControlSMEvents

struct DateTimeControl {
  static constexpr auto enable_dategui(const DateGui::Underscore& underscore) {
    return [=](ControlSMContext& sc) {
      sc.dateGui->show       = true;
      sc.dateGui->underscore = underscore;
    };
  }
  static constexpr auto enable_timegui(const TimeGui::Underscore& underscore) {
    return [=](ControlSMContext& sc) {
      sc.timeGui->show       = true;
      sc.timeGui->underscore = underscore;
    };
  }

  auto operator()() const {
    using namespace sml;
    using namespace ControlSMEvents;

    /*
     * Initial state: *year_control
     * Transition DSL: src_state + event [ guard ] / action = dst_state
     */
    // clang-format off
     return make_transition_table(
         *"year_control"_s   + on_entry<_>        / enable_dategui(DateGui::Underscore::year),
          "year_control"_s   + event<ButtonPress> / nothing                                                          = "month_control"_s,
          "year_control"_s   + event<RotateLeft>  / [](ControlSMContext & sc){sc.stelCore->subtractJulianYear();},
          "year_control"_s   + event<RotateRight> / [](ControlSMContext & sc){sc.stelCore->addJulianYear();},

          "month_control"_s  + on_entry<_>        / enable_dategui(DateGui::Underscore::month),
          "month_control"_s  + event<ButtonPress> / nothing                                                          = "day_control"_s,
          "month_control"_s  + event<RotateLeft>  / [](ControlSMContext & sc){sc.stelCore->subtractCalendricMonth();},
          "month_control"_s  + event<RotateRight> / [](ControlSMContext & sc){sc.stelCore->addCalendricMonth();},

          "day_control"_s    + on_entry<_>        / enable_dategui(DateGui::Underscore::day),
          "day_control"_s    + event<ButtonPress> / nothing                                                          = "hour_control"_s,
          "day_control"_s    + event<RotateLeft>  / [](ControlSMContext & sc){sc.stelCore->subtractDay();},
          "day_control"_s    + event<RotateRight> / [](ControlSMContext & sc){sc.stelCore->addDay();},
          "day_control"_s    + sml::on_exit<_>    / [](ControlSMContext & sc){sc.dateGui->show = false;},

          "hour_control"_s   + on_entry<_>        / enable_timegui(TimeGui::Underscore::hour),
          "hour_control"_s   + event<ButtonPress> / nothing                                                          = "minute_control"_s,
          "hour_control"_s   + event<RotateLeft>  / [](ControlSMContext & sc){sc.stelCore->subtractHour();},
          "hour_control"_s   + event<RotateRight> / [](ControlSMContext & sc){sc.stelCore->addHour();},

          "minute_control"_s + on_entry<_>        / enable_timegui(TimeGui::Underscore::minute),
          "minute_control"_s + event<ButtonPress> / process(ReturnFromSub{}),
          "minute_control"_s + event<RotateLeft>  / [](ControlSMContext & sc){sc.stelCore->subtractMinute();},
          "minute_control"_s + event<RotateRight> / [](ControlSMContext & sc){sc.stelCore->addMinute();},
          "minute_control"_s + sml::on_exit<_>    / [](ControlSMContext & sc){sc.timeGui->show = false;}
    );
    }
  // clang-format on
};

struct ControlSM {
  auto operator()() const {
    using namespace sml;
    using namespace ControlSMEvents;

    constexpr auto stop_zooming = [](ControlSMContext& sc) {
      sc.stelMovementMgr->zoomIn(false);
      sc.stelMovementMgr->zoomOut(false);
    };
    constexpr auto zoom_in = [](ControlSMContext& sc) {
      sc.stelMovementMgr->zoomIn(true);
    };
    constexpr auto zoom_out = [](ControlSMContext& sc) {
      sc.stelMovementMgr->zoomOut(true);
    };
    constexpr auto reset_zoom = [](ControlSMContext& sc) {
      sc.stelMovementMgr->zoomTo(sc.stelMovementMgr->getInitFov(), .200);
    };

    /**
     * Initial state: *zm
     * Transition DSL: src_state + event [ guard ] / action = dst_state
     */
    // clang-format off
     return make_transition_table(
         *"zm"_s                 + event<DoubleButtonPress>              = state<DateTimeControl>,
          "zm"_s                 + event<ButtonPress>   / reset_zoom ,
          "zm"_s                 + event<RotateRight>              = "zi"_s,
          "zm"_s                 + event<RotateLeft>               = "zo"_s,

          "zi"_s                 + on_entry<_>          / zoom_in,
          "zi"_s                 + event<RotateLeft>               = "zo"_s,
          "zi"_s                 + event<RotateTimeout>            = "zm"_s,
          "zi"_s                 + sml::on_exit<_>      / stop_zooming,

          "zo"_s                 + on_entry<_>          / zoom_out,
          "zo"_s                 + event<RotateRight>              = "zi"_s,
          "zo"_s                 + event<RotateTimeout>            = "zm"_s,
          "zo"_s                 + sml::on_exit<_>      / stop_zooming,
          state<DateTimeControl> + event<ReturnFromSub>           = "zm"_s
     );
    // clang-format on
  }
};

using ControlSM_t =
  sml::sm<ControlSM, sml::process_queue<std::queue>, sml::logger<QtSMLLogger>>;
ControlSM_t make_sm(ControlSMContext& csmc);
