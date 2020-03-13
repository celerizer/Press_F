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
#include <QAudioBuffer>
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
    QGridLayout *Layout = new QGridLayout;

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
    QFile BiosA("F:/msys64/home/andre/retroarch-cle/system/sl31253.rom");
    QFile BiosB("F:/msys64/home/andre/retroarch-cle/system/sl31254.rom");
    if (!BiosA.open(QIODevice::ReadOnly) || !BiosB.open(QIODevice::ReadOnly))
        exit(-1);
    BiosA.read((char*)&g_ChannelF.rom[ROM_BIOS_A], ROM_BIOS_SIZE);
    BiosB.read((char*)&g_ChannelF.rom[ROM_BIOS_B], ROM_BIOS_SIZE);

    /* Init sound */
    QAudioFormat Format;
    Format.setSampleRate(PF_FREQUENCY);
    Format.setChannelCount(2);
    Format.setSampleType(QAudioFormat::SignedInt);
    Format.setSampleSize(16);
    Format.setCodec("audio/pcm");

    setAcceptDrops(true);

    /* Init gamepad */
    m_Gamepads[0].setDeviceId(0);
    m_Gamepads[1].setDeviceId(1);

    /* Init audio */
    m_AudioOutput = new QAudioOutput(Format, this);

    m_AudioBuffer = new QBuffer(this);
    m_AudioBuffer->setData(QByteArray::fromRawData((char*)samples, sizeof(samples)));
    m_AudioBuffer->open(QIODevice::ReadOnly);
    m_AudioOutput->start(m_AudioBuffer);

    pressf_init(&g_ChannelF);

    /* TODO: Remove */
    RegistersWindow *m_Regs = new RegistersWindow();
    m_Regs->show();

    /* Set up the widget that displays emulated frames */
    m_Framebuffer = new QPfFramebuffer(this);

    /* Finalize main window layout */
    Layout->addWidget(m_Toolbar,     0, 0);
    Layout->addWidget(m_Framebuffer, 1, 0);
    Layout->setMargin(0);
    Layout->setSpacing(0);
    //Layout->setAlignment(m_Framebuffer, Qt::AlignHCenter);
    setLayout(Layout);
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
    m_AudioBuffer->reset();
}

void MainWindow::onEjectCart()
{
    pressf_reset(&g_ChannelF);
    memset((void*)&(g_ChannelF.rom[ROM_CARTRIDGE]), 0, ROM_CART_SIZE);
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
        Rom.read((char*)&g_ChannelF.rom[ROM_CARTRIDGE], ROM_CART_SIZE);
        pressf_load_rom(&g_ChannelF);

        /* Update the window title to something friendly looking
         * "/home/user/Bowling (1978) (Fairchild-21).chf" -> "Bowling" */
        setWindowTitle(tr("Press F - ") + Filename.split('/').last().split('(').first());

        return true;
    }
}

void MainWindow::onLoadCart()
{
    loadCartridge(QFileDialog::getOpenFileName(this,
        tr("Load Cartridge"), "./games", tr("Channel F ROM Files (*.bin *.chf)")));
}

void MainWindow::closeEvent(QCloseEvent *Event)
{
   QApplication::quit();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *Event)
{
    Event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *Event)
{
    const QMimeData* Data = Event->mimeData();

    if (Data->hasUrls())
    {
        Event->acceptProposedAction();
        loadCartridge(Data->urls().at(0).toLocalFile());
    }
}

void MainWindow::resizeEvent(QResizeEvent *Event)
{
    m_Framebuffer->setScale(Event->size());
}

#endif
