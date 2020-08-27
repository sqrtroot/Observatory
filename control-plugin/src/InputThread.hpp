//
// Created by robert on 8/15/20.
//

#ifndef CONTROL_PLUGIN_INPUTTHREAD_HPP
#define CONTROL_PLUGIN_INPUTTHREAD_HPP
#include "StateMachine.hpp"
#include <QThread>
#include <QTimer>
#include <atomic>
#include <memory>

class InputThread : public QThread {
  std::atomic<bool> running = false;

  ControlSMContext stateContext;
  ControlSM_t      statemachine;

  std::unique_ptr<QTimer> turning_timeout;
  void init_turning_timeout();

public:
  InputThread();
  void stop();

protected:
  void run() override;
};

#endif    // CONTROL_PLUGIN_INPUTTHREAD_HPP
