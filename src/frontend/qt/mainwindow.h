#ifndef PRESS_F_QT_MAINWINDOW_H
#define PRESS_F_QT_MAINWINDOW_H

#include <QAudioOutput>
#include <QBuffer>
#include <QGamepad>
#include <QLabel>
#include <QToolBar>

#include "framebuffer_widget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();

private:
    QGamepad  m_Gamepads[2];
    QTimer   *m_Timer;
    QToolBar *m_Toolbar;

    QPfFramebuffer *m_Framebuffer;

    /* Audio stuff (TODO: Move to own file/class) */
    QBuffer      *m_AudioBuffer;
    QAudioOutput *m_AudioOutput;

    bool loadCartridge(QString Filename);

private slots:
    void onEjectCart();
    void onFrame();
    void onLoadCart();

protected:
    void closeEvent    (QCloseEvent *Event)     override;
    void dragEnterEvent(QDragEnterEvent *Event) override;
    void dropEvent     (QDropEvent *Event)      override;
    void resizeEvent   (QResizeEvent *Event)    override;
};

#endif
