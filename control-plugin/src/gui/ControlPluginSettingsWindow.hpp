#pragma once
#include <memory>

#include "StelDialog.hpp"

class Ui_controlPluginSettingsWindowForm;

class ControlPluginSettingsWindow : public StelDialog
{
    Q_OBJECT

public:
  ControlPluginSettingsWindow();
    ~ControlPluginSettingsWindow();
public slots:
  virtual void retranslate(){};
protected:
    void createDialogContent();

private:
    std::unique_ptr<Ui_controlPluginSettingsWindowForm> ui;
};
