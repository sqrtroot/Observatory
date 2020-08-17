#include "ControlThread.hpp"
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelMovementMgr.hpp>

ControlThread::ControlThread(): fifoPath(std::move(fifoPath)) {
  fifoPath = (fs::temp_directory_path() / "stellariumFifo");
  if(!fs::exists(fifoPath)) {
    if(mkfifo(this->fifoPath.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH) != 0) {
      throw std::runtime_error("Couldn't create fifo");
    }
  }
  qDebug() << "Constructed ControlThread";
}
void ControlThread::run() {
  qDebug() << "Starting control thread";
  running = true;
  std::ifstream fifo(fifoPath);
  auto          stelCore        = StelApp::getInstance().getCore();
  auto          stelMovementMgr = stelCore->getMovementMgr();
  while(running) {
    std::string input;
    std::getline(fifo, input);
    if(input == "ZoomIn") {
      auto fov = stelMovementMgr->getAimFov();
      stelMovementMgr->zoomTo(fov - (fov / 10));
    } else if(input == "ZoomOut") {
      auto fov = stelMovementMgr->getAimFov();
      stelMovementMgr->zoomTo(fov + (fov / 10));
    } else if(input == "Home") {
      stelMovementMgr->setViewDirectionJ2000(stelCore->altAzToJ2000(
        stelMovementMgr->getInitViewingDirection(), StelCore::RefractionOff));
      stelMovementMgr->zoomTo(stelMovementMgr->getInitFov(), 1.);
    } else if(input == "SetHome") {
      stelMovementMgr->setInitFov(stelMovementMgr->getAimFov());
      stelMovementMgr->setInitViewDirectionToCurrent();
    }
    if(fifo.eof()) {
      fifo.clear();
    }
  }
  qDebug() << "Ending thread";
}

void ControlThread::stop() {
  running = false;
}
