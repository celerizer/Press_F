#ifndef PRESS_F_QT_REGISTERS_CPP
#define PRESS_F_QT_REGISTERS_CPP

#include <QGridLayout>

#include "main.h"
#include "registerswindow.h"

RegistersWindow::RegistersWindow()
{
    QGridLayout *Layout = new QGridLayout;

    m_Timer = new QTimer(this);
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(onRefresh()));
    m_Timer->start(10);

    m_TxtA   = new QLineEdit();
    m_TxtPc0 = new QLineEdit();
    m_TxtPc1 = new QLineEdit();
    m_TxtDc0 = new QLineEdit();
    m_TxtDc1 = new QLineEdit();

    /* Finalize window layout */
    Layout->addWidget(m_TxtA,   0, 0);
    Layout->addWidget(m_TxtPc0, 1, 0);
    Layout->addWidget(m_TxtPc1, 1, 1);
    Layout->addWidget(m_TxtDc0, 2, 0);
    Layout->addWidget(m_TxtDc1, 2, 1);
    setLayout(Layout);
}

void RegistersWindow::onRefresh()
{
    m_TxtA->setText(QString::number(g_ChannelF.c3850.accumulator, 16));
    m_TxtPc0->setText(QString::number(g_ChannelF.pc0, 16));
    m_TxtPc1->setText(QString::number(g_ChannelF.pc1, 16));
    m_TxtDc0->setText(QString::number(g_ChannelF.dc0, 16));
    m_TxtDc1->setText(QString::number(g_ChannelF.dc1, 16));
}

#endif
