#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "translations.h"
#include "media.h"
// =====================================================
// Qt UI
// =====================================================

#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QDebug>
#include <QTimer>
#include <QSoundEffect>

#include <QFile>
#include <QTextStream>
#include <QtMath>

// Video Management
#include <QVBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>



// =====================================================
// Constructor / Destructor
// =====================================================

/*
 * Main window constructor.
 *
 * What this function does:
 * - builds the UI
 * - creates network and button group helpers
 * - configures input fields
 * - sets the default language
 * - shows the startup page
 * - starts the serial card reader
 * - connects all button signals
 *
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))
{
    ui->setupUi(this);

    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    clockTimer->start(1000);
    updateDateTime();

    // Set application window icon
    #ifdef Q_OS_MAC
        this->setWindowIcon(QIcon(":/resources/logo.icns"));
    #else
        this->setWindowIcon(QIcon(":/logo.png"));
    #endif

    // Setup for media - including Idle Video
    media = new Media(this);
    media->setupVideo(ui->videoContainer);
    media->setupIdleVideo(ui->idleVideoContainer);



    showIdlePage();

    // Create network manager for backend API calls
    networkManager = new QNetworkAccessManager(this);

    // Timers
    pinTimer = new QTimer(this);
    pinTimer->setSingleShot(true);
    connect(pinTimer, &QTimer::timeout, this, &MainWindow::resetToWelcome);
    exitTimer = new QTimer(this);
    exitTimer->setSingleShot(true);
    connect(exitTimer, &QTimer::timeout, this, &MainWindow::returnToIdleFromExit);

    inactivityTimer = new QTimer(this);
    inactivityTimer->setInterval(30000); // // 30 seconds
    connect(inactivityTimer, &QTimer::timeout, this, &MainWindow::showInactivityPage);


    autoLogoutTimer = new QTimer(this);
    autoLogoutTimer->setSingleShot(true);
    connect(autoLogoutTimer, &QTimer::timeout, this, &MainWindow::resetToWelcome);

    QList<QPushButton *> allButtons = this->findChildren<QPushButton *>();
    for(QPushButton* btn : allButtons) {
        btn->setFocusPolicy(Qt::NoFocus);
    }

    for (QPushButton *btn : allButtons) {
        // Connect every button click to the inactivity reset handler
        connect(btn, &QPushButton::clicked, this, &MainWindow::resetInactivity);
    }



    // Create button groups for donation selection
    donationOrgGroup = new QButtonGroup(this);
    donationAmountGroup = new QButtonGroup(this);

    donationOrgGroup->setExclusive(true);
    donationAmountGroup->setExclusive(true);

    // Make donation organization buttons checkable
    ui->btn_donation_choice_1->setCheckable(true);
    ui->btn_donation_choice_2->setCheckable(true);
    ui->btn_donation_choice_3->setCheckable(true);
    ui->btn_donation_choice_4->setCheckable(true);

    // Make donation amount buttons checkable
    ui->btn_amount_choice_1->setCheckable(true);
    ui->btn_amount_choice_2->setCheckable(true);
    ui->btn_amount_choice_3->setCheckable(true);
    ui->btn_amount_choice_4->setCheckable(true);

    // Add donation organization buttons to button group
    donationOrgGroup->addButton(ui->btn_donation_choice_1, 1);
    donationOrgGroup->addButton(ui->btn_donation_choice_2, 2);
    donationOrgGroup->addButton(ui->btn_donation_choice_3, 3);
    donationOrgGroup->addButton(ui->btn_donation_choice_4, 4);

    // Add donation amount buttons to button group
    donationAmountGroup->addButton(ui->btn_amount_choice_1, 5);
    donationAmountGroup->addButton(ui->btn_amount_choice_2, 10);
    donationAmountGroup->addButton(ui->btn_amount_choice_3, 20);
    donationAmountGroup->addButton(ui->btn_amount_choice_4, 50);

    // Configure PIN input field
    ui->pinInput->setCursor(Qt::BlankCursor);
    ui->pinInput->clear();
    ui->pinInput->setMaxLength(4);
    ui->pinInput->setEchoMode(QLineEdit::Password);

    // Configure amount input field
    ui->amountInput->setText("0 €");

    // Set initial keypad mode
    currentMode = PinMode;

    // Configure language buttons
    ui->btnLanguageEnglish->setCheckable(true);
    ui->btnLanguageFinnish->setCheckable(true);
    ui->btnLanguagePolish->setCheckable(true);
    ui->btnCreditDebit->setCheckable(true);

    ui->btnLanguageEnglish->setAutoExclusive(true);
    ui->btnLanguageFinnish->setAutoExclusive(true);
    ui->btnLanguagePolish->setAutoExclusive(true);



    // Set initial language
    setLanguage("EN");

    // Set initial Page Mode
    ui->btnContrast->setCheckable(true);
    ui->btnContrast->setText("CO");

    // DO NOT Show welcome page at startup --- TO BE LOOP
    // ui->display->setCurrentWidget(ui->page01_Welcome);

    // Start RFID / serial reader
    setupSerialReader();

    // Connect all UI signals
    connectSignals();

    // Style management
    setupStyles();
    applyCurrentStyle();
}

/*
 * Main window destructor.
 *
 * What this function does:
 * - releases the UI object
 */
MainWindow::~MainWindow()
{
    delete ui;
}

// =====================================================
// Signal Connections
// =====================================================

/*
 * Connects all button and UI signals to their actions.
 *
 * What this function does:
 * - connects language buttons
 * - connects keypad number buttons
 * - connects Clear / Cancel / OK buttons
 * - connects main menu buttons
 *
 * Note:
 * Contrast button connection has been removed on purpose.
 */
