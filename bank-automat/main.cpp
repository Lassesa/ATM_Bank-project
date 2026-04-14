#include "mainwindow.h"
#include "ui_mainwindow.h"  

#include <QApplication>
#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>
#include <QSerialPort>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
