#include "ControlPlugin.hpp"
#include <QComboBox>
#include <StelCore.hpp>
#include <StelModuleMgr.hpp>
#include <StelPainter.hpp>

StelModule *ControlPluginInterface::getStelModule() const {
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
    window(std::make_unique<ControlPluginSettingsWindow>()) {
  setObjectName("control_plugin");
}

void ControlPlugin::init() {
  qDebug() << "Initializing Control plugin";
  ct.moveToThread(&inputThread);
  inputThread.start();
}
void ControlPlugin::deinit() {
  StelModule::deinit();
  inputThread.quit();
  inputThread.wait();
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