void MainWindow::connectSignals()
{
    // -----------------------------
    // Language buttons
    // -----------------------------
    connect(ui->btnLanguageEnglish, &QPushButton::clicked, this, [this]() {
        setLanguage("EN");
    });

    connect(ui->btnLanguagePolish, &QPushButton::clicked, this, [this]() {
        setLanguage("PL");
    });

    connect(ui->btnLanguageFinnish, &QPushButton::clicked, this, [this]() {
        setLanguage("FI");
    });

    // -----------------------------
    // DEBIT / CREDIT Button
    // -----------------------------
    connect(ui->btnCreditDebit, &QPushButton::clicked, this, &MainWindow::handleCreditDebitClick);
    /*connect(ui->btnCreditDebit, &QPushButton::clicked,
            this, &MainWindow::toggleAccountType);*/

    // -----------------------------
    // Keypad number buttons
    // -----------------------------

    // OLD VERSION - without sounds
    /*connect(ui->num_0, &QPushButton::clicked, this, [this]() { handleDigit("0"); });
    connect(ui->num_1, &QPushButton::clicked, this, [this]() { handleDigit("1"); });
    connect(ui->num_2, &QPushButton::clicked, this, [this]() { handleDigit("2"); });
    connect(ui->num_3, &QPushButton::clicked, this, [this]() { handleDigit("3"); });
    connect(ui->num_4, &QPushButton::clicked, this, [this]() { handleDigit("4"); });
    connect(ui->num_5, &QPushButton::clicked, this, [this]() { handleDigit("5"); });
    connect(ui->num_6, &QPushButton::clicked, this, [this]() { handleDigit("6"); });
    connect(ui->num_7, &QPushButton::clicked, this, [this]() { handleDigit("7"); });
    connect(ui->num_8, &QPushButton::clicked, this, [this]() { handleDigit("8"); });
    connect(ui->num_9, &QPushButton::clicked, this, [this]() { handleDigit("9"); });*/

    connect(ui->num_0, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("0");
        media->playKeypad();
    });

    connect(ui->num_1, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("1");
        media->playKeypad();
    });

    connect(ui->num_2, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("2");
        media->playKeypad();
    });

    connect(ui->num_3, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("3");
        media->playKeypad();
    });

    connect(ui->num_4, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("4");
        media->playKeypad();
    });

    connect(ui->num_5, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("5");
        media->playKeypad();
    });

    connect(ui->num_6, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("6");
        media->playKeypad();
    });

    connect(ui->num_7, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("7");
        media->playKeypad();
    });

    connect(ui->num_8, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("8");
        media->playKeypad();
    });

    connect(ui->num_9, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page00_Idle) {
            media->playKeypad();
            leaveIdleToWelcome();
            return;
        }
        handleDigit("9");
        media->playKeypad();
    });



    // -----------------------------
    // Clear button YELLOW
    // -----------------------------
    connect(ui->button_2yellow_CLEAR, &QPushButton::clicked, this, [this]() {

        media->playClear();

        if (ui->display->currentWidget() == ui->page00_Idle) {
            leaveIdleToWelcome();
            return;
        }

        if (ui->display->currentWidget() == ui->page02_Pin) {
            QString text = ui->pinInput->text();
            text.chop(1);
            ui->pinInput->setText(text);
        }
        else if (ui->display->currentWidget() == ui->page04_Withdraw) {
            QString text = ui->amountInput->text();
            text.remove("€");
            text = text.trimmed();

            if (!text.isEmpty()) {
                text.chop(1);
            }

            if (text.isEmpty()) {
                ui->amountInput->setText("0 €");
            } else {
                ui->amountInput->setText(text + " €");
            }
        }
        else if (ui->display->currentWidget() == ui->page06_Transfer) {
            if (ui->PhoneNumberInput_Transfer->hasFocus()) {
                QString text = ui->PhoneNumberInput_Transfer->text();
                text.chop(1);
                ui->PhoneNumberInput_Transfer->setText(text);
            }
            else if (ui->amountInput_Transfer->hasFocus()) {
                QString text = ui->amountInput_Transfer->text();
                text.chop(1);
                ui->amountInput_Transfer->setText(text);
            }
            else {
                QString text = ui->PhoneNumberInput_Transfer->text();
                text.chop(1);
                ui->PhoneNumberInput_Transfer->setText(text);
            }
        }
    });

    // -----------------------------
    // Cancel button RED
    // -----------------------------
    connect(ui->button_1red_CANCEL, &QPushButton::clicked, this, [this]() {

        media->playCancel();

        if (ui->display->currentWidget() == ui->page00_Idle) {
            leaveIdleToWelcome();
            return;
        }

        QWidget *current = ui->display->currentWidget();

        if (current == ui->page02_Pin || current == ui->page11_Time) {
            ui->pinInput->clear();
            ui->display->setCurrentWidget(ui->page08_Exit);
            exitTimer->start(5000);
        }
        else if (current == ui->page01_Welcome) {
            ui->pinInput->clear();
            showIdlePage();
        }
        else if (current == ui->page03_Main) {
            ui->display->setCurrentWidget(ui->page08_Exit);
            exitTimer->start(5000);
        }
        else if (current == ui->page04_Withdraw ||
                 current == ui->page05_Balance ||
                 current == ui->page09_Other) {
            ui->display->setCurrentWidget(ui->page03_Main);
        }
        else if (current == ui->page06_Transfer) {
            resetTransferForm();
            ui->display->setCurrentWidget(ui->page03_Main);
        }
        else if (current == ui->page07_Donation) {
            resetDonationSelection();
            ui->display->setCurrentWidget(ui->page03_Main);
        }
        else if (current == ui->page12_Accounts ||
                 current == ui->page13_Transactions) {
            ui->display->setCurrentWidget(ui->page05_Balance);
        }
    });

    // -----------------------------
    // OK button GREEN
    // -----------------------------
    connect(ui->button_3green_OK, &QPushButton::clicked, this, [this]() {

        media->playOk();

        if (ui->display->currentWidget() == ui->page00_Idle) {
            leaveIdleToWelcome();
            return;
        }

        if (ui->display->currentWidget() == ui->page01_Welcome) {

            if (ui->btnLanguageFinnish->isChecked()) setLanguage("FI");
            else if (ui->btnLanguagePolish->isChecked()) setLanguage("PL");
            else setLanguage("EN");

            ui->display->setCurrentWidget(ui->page02_Pin);
            ui->pinInput->clear();
            ui->pinInput->setFocus();
            pinTimer->start(10000);
            qDebug() << "PIN-ajastin käynnistetty (10s)";
            resetInactivity();
        }
        else if (ui->display->currentWidget() == ui->page02_Pin) {
            QString currentCard = ui->CardNumberDisplay->text().trimmed();
            QString currentPin = ui->pinInput->text().trimmed();
            pinTimer->stop();

            if (currentCard.isEmpty() || currentPin.isEmpty()) {
                qDebug() << "Error: card number or PIN is missing from UI.";
                return;
            }

            qDebug() << "Sending login request with card:" << currentCard << "and PIN:" << currentPin;
            makeLoginRequest(currentCard, currentPin);
        }
        else if (ui->display->currentWidget() == ui->page04_Withdraw) {
            qDebug() << "Yritetään nostoa tililtä:" << activeAccountId; // LISÄÄ TÄMÄ RIVI
            QString amountText = ui->amountInput->text();
            amountText.remove("€");
            amountText = amountText.trimmed();

            bool ok = false;
            int amount = amountText.toInt(&ok);

            if (ok && amount > 0) {
                makeWithdrawalRequest(amount, "ATM WITHDRAW");
            } else {
                ui->amountInput->setText("0 €");
            }
        }
        else if (ui->display->currentWidget() == ui->page06_Transfer) {
            handleTransferOk();
        }
        else if (ui->display->currentWidget() == ui->page07_Donation) {
            qDebug() << "OK painettu lahjoitussivulla";
            on_btnConfirmDonation_clicked();
        }
    });

    // -----------------------------
    // Main menu buttons
    // -----------------------------
    connect(ui->btn_main_choice_1, &QPushButton::clicked, this, [this]() {

        media->playButton();

        selectAmount(50);
        showPage(ui->page04_Withdraw);
    });

    connect(ui->btn_main_choice_2, &QPushButton::clicked, this, [this]() {

        media->playButton();

        selectAmount(100);
        showPage(ui->page04_Withdraw);
    });

    connect(ui->btn_main_choice_3, &QPushButton::clicked, this, [this]() {

        media->playButton();

        selectAmount(0);
        showPage(ui->page04_Withdraw);
    });

    connect(ui->btn_main_choice_4, &QPushButton::clicked, this, [this]() {

       media->playButton();

        updateBalanceDisplay();
        updateTransactionsDisplay();
        showPage(ui->page05_Balance);
    });

    connect(ui->btn_main_choice_5, &QPushButton::clicked, this, [this]() {

        media->playButton();

        resetTransferForm();
        showPage(ui->page06_Transfer);
    });

    connect(ui->btn_main_choice_6, &QPushButton::clicked, this, [this]() {

        media->playButton();

        showPage(ui->page07_Donation);
    });

    connect(ui->btn_main_choice_7, &QPushButton::clicked, this, [this]() {
        media->playButton();
        showPage(ui->page08_Exit);
        exitTimer->start(5000);
    });


    connect(ui->btn_main_choice_8, &QPushButton::clicked, this, [this]() {
        media->playButton();

        showPage(ui->page09_Other);
    });



