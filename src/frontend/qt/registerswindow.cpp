#ifndef PRESS_F_QT_REGISTERS_CPP
#define PRESS_F_QT_REGISTERS_CPP

#include <QFocusEvent>
#include <QFontDatabase>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>

#include "main.h"
#include "registerswindow.h"

extern "C"
{
   #include "../../emu.h"
}

RegisterLineEdit::RegisterLineEdit(const QString name, void *pointer,
  u8 length = 1)
{
  m_DataPtr = pointer;
  m_DataLen = length;

  m_LineEdit = new QLineEdit();
  m_Label    = new QLabel(name, m_LineEdit);

  /* Set up update timer */
  m_Timer = new QTimer(this);
  m_Timer->start(10);

  /* Event slots (different events are used for 1-byte and 2-byte values */
  if (length == 2)
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
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(m_Label);
  layout->addWidget(m_LineEdit);
  setLayout(layout);
}

/*
   Will 2-byte registers always be native-endian?
   If not, we should byteswap here.
*/
void RegisterLineEdit::onEntered8Bit()
{
   auto value = static_cast<u8>(m_LineEdit->text().toInt(nullptr, 16));
   memcpy(m_DataPtr, &value, sizeof(value));
}

void RegisterLineEdit::onEntered16Bit()
{
   auto value = static_cast<u16>(m_LineEdit->text().toInt(nullptr, 16));
   memcpy(m_DataPtr, &value, sizeof(value));
}

void RegisterLineEdit::onRefresh8Bit()
{
  if (m_LineEdit->hasFocus())
    return;
  else
  {
    auto ptr = reinterpret_cast<u8*>(m_DataPtr);
    m_LineEdit->setText(QStringLiteral("%1").arg(
      *ptr, 2, 16, QLatin1Char('0')).toUpper());
  }
}

void RegisterLineEdit::onRefresh16Bit()
{
  if (m_LineEdit->hasFocus())
    return;
  else
  {
    auto ptr = reinterpret_cast<u16*>(m_DataPtr);
    m_LineEdit->setText(QStringLiteral("%1").arg(
      *ptr, 4, 16, QLatin1Char('0')).toUpper());
  }
}

RegistersWindow::RegistersWindow()
{
  char temp_string[256];
  u32 i, j;

  /*
  m_CommandsList = new QTableWidget();
  m_CommandsList->horizontalHeader()->setVisible(false);
  m_CommandsList->horizontalHeader()->setStretchLastSection(true);
  m_CommandsList->verticalHeader()->setVisible(false);
  m_CommandsList->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_CommandsList->setRowCount(sizeof(g_ChannelF.rom));
  m_CommandsList->setColumnCount(2);
  connect(m_CommandsList, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(onClickCommand(int, int, int, int)));
  for (i = 0; i < sizeof(g_ChannelF.rom); i++)
  {
    snprintf(temp_string, sizeof(temp_string), "%04X", i);
    m_CommandsList->setItem(i, 0, new QTableWidgetItem(QString(temp_string)));

    if (g_ChannelF.rom[i] < 16)
      snprintf(temp_string, 256, opcodes[g_ChannelF.rom[i]].format, g_ChannelF.rom[i], g_ChannelF.rom[i + 1], g_ChannelF.rom[i + 2]);
    else
      snprintf(temp_string, 256, "Command: %02X", g_ChannelF.rom[i]);
    m_CommandsList->setItem(i, 1, new QTableWidgetItem(QString(temp_string)));
  }

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
  */

  QGroupBox *ScratchpadGroupBox = new QGroupBox("Scratchpad RAM");
  QGridLayout* ScratchpadBox = new QGridLayout();
  m_ScratchpadTable = new QTableWidget();
  m_ScratchpadTable->setRowCount(8);
  m_ScratchpadTable->setColumnCount(8);
  m_ScratchpadTable->horizontalHeader()->setVisible(false);
  m_ScratchpadTable->horizontalHeader()->setDefaultSectionSize(16);
  m_ScratchpadTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_ScratchpadTable->verticalHeader()->setVisible(false);
  m_ScratchpadTable->verticalHeader()->setDefaultSectionSize(16);
  m_ScratchpadTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ScratchpadBox->addWidget(m_ScratchpadTable);
  ScratchpadGroupBox->setLayout(ScratchpadBox);
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      m_ScratchpadTable->setItem(i, j, new QTableWidgetItem());

  QGridLayout *layout = new QGridLayout;
  //layout->addWidget(m_CommandsList,       0, 0, 2, 1);
  //layout->addWidget(RegistersGroupBox,    0, 1, 1, 1);
  layout->addWidget(ScratchpadGroupBox,   1, 1, 1, 1);
  //layout->addWidget(m_CommandDescription, 2, 0, 1, 2);
  setLayout(layout);

  m_Timer = new QTimer(this);
  m_Timer->start(10);
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(onRefresh()));
}

void RegistersWindow::onClickCommand(int Row, int Column, int PreRow, int PreColumn)
{
  Q_UNUSED(Column)
  Q_UNUSED(PreRow)
  Q_UNUSED(PreColumn)
  //m_CommandDescription->setText(opcodes[g_ChannelF.rom[Row]].description);
}

void RegistersWindow::onRefresh()
{
  QTableWidgetItem *item;
  u8 i, j;

  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      m_ScratchpadTable->item(i, j)->setText(QString::number(
        g_ChannelF.main_cpu->scratchpad[i * 8 + j].u, 16).toUpper());
      m_ScratchpadTable->item(i, j)->setBackground(Qt::black);
    }
  }

  /* Highlight ISAR pointer in blue */
  i = g_ChannelF.main_cpu->isar / 8;
  j = g_ChannelF.main_cpu->isar - i * 8;
  m_ScratchpadTable->item(i, j)->setBackground(Qt::blue);

  /* Highlight stack usage in pink */
  i = g_ChannelF.main_cpu->scratchpad[59].u;
  while (i > 40)
  {
    item = m_ScratchpadTable->item(i / 8, (i % 8) - 1);

    if (item)
    {
      item->setBackground(Qt::magenta);
      i--;
    }
    else
      break;
  }
}

#endif
