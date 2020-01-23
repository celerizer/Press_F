#ifndef PRESS_F_QT_MAINWINDOW_H
#define PRESS_F_QT_MAINWINDOW_H

#include <QAudioOutput>
#include <QBuffer>
#include <QGamepad>
#include <QLabel>
#include <QToolBar>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();

    QImage *m_Framebuffer;

private:
    QGamepad  m_Gamepads[2];
    QLabel   *m_Label;
    QTimer   *m_Timer;
    QToolBar *m_Toolbar;

    /* Audio stuff (TODO: Move to own file/class) */
    QBuffer      *m_AudioBuffer;
    QAudioOutput *m_AudioOutput;

    bool loadCartridge(QString Filename);

private slots:
    void onEjectCart();
    void onFrame();
    void onLoadCart();

protected:
    void dragEnterEvent(QDragEnterEvent *Event) override final;
    void dropEvent     (QDropEvent *Event)      override final;
    void resizeEvent   (QResizeEvent *Event)    override final;
};

#endif