// -----------------------------
// Balance menu buttons
// -----------------------------

    connect(ui->Balance_btn_choice_1, &QPushButton::clicked, this, [this]() {

        media->playButton();

        updateAccountsPage();
        showPage(ui->page12_Accounts);
    });

    connect(ui->Balance_btn_choice_2, &QPushButton::clicked, this, [this]() {

        media->playButton();

        showPage(ui->page13_Transactions);
    });

    // NEXT-nappi
    connect(ui->Transactions_btn_choice_next, &QPushButton::clicked, this, [this]() {
        media->playButton();

        if (currentStartIndex + PAGE_SIZE < allTransactions.size()) {
            currentStartIndex += PAGE_SIZE;
            renderTransactionPage();
        }
    });

    // PREVIOUS-nappi
    connect(ui->Transactions_btn_choice_previous, &QPushButton::clicked, this, [this]() {
        media->playButton();

        if (currentStartIndex - PAGE_SIZE >= 0) {
            currentStartIndex -= PAGE_SIZE;
            renderTransactionPage();
        }
    });

    // -----------------------------
    // Donation menu buttons
    // -----------------------------


    connect(ui->btn_donation_choice_1, &QPushButton::clicked, this, &MainWindow::handleDonationSelection);
    connect(ui->btn_donation_choice_2, &QPushButton::clicked, this, &MainWindow::handleDonationSelection);
    connect(ui->btn_donation_choice_3, &QPushButton::clicked, this, &MainWindow::handleDonationSelection);
    connect(ui->btn_donation_choice_4, &QPushButton::clicked, this, &MainWindow::handleDonationSelection);

    connect(ui->btn_amount_choice_1, &QPushButton::clicked, this, &MainWindow::handleDonationAmountSelection);
    connect(ui->btn_amount_choice_2, &QPushButton::clicked, this, &MainWindow::handleDonationAmountSelection);
    connect(ui->btn_amount_choice_3, &QPushButton::clicked, this, &MainWindow::handleDonationAmountSelection);
    connect(ui->btn_amount_choice_4, &QPushButton::clicked, this, &MainWindow::handleDonationAmountSelection);

    // -----------------------------
    // STOP VIDEO  buttons
    // -----------------------------

    connect(ui->btnIdleStart, &QPushButton::clicked, this, &MainWindow::leaveIdleToWelcome);


}




// =====================================================
// Language Handling
// =====================================================

/*
 * Updates visible UI texts according to the selected language.
 *
 * Supported languages:
 * - EN = English
 * - PL = Polish
 * - FI = Finnish
 *
 * What this function changes:
 * - welcome page texts
 * - transaction page labels
 * - main menu button texts
 * - donation option texts
 */
void MainWindow::setLanguage(const QString &lang)
{
    currentLanguage = lang;
    UiTexts texts = getTexts(lang);

    ui->btnLanguageEnglish->setChecked(lang == "EN");
    ui->btnLanguagePolish->setChecked(lang == "PL");
    ui->btnLanguageFinnish->setChecked(lang == "FI");

    ui->labelWelcome->setText(texts.welcomeTitle);
    ui->labelInstruction->setText(texts.welcomeInstruction);

    ui->labelWelcome_PIN->setText(texts.pinTitle);
    ui->labelInstruction_PIN->setText(texts.pinInstruction);

    ui->labelWelcome_Main->setText(texts.mainTitle);
    ui->labelInstruction_Main->setText(texts.mainInstruction);

    ui->labelWelcome_Withdraw->setText(texts.withdrawTitle);
    ui->labelInstruction_Withdraw->setText(texts.withdrawInstruction);

    ui->labelWelcome_Balance->setText(texts.balanceTitle);
    ui->labelInstruction_Balance->setText(texts.balanceInstruction);

    ui->labelWelcome_Transfer->setText(texts.transferTitle);
    ui->labelInstruction_Transfer->setText(texts.transferInstruction);

    ui->labelWelcome_Donation->setText(texts.donationTitle);
    ui->labelInstruction_Donation->setText(texts.donationInstruction);

    ui->labelWelcome_Exit->setText(texts.exitTitle);
    ui->labelInstruction_Exit->setText(texts.exitInstruction);

    ui->labelWelcome_Other->setText(texts.otherTitle);
    ui->labelInstruction_Other->setText(texts.otherInstruction);

    ui->btn_main_choice_1->setText(texts.mainChoice1);
    ui->btn_main_choice_2->setText(texts.mainChoice2);
    ui->btn_main_choice_3->setText(texts.mainChoice3);
    ui->btn_main_choice_4->setText(texts.mainChoice4);
    ui->btn_main_choice_5->setText(texts.mainChoice5);
    ui->btn_main_choice_6->setText(texts.mainChoice6);
    ui->btn_main_choice_7->setText(texts.mainChoice7);
    ui->btn_main_choice_8->setText(texts.mainChoice8);

    msgInvalidAmount = texts.msgInvalidAmount;
    msgWithdrawSuccess = texts.msgWithdrawSuccess;
    msgNetError = texts.msgNetError;
    msgAtmError = texts.msgAtmError;

    ui->Balance_TitleAccountSelect->setText(texts.balanceMainAccount);
    ui->Balance_TitleRecentTransactions->setText(texts.balanceRecentTransactions);
    ui->Balance_btn_choice_1->setText(texts.balanceOtherAccounts);
    ui->Balance_btn_choice_2->setText(texts.balanceMoreTransactions);

    ui->btn_donation_choice_1->setText(texts.donationChoice1);
    ui->btn_donation_choice_2->setText(texts.donationChoice2);
    ui->btn_donation_choice_3->setText(texts.donationChoice3);
    ui->btn_donation_choice_4->setText(texts.donationChoice4);

    ui->labelWelcome_Error->setText(texts.errorTitle);
    ui->labelInstruction_Error->setText(texts.errorInstruction);

    ui->labelWelcome_Time->setText(texts.timeTitle);
    ui->labelInstruction_Time->setText(texts.timeInstruction);

    ui->labelWelcome_Accounts->setText(texts.accountsTitle);
    ui->labelInstruction_Accounts->setText(texts.accountsInstruction);

    ui->labelWelcome_Transactions->setText(texts.transactionsTitle);
    ui->labelInstruction_Transactions->setText(texts.transactionsInstruction);

    ui->Accounts_MainAccount_Title->setText(texts.accountsMainTitle);
    ui->Accounts_Balance_Title->setText(texts.accountsBalanceTitle);
    ui->Accounts_Available_Title->setText(texts.accountsAvailableTitle);
    ui->Accounts_Limit_Title->setText(texts.accountsLimitTitle);
    ui->Accounts_Limit_Used_Title->setText(texts.accountsLimitUsedTitle);

    ui->Accounts_Credit_Title->setText(texts.accountsCreditTitle);
    ui->Accounts_CreditLimit_Title->setText(texts.accountsCreditLimitTitle);
    ui->Accounts_CreditUsed_Title->setText(texts.accountsCreditUsedTitle);
    ui->Accounts_AvailableCredit_Title->setText(texts.accountsAvailableCreditTitle);

    ui->Transactions_btn_choice_next->setText(texts.transactionsNextButton);
    ui->Transactions_btn_choice_previous->setText(texts.transactionsPreviousButton);

    ui->PhoneNumberInput_Transfer->setPlaceholderText(texts.transferPhonePlaceholder);
    ui->amountInput_Transfer->setPlaceholderText(texts.transferAmountPlaceholder);

    msgTransferSuccess = texts.msgTransferSuccess;
    msgTransferFailed = texts.msgTransferFailed;
    msgTransferMissingPhone = texts.msgTransferMissingPhone;
    msgTransferMissingAmount = texts.msgTransferMissingAmount;
    msgTransferInvalidAmount = texts.msgTransferInvalidAmount;

    updateCreditDebitButton();
    updateAccountsPage();


}

// =====================================================
// Input Handling
// =====================================================

/*
 * Handles one digit from the keypad or keyboard.
 *
 * What this function does:
 * - on PIN page: appends digit to PIN field
 * - on Withdraw page: appends digit to amount field
 * - on Transfer page: appends digit to focused input (phone or amount)
 */
