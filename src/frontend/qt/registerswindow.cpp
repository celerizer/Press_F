#ifndef PRESS_F_QT_REGISTERS_CPP
#define PRESS_F_QT_REGISTERS_CPP

#include <QFontDatabase>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>

#include "main.h"
#include "registerswindow.h"

#include "../../emu.h"

RegisterLineEdit::RegisterLineEdit(const QString Name, void *Pointer, u8 Length = 1)
{
   m_DataPtr = Pointer;
   m_DataLen = Length;

   m_LineEdit = new QLineEdit();
   m_Label    = new QLabel(Name, m_LineEdit);

   /* Set up update timer */
   m_Timer = new QTimer(this);
   m_Timer->start(10);

   /* Event slots (different events are used for 1-byte and 2-byte values */
   if (Length == 2)
   {
      connect(m_LineEdit, SIGNAL(returnPressed()), this, SLOT(onEntered16Bit()));
      connect(m_Timer, SIGNAL(timeout()), this, SLOT(onRefresh16Bit()));
   }
   else
   {
      connect(m_LineEdit, SIGNAL(returnPressed()), this, SLOT(onEntered8Bit()));
      connect(m_Timer, SIGNAL(timeout()), this, SLOT(onRefresh8Bit()));
   }

   /* Finalize layout */
   QVBoxLayout *Layout = new QVBoxLayout();
   Layout->addWidget(m_Label);
   Layout->addWidget(m_LineEdit);
   setLayout(Layout);
}

/*
   Will 2-byte registers always be native-endian?
   If not, we should byteswap here.
*/
void RegisterLineEdit::onEntered8Bit()
{
   u8 Value = m_LineEdit->text().toInt(nullptr, 16);
   memcpy(m_DataPtr, &Value, 1);
}

void RegisterLineEdit::onEntered16Bit()
{
   u16 Value = m_LineEdit->text().toInt(nullptr, 16);
   memcpy(m_DataPtr, &Value, 2);
}

void RegisterLineEdit::onRefresh8Bit()
{
   if (m_LineEdit->hasFocus())
      return;
   else
      m_LineEdit->setText(QString::number(*(u8*)m_DataPtr, 16).toUpper());
}

void RegisterLineEdit::onRefresh16Bit()
{
   if (m_LineEdit->hasFocus())
      return;
   else
   {
      u16 Value = *(u16*)m_DataPtr;
      m_LineEdit->setText(QString::number(Value, 16).toUpper());
   }
}

RegistersWindow::RegistersWindow()
{
    QGridLayout *Layout = new QGridLayout;

    m_CommandsList = new QTableWidget();
    m_CommandsList->horizontalHeader()->setVisible(false);
    m_CommandsList->horizontalHeader()->setStretchLastSection(true);
    m_CommandsList->verticalHeader()->setVisible(false);
    m_CommandsList->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_CommandsList->setRowCount(sizeof(g_ChannelF.rom));
    m_CommandsList->setColumnCount(2);
    connect(m_CommandsList, SIGNAL(cellClicked(int, int)), this, SLOT(onClickCommand(int, int)));

    m_CommandDescription = new QLabel();
    m_A    = new RegisterLineEdit("A (Accumulator)",              &g_ChannelF.c3850.accumulator);
    m_W    = new RegisterLineEdit("W (Status)",                   &g_ChannelF.c3850.status_register);
    m_Isar = new RegisterLineEdit("ISAR (Indirect RAM pointer)",  &g_ChannelF.c3850.isar);
    m_Pc0  = new RegisterLineEdit("PC0 (Process counter)",        &g_ChannelF.pc0, 2);
    m_Pc1  = new RegisterLineEdit("PC1 (Process counter backup)", &g_ChannelF.pc1, 2);
    m_Dc0  = new RegisterLineEdit("DC0 (Data counter)",           &g_ChannelF.dc0, 2);
    m_Dc1  = new RegisterLineEdit("DC1 (Data counter backup)",    &g_ChannelF.dc1, 2);

    QGroupBox *RegistersGroupBox = new QGroupBox("Registers");
    QGridLayout* RegistersBox = new QGridLayout();
    RegistersBox->addWidget(m_A, 0, 0);
    RegistersBox->addWidget(m_W, 0, 1);
    RegistersBox->addWidget(m_Isar, 0, 2);
    RegistersBox->addWidget(m_Pc0, 1, 0);
    RegistersBox->addWidget(m_Pc1, 1, 1);
    RegistersBox->addWidget(m_Dc0, 2, 0);
    RegistersBox->addWidget(m_Dc1, 2, 1);
    RegistersGroupBox->setLayout(RegistersBox);

    m_ScratchpadTable = new QTableWidget();
    m_ScratchpadTable->setRowCount(8);
    m_ScratchpadTable->setColumnCount(8);
    m_ScratchpadTable->horizontalHeader()->setVisible(false);
    m_ScratchpadTable->horizontalHeader()->setDefaultSectionSize(16);
    m_ScratchpadTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ScratchpadTable->verticalHeader()->setVisible(false);
    m_ScratchpadTable->verticalHeader()->setDefaultSectionSize(16);
    m_ScratchpadTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    /* Finalize window layout */
    Layout->addWidget(m_CommandsList,       0, 0, 2, 1);
    Layout->addWidget(RegistersGroupBox,    0, 1, 1, 1);
    Layout->addWidget(m_ScratchpadTable,    1, 1, 1, 1);
    Layout->addWidget(m_CommandDescription, 2, 0, 1, 2);
    setLayout(Layout);

    char temp_string[256];
    for (int i = 0; i < sizeof(g_ChannelF.rom); i++)
    {
       snprintf(temp_string, sizeof(temp_string), "%04X", i);
       m_CommandsList->setItem(i, 0, new QTableWidgetItem(QString(temp_string)));

       if (g_ChannelF.rom[i] < 12)
          snprintf(temp_string, 256, opcodes[g_ChannelF.rom[i]].format, g_ChannelF.rom[i], g_ChannelF.rom[i + 1], g_ChannelF.rom[i + 2]);
       else
          snprintf(temp_string, 256, "Command: %02X", g_ChannelF.rom[i]);
       m_CommandsList->setItem(i, 1, new QTableWidgetItem(QString(temp_string)));
    }
}

void RegistersWindow::onClickCommand(int Row, int Column)
{
   m_CommandDescription->setText(opcodes[g_ChannelF.rom[Row]].description);
}

#endif
