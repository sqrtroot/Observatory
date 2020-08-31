#include "ControlPlugin.hpp"
#include <QApplication>
#include <QComboBox>
#include <QProcess>
#include <QtGui/QPainter>
#include <SkyGui.hpp>
#include <StelCore.hpp>
#include <StelGuiItems.hpp>
#include <StelModuleMgr.hpp>
#include <StelPainter.hpp>
#include <StelUtils.hpp>

StelModule* ControlPluginInterface::getStelModule() const {
  return new ControlPlugin();
}

StelPluginInfo ControlPluginInterface::getPluginInfo() const {
  StelPluginInfo info;
  info.id               = "control_plugin";
  info.displayedName    = "Control plugin";
  info.authors          = "Robert Bezem";
  info.contact          = "info@sqrtroot.com";
  info.description      = "Allow gpio control of functions";
  info.acknowledgements = "ME ME ME";
  info.license          = "PRIVATE";
  info.version          = "1.0.0";

  return info;
}
ControlPlugin::ControlPlugin():

    window(std::make_unique<ControlPluginSettingsWindow>()),
    stateContext{StelApp::getInstance().getCore(),
                 StelApp::getInstance().getCore()->getMovementMgr(),
                 &dateGui,
                 &timeGui},
    statemachine(make_sm(stateContext)),
    ct(statemachine) {
  setObjectName("control_plugin");
  font.setPixelSize(25);
}

void ControlPlugin::init() {
  dateGui.init();
  timeGui.init();
  ct.start();
  qDebug() << "Initializing Control plugin";
  initWifiButton();
}

void ControlPlugin::initWifiButton() {
  auto wifiAction = addAction("action_Show_Wifi_Settings",
                              "Show Wifi Settings",
                              "Show Wifi Settings",
                              "showWifiSettings()",
                              "Ctrl+w");
  try {
    auto gui = dynamic_cast<StelGui*>(StelApp::getInstance().getGui());

    if(gui != Q_NULLPTR) {
      wifiButton =
        std::make_unique<StelButton>(nullptr,
                                     QPixmap(":/control-plugin/wifi-button.png"),
                                     QPixmap(":/control-plugin/wifi-button.png"),
                                     QPixmap(),
                                     wifiAction);
      gui->getButtonBar()->addButton(wifiButton.get(), "065-pluginsGroup");
    }
  } catch(std::runtime_error& e) {
    qWarning() << "WARNING: unable create toolbar button for AngleMeasure plugin: "
               << e.what();
  }
}

void ControlPlugin::deinit() {
  ct.stop();
  qProcess.kill();
  StelModule::deinit();
}

double ControlPlugin::getCallOrder(StelModule::StelModuleActionName actionName) const {
  return StelModule::getCallOrder(actionName);
}
bool ControlPlugin::configureGui(bool show) {
  if(show) {
    window->setVisible(true);
  }
  return true;
}
void ControlPlugin::update(double delta) {}

void ControlPlugin::showWifiSettings() {
  if(qProcess.state() != QProcess::NotRunning) {
    qDebug() << "Killing previous instance";
    qProcess.kill();
  } else {
    qDebug() << "Showing wifi connection manager\n";
    while(!qProcess.state() == QProcess::NotRunning) {
      QApplication::processEvents();
    }
    qProcess.start("wicd-gtk", {"-n"});
  }
}
void ControlPlugin::draw(StelCore* core) {
  dateGui.draw(core);
  timeGui.draw(core);
}
