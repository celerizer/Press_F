#ifndef PRESS_F_QT_MAINWINDOW
#define PRESS_F_QT_MAINWINDOW

extern "C"
{
  #include "../../emu.h"
  #include "../../file.h"
  #include "../../input.h"
  #include "../../sound.h"
}

#include "main.h"
#include "mainwindow.h"
#include "registerswindow.h"

#include <string>

#include <QApplication>
#include <QDropEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QIcon>
#include <QFile>
#include <QMimeData>
#include <QTimer>
#include <QToolButton>

MainWindow::MainWindow()
{
  QGridLayout *layout = new QGridLayout;

  m_Timer = new QTimer(this);
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(onFrame()));
  m_Timer->start(1000 / 60);

  /* Setup toolbar */
  m_Toolbar = new QToolBar("File", this);

  QToolButton *ButFile = new QToolButton(this);
  ButFile->setIcon(QIcon(":/icons/sys"));
  ButFile->setPopupMode(QToolButton::InstantPopup);
  ButFile->setStyleSheet("QToolButton::menu-indicator { image: none; }");
  ButFile->setText(tr("File"));
  m_Toolbar->addWidget(ButFile);

  QAction *ActLoadCart = new QAction(QIcon(":/icons/logo"), tr("Load Cart..."));
  connect(ActLoadCart, SIGNAL(triggered()), this, SLOT(onLoadCart()));
  ButFile->addAction(ActLoadCart);

  QAction *ActEjectCart = new QAction(QIcon(":/icons/logo"), tr("Eject Cart"));
  connect(ActEjectCart, SIGNAL(triggered()), this, SLOT(onEjectCart()));
  ButFile->addAction(ActEjectCart);

  /* Setup window properties */
  setWindowTitle("Press F");
  setWindowIcon(QIcon(":/icons/logo"));

  /* Initialize emulation (TODO) */
  QFile bios_a_file(QDir::currentPath() + "/bios/sl31253.rom");
  QFile bios_b_file(QDir::currentPath() + "/bios/sl31254.rom");

  if (!bios_a_file.open(QIODevice::ReadOnly) ||
      !bios_b_file.open(QIODevice::ReadOnly))
    exit(1);

  QByteArray bios_a_data = bios_a_file.read(ROM_BIOS_SIZE);
  QByteArray bios_b_data = bios_b_file.read(ROM_BIOS_SIZE);

  if (bios_a_data.size() != ROM_BIOS_SIZE ||
      bios_b_data.size() != ROM_BIOS_SIZE)
    exit(2);

  load_single_bios(&g_ChannelF, reinterpret_cast<const u8*>(bios_a_data.data()),
    ROM_BIOS_SIZE, FALSE);
  load_single_bios(&g_ChannelF, reinterpret_cast<const u8*>(bios_b_data.data()),
    ROM_BIOS_SIZE, TRUE);

  /* Init sound */
  QAudioFormat Format;
  Format.setSampleRate(PF_FREQUENCY);
  Format.setChannelCount(2);
  Format.setSampleType(QAudioFormat::SignedInt);
  Format.setSampleSize(16);
  Format.setCodec("audio/pcm");

  setAcceptDrops(true);
  resize(92 * 4, 58 * 4);

  /* Init gamepad */
  m_Gamepads[0].setDeviceId(0);
  m_Gamepads[1].setDeviceId(1);

  /* Init audio */
  m_AudioOutput = new QAudioOutput(Format, this);
  m_AudioDevice = m_AudioOutput->start();

  pressf_init(&g_ChannelF);

  /* TODO: Remove */
  RegistersWindow *m_Regs = new RegistersWindow();
  m_Regs->show();

  /* Set up the widget that displays emulated frames */
  m_Framebuffer = new QPfFramebuffer(this);

  /* Finalize main window layout */
  layout->addWidget(m_Toolbar,     0, 0);
  layout->addWidget(m_Framebuffer, 1, 0);
  layout->setMargin(0);
  layout->setSpacing(0);
  setLayout(layout);
}

