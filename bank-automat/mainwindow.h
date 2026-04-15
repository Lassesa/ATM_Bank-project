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
// Qt Sound Effects
// =====================================================

#include <QSoundEffect>
#include <QTimer>


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

    void on_button_3green_OK_clicked();
    void handleTransferResponse(QNetworkReply *reply);
    void on_btnContrast_clicked();



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
    // LISÄÄ NÄMÄ TÄHÄN (Säilytypaikka tiedoille)
    // =====================================================
    int id_account;      // Tallentaa tilin ID:n
    QByteArray webToken; // Tallentaa JWT-tokenin


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
    void handleLoginResponse(QNetworkReply *reply);

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
    // Sound  Management
    // =====================================================

    QSoundEffect *keypadSound;
    QSoundEffect *okSound;
    QSoundEffect *cancelSound;
    QSoundEffect *clearSound;
    QSoundEffect *buttonSound;
    QSoundEffect *successSound;
    QSoundEffect *errorSound;
    QSoundEffect *withdrawSound;
    QSoundEffect *timeoutSound;

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

};

#endif // MAINWINDOW_H
