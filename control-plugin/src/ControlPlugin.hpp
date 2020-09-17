#pragma once
#include "InputThread.hpp"
#include "gui/ControlPluginSettingsWindow.hpp"
#include "gui/DateTimeGui.hpp"
#include <QFont>
#include <QObject>
#include <QProcess>
#include <StelGui.hpp>
#include <StelGuiItems.hpp>
#include <StelModule.hpp>
#include <StelPluginInterface.hpp>
#include <memory>

class ControlPlugin : public StelModule {
  Q_OBJECT
  QFont font;

  ControlSMContext stateContext;
  ControlSM_t      statemachine;
  EncoderActions   encoderActions;

  DateGui dateGui;
  TimeGui timeGui;

  InputThread ct;

public:
  ControlPlugin();
  ~ControlPlugin() override = default;

  // Methods defined in the StelModule class
  void   init() override;
  void   deinit() override;
  double getCallOrder(StelModuleActionName actionName) const override;
  bool   configureGui(bool show) override;
  void   update(double delta) override;
  void   draw(StelCore* core) override;

public slots:
  void showWifiSettings();

private:
  std::unique_ptr<ControlPluginSettingsWindow> window;
  std::unique_ptr<StelButton>                  wifiButton;
  QProcess                                     wifiProcess;
  void                                         initWifiButton();
};

class ControlPluginInterface : public QObject, public StelPluginInterface {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID StelPluginInterface_iid)
  Q_INTERFACES(StelPluginInterface)
public:
  [[nodiscard]] StelModule* getStelModule() const override;
  StelPluginInfo            getPluginInfo() const override;
  QObjectList getExtensionList() const override { return QObjectList(); }
};
