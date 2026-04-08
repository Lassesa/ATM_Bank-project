#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// =====================================================
// Qt Core / Widgets
// =====================================================

#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QKeyEvent>

// =====================================================
// Qt Network / JSON
// =====================================================

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// =====================================================
// Qt Serial Port
// =====================================================

#include <QtSerialPort/QSerialPort>

// =====================================================
// Qt Utility Classes
// =====================================================

#include <QButtonGroup>

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
    // =====================================================
    // Input Mode
    // =====================================================
    enum InputMode {
        PinMode,
        AmountMode
    };

    // =====================================================
    // UI / Core Objects
    // =====================================================
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QNetworkAccessManager *networkManager;

    // =====================================================
    // Runtime State
    // =====================================================
    InputMode currentMode = PinMode;
    QString currentCardUid;
    QString sessionToken;
    QString sessionCardNumber;

    int selectedAmount = 0;
    int accountId = 0;

    // =====================================================
    // Donation Button Groups
    // =====================================================
    QButtonGroup *donationOrgGroup;
    QButtonGroup *donationAmountGroup;

    // =====================================================
    // Setup / Connections
    // =====================================================
    void connectSignals();

    // =====================================================
    // Language Handling
    // =====================================================
    void setLanguage(const QString &lang);

    // =====================================================
    // Input Handling
    // =====================================================
    void handleDigit(const QString &digit);

    // =====================================================
    // Serial Card Reader
    // =====================================================
    void setupSerialReader();
    void readCardData();

    // =====================================================
    // Navigation
    // =====================================================
    void showPage(QWidget *page);

    // =====================================================
    // Backend Requests
    // =====================================================
    void makeLoginRequest(QString cardNum, QString pin);
    void updateBalanceDisplay();
    void updateTransactionsDisplay();
    void makeWithdrawalRequest(int amount, QString description);

    // =====================================================
    // Amount Helpers
    // =====================================================
    void selectAmount(int amount);
    QString formatAmount(int amount);
};

#endif // MAINWINDOW_H