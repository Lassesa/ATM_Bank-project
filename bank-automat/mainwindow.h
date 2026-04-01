#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void setLanguage(const QString &lang);
    enum InputMode {
        PinMode,
        AmountMode
    };

    InputMode currentMode;

    void handleDigit(const QString &digit);
    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
