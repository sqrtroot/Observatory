#include "MaskPlugin.hpp"
#include <StelGui.hpp>
#include <GL/gl.h>
#include <QApplication>
#include <QComboBox>
#include <QGLContext>
#include <QPainter>
#include <QProcess>
#include <QtGui/QPainter>
#include <SkyGui.hpp>
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelGuiItems.hpp>
#include <StelMainView.hpp>
#include <StelModuleMgr.hpp>
#include <StelPainter.hpp>
#include <StelUtils.hpp>

StelModule* MaskPluginInterface::getStelModule() const {
  return new MaskPlugin();
}

StelPluginInfo MaskPluginInterface::getPluginInfo() const {
  StelPluginInfo info;
  info.id               = "mask_plugin";
  info.displayedName    = "Mask plugin";
  info.authors          = "Robert Bezem";
  info.contact          = "info@sqrtroot.com";
  info.description      = "Allow custom mask on display";
  info.acknowledgements = "Commonplace studio\n SqrtRoot";
  info.license          = "GPL";
  info.version          = "1.0.0";

  return info;
}

MaskPlugin::MaskPlugin() {}
// MaskPlugin::MaskPlugin():
//     mask(":/mask-plugin/mask.svg"){}

void MaskPlugin::init() {
  qDebug() << "INIT??";
  auto image = QImage(":/mask-plugin/mask.png");
  for(int y = 0; y < image.height(); ++y)    // to get each line
  {
    for(int x = 0; x < image.width(); ++x)    // iterate over each pixel in each line
    {
      auto pixel = image.pixelColor(x, y);
      if(pixel.alpha() == 0) {
        image.setPixelColor(x, y, QColor(255, 255, 255));
      } else {
        image.setPixelColor(x, y, QColor(0, 0, 0, 128));
      }
    }
  }

  t = StelApp::getInstance().getTextureManager().createTexture(image);
  t->waitForLoaded();
  qDebug() << "DONE??";
}

void MaskPlugin::deinit() {}

double MaskPlugin::getCallOrder(StelModule::StelModuleActionName actionName) const {
  return std::numeric_limits<double>::infinity();
  // return 0;
}
bool MaskPlugin::configureGui(bool show) {
  return false;
}

void MaskPlugin::update(double delta) {}

void MaskPlugin::draw(StelCore* core) {
  auto&       mv = StelMainView::getInstance();
  auto gui = dynamic_cast<StelGui*>(StelApp::getInstance().getGui());
  auto x = gui->getSkyGui()->getSkyGuiWidth();
  auto y = gui->getSkyGui()->getSkyGuiHeight();

  StelPainter painter(core->getProjection2d());
  auto        gl = painter.glFuncs();
  painter.setColor(1, 1, 1, 1);
  painter.setBlending(true, GL_DST_COLOR, GL_DST_COLOR);
  t->bind();
  painter.drawRect2d(0, 0, x, y, true);
}