void MainWindow::handleDigit(const QString &digit)
{
    if (ui->display->currentWidget() == ui->page02_Pin) {
        QString currentText = ui->pinInput->text();
        if (currentText.length() < 4) {
            ui->pinInput->setText(currentText + digit);
        }
    }
    else if (ui->display->currentWidget() == ui->page04_Withdraw) {
        QString currentText = ui->amountInput->text();
        currentText.remove("€");
        currentText = currentText.trimmed();

        if (currentText == "0") {
            currentText = "";
        }

        ui->amountInput->setText(currentText + digit + " €");
    }
    else if (ui->display->currentWidget() == ui->page06_Transfer) {

        // If phone number field is focused → append digit there
        if (ui->PhoneNumberInput_Transfer->hasFocus()) {
            ui->PhoneNumberInput_Transfer->setText(
                ui->PhoneNumberInput_Transfer->text() + digit
                );
        }

        // If amount field is focused → append digit there
        else if (ui->amountInput_Transfer->hasFocus()) {
            ui->amountInput_Transfer->setText(
                ui->amountInput_Transfer->text() + digit
                );
        }

        // If nothing is focused → default to phone number field
        else {
            ui->PhoneNumberInput_Transfer->setFocus();
            ui->PhoneNumberInput_Transfer->setText(
                ui->PhoneNumberInput_Transfer->text() + digit
                );
        }
    }
}

/*
 * Handles keyboard input for the main window.
 *
 * Supported keys:
 * - 0-9        -> input digit
 * - Backspace  -> remove last PIN digit
 * - Enter      -> press OK
 * - Escape     -> press Cancel
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {
        handleDigit(QString::number(event->key() - Qt::Key_0));
    }
    else if (event->key() == Qt::Key_Backspace) {
        if (ui->display->currentWidget() == ui->page02_Pin) {
            QString text = ui->pinInput->text();
            text.chop(1);
            ui->pinInput->setText(text);
        }
        else if (ui->display->currentWidget() == ui->page06_Transfer) {
            if (ui->PhoneNumberInput_Transfer->hasFocus()) {
                QString text = ui->PhoneNumberInput_Transfer->text();
                text.chop(1);
                ui->PhoneNumberInput_Transfer->setText(text);
            }
            else if (ui->amountInput_Transfer->hasFocus()) {
                QString text = ui->amountInput_Transfer->text();
                text.chop(1);
                ui->amountInput_Transfer->setText(text);
            }
        }
    }
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        ui->button_3green_OK->click();
    }
    else if (event->key() == Qt::Key_Escape) {
        ui->button_1red_CANCEL->click();
    }

    QMainWindow::keyPressEvent(event);
}

// =====================================================
// Serial Card Reader
// =====================================================

/*
 * Configures and opens the serial port used by the RFID reader.
 *
 * Important note:
 * The serial port name may be different on your machine and on Lasse's machine.
 * Update setPortName() when needed.
 */
void MainWindow::setupSerialReader()
{

    // Configure the serial port used by the RFID reader
    serial->setPortName("com5");

    // Your current serial port:
  //  serial->setPortName("/dev/tty.usbmodem146301");
   // serial->setPortName("COM3");

    // Example alternative if another machine uses a different port:
    // serial->setPortName("/dev/tty.usbmodemXXXXXX");


    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadOnly)) {
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::readCardData);
        qDebug() << "Serial port opened successfully.";
    } else {
        qDebug() << "Failed to open serial port:" << serial->errorString();
    }
}

/*
 * Reads incoming RFID card data from the serial port.
 *
 * What this function does:
 * - reads complete lines from the serial buffer
 * - ignores empty lines and prompt markers
 * - saves the scanned card UID
 * - shows the welcome page
 * - displays the scanned card number on screen
 */
void MainWindow::readCardData()
{
    static QString buffer;
    buffer += QString::fromUtf8(serial->readAll());

    while (buffer.contains('\n')) {
        int endIndex = buffer.indexOf('\n');
        QString line = buffer.left(endIndex).trimmed();
        buffer.remove(0, endIndex + 1);

        qDebug() << "Received line:" << line;

        if (line.isEmpty() || line == ">") {
            continue;
        }

        currentCardUid = line;

        UiTexts texts = getTexts(currentLanguage);

        ui->labelInstruction_PIN->setText(texts.msgPinCover);

        media->stopIdleVideo();
        ui->idleVideoContainer->hide();
        ui->display->setCurrentWidget(ui->page01_Welcome);

        ui->labelInstruction->setText(texts.msgCardDetected);

        ui->CardNumberDisplay->setText(currentCardUid);
        ui->CardNumberDisplay->update();

        qDebug() << "Scanned card:" << currentCardUid;
        qDebug() << "Displayed text:" << ui->CardNumberDisplay->text();
    }
}

// =====================================================
// Navigation / Helpers
// =====================================================

/*
 * Switches the stacked widget to the requested page.
 */
void MainWindow::showPage(QWidget *page)
{
    ui->display->setCurrentWidget(page);
}

/*
 * Stores the selected amount and opens the withdraw page.
 */
void MainWindow::selectAmount(int amount)
{
    selectedAmount = amount;
    ui->amountInput->setText(formatAmount(amount));
    showPage(ui->page04_Withdraw);
}

/*
 * Formats a numeric amount as a UI string.
 *
 * Example:
 * 50 -> "50 €"
 */
QString MainWindow::formatAmount(int amount)
{
    return QString::number(amount) + " €";
}

// =====================================================
// Backend / API
// =====================================================

/*
 * Sends a login request to the backend using card number and PIN.
 *
 * What this function does:
 * - posts login data as JSON
 * - reads the returned JWT token
 * - stores the account ID
 * - opens the main menu page on success
 */
void MainWindow::makeLoginRequest(QString cardNum, QString pin)
{
    QUrl url("http://localhost:3000/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["card_number"] = cardNum;
    json["card_pin"] = pin;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray responseData = reply->readAll();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument resDoc = QJsonDocument::fromJson(responseData);
            if (!resDoc.isNull() && resDoc.isObject()) {
                QJsonObject jsonObj = resDoc.object();

                // 1. Tallennetaan Tokenit
                sessionToken = jsonObj.value("token").toString();
                this->webToken = sessionToken.toUtf8();

                // 2. Selvitetään kortin tyyppi (tuki sekä "dual" että "debit+credit" merkkijonoille)
                QString cardType = jsonObj["card_type"].toString().toLower();
                isDualCard = (cardType == "dual" || cardType == "debit+credit");

                // 3. Asetetaan tili-ID:t huolellisesti
                if (isDualCard) {
                    ui->btnCreditDebit->setEnabled(true);
                    ui->btnCreditDebit->setText("DEBIT");

                    // Pakotetaan valittu tila enum-muuttujaan (jos käytössä)
                    selectedAccountType = DebitAccount;
                    accountMode = "debit";

                    // HAETAAN ID:T - kokeillaan useampaa nimeä siltä varalta, että bäkärissä on eri avaimet
                    if (jsonObj.contains("id_debit")) debitAccountId = jsonObj["id_debit"].toInt();
                    else if (jsonObj.contains("debit_id")) debitAccountId = jsonObj["debit_id"].toInt();
                    else debitAccountId = 6; // Viimeinen hätävara, jos bäkärin vaste puutteellinen

                    if (jsonObj.contains("id_credit")) creditAccountId = jsonObj["id_credit"].toInt();
                    else if (jsonObj.contains("credit_id")) creditAccountId = jsonObj["credit_id"].toInt();
                    else creditAccountId = 14; // Viimeinen hätävara

                    // Asetetaan oletukseksi debit kirjautumisen jälkeen
                    this->activeAccountId = debitAccountId;
                    this->accountId = debitAccountId;
                }
                else {
                    ui->btnCreditDebit->setEnabled(false);
                    ui->btnCreditDebit->setText(cardType.toUpper());
                    selectedAccountType = DebitAccount; // Yksittäiskortilla aina oletus

                    // Haetaan yksittäinen ID
                    int id = 0;
                    if (jsonObj.contains("idaccount")) id = jsonObj["idaccount"].toInt();
                    else if (jsonObj.contains("id_account")) id = jsonObj["id_account"].toInt();
                    else if (jsonObj.contains("accountId")) id = jsonObj["accountId"].toInt();

                    this->activeAccountId = id;
                    this->accountId = id;
                    this->debitAccountId = id; // Tallennetaan myös tänne varmuuden vuoksi
                }

                // DEBUG-TULOSTEET - Näet heti konsolista, saatiinko ID:t talteen
                qDebug() << "-----------------------------------------";
                qDebug() << "LOGIN ONNISTUI!";
                qDebug() << "Kortin tyyppi:" << cardType;
                qDebug() << "Debit ID:" << this->debitAccountId;
                qDebug() << "Credit ID:" << this->creditAccountId;
                qDebug() << "Aktiivinen tili nyt:" << this->activeAccountId;
                qDebug() << "-----------------------------------------";

                // Käyttöliittymän päivitys
                updateCreditDebitButton();
                updateBalanceDisplay();
                updateTransactionsDisplay();
                updateAccountsPage();

                // Ajastimet ja äänet
                inactivityTimer->start(30000);
                media->playSuccess();

                // Siirtyminen pääsivulle
                ui->display->setCurrentWidget(ui->page03_Main);
                ui->pinInput->clear();
            }
        }
        else if (statusCode == 401) {
            UiTexts texts = getTexts(currentLanguage);

            QJsonObject resObj = QJsonDocument::fromJson(responseData).object();
            int remaining = resObj.value("remaining").toInt();

            ui->labelInstruction_PIN->setText(texts.msgWrongPinRemaining.arg(remaining));
            ui->pinInput->clear();

            media->playError();
        }
        else {
            qDebug() << "Login error:" << reply->errorString();
            media->playError();
        }

        reply->deleteLater();
    });
}


