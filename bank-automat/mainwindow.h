#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QKeyEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSerialPort/QSerialPort>
#include <QTimer>

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
    QString defaultStyle;

    bool highContrast = false;
    int selectedAmount = 0;
    int accountId;

    QNetworkAccessManager *networkManager;
    QString sessionToken; // Tähän tallennetaan bäckäriltä saatu JWT-token
    QString sessionCardNumber;
    void makeLoginRequest(QString cardNum, QString pin);

    void setLanguage(const QString &lang);
    void handleDigit(const QString &digit);
    void setupSerialReader();
    void readCardData();
    void showPage(QWidget *page);

    void updateBalanceDisplay();
    void updateTransactionsDisplay();
    void makeWithdrawalRequest(int amount, QString description);

    void applyHighContrastTheme();
    void applyDefaultTheme();
    void applyMonitorStyleToPage(QWidget *page, bool highContrastEnabled);

    void selectAmount(int amount);
    QString formatAmount(int amount);

};

#endif // MAINWINDOW_H
