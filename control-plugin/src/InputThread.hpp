//
// Created by robert on 8/15/20.
//

#ifndef CONTROL_PLUGIN_INPUTTHREAD_HPP
#define CONTROL_PLUGIN_INPUTTHREAD_HPP
#include "StateMachine.hpp"
#include <QBasicTimer>
#include <QEventLoop>
#include <QThread>
#include <atomic>
#include <chrono>
#include <optional>

class InputThread : public QThread {
  std::atomic<bool> running = false;

  ControlSM_t& statemachine;

  static constexpr const auto rotationStopDelay = std::chrono::milliseconds(100);
  static constexpr const auto doubleClickDelay  = std::chrono::milliseconds(300);
  bool                        rotating          = false;
  using clock                                   = std::chrono::high_resolution_clock;
  std::optional<decltype(clock::now())> last_press;

public:
  explicit InputThread(ControlSM_t& statemachine);
  void stop();

protected:
  void run() override;
};

#endif    // CONTROL_PLUGIN_INPUTTHREAD_HPP
