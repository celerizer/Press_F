#include <QApplication>
#include <QMainWindow>
#include <QTimer>

#include "main.h"
#include "mainwindow.h"

channelf_t g_ChannelF;

int main(int argc, char *argv[])
{
    MainWindow *m_Window;
    QApplication a(argc, argv);

    m_Window = new MainWindow();
    m_Window->show();

    return a.exec();
}