/*
 * Requests the current account balance from the backend
 * and updates the balance label on screen.
 */
void MainWindow::updateBalanceDisplay()
{
    qDebug() << "===== BALANCE REQUEST =====";
    qDebug() << "activeAccountId used:" << activeAccountId;

    if (activeAccountId == 0) {
        qDebug() << "Virhe: activeAccountId on 0, ei voida hakea saldoa.";
        return;
    }

    QUrl url("http://localhost:3000/account/balance/me/" + QString::number(activeAccountId));
    qDebug() << "URL:" << url.toString();

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + this->webToken);

    qDebug() << "Lähetetään saldokysely osoitteeseen:" << url.toString();

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            qDebug() << "BALANCE RESPONSE:" << responseData;

            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject obj = jsonDoc.object();

            if (obj.contains("account_balance")) {
                double balance = obj.value("account_balance").toVariant().toDouble();
                ui->Balance_Amount->setText(QString::number(balance, 'f', 2) + " €");
                qDebug() << "Saldo päivitetty ID:lle" << activeAccountId << ":" << balance;
            }
        } else {
            qDebug() << "Saldon haku epäonnistui:" << reply->errorString();
            qDebug() << "Backend virhe:" << reply->readAll();
            UiTexts texts = getTexts(currentLanguage);
            ui->Balance_Amount->setText(texts.msgGenericError);
        }
        reply->deleteLater();
    });
}

/*
 * Requests recent transactions from the backend
 * and displays the latest entries in the balance page list.
 */
void MainWindow::updateTransactionsDisplay()
{
    UiTexts texts = getTexts(currentLanguage);

    if (activeAccountId <= 0) {
        qDebug() << "Transactions fetch skipped: invalid activeAccountId:" << activeAccountId;
        ui->Balance_ListRecentTransactions->clear();
        ui->Transactions_List_Full->clear();
        ui->Balance_ListRecentTransactions->addItem(texts.msgNoAccountSelected);
        return;
    }

    QString urlStr = QString("http://localhost:3000/transaction/me/%1").arg(activeAccountId);
    QUrl url(urlStr);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + sessionToken.toUtf8());

    qDebug() << "Haetaan tilitapahtumat osoitteesta:" << urlStr;

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        UiTexts texts = getTexts(currentLanguage);

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            allTransactions = QJsonDocument::fromJson(responseData).array();
            currentStartIndex = 0;

            ui->Balance_ListRecentTransactions->clear();
            if (allTransactions.isEmpty()) {
                ui->Balance_ListRecentTransactions->addItem(texts.msgNoTransactions);
            } else {
                int countShort = qMin(allTransactions.size(), 5);
                for (int i = 0; i < countShort; ++i) {
                    QJsonObject obj = allTransactions.at(i).toObject();
                    ui->Balance_ListRecentTransactions->addItem(formatTransactionRow(obj));
                }
            }
            renderTransactionPage();

        } else {
            qDebug() << "Transaction fetch failed:" << reply->errorString();
            ui->Balance_ListRecentTransactions->clear();
            ui->Balance_ListRecentTransactions->addItem(texts.msgTransactionsLoadError);
        }
        reply->deleteLater();
    });
}

void MainWindow::renderTransactionPage() {
    ui->Transactions_List_Full->clear();

    // Lasketaan kuinka monta näytetään (max 5 tai mitä on jäljellä)
    int count = qMin(allTransactions.size() - currentStartIndex, PAGE_SIZE);

    for (int i = 0; i < count; ++i) {
        QJsonObject obj = allTransactions.at(currentStartIndex + i).toObject();
        ui->Transactions_List_Full->addItem(formatTransactionRow(obj));
    }
    int pageNum = (currentStartIndex / 5) + 1;

    // Lasketaan kokonaissivunmäärä:
    // ceil(koko / 5.0)
    int totalPages = qCeil(allTransactions.size() / 5.0);
    if (totalPages == 0) totalPages = 1; // Estetään "Sivu 1 / 0"

    ui->Transactions_lbl_PageNumber->setText(QString(" %1 / %2").arg(pageNum).arg(totalPages));
}


QString MainWindow::formatTransactionRow(QJsonObject obj) {
    double amount = obj.value("transaction_amount").toVariant().toDouble();
    QString dateStr = obj.value("transaction_date").toString().left(10);
    QString description = obj.value("transaction_description").toString();

    UiTexts texts = getTexts(currentLanguage);
    if (description.isEmpty()) {
        description = texts.defaultTransactionDescription;
    }

    return QString("%1  %2  %3 €")
        .arg(dateStr)
        .arg(description.left(50).trimmed().toUpper())
        .arg(QString::number(amount, 'f', 2));
}

/*
 * Sends a withdrawal request to the backend.
 *
 * On success:
 * - refreshes balance
 * - refreshes recent transactions
 * - returns to the main menu page
 */
