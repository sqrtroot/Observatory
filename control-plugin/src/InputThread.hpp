//
// Created by robert on 8/15/20.
//

#ifndef CONTROL_PLUGIN_INPUTTHREAD_HPP
#define CONTROL_PLUGIN_INPUTTHREAD_HPP
#include "EncoderStateMachine.hpp"
#include "StateMachine.hpp"
#include <QBasicTimer>
#include <QEventLoop>
#include <QThread>
#include <atomic>
#include <chrono>
#include <optional>

class InputThread : public QThread {
  std::atomic<bool> running = false;

  EncoderSMContext encoderSmContext;
  EncoderSM_t      encoderSm;
  QtSMLLogger      logger;

  using clock = std::chrono::high_resolution_clock;
  std::optional<decltype(clock::now())> last_press;

public:
  explicit InputThread(const EncoderActions& actions);
  void stop();

protected:
  void run() override;
};

#endif    // CONTROL_PLUGIN_INPUTTHREAD_HPP
