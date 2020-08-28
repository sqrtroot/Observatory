//
// Created by robert on 8/15/20.
//

#ifndef CONTROL_PLUGIN_INPUTTHREAD_HPP
#define CONTROL_PLUGIN_INPUTTHREAD_HPP
#include "StateMachine.hpp"
#include <chrono>
#include <QThread>
#include <QBasicTimer>
#include <QEventLoop> 
#include <atomic>

class InputThread : public QThread {
  std::atomic<bool> running = false;

  ControlSMContext stateContext;
  ControlSM_t      statemachine;

  bool inputTimerOn = false;
  using clock = std::chrono::high_resolution_clock;
  decltype(clock::now()) lastInputTimestamp;
  static constexpr auto stopDelay = std::chrono::seconds(1);

public:
  InputThread();
  void stop();

protected:
  void run() override;
};

#endif    // CONTROL_PLUGIN_INPUTTHREAD_HPP