void MainWindow::makeWithdrawalRequest(int amount, QString description)
{
    QString originalText = ui->labelInstruction_Withdraw->text();

    // 1. Summan tarkistus (10 jaollisuus)
    if (amount <= 0 || amount % 10 != 0) {
        ui->labelInstruction_Withdraw->setText(msgInvalidAmount);
        ui->labelInstruction_Withdraw->setStyleSheet("color: red; font-weight: bold;");
        QTimer::singleShot(3000, [this, originalText]() {
            ui->labelInstruction_Withdraw->setText(originalText);
            ui->labelInstruction_Withdraw->setStyleSheet("");
        });
        return;
    }

    // 2. DYNAAMINEN VALINTA (Käytetään accountMode-muuttujaa, joka on joko "debit" tai "credit")
    QString path;
    int currentId;

    if (this->accountMode == "credit") {
        path = "/transaction/credit-withdrawal";
        currentId = this->creditAccountId; // ID 14
    } else {
        path = "/transaction/atm-withdrawal";
        currentId = this->debitAccountId;  // ID 6
    }

    QUrl url("http://localhost:3000" + path);

    // Debug-varmistus konsoliin ennen lähetystälo
    qDebug() << "--- NOSTO-LÄHETYS ALKAA ---";
    qDebug() << "Tila:" << accountMode;
    qDebug() << "URL:" << url.toString();
    qDebug() << "Käytettävä ID:" << currentId;

    // 3. Verkkopyynnön valmistelu
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + sessionToken.toUtf8());

    // 4. JSON-datan rakentaminen
    QJsonObject json;
    json["idaccount"] = currentId;
    json["amount"] = amount;
    json["description"] = description;
    json["account_type"] = accountMode;

    qDebug() << "DATA:" << QJsonDocument(json).toJson(QJsonDocument::Compact);

    // 5. Lähetys
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());

    // 6. Vastauksen käsittely
    connect(reply, &QNetworkReply::finished, this, [this, reply, originalText]() {
        if (reply->error() == QNetworkReply::NoError) {
            media->playWithdraw();

            ui->labelInstruction_Withdraw->setText(msgWithdrawSuccess);
            ui->labelInstruction_Withdraw->setStyleSheet("color: green; font-weight: bold;");

            QTimer::singleShot(2000, [this, originalText]() {
                updateBalanceDisplay();
                updateTransactionsDisplay();
                ui->labelInstruction_Withdraw->setText(originalText);
                ui->labelInstruction_Withdraw->setStyleSheet("");
                ui->display->setCurrentWidget(ui->page03_Main);
            });
        } else {
            // Virhetilanne (esim. katteen puute tai bäkärin esto)
            QByteArray responseData = reply->readAll();
            qDebug() << "BACKEND VIRHE:" << responseData;

            // Jos bäkäriltä tulee viesti, näytetään se, muuten yleinen virheviesti
            ui->labelInstruction_Withdraw->setText(msgAtmError);
            ui->labelInstruction_Withdraw->setStyleSheet("color: red; font-weight: bold;");

            media->playError();

            QTimer::singleShot(4000, [this, originalText]() {
                ui->labelInstruction_Withdraw->setText(originalText);
                ui->labelInstruction_Withdraw->setStyleSheet("");
            });
        }
        reply->deleteLater();
    });
}


QString MainWindow::formatMoney(double amount) const
{
    return QString::number(amount, 'f', 2) + " €";
}

void MainWindow::setAmountLabel(QLabel *label, double amount)
{
    if (!label) return;

    label->setText(formatMoney(amount));

    // Optional: mark negative values
    if (amount < 0) {
        label->setProperty("negative", true);
    } else {
        label->setProperty("negative", false);
    }

    label->style()->unpolish(label);
    label->style()->polish(label);
    label->update();
}

void MainWindow::clearAccountsPage()
{
    ui->Accounts_Balance->setText("-- €");
    ui->Accounts_Available->setText("-- €");
    ui->Accounts_Limit->setText("-- €");
    ui->Accounts_Limit_Used->setText("-- €");

    ui->Accounts_CreditLimit->setText("-- €");
    ui->Accounts_CreditUsed->setText("-- €");
    ui->Accounts_AvailableCredit->setText("-- €");
}

void MainWindow::updateAccountsPage()
{
    clearAccountsPage();

    if (sessionToken.isEmpty()) {
        qDebug() << "Accounts page update skipped: no session token.";
        return;
    }

    // -----------------------------
    // 1. Main / active debit account
    // -----------------------------
    if (debitAccountId > 0) {
        QUrl debitUrl("http://localhost:3000/account/balance/me/" + QString::number(debitAccountId));
        QNetworkRequest debitRequest(debitUrl);
        debitRequest.setRawHeader("Authorization", "Bearer " + sessionToken.toUtf8());

        QNetworkReply *debitReply = networkManager->get(debitRequest);

        connect(debitReply, &QNetworkReply::finished, this, [this, debitReply]() {
            QByteArray responseData = debitReply->readAll();
            qDebug() << "MAIN ACCOUNT JSON:" << responseData;

            if (debitReply->error() == QNetworkReply::NoError) {
                QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
                QJsonObject obj = jsonDoc.object();

                double balance = obj.value("account_balance").toVariant().toDouble();
                double limit = obj.value("account_limit").toVariant().toDouble();

                double usedOverdraft = qMax(0.0, -balance);
                double availableFunds = balance + limit;

                setAmountLabel(ui->Accounts_Balance, balance);
                setAmountLabel(ui->Accounts_Available, availableFunds);
                setAmountLabel(ui->Accounts_Limit, limit);
                setAmountLabel(ui->Accounts_Limit_Used, usedOverdraft);

                qDebug() << "Main account data updated.";
            } else {
                qDebug() << "Failed to load main account data:" << debitReply->errorString();
                qDebug() << "Backend response:" << responseData;
            }

            debitReply->deleteLater();
        });
    }

    // -----------------------------
    // 2. Credit account
    // -----------------------------
    if (creditAccountId > 0) {
        QUrl creditUrl("http://localhost:3000/account/balance/me/" + QString::number(creditAccountId));
        QNetworkRequest creditRequest(creditUrl);
        creditRequest.setRawHeader("Authorization", "Bearer " + sessionToken.toUtf8());

        QNetworkReply *creditReply = networkManager->get(creditRequest);

        connect(creditReply, &QNetworkReply::finished, this, [this, creditReply]() {
            QByteArray responseData = creditReply->readAll();
            qDebug() << "CREDIT ACCOUNT JSON:" << responseData;

            if (creditReply->error() == QNetworkReply::NoError) {
                QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
                QJsonObject obj = jsonDoc.object();

                // Adjust these names to match backend response
                double balance = obj.value("account_balance").toVariant().toDouble();
                double limit = obj.value("account_limit").toVariant().toDouble();

                // Credit logic based on current DB structure
                double usedCredit = qAbs(balance);
                double availableCredit = limit - usedCredit;

                setAmountLabel(ui->Accounts_CreditLimit, limit);
                setAmountLabel(ui->Accounts_CreditUsed, usedCredit);
                setAmountLabel(ui->Accounts_AvailableCredit, availableCredit);

                qDebug() << "Credit account data updated.";
            } else {
                qDebug() << "Failed to load credit account data:" << creditReply->errorString();
                qDebug() << "Backend response:" << responseData;
            }

            creditReply->deleteLater();
        });
    }
}



void MainWindow::resetToWelcome()
{
    qDebug() << "Returning to the welcome screen...";

    // Stop all timers
    pinTimer->stop();
    exitTimer->stop();
    inactivityTimer->stop();
    autoLogoutTimer->stop();

    // Clear session data
    ui->pinInput->clear();
    ui->pinInput->setEnabled(true);
    ui->CardNumberDisplay->clear();
    currentCardUid = "";
    sessionToken = "";
    accountId = 0;
    activeAccountId = 0;
    debitAccountId = 0;
    creditAccountId = 0;
    accountMode = "debit";
    clearAccountsPage();
    resetDonationSelection();
    resetTransferForm();
    hasDebit = false;
    hasCredit = false;
    selectedAccountType = DebitAccount;
    updateCreditDebitButton();

    // Restore the default PIN instruction text
    UiTexts texts = getTexts(currentLanguage);
    ui->labelInstruction_PIN->setText(texts.msgPinCover);

    // Return to the welcome page ->> idle video
    // ui->display->setCurrentWidget(ui->page01_Welcome);
    ui->idleVideoContainer->show();
    showIdlePage();
}

void MainWindow::resetInactivity()
{
    if (ui->display->currentWidget() == ui->page11_Time) {
        autoLogoutTimer->stop();
        ui->display->setCurrentWidget(ui->page03_Main);
        inactivityTimer->start(30000);
        qDebug() << "Palattu aikakatkaisusta napin painalluksella";
        return;
    }

    if (inactivityTimer->isActive()) {
        inactivityTimer->start(30000);
    }
}

void MainWindow::showInactivityPage()
{
    inactivityTimer->stop();


    ui->display->setCurrentWidget(ui->page11_Time);

    // Start the final countdown before automatic logout
    autoLogoutTimer->start(15000);
}