void MainWindow::onFrame()
{
  /* Input */
  set_input_button(0, INPUT_TIME,  m_Gamepads[0].buttonL1());
  set_input_button(0, INPUT_MODE,  m_Gamepads[0].buttonSelect());
  set_input_button(0, INPUT_HOLD,  m_Gamepads[0].buttonR1());
  set_input_button(0, INPUT_START, m_Gamepads[0].buttonStart());

  set_input_button(1, INPUT_RIGHT,      m_Gamepads[0].buttonRight());
  set_input_button(1, INPUT_LEFT,       m_Gamepads[0].buttonLeft());
  set_input_button(1, INPUT_BACK,       m_Gamepads[0].buttonDown());
  set_input_button(1, INPUT_FORWARD,    m_Gamepads[0].buttonUp());
  set_input_button(1, INPUT_ROTATE_CCW, m_Gamepads[0].buttonX());
  set_input_button(1, INPUT_ROTATE_CW,  m_Gamepads[0].buttonB());
  set_input_button(1, INPUT_PULL,       m_Gamepads[0].buttonY());
  set_input_button(1, INPUT_PUSH,       m_Gamepads[0].buttonA());

  set_input_button(4, INPUT_RIGHT,      m_Gamepads[1].buttonRight());
  set_input_button(4, INPUT_LEFT,       m_Gamepads[1].buttonLeft());
  set_input_button(4, INPUT_BACK,       m_Gamepads[1].buttonDown());
  set_input_button(4, INPUT_FORWARD,    m_Gamepads[1].buttonUp());
  set_input_button(4, INPUT_ROTATE_CCW, m_Gamepads[1].buttonX());
  set_input_button(4, INPUT_ROTATE_CW,  m_Gamepads[1].buttonB());
  set_input_button(4, INPUT_PULL,       m_Gamepads[1].buttonY());
  set_input_button(4, INPUT_PUSH,       m_Gamepads[1].buttonA());

  /* Emulation loop */
  pressf_run(&g_ChannelF);

  /* Video */
  m_Framebuffer->update();

  /* Audio */
  sound_write();
  m_AudioBuffer.append(reinterpret_cast<const char*>(samples), PF_SAMPLES * 4);
  if (m_AudioBuffer.size() > PF_SAMPLES * 4 * 2)
  {
    m_AudioDevice->write(m_AudioBuffer.data(), PF_SAMPLES * 4);
    m_AudioBuffer.remove(0, static_cast<int>(PF_SAMPLES * 4));
  }
}

void MainWindow::onEjectCart()
{
  pressf_reset(&g_ChannelF);
  memset(reinterpret_cast<void*>(&(g_ChannelF.rom[ROM_CARTRIDGE])), 0,
    ROM_CART_SIZE);
  pressf_load_rom(&g_ChannelF);
}

bool MainWindow::loadCartridge(QString Filename)
{
  QFile Rom(Filename);

  if (!Rom.open(QIODevice::ReadOnly))
    return false;
  else
  {
    pressf_reset(&g_ChannelF);
    Rom.read(reinterpret_cast<char*>(&g_ChannelF.rom[ROM_CARTRIDGE]),
             ROM_CART_SIZE);
    pressf_load_rom(&g_ChannelF);

    /*
      Update the window title to something friendly looking, ie:
      "/home/user/Bowling (1978) (Fairchild-21).chf" -> "Bowling"
    */
    setWindowTitle(tr("Press F - ") +
                   Filename.split('/').last().split('(').first());

    return true;
  }
}

void MainWindow::onLoadCart()
{
  loadCartridge(QFileDialog::getOpenFileName(this,
    tr("Load Cartridge"), "./games", tr("Channel F ROM Files (*.bin *.chf)")));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event)
  QApplication::quit();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
  event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
  const QMimeData* Data = event->mimeData();

  if (Data->hasUrls())
  {
    event->acceptProposedAction();
    loadCartridge(Data->urls().at(0).toLocalFile());
  }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  m_Framebuffer->setScale(event->size());
}

#endif
