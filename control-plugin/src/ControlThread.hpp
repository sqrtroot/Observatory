//
// Created by robert on 8/15/20.
//

#ifndef CONTROL_PLUGIN_CONTROLTHREAD_HPP
#define CONTROL_PLUGIN_CONTROLTHREAD_HPP
#include <QLoggingCategory>
#include <QThread>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

namespace fs = std::filesystem;
class ControlThread : public QThread {
  fs::path          fifoPath;
  std::atomic<bool> running = false;

public:
  ControlThread();
  void stop();

protected:
  void run() override;
};

#endif    // CONTROL_PLUGIN_CONTROLTHREAD_HPP
