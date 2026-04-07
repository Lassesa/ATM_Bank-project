#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QSerialPort>
#include <QString>

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

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    enum InputMode {
        PinMode,
        AmountMode
    };

    Ui::MainWindow *ui;
    InputMode currentMode;

    QSerialPort *serial;
    QString currentCardUid;

    bool highContrast = false;
    QString defaultStyle;

    void setLanguage(const QString &lang);
    void handleDigit(const QString &digit);
    void setupSerialReader();
    void readCardData();
    void showPage(QWidget *page);

    void applyHighContrastTheme();
    void applyDefaultTheme();
    void applyMonitorStyleToPage(QWidget *page, bool highContrastEnabled);

};

#endif // MAINWINDOW_H
