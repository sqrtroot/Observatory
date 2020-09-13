//
// Created by Robert on 29/08/2020.
//

#include "DateTimeGui.hpp"

DateTimeGui::DateTimeGui() {
  gui = dynamic_cast<StelGui*>(StelApp::getInstance().getGui());
}

void DateTimeGui::init() {
  center_x = gui->getSkyGui()->getSkyGuiWidth() / 2;
  center_y = gui->getSkyGui()->getSkyGuiHeight() / 2;
  underlineFont.setUnderline(true);
}

void DateTimeGui::draw(StelCore* core) {
  if(!show) return;
  const auto   JD    = core->getJD();
  const double shift = core->getUTCOffset(JD) * 0.041666666666;
  const auto   text  = getText(JD + shift);

  const auto startSize     = fontMetrics.size(Qt::TextSingleLine, text[0]);
  const auto underlineSize = underlineFontMetrics.size(Qt::TextSingleLine, text[1]);
  const auto endSize       = fontMetrics.size(Qt::TextSingleLine, text[2]);
  const auto totalWidth = startSize + underlineSize + endSize;
  const auto center_x_start = (float) center_x - (float) totalWidth.width() / 2;
  const auto center_y_start = (float) center_y - (float) startSize.height() / 2;

  StelPainter painter(core->getProjection2d());
  painter.setColor(1, 1, 1);
  painter.setFont(font);
  painter.drawText(center_x_start, center_y_start, text[0]);
  painter.setFont(underlineFont);
  painter.drawText(center_x_start + startSize.width(), center_y_start, text[1]);
  painter.setFont(font);
  painter.drawText(
    center_x_start + startSize.width() + underlineSize.width(), center_y_start, text[2]);
}

std::array<QString, 3> DateGui::getText(const double JD) {
  int year, month, day;
  StelUtils::getDateFromJulianDay(JD, &year, &month, &day);
  switch(underscore) {
    case Underscore::year:
      return {QString(""),
              QString("%1").arg(year, 4, 10),
              QString("-%1-%2").arg(month, 2, 10, QLatin1Char('0')).arg(day, 2, 10, QLatin1Char('0'))};
    case Underscore::month:
      return {QString("%1-").arg(year, 4, 10),
              QString("%1").arg(month, 2, 10, QLatin1Char('0')),
              QString("-%1").arg(day, 2, 10, QLatin1Char('0'))};
    case Underscore::day:
      return {QString("%1-%2-").arg(year, 4, 10).arg(month, 2, 10, QLatin1Char('0')),
              QString("%1").arg(day, 2, 10, QLatin1Char('0')),
              QString("")};
  }
  return std::array<QString, 3>({
    QString(""),
  });
}
std::array<QString, 3> TimeGui::getText(const double JD) {
  int hour, minute, seconds;
  StelUtils::getTimeFromJulianDay(JD, &hour, &minute, &seconds);
  switch(underscore) {
    case Underscore::hour:
      return {QString(""),
              QString("%1").arg(hour, 2, 10, QLatin1Char('0')),
              QString(":%1").arg(minute, 2, 10, QLatin1Char('0'))};
    case Underscore::minute:
      return {QString("%1:").arg(hour, 2, 10, QLatin1Char('0')),
              QString("%1").arg(minute, 2, 10, QLatin1Char('0')),
              QString("")};
  }
  return std::array<QString, 3>({
    QString(""),
  });
}
