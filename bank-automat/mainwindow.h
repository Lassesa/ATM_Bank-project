#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// =====================================================
// Qt Core / Widgets
// =====================================================

#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QKeyEvent>
#include <QTimer>

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


private slots:
    void on_btnContrast_clicked();
    void onOkClicked();
    void onClearClicked();
    void onCancelClicked();
    void on_btnConfirmDonation_clicked();

    void handleDonationSelection();
    void handleDonationAmountSelection();

    void on_btnConfirmTransfer_clicked();
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
    int pinAttempts = 0;

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
    void lockCardRequest(QString cardNum);

    // =====================================================
    // Amount Helpers
    // =====================================================
    void selectAmount(int amount);
    QString formatAmount(int amount);

    // =====================================================
    // Styles Management
    // =====================================================
    QString lightStyle;
    QString contrastStyle;
    bool contrastEnabled = false;

    void setupStyles();
    void applyCurrentStyle();

    // =====================================================
    // Timer Managment
    // =====================================================
    QTimer *pinTimer;
    QTimer *exitTimer;
    void resetToWelcome();
    QTimer *inactivityTimer; // 30s waiting time
    QTimer *autoLogoutTimer; // 15s extra time after the popup

    void resetInactivity();
    void showInactivityPage();

    // =====================================================
    // Donation
    // =====================================================

    void makeDonationRequest(int amount);
    QString selectedCharity;
    int pendingDonationAmount;

    QString defaultStyle;



};

#endif // MAINWINDOW_H
