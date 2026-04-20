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
// Qt Sound Effects
// =====================================================

#include <QSoundEffect>
#include <QTimer>


// =====================================================
// Qt Utility Classes
// =====================================================

#include <QButtonGroup>

// =====================================================
// Qt VIDEO
// =====================================================

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>

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

    void handleTransferOk();
    void handleTransferResponse(QNetworkReply *reply);
    void on_btnContrast_clicked();
    void onOkClicked();
    void onClearClicked();
    void onCancelClicked();

    // Donations
    void on_btnConfirmDonation_clicked();
    void handleDonationSelection();
    void handleDonationAmountSelection();
    void resetDonationSelection();
    void makeDonationRequest(int amount, const QString &charity);
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
    // LISÄÄ NÄMÄ TÄHÄN (Säilytypaikka tiedoille)
    // =====================================================
   // int id_account;
    QByteArray webToken;
    int activeAccountId;
    int debitAccountId;
    int creditAccountId;
    QString accountMode;
    bool isDualCard;
    void on_btnCreditDebit_clicked();


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
    void renderTransactionPage();
    QString formatTransactionRow(QJsonObject obj);
    void makeWithdrawalRequest(int amount, QString description);
    void lockCardRequest(QString cardNum);
    void handleLoginResponse(QNetworkReply *reply);

    QJsonArray allTransactions;
    int currentStartIndex = 0;
    static constexpr int PAGE_SIZE = 5;

    // =====================================================
    // Amount Helpers
    // =====================================================
    void selectAmount(int amount);
    QString formatAmount(int amount);


    // =====================================================
    // Withdraw / ATM messages
    // =====================================================
    QString msgInvalidAmount;
    QString msgWithdrawSuccess;
    QString msgNetError;
    QString msgAtmError;

    // =====================================================
    // Sound Management
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
    // Video Management
    // =====================================================
    QMediaPlayer *moreVideoPlayer;
    QAudioOutput *moreVideoAudio;
    QVideoWidget *moreVideoWidget;

    // =====================================================
    // Timer Management
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

   // void makeDonationRequest(int amount); to be removed??
    QString selectedCharity;
    int pendingDonationAmount = 0;

    // =====================================================
    // Styles Management
    // =====================================================

    void setupStyles();
    void applyCurrentStyle();
    QString loadStyleSheet(const QString &path);

    QString lightStyle;
    QString contrastStyle;
    bool contrastEnabled = false;

    // =====================================================
    // Card / Account Type Handling
    // =====================================================

    // Defines which account is currently selected
    enum AccountType {
        DebitAccount,
        CreditAccount
    };

    bool hasDebit = false;     // Does the card support debit?
    bool hasCredit = false;    // Does the card support credit?

    AccountType selectedAccountType = DebitAccount; // Default selection

    QString currentLanguage = "EN"; // Current UI language

    void toggleAccountType();        // Handles button click (switch Debit <-> Credit)
    void updateCreditDebitButton();  // Updates button text and state


};

#endif // MAINWINDOW_H
