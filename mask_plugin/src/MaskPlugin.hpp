#pragma once
#include <QIcon>
#include <QOpenGLTexture>
#include <QPixmap>
#include <StelModule.hpp>
#include <StelPluginInterface.hpp>
#include <StelTexture.hpp>
#include <StelTextureMgr.hpp>

class MaskPlugin : public StelModule {
  Q_OBJECT
  // QIcon mask;
  // QPixmap pm;
  StelTextureSP t;
  // QOpenGLTexture *t;

public:
  MaskPlugin();
  ~MaskPlugin() override = default;

  // Methods defined in the StelModule class
  void   init() override;
  void   deinit() override;
  double getCallOrder(StelModuleActionName actionName) const override;
  bool   configureGui(bool show) override;
  void   update(double delta) override;
  void   draw(StelCore* core) override;
};

class MaskPluginInterface : public QObject, public StelPluginInterface {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID StelPluginInterface_iid)
  Q_INTERFACES(StelPluginInterface)
public:
  [[nodiscard]] StelModule* getStelModule() const override;
  StelPluginInfo            getPluginInfo() const override;
  QObjectList getExtensionList() const override { return QObjectList(); }
};
