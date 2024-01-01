#ifndef PRESS_F_QT_MAINWINDOW_H
#define PRESS_F_QT_MAINWINDOW_H

#include <QAudioOutput>
#include <QBuffer>
#include <QGamepad>
#include <QLabel>
#include <QToolBar>
#include "framebuffer_widget.h"
#include "settings.h"

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
  QPfSettings     m_Settings;

  /* Audio stuff (TODO: Move to own file/class) */
  QAudioOutput *m_AudioOutput;
  QByteArray    m_AudioBuffer;
  QIODevice    *m_AudioDevice;

  bool m_ClickedButtons[4];

  bool loadCartridge(QString Filename);

  /**
   * Returns whether or not one of the UI buttons for console input was recently
   * clicked, so general controller input doesn't override it.
   * @param index 0-3
   */
  bool wasClicked(unsigned index);

private slots:
  void onButton1();
  void onButton2();
  void onButton3();
  void onButton4();
  void onEjectCart();
  void onFrame();
  void onLoadCart();

protected:
  void closeEvent    (QCloseEvent *event)     override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent     (QDropEvent *event)      override;
  void resizeEvent   (QResizeEvent *event)    override;
};

#endif
