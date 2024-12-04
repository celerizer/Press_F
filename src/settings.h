#ifndef PRESS_F_QT_SETTINGS_H
#define PRESS_F_QT_SETTINGS_H

#include <QSettings>

class QPfSettings
{
public:
  QPfSettings();
  QPfSettings(const QString& filename);

  template <typename T>
  T getValue(const QString& key, T default_val = nullptr);

  template <typename T>
  void setValue(const QString& key, T new_val);

private:
  QString m_Filename;
};

#endif
