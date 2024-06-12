#ifndef PRESS_F_QT_SETTINGS_CPP
#define PRESS_F_QT_SETTINGS_CPP

#include <QApplication>
#include <QDir>

extern "C"
{
  #include <src/config.h>
  #include <src/font.h>
}

#include "settings.h"

QPfSettings::QPfSettings()
{
  QPfSettings(QDir::currentPath() + "/config.ini");
}

QPfSettings::QPfSettings(const QString& filename)
{
  QSettings settings(filename, QSettings::IniFormat);

  m_Filename = filename;
  settings.setValue("bios_path", QDir::currentPath());
  settings.setValue("font", FONT_FAIRCHILD);
  settings.setValue("screen_size", 0);
  settings.setValue("skip_cartridge_verification", true);
  settings.setValue("tv_powww", false);
  settings.sync();
}

template <typename T>
T QPfSettings::getValue(const QString& key, T default_val)
{
  QSettings settings(m_Filename, QSettings::IniFormat);

  /* Use a default value if one was passed, but don't otherwise. */
  if (default_val == nullptr)
    return settings.value(key).value<T>();
  else
    return settings.value(key, QVariant(default_val)).value<T>();
}

template <typename T>
void QPfSettings::setValue(const QString& key, T new_val)
{
  QSettings settings(m_Filename, QSettings::IniFormat);

  settings.setValue(key, new_val);
}

#endif
