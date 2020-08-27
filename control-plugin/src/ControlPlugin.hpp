#pragma once
#include "InputThread.hpp"
#include "gui/ControlPluginSettingsWindow.hpp"
#include <QFont>
#include <QObject>
#include <StelModule.hpp>
#include <StelPluginInterface.hpp>
#include <memory>

class ControlPlugin : public StelModule {
  Q_OBJECT
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

private:
  std::unique_ptr<ControlPluginSettingsWindow> window;
};

class ControlPluginInterface : public QObject, public StelPluginInterface {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID StelPluginInterface_iid)
  Q_INTERFACES(StelPluginInterface)
public:
  [[nodiscard]] StelModule *getStelModule() const override;
  StelPluginInfo            getPluginInfo() const override;
  QObjectList getExtensionList() const override { return QObjectList(); }
};