#ifndef PRESS_F_QT_REGISTERS_H
#define PRESS_F_QT_REGISTERS_H

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QTimer>

#include "../../types.h"

class RegisterLineEdit : public QWidget
{
  Q_OBJECT

public:
  RegisterLineEdit(const QString name, void *pointer, u8 length);

private:
  QLabel    *m_Label;
  QLineEdit *m_LineEdit;
  QTimer    *m_Timer;
  void      *m_DataPtr;
  u8         m_DataLen;

private slots:
  void onEntered8Bit();
  void onEntered16Bit();
  void onRefresh8Bit();
  void onRefresh16Bit();
};

class RegistersWindow : public QWidget
{
  Q_OBJECT

public:
  RegistersWindow();

private:
  QTableWidget     *m_CommandsList;
  QLabel           *m_CommandDescription;

  RegisterLineEdit *m_A;
  RegisterLineEdit *m_W;
  RegisterLineEdit *m_Isar;
  RegisterLineEdit *m_Pc0;
  RegisterLineEdit *m_Pc1;
  RegisterLineEdit *m_Dc0;
  RegisterLineEdit *m_Dc1;

  QTableWidget     *m_ScratchpadTable;

  QTimer           *m_Timer;

private slots:
  void onClickCommand(int Row, int Column, int PreRow, int PreColumn);
  void onRefresh     ();
};

#endif
