//
// Created by robert on 8/15/20.
//

#ifndef CONTROL_PLUGIN_INPUTTHREAD_HPP
#define CONTROL_PLUGIN_INPUTTHREAD_HPP
#include "StateMachine.hpp"
#include <QThread>
#include <QTimer>
#include <atomic>

//class Worker : public QObject
//{
//Q_OBJECT
//
//public slots:
//  void doWork(const QString &parameter) {
//    QString result;
//    /* ... here is the expensive or blocking operation ... */
//    emit resultReady(result);
//  }
//
//signals:
//  void resultReady(const QString &result);
//};

class InputThread : public QObject {
  Q_OBJECT

  ControlSMContext stateContext;
  ControlSM_t      statemachine;

  QTimer turning_timeout;

public:
  InputThread();
  void run();
};

#endif    // CONTROL_PLUGIN_INPUTTHREAD_HPP