void MainWindow::lockCardRequest(QString cardNum)
{
    QUrl url("http://localhost:3000/card/lock");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["card_number"] = cardNum;

    networkManager->put(request, QJsonDocument(json).toJson());

    UiTexts texts = getTexts(currentLanguage);
    ui->labelInstruction_PIN->setText(texts.msgCardLocked);
    ui->pinInput->setEnabled(false);

    QTimer::singleShot(5000, this, &MainWindow::resetToWelcome);
}



/*
 * DONATION FUNCTIONS
 *
 */

void MainWindow::handleDonationSelection()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    selectedCharity = button->text();
    qDebug() << "Valittu kohde:" << selectedCharity;
}

void MainWindow::handleDonationAmountSelection()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString val = button->text();
    val.remove("€");
    val = val.trimmed();

    pendingDonationAmount = val.toInt();
    qDebug() << "Valittu summa:" << pendingDonationAmount;
}

void MainWindow::on_btnConfirmDonation_clicked()
{
    qDebug() << "DONATION CLICK WORKS";
    qDebug() << "Donation attempt:";
    qDebug() << "activeAccountId =" << activeAccountId;
    qDebug() << "selectedCharity =" << selectedCharity;
    qDebug() << "pendingDonationAmount =" << pendingDonationAmount;

    if (pendingDonationAmount <= 0 || selectedCharity.isEmpty()) {
        qDebug() << "Valitse kohde ja summa ensin!";
        return;
    }

    if (activeAccountId <= 0) {
        qDebug() << "Donation blocked: invalid activeAccountId:" << activeAccountId;

        UiTexts texts = getTexts(currentLanguage);
        ui->labelInstruction_Donation->setText(texts.msgAccountDataFailed);

        return;
    }
    makeDonationRequest(pendingDonationAmount, selectedCharity);
}

void MainWindow::resetDonationSelection()
{
    selectedCharity.clear();
    pendingDonationAmount = 0;

    donationOrgGroup->setExclusive(false);
    ui->btn_donation_choice_1->setChecked(false);
    ui->btn_donation_choice_2->setChecked(false);
    ui->btn_donation_choice_3->setChecked(false);
    ui->btn_donation_choice_4->setChecked(false);
    donationOrgGroup->setExclusive(true);

    donationAmountGroup->setExclusive(false);
    ui->btn_amount_choice_1->setChecked(false);
    ui->btn_amount_choice_2->setChecked(false);
    ui->btn_amount_choice_3->setChecked(false);
    ui->btn_amount_choice_4->setChecked(false);
    donationAmountGroup->setExclusive(true);
}

void MainWindow::makeDonationRequest(int amount, const QString &charity)
{
    QString originalText = ui->labelInstruction_Donation->text();

    if (amount <= 0) {
        UiTexts texts = getTexts(currentLanguage);
        ui->labelInstruction_Donation->setText(texts.msgDonationInvalidAmount);

        ui->labelInstruction_Donation->setStyleSheet("color: red; font-weight: bold;");

        QTimer::singleShot(3000, [this, originalText]() {
            ui->labelInstruction_Donation->setText(originalText);
            ui->labelInstruction_Donation->setStyleSheet("");
        });
        return;
    }

    QUrl url("http://localhost:3000/donation");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + sessionToken.toUtf8());

    QJsonObject json;
    json["idaccount"] = activeAccountId;
    json["amount"] = amount;
    json["description"] = "DONATION: " + charity;
    json["account_type"] = (selectedAccountType == DebitAccount) ? "debit" : "credit";

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, originalText]() {

        if (reply->error() == QNetworkReply::NoError) {

            media->playSuccess();

            UiTexts texts = getTexts(currentLanguage);
            ui->labelInstruction_Donation->setText(texts.msgDonationSuccess);

            ui->labelInstruction_Donation->setStyleSheet("color: green; font-weight: bold;");

            QTimer::singleShot(2000, [this, originalText]() {
                updateBalanceDisplay();
                updateTransactionsDisplay();
                resetDonationSelection();
                ui->labelInstruction_Donation->setText(originalText);
                ui->labelInstruction_Donation->setStyleSheet("");
                ui->display->setCurrentWidget(ui->page03_Main);
            });

        } else {

            qDebug() << "Donation backend error:" << reply->readAll();

            media->playError();

            UiTexts texts = getTexts(currentLanguage);
            ui->labelInstruction_Donation->setText(texts.msgDonationFailed);

            ui->labelInstruction_Donation->setStyleSheet("color: red; font-weight: bold;");

            QTimer::singleShot(4000, [this, originalText]() {
                ui->labelInstruction_Donation->setText(originalText);
                ui->labelInstruction_Donation->setStyleSheet("");
            });
        }

        reply->deleteLater();
    });
}

/*
 * Managing STYLES
 *
 */

void MainWindow::setupStyles()
{
    lightStyle = loadStyleSheet(":/styles/light.qss");
    contrastStyle = loadStyleSheet(":/styles/contrast.qss");

    qDebug() << "lightStyle path: :/styles/light.qss";
    qDebug() << "contrastStyle path: :/styles/contrast.qss";
    qDebug() << "lightStyle length:" << lightStyle.length();
    qDebug() << "contrastStyle length:" << contrastStyle.length();
}


void MainWindow::applyCurrentStyle()
{

    if (contrastEnabled) {
        this->setStyleSheet(contrastStyle);
        ui->btnContrast->setChecked(true);
    } else {
        this->setStyleSheet(lightStyle);
        ui->btnContrast->setChecked(false);
    }
}

/*
 * Contrast Button
 *
 */
void MainWindow::on_btnContrast_clicked()
{
    contrastEnabled = !contrastEnabled;
    applyCurrentStyle();
}

// Nämä poistavat "undefined reference" -virheet
void MainWindow::onOkClicked() {
    qDebug() << "OK painettu";
}

void MainWindow::onClearClicked() {
    qDebug() << "Clear painettu";
}

void MainWindow::onCancelClicked() {
    qDebug() << "Cancel painettu";
}

void MainWindow::on_btnConfirmTransfer_clicked() {
    qDebug() << "Siirto vahvistettu";
}


QString MainWindow::loadStyleSheet(const QString &path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot load stylesheet:" << path;
        return "";
    }

    QTextStream stream(&file);
    return stream.readAll();
}


/*
 * Toggles between Debit and Credit account (only for DUO cards).
 *
 * What this function does:
 * - checks if the card supports both debit and credit
 * - switches the selected account type
 * - updates the button text accordingly
 */
void MainWindow::toggleAccountType()
{
    qDebug() << "Nappia painettu. Tila ennen:" << accountMode << "isDual:" << isDualCard;

    if (!isDualCard) {
        qDebug() << "Ei dual-kortti, keskeytetään vaihto.";
        return;
    }

    if (accountMode == "debit") {
        accountMode = "credit";
        activeAccountId = creditAccountId;
    } else {
        accountMode = "debit";
        activeAccountId = debitAccountId;
    }

    qDebug() << "Tila nyt:" << accountMode << "ID nyt:" << activeAccountId;

    // Kutsutaan päivitysfunktioita
    updateCreditDebitButton();
    updateBalanceDisplay();
    updateTransactionsDisplay();
    updateAccountsPage();
}
/*
 * Updates the Credit/Debit button text and enabled state.
 *
 * What this function does:
 * - sets correct label based on:
 *   - selected account type (Debit / Credit)
 *   - current language
 * - enables/disables button depending on card capabilities
 */
void MainWindow::updateCreditDebitButton()
{
    UiTexts texts = getTexts(currentLanguage);

    if (isDualCard) {
        ui->btnCreditDebit->setEnabled(true);

        if (accountMode == "debit")
            ui->btnCreditDebit->setText(texts.debitCardLabel);
        else
            ui->btnCreditDebit->setText(texts.creditCardLabel);
    } else {
        ui->btnCreditDebit->setEnabled(false);

        if (accountMode == "credit")
            ui->btnCreditDebit->setText(texts.creditCardLabel);
        else
            ui->btnCreditDebit->setText(texts.debitCardLabel);
    }
}

