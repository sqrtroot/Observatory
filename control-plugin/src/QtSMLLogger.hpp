#pragma once
#include <boost/sml.hpp>
#include <QLoggingCategory>

namespace sml = boost::sml;

struct QtSMLLogger {
  template <class SM, class TEvent>
  void log_process_event(const TEvent&) {
    qDebug("[%s][process_event] %s\n", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TEvent>());
  }

  template <class SM, class TGuard, class TEvent>
  void log_guard(const TGuard&, const TEvent&, bool result) {}

  template <class SM, class TAction, class TEvent>
  void log_action(const TAction&, const TEvent&) {
    qDebug("[%s][action] %s %s\n", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TAction>(),
           sml::aux::get_type_name<TEvent>());
  }

  template <class SM, class TSrcState, class TDstState>
  void log_state_change(const TSrcState& src, const TDstState& dst) {
    qDebug("[%s][transition] %s -> %s\n", sml::aux::get_type_name<SM>(), src.c_str(), dst.c_str());
  }
};