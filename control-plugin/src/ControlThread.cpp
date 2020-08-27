#include "ControlThread.hpp"
#include <chrono>
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelMovementMgr.hpp>
#include <unistd.h>
#include <gpiod.hpp>

static const gpiod::line_request LINE_REQUEST_INPUT{ "ControlThread",gpiod::line_request::EVENT_BOTH_EDGES };

ControlThread::ControlThread() : stateContext{StelApp::getInstance().getCore(), StelApp::getInstance().getCore()->getMovementMgr()}, statemachine(make_sm(stateContext)){
  qDebug() << "Constructed ControlThread";
}
void ControlThread::run() {
  using namespace ControlSMEvents;
  qDebug() << "Starting control thread";
  running = true;
  auto chip = gpiod::chip("gpiochip0");
  auto buttons = chip.get_lines({26,19, 21});
  buttons.request(LINE_REQUEST_INPUT);
  while(true){
    auto event_lines = buttons.event_wait(std::chrono::nanoseconds(5));
    if(event_lines){
      for(const auto & line : event_lines){
        if(line == buttons[0]){
	        auto event = line.event_read();
          if (event.event_type == gpiod::line_event::RISING_EDGE){
            statemachine.process_event(RotateRight{});
            
          }else{
            statemachine.process_event(RotateTimeout{});
          }
        }else if(line == buttons[1]){
	        auto event = line.event_read();
          if (event.event_type == gpiod::line_event::RISING_EDGE){
            statemachine.process_event(RotateLeft{});
            
          }else{
            statemachine.process_event(RotateTimeout{});
          }
        }else if(line == buttons[2]){
	        auto event = line.event_read();
          if (event.event_type == gpiod::line_event::RISING_EDGE){
            statemachine.process_event(ButtonPress{});
          }
        }
      }
    }
  }
  qDebug() << "Ending thread";
}

void ControlThread::stop() {
  running = false;
}
