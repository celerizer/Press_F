#ifndef PRESS_F_QT_REGISTERS_H
#define PRESS_F_QT_REGISTERS_H

#include <QLabel>
#include <QLineEdit>
#include <QTimer>

class RegistersWindow : public QWidget
{
    Q_OBJECT

public:
    RegistersWindow();

private:
    QLineEdit *m_TxtA;
    QLineEdit *m_TxtW;
    QLineEdit *m_TxtDc0;
    QLineEdit *m_TxtDc1;
    QLineEdit *m_TxtPc0;
    QLineEdit *m_TxtPc1;
    QTimer    *m_Timer;

private slots:
    void onRefresh();
};

#endif