/*
 * Contrast Button
 *
 */

//siirto

/*
 * Handles OK button on Transfer page.
 *
 * Flow:
 * - if phone number is missing, show error
 * - if amount is missing, move focus or show error
 * - if both values are valid, send transfer request
 */
void MainWindow::handleTransferOk()
{
    QString originalText = ui->labelInstruction_Transfer->text();

    QString phone = ui->PhoneNumberInput_Transfer->text().trimmed();
    QString amountStr = ui->amountInput_Transfer->text().trimmed();

    // If phone is missing, show error and keep focus on phone field
    if (phone.isEmpty()) {
        ui->PhoneNumberInput_Transfer->setFocus();
        ui->labelInstruction_Transfer->setText(msgTransferMissingPhone);
        ui->labelInstruction_Transfer->setStyleSheet("color: red; font-weight: bold;");

        media->playError();

        QTimer::singleShot(3000, [this, originalText]() {
            ui->labelInstruction_Transfer->setText(originalText);
            ui->labelInstruction_Transfer->setStyleSheet("");
        });
        return;
    }

    // If phone exists but amount is empty, move to amount field
    if (!phone.isEmpty() && amountStr.isEmpty()) {
        ui->amountInput_Transfer->setFocus();
        return;
    }

    bool ok = false;
    int amount = amountStr.toInt(&ok);

    // If amount is invalid, show error
    if (!ok || amount <= 0) {
        ui->amountInput_Transfer->setFocus();
        ui->labelInstruction_Transfer->setText(msgTransferInvalidAmount);
        ui->labelInstruction_Transfer->setStyleSheet("color: red; font-weight: bold;");

        media->playError();

        QTimer::singleShot(3000, [this, originalText]() {
            ui->labelInstruction_Transfer->setText(originalText);
            ui->labelInstruction_Transfer->setStyleSheet("");
        });
        return;
    }

    QJsonObject json;
    json["source_id"] = this->activeAccountId;
    json["phonenumber"] = phone;
    json["amount"] = amount;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QUrl url("http://localhost:3000/transaction/transfer");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + webToken));

    QNetworkReply *reply = networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [this, reply, originalText]() {
        handleTransferResponse(reply, originalText);
    });

    media->playButton();
}


/*
 * Handles backend response for transfer request.
 *
 * On success:
 * - shows success message
 * - refreshes balance and transactions
 * - clears transfer form
 * - returns to main menu
 *
 * On failure:
 * - shows error message
 * - keeps user on transfer page
 */
void MainWindow::handleTransferResponse(QNetworkReply *reply, const QString &originalText)
{
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Transfer completed successfully!";
        qDebug() << "Backend response:" << reply->readAll();

        media->playSuccess();

        ui->labelInstruction_Transfer->setText(msgTransferSuccess);
        ui->labelInstruction_Transfer->setStyleSheet("color: green; font-weight: bold;");

        QTimer::singleShot(2000, [this, originalText]() {
            updateBalanceDisplay();
            updateTransactionsDisplay();
            resetTransferForm();
            ui->labelInstruction_Transfer->setText(originalText);
            ui->labelInstruction_Transfer->setStyleSheet("");
            ui->display->setCurrentWidget(ui->page03_Main);
        });
    }
    else {
        QByteArray responseData = reply->readAll();
        qDebug() << "Transfer failed:" << responseData;

        media->playError();

        ui->labelInstruction_Transfer->setText(msgTransferFailed);
        ui->labelInstruction_Transfer->setStyleSheet("color: red; font-weight: bold;");

        QTimer::singleShot(4000, [this, originalText]() {
            ui->labelInstruction_Transfer->setText(originalText);
            ui->labelInstruction_Transfer->setStyleSheet("");
        });
    }

    reply->deleteLater();
}


/*void MainWindow::on_btnContrast_clicked()
{

}*/

// mainwindow.cpp

void MainWindow::handleLoginResponse(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response_data = reply->readAll();
        QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
        QJsonObject json_obj = json_doc.object();

        qDebug() << "Login JSON:" << json_doc.toJson();

        // DEBUG: Tulosta koko vastaus nähdäksesi mitä sieltä tulee
        qDebug() << "Backend login response:" << json_doc.toJson();

        this->webToken = json_obj.value("token").toString().toUtf8();

        QString cardType = json_obj["card_type"].toString();
        isDualCard = (cardType == "dual" || cardType == "admin");

        if (isDualCard) {
            this->debitAccountId = json_obj["id_debit"].toInt();
            this->creditAccountId = json_obj["id_credit"].toInt();
            this->activeAccountId = this->debitAccountId;
            qDebug() << "Kortti tunnistettu DUAL-kortiksi. Debit ID:" << debitAccountId << "Credit ID:" << creditAccountId;
        } else {
            this->activeAccountId = json_obj["idaccount"].toInt();
        }

        qDebug() << "Asetettu activeAccountId:" << activeAccountId;

        ui->display->setCurrentIndex(2);
        updateCreditDebitButton();
        updateBalanceDisplay(); // Nyt tämän pitäisi lähteä osoitteeseen /me/6
    }
    reply->deleteLater();
}

void MainWindow::handleCreditDebitClick()
{
    qDebug() << "===== CLICK CREDIT/DEBIT =====";
    qDebug() << "BEFORE -> mode:" << accountMode
             << "activeAccountId:" << activeAccountId
             << "debitId:" << debitAccountId
             << "creditId:" << creditAccountId;

    if (!isDualCard) {
        qDebug() << "Ei dual-kortti, ei tehdä mitään.";
        return;
    }

    if (accountMode == "debit") {
        accountMode = "credit";
        activeAccountId = creditAccountId;
    } else {
        accountMode = "debit";
        activeAccountId = debitAccountId;
    }

    qDebug() << "AFTER -> mode:" << accountMode
             << "activeAccountId:" << activeAccountId;

    updateCreditDebitButton();
    updateBalanceDisplay();
    updateTransactionsDisplay();
    updateAccountsPage();
}


/*
 * Clears transfer form fields and restores default focus.
 */
void MainWindow::resetTransferForm()
{
    ui->PhoneNumberInput_Transfer->clear();
    ui->amountInput_Transfer->clear();
    ui->PhoneNumberInput_Transfer->setFocus();
}

void MainWindow::updateDateTime()
{

    QString dateTimeString = QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");

    ui->labelDateTime->setText(dateTimeString);
}

/*
 * VIDEO MAINOS in LOOP
 */
void MainWindow::showIdlePage()
{
    if (ui->display->currentWidget() != ui->page00_Idle) {
        ui->display->setCurrentWidget(ui->page00_Idle);
    }

    ui->idleVideoContainer->show();
    media->playIdleVideo();
}


void MainWindow::leaveIdleToWelcome()
{
    media->stopIdleVideo();
    ui->idleVideoContainer->hide();

    if (ui->display->currentWidget() != ui->page01_Welcome) {
        ui->display->setCurrentWidget(ui->page01_Welcome);
    }
}

void MainWindow::returnToIdleFromExit()
{
    exitTimer->stop();
    pinTimer->stop();
    inactivityTimer->stop();
    autoLogoutTimer->stop();

    ui->pinInput->clear();
    ui->CardNumberDisplay->clear();
    currentCardUid.clear();
    sessionToken.clear();
    webToken.clear();

    accountId = 0;
    activeAccountId = 0;
    debitAccountId = 0;
    creditAccountId = 0;
    accountMode = "debit";
    isDualCard = false;

    clearAccountsPage();
    resetDonationSelection();
    resetTransferForm();
    selectedAccountType = DebitAccount;
    updateCreditDebitButton();

    showIdlePage();
}