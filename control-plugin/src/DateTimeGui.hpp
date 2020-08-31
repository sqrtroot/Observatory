//
// Created by Robert on 29/08/2020.
//

#ifndef CONTROL_PLUGIN_DATETIMEGUI_H
#define CONTROL_PLUGIN_DATETIMEGUI_H
#include <QFont>
#include <QGraphicsItem>
#include <SkyGui.hpp>
#include <StelApp.hpp>
#include <StelCore.hpp>
#include <StelGui.hpp>
#include <StelPainter.hpp>
#include <StelUtils.hpp>
#include <array>
#include <atomic>
struct DateTimeGui {
  int center_x, center_y;

  StelGui* gui;

  QFont        font;
  QFont        underlineFont;
  QFontMetrics fontMetrics          = QFontMetrics(font);
  QFontMetrics underlineFontMetrics = QFontMetrics(underlineFont);

  std::atomic<bool> show = false;
  DateTimeGui();

  void                           draw(StelCore* core);
  virtual std::array<QString, 3> getText(const double JD) = 0;
  void                           init();
};

struct DateGui : public DateTimeGui {
  enum class Underscore { year, month, day };
  Underscore             underscore;
  std::array<QString, 3> getText(const double JD) override;
};
struct TimeGui : public DateTimeGui {
  enum class Underscore { hour, minute };
  Underscore             underscore;
  std::array<QString, 3> getText(const double JD) override;
};

#endif    // CONTROL_PLUGIN_DATETIMEGUI_H
