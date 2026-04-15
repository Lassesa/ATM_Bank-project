#include "mainwindow.h"
#include "ui_mainwindow.h"

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


    // Sound management
    keypadSound = new QSoundEffect(this);
    keypadSound->setSource(QUrl("qrc:/sounds/keypad.wav"));
    keypadSound->setVolume(0.5);

    okSound = new QSoundEffect(this);
    okSound->setSource(QUrl("qrc:/sounds/ok.wav"));
    okSound->setVolume(0.5);

    cancelSound = new QSoundEffect(this);
    cancelSound->setSource(QUrl("qrc:/sounds/cancel.wav"));
    cancelSound->setVolume(0.5);

    clearSound = new QSoundEffect(this);
    clearSound->setSource(QUrl("qrc:/sounds/clear.wav"));
    clearSound->setVolume(0.5);

    buttonSound = new QSoundEffect(this);
    buttonSound->setSource(QUrl("qrc:/sounds/button.wav"));
    buttonSound->setVolume(0.5);

    successSound = new QSoundEffect(this);
    successSound->setSource(QUrl("qrc:/sounds/success.wav"));
    successSound->setVolume(0.5);

    errorSound = new QSoundEffect(this);
    errorSound->setSource(QUrl("qrc:/sounds/error.wav"));
    errorSound->setVolume(0.5);

    withdrawSound = new QSoundEffect(this);
    withdrawSound->setSource(QUrl("qrc:/sounds/withdraw.wav"));
    withdrawSound->setVolume(0.5);

    timeoutSound = new QSoundEffect(this);
    timeoutSound->setSource(QUrl("qrc:/sounds/timeout.wav"));
    timeoutSound->setVolume(0.5);


    // Create network manager for backend API calls
    networkManager = new QNetworkAccessManager(this);

    // Timers
    pinTimer = new QTimer(this);
    pinTimer->setSingleShot(true);
    connect(pinTimer, &QTimer::timeout, this, &MainWindow::resetToWelcome);
    exitTimer = new QTimer(this);
    exitTimer->setSingleShot(true);
    connect(exitTimer, &QTimer::timeout, this, &MainWindow::resetToWelcome);

    inactivityTimer = new QTimer(this);
    inactivityTimer->setInterval(30000); // // 30 seconds
    connect(inactivityTimer, &QTimer::timeout, this, &MainWindow::showInactivityPage);


    autoLogoutTimer = new QTimer(this);
    autoLogoutTimer->setSingleShot(true);
    connect(autoLogoutTimer, &QTimer::timeout, this, &MainWindow::resetToWelcome);

    QList<QPushButton *> allButtons = this->findChildren<QPushButton *>();

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

    // Set application window icon
    this->setWindowIcon(QIcon(":/logo.svg"));

    // Set initial keypad mode
    currentMode = PinMode;

    // Configure language buttons
    ui->btnLanguageEnglish->setCheckable(true);
    ui->btnLanguageFinnish->setCheckable(true);
    ui->btnLanguagePolish->setCheckable(true);

    ui->btnLanguageEnglish->setAutoExclusive(true);
    ui->btnLanguageFinnish->setAutoExclusive(true);
    ui->btnLanguagePolish->setAutoExclusive(true);

    ui->btnLanguageEnglish->setChecked(true);

    // Set initial language
    setLanguage("EN");

    // Set initial Page Mode
    ui->btnContrast->setCheckable(true);
    ui->btnContrast->setText("CO");

    // Show welcome page at startup
    ui->display->setCurrentWidget(ui->page01_Welcome);

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
        handleDigit("0");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });

    connect(ui->num_1, &QPushButton::clicked, this, [this]() {
        handleDigit("1");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });

    connect(ui->num_2, &QPushButton::clicked, this, [this]() {
        handleDigit("2");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });

    connect(ui->num_3, &QPushButton::clicked, this, [this]() {
        handleDigit("3");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });

    connect(ui->num_4, &QPushButton::clicked, this, [this]() {
        handleDigit("4");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });

    connect(ui->num_5, &QPushButton::clicked, this, [this]() {
        handleDigit("5");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });

    connect(ui->num_6, &QPushButton::clicked, this, [this]() {
        handleDigit("6");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });

    connect(ui->num_7, &QPushButton::clicked, this, [this]() {
        handleDigit("7");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });

    connect(ui->num_8, &QPushButton::clicked, this, [this]() {
        handleDigit("8");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });

    connect(ui->num_9, &QPushButton::clicked, this, [this]() {
        handleDigit("9");
        QTimer::singleShot(0, this, [this]() { if (keypadSound) keypadSound->play(); });
    });



    // -----------------------------
    // Clear button YELLOW
    // -----------------------------
    connect(ui->button_2yellow_CLEAR, &QPushButton::clicked, this, [this]() {

        if (clearSound)
            clearSound->play();

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
    });

    // -----------------------------
    // Cancel button RED
    // -----------------------------
    connect(ui->button_1red_CANCEL, &QPushButton::clicked, this, [this]() {

        if (cancelSound)
            cancelSound->play();

        QWidget *current = ui->display->currentWidget();

        if (current == ui->page02_Pin || current == ui->page01_Welcome || current == ui->page11_Time) {
            ui->pinInput->clear();
            ui->display->setCurrentWidget(ui->page08_Exit);
            exitTimer->start(5000);
        }
        else if (current == ui->page03_Main) {
            ui->display->setCurrentWidget(ui->page08_Exit);
            exitTimer->start(5000);
        }
        else if (current == ui->page04_Withdraw ||
                 current == ui->page05_Balance ||
                 current == ui->page06_Transfer ||
                 current == ui->page09_Other) {
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

        if (okSound)
            okSound->play();

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
        else if (ui->display->currentWidget() == ui->page07_Donation) {
            qDebug() << "OK painettu lahjoitussivulla";
            on_btnConfirmDonation_clicked();
            resetDonationSelection(); // added check
        }
    });

    // -----------------------------
    // Main menu buttons
    // -----------------------------
    connect(ui->btn_main_choice_1, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        selectAmount(50);
        showPage(ui->page04_Withdraw);
    });

    connect(ui->btn_main_choice_2, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        selectAmount(100);
        showPage(ui->page04_Withdraw);
    });

    connect(ui->btn_main_choice_3, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        selectAmount(0);
        showPage(ui->page04_Withdraw);
    });

    connect(ui->btn_main_choice_4, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        updateBalanceDisplay();
        updateTransactionsDisplay();
        showPage(ui->page05_Balance);
    });

    connect(ui->btn_main_choice_5, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        showPage(ui->page06_Transfer);
    });

    connect(ui->btn_main_choice_6, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        showPage(ui->page07_Donation);
    });

    connect(ui->btn_main_choice_7, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        showPage(ui->page08_Exit);
        exitTimer->start(5000);
    });


    connect(ui->btn_main_choice_8, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        showPage(ui->page09_Other); /// to be changed
    });



// -----------------------------
// Balance menu buttons
// -----------------------------

    connect(ui->Balance_btn_choice_1, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        showPage(ui->page12_Accounts);
    });

    connect(ui->Balance_btn_choice_2, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        showPage(ui->page13_Transactions);
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
    // Transactions menu buttons
    // -----------------------------


    connect(ui->Transactions_btn_choice_next, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        showPage(ui->page04_Withdraw);
    });

    connect(ui->Transactions_btn_choice_previous, &QPushButton::clicked, this, [this]() {

        if (buttonSound)
            buttonSound->play();

        showPage(ui->page04_Withdraw);
    });

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
    ui->btnLanguageEnglish->setChecked(lang == "EN");
    ui->btnLanguagePolish->setChecked(lang == "PL");
    ui->btnLanguageFinnish->setChecked(lang == "FI");

    if (lang == "EN") {
        ui->labelWelcome->setText("Welcome to S/R Bank");
        ui->labelInstruction->setText("Insert your card to begin");

        ui->labelWelcome_PIN->setText("Please enter your PIN");
        ui->labelInstruction_PIN->setText("Please cover the keypad while entering your PIN");

        ui->labelWelcome_Main->setText("Choose a transaction");
        ui->labelInstruction_Main->setText("Select a service to continue");

        ui->labelWelcome_Withdraw->setText("Withdraw Cash");
        ui->labelInstruction_Withdraw->setText("Enter amount and press OK");

        ui->labelWelcome_Balance->setText("Account balance");
        ui->labelInstruction_Balance->setText("View your current available balance");


        ui->labelWelcome_Transfer->setText("Transfer Money");
        ui->labelInstruction_Transfer->setText("Send money to another account");

        ui->labelWelcome_Donation->setText("Donation");
        ui->labelInstruction_Donation->setText("Support a cause or organization");

        ui->labelWelcome_Exit->setText("Thank you!");
        ui->labelInstruction_Exit->setText("Please remember to take your card");

        ui->labelWelcome_Other->setText("Other");
        ui->labelInstruction_Other->setText("Access more features and settings");

        ui->btn_main_choice_1->setText("50 €");
        ui->btn_main_choice_2->setText("100 €");
        ui->btn_main_choice_3->setText("3 Other amount");
        ui->btn_main_choice_4->setText("4 Balance");
        ui->btn_main_choice_5->setText("5 Transfer");
        ui->btn_main_choice_6->setText("6 Donation");
        ui->btn_main_choice_7->setText("7 Exit");
        ui->btn_main_choice_8->setText("8 More");

        ui->Balance_TitleAccountSelect->setText("Main account");
        ui->Balance_TitleRecentTransactions->setText("Last 3 transactions");
        ui->Balance_btn_choice_1->setText("Other Accounts");
        ui->Balance_btn_choice_2->setText("More Transactions");

        ui->btn_donation_choice_1->setText("Red Cross");
        ui->btn_donation_choice_2->setText("Cancer Foundation");
        ui->btn_donation_choice_3->setText("UNICEF");
        ui->btn_donation_choice_4->setText("Hair transplant for Arttu");

        ui->labelWelcome_Error->setText("Out of Service");
        ui->labelInstruction_Error->setText("Please use another ATM.");

        ui->labelWelcome_Time->setText("Are you still there?");
        ui->labelInstruction_Time->setText("Your session will end soon.");

        ui->labelWelcome_Accounts->setText("Available accounts");
        ui->labelInstruction_Accounts->setText("Accounts you can use");

        ui->labelWelcome_Transactions->setText("Transactions");
        ui->labelInstruction_Transactions->setText("Recent transactions");


    }
    else if (lang == "PL") {
        ui->labelWelcome->setText("Witamy w S/R Banku");
        ui->labelInstruction->setText("Włóż kartę, aby rozpocząć");

        ui->labelWelcome_PIN->setText("Proszę wprowadzić PIN");
        ui->labelInstruction_PIN->setText("Proszę zasłonić klawiaturę podczas wpisywania PIN-u");

        ui->labelWelcome_Main->setText("Wybierz transakcję");
        ui->labelInstruction_Main->setText("Wybierz usługę, aby kontynuować");

        ui->labelWelcome_Withdraw->setText("Wypłata gotówki");
        ui->labelInstruction_Withdraw->setText("Wpisz kwotę i naciśnij OK");

        ui->labelWelcome_Balance->setText("Saldo konta");
        ui->labelInstruction_Balance->setText("Sprawdź aktualne dostępne saldo");

        ui->labelWelcome_Transfer->setText("Przelew");
        ui->labelInstruction_Transfer->setText("Wyślij pieniądze na inne konto");

        ui->labelWelcome_Donation->setText("Darowizna");
        ui->labelInstruction_Donation->setText("Wesprzyj wybraną organizację");

        ui->labelWelcome_Exit->setText("Dziękujemy!");
        ui->labelInstruction_Exit->setText("Pamiętaj, aby zabrać kartę");

        ui->labelWelcome_Other->setText("Inne usługi");
        ui->labelInstruction_Other->setText("Dostęp do dodatkowych usług bankowych");

        ui->btn_main_choice_1->setText("50 €");
        ui->btn_main_choice_2->setText("100 €");
        ui->btn_main_choice_3->setText("3 Inna kwota");
        ui->btn_main_choice_4->setText("4 Saldo");
        ui->btn_main_choice_5->setText("5 Przelew");
        ui->btn_main_choice_6->setText("6 Darowizna");
        ui->btn_main_choice_7->setText("7 Wyjście");
        ui->btn_main_choice_8->setText("8 Więcej");

        ui->Balance_TitleAccountSelect->setText("Konto główne");
        ui->Balance_TitleRecentTransactions->setText("Ostatnie 3 transakcje");
        ui->Balance_btn_choice_1->setText("Inne konta");
        ui->Balance_btn_choice_2->setText("Więcej transakcji");

        ui->btn_donation_choice_1->setText("Czerwony Krzyż");
        ui->btn_donation_choice_2->setText("Fundacja Onkologiczna");
        ui->btn_donation_choice_3->setText("UNICEF");
        ui->btn_donation_choice_4->setText("Przeszczep włosów dla Arttu");

        ui->labelWelcome_Error->setText("Przerwa techniczna");
        ui->labelInstruction_Error->setText("Prosimy skorzystać z innego bankomatu.");

        ui->labelWelcome_Time->setText("Czy nadal jesteś tam?");
        ui->labelInstruction_Time->setText("Twoja sesja wkrótce wygaśnie.");

        ui->labelWelcome_Accounts->setText("Dostępne konta");
        ui->labelInstruction_Accounts->setText("Twoje konta");

        ui->labelWelcome_Transactions->setText("Transakcje");
        ui->labelInstruction_Transactions->setText("Ostatnie operacje");
    }
    else if (lang == "FI") {
        ui->labelWelcome->setText("Tervetuloa S/R Pankkiin");
        ui->labelInstruction->setText("Aseta kortti aloittaaksesi");

        ui->labelWelcome_PIN->setText("Anna PIN-koodi");
        ui->labelInstruction_PIN->setText("Suojaa näppäimistö PIN-koodia syöttäessäsi");

        ui->labelWelcome_Main->setText("Valitse tapahtuma");
        ui->labelInstruction_Main->setText("Valitse palvelu jatkaaksesi");

        ui->labelWelcome_Withdraw->setText("Nosta käteistä");
        ui->labelInstruction_Withdraw->setText("Syötä summa ja paina OK");

        ui->labelWelcome_Balance->setText("Tilin saldo");
        ui->labelInstruction_Balance->setText("Näet tilisi tämänhetkisen saldon");

        ui->labelWelcome_Transfer->setText("Tilisiirto");
        ui->labelInstruction_Transfer->setText("Lähetä rahaa toiselle tilille");

        ui->labelWelcome_Donation->setText("Lahjoitus");
        ui->labelInstruction_Donation->setText("Tue valitsemaasi kohdetta");

        ui->labelWelcome_Exit->setText("Kiitos!");
        ui->labelInstruction_Exit->setText("Muista ottaa korttisi");

        ui->labelWelcome_Other->setText("Muut palvelut");
        ui->labelInstruction_Other->setText("Käytä muita pankkipalveluita");

        ui->btn_main_choice_1->setText("50 €");
        ui->btn_main_choice_2->setText("100 €");
        ui->btn_main_choice_3->setText("3 Muu summa");
        ui->btn_main_choice_4->setText("4 Saldo");
        ui->btn_main_choice_5->setText("5 Siirto");
        ui->btn_main_choice_6->setText("6 Lahjoitus");
        ui->btn_main_choice_7->setText("7 Poistu");
        ui->btn_main_choice_8->setText("8 Lisää");

        ui->Balance_TitleAccountSelect->setText("Päätili");
        ui->Balance_TitleRecentTransactions->setText("Viimeiset 3 tapahtumaa");
        ui->Balance_btn_choice_1->setText("Muut tilit");
        ui->Balance_btn_choice_2->setText("Lisää tapahtumia");

        ui->btn_donation_choice_1->setText("Punainen Risti");
        ui->btn_donation_choice_2->setText("Syöpäsäätiö");
        ui->btn_donation_choice_3->setText("UNICEF");
        ui->btn_donation_choice_4->setText("Artulle hiussiirto");

        ui->labelWelcome_Error->setText("Tilapäinen häiriö");
        ui->labelInstruction_Error->setText("Käytä toista pankkiautomaattia.");

        ui->labelWelcome_Time->setText("Oletko vielä paikalla?");
        ui->labelInstruction_Time->setText("Istuntosi päättyy pian.");

        ui->labelWelcome_Accounts->setText("Saatavilla olevat tilit");
        ui->labelInstruction_Accounts->setText("Omat tilit");

        ui->labelWelcome_Transactions->setText("Tapahtumat");
        ui->labelInstruction_Transactions->setText("Viimeisimmät tapahtumat");


    }
}

// =====================================================
// Input Handling
// =====================================================

/*
 * Handles one digit from the keypad or keyboard.
 *
 * What this function does:
 * - on PIN page: appends one digit to the PIN field
 * - on withdraw page: appends one digit to the amount field
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
    // Your current serial port:
    serial->setPortName("COM3");

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

        // Restore the PIN instruction text based on the selected language
        if (ui->btnLanguageFinnish->isChecked()) {
            ui->labelInstruction_PIN->setText("Suojaa näppäimistö PIN-koodia syöttäessäsi");
        } else if (ui->btnLanguagePolish->isChecked()) {
            ui->labelInstruction_PIN->setText("Proszę zasłonić klawiaturę podczas wpisywania PIN-u");
        } else {
            ui->labelInstruction_PIN->setText("Please cover the keypad while entering your PIN");
        }

        // Show the welcome page and display the scanned card
        ui->display->setCurrentWidget(ui->page01_Welcome);

        if (ui->btnLanguageFinnish->isChecked()) {
            ui->labelInstruction->setText("Kortti tunnistettu");
        } else if (ui->btnLanguagePolish->isChecked()) {
            ui->labelInstruction->setText("Wykryto kartę");
        } else {
            ui->labelInstruction->setText("Card detected");
        }

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
        // Read the response once and store it
        QByteArray responseData = reply->readAll();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // Successful login
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument resDoc = QJsonDocument::fromJson(responseData);

            if (!resDoc.isNull() && resDoc.isObject()) {
                QJsonObject resObj = resDoc.object();
                sessionToken = resObj.value("token").toString();

                qDebug() << "Login successful!";
                qDebug() << "Token start:" << sessionToken.left(10) << "...";

                // Load real account data after login
                updateBalanceDisplay();
                updateTransactionsDisplay();

                inactivityTimer->start(30000);

                if (successSound)
                    successSound->play();

                ui->display->setCurrentWidget(ui->page03_Main);
                ui->pinInput->clear();
            }
        }
        // Wrong PIN
        else if (statusCode == 401) {
            QJsonObject resObj = QJsonDocument::fromJson(responseData).object();
            int remaining = resObj.value("remaining").toInt();

            QString errorMsg;
            if (ui->btnLanguageFinnish->isChecked()) {
                errorMsg = QString("Väärä PIN! Yrityksiä jäljellä: %1").arg(remaining);
            } else if (ui->btnLanguagePolish->isChecked()) {
                errorMsg = QString("Błędny PIN! Pozostało prób: %1").arg(remaining);
            } else {
                errorMsg = QString("Wrong PIN! Remaining attempts: %1").arg(remaining);
            }

            ui->labelInstruction_PIN->setText(errorMsg);
            ui->pinInput->clear();
            pinTimer->start(10000);

            if (errorSound)
                errorSound->play();
        }
        // Locked card
        else if (statusCode == 403) {
            QString errorMsg;
            if (ui->btnLanguageFinnish->isChecked()) {
                errorMsg = "Kortti on lukittu. Ota yhteys pankkiin.";
            } else if (ui->btnLanguagePolish->isChecked()) {
                errorMsg = "Karta jest zablokowana. Skontaktuj się z bankiem.";
            } else {
                errorMsg = "Card is locked. Please contact the bank.";
            }

            ui->labelInstruction_PIN->setText(errorMsg);
            ui->pinInput->clear();
            ui->pinInput->setEnabled(false);

            if (errorSound)
                errorSound->play();

            QTimer::singleShot(5000, this, &MainWindow::resetToWelcome);
        }
        // Other network or backend error
        else {
            qDebug() << "Login error:" << reply->errorString();
            qDebug() << "Backend response:" << responseData;

            if (errorSound)
                errorSound->play();

            ui->pinInput->clear();
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
    QUrl url("http://localhost:3000/account/balance/me");
    QNetworkRequest request(url);

    request.setRawHeader("Authorization", "Bearer " + sessionToken.toUtf8());

    qDebug() << "Starting balance request to:" << url.toString();

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            qDebug() << "Backend raw balance data:" << responseData;

            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject obj;

            if (jsonDoc.isArray()) {
                obj = jsonDoc.array().first().toObject();
            } else {
                obj = jsonDoc.object();
            }

            if (obj.contains("idaccount")) {
                accountId = obj.value("idaccount").toVariant().toInt();
                qDebug() << "Updated real account ID from balance endpoint:" << accountId;
            }

            if (obj.contains("idaccount")) {
                accountId = obj.value("idaccount").toVariant().toInt();
                qDebug() << "Updated real account ID from balance endpoint:" << accountId;
            }

            if (obj.contains("account_balance")) {
                double balance = obj.value("account_balance").toVariant().toDouble();
                ui->Balance_Amount->setText(QString::number(balance, 'f', 2) + " €");
                qDebug() << "Balance updated successfully:" << balance;
            } else {
                qDebug() << "Error: 'account_balance' field not found. Keys:" << obj.keys();
                ui->Balance_Amount->setText("Virhe");
            }

        } else {
            qDebug() << "Balance fetch failed:" << reply->errorString();
            qDebug() << "Server error message:" << reply->readAll();
            ui->Balance_Amount->setText("Yhteysvirhe");
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
    if (accountId <= 0) {
        qDebug() << "Transactions fetch skipped: invalid accountId:" << accountId;
        ui->Balance_ListRecentTransactions->clear();
        ui->Balance_ListRecentTransactions->addItem("No transactions available.");
        return;
    }

    QString urlStr = QString("http://localhost:3000/transaction/%1").arg(accountId);
    QUrl url(urlStr);
    QNetworkRequest request(url);

    request.setRawHeader("Authorization", "Bearer " + sessionToken.toUtf8());

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        ui->Balance_ListRecentTransactions->clear();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonArray transArray = QJsonDocument::fromJson(responseData).array();

            if (transArray.isEmpty()) {
                ui->Balance_ListRecentTransactions->addItem("Ei aiempia tapahtumia.");
            } else {
                int count = qMin(transArray.size(), 3);

                for (int i = 0; i < count; ++i) {
                    QJsonObject obj = transArray.at(i).toObject();

                    double amount = obj.value("transaction_amount").toVariant().toDouble();
                    QString dateStr = obj.value("transaction_date").toString().left(10);

                    QString description = obj.value("transaction_description").toString();
                    if (description.isEmpty()) {
                        description = "ATM WITHDRAW";
                    }

                    QString row = QString("%1  %2  %3 €")
                                      .arg(dateStr)
                                      .arg(description.left(15).trimmed().toUpper())
                                      .arg(QString::number(amount, 'f', 2));

                    ui->Balance_ListRecentTransactions->addItem(row);
                }
            }
        } else {
            qDebug() << "Transaction fetch failed:" << reply->errorString();
            ui->Balance_ListRecentTransactions->addItem("Tapahtumia ei voitu ladata.");
        }

        reply->deleteLater();
    });
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
    if (accountId <= 0) {
        qDebug() << "Withdrawal blocked: invalid accountId:" << accountId;

        if (ui->btnLanguageFinnish->isChecked()) {
            ui->labelInstruction_Withdraw->setText("Tilitietoja ei voitu ladata.");
        } else if (ui->btnLanguagePolish->isChecked()) {
            ui->labelInstruction_Withdraw->setText("Nie udało się wczytać danych konta.");
        } else {
            ui->labelInstruction_Withdraw->setText("Failed to load account data.");
        }

        return;
    }
    QUrl url("http://localhost:3000/transaction/withdrawal");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + sessionToken.toUtf8());

    QJsonObject json;
    json["id_account"] = accountId;
    json["amount"] = amount;
    json["description"] = description;

    qDebug() << "Starting withdrawal request";
    qDebug() << "Account ID:" << accountId;
    qDebug() << "Amount:" << amount;
    qDebug() << "Description:" << description;
    qDebug() << "Token start:" << sessionToken.left(20);

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, description]() {
        QByteArray responseData = reply->readAll();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Successful transaction:" << description;
            qDebug() << "HTTP status:" << statusCode;
            qDebug() << "Backend response:" << responseData;

            if (withdrawSound)
                withdrawSound->play();

            updateBalanceDisplay();
            updateTransactionsDisplay();
            ui->display->setCurrentWidget(ui->page03_Main);
        } else {
            qDebug() << "Transaction error:" << reply->errorString();
            qDebug() << "HTTP status:" << statusCode;
            qDebug() << "Backend response:" << responseData;
        }

        reply->deleteLater();
    });
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
    resetDonationSelection();

    // Restore the default PIN instruction text
    if (ui->btnLanguageFinnish->isChecked()) {
        ui->labelInstruction_PIN->setText("Suojaa näppäimistö PIN-koodia syöttäessäsi");
    } else if (ui->btnLanguagePolish->isChecked()) {
        ui->labelInstruction_PIN->setText("Proszę zasłonić klawiaturę podczas wpisywania PIN-u");
    } else {
        ui->labelInstruction_PIN->setText("Please cover the keypad while entering your PIN");
    }

    // Return to the welcome page
    ui->display->setCurrentWidget(ui->page01_Welcome);
}

void MainWindow::resetInactivity()
{
    // If the timeout warning page is open, return to the main menu
    if (ui->display->currentWidget() == ui->page11_Time) {
        autoLogoutTimer->stop();
        ui->display->setCurrentWidget(ui->page03_Main);
        inactivityTimer->start(30000); // Restart the 30-second inactivity timer
        qDebug() << "Palattu aikakatkaisusta napin painalluksella";
        return;
    }

    // If the user is logged in, reset the inactivity timer
    if (inactivityTimer->isActive()) {
        inactivityTimer->start(30000);
        qDebug() << "Ajastin resetoitu napin painalluksesta";
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
    QUrl url("http://localhost:3000/card/lock"); // Make sure the backend route is correct
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["card_number"] = cardNum;

    // Send the card lock request to the backend
    networkManager->put(request, QJsonDocument(json).toJson());

    // Show a message to the user
    ui->labelInstruction_PIN->setText("Kortti on lukittu. Ota yhteys pankkiin.");
    ui->pinInput->setEnabled(false); // Estetään syöttö

    // Return to the welcome screen after 5 seconds
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
    qDebug() << "Donation attempt:";
    qDebug() << "accountId =" << accountId;
    qDebug() << "selectedCharity =" << selectedCharity;
    qDebug() << "pendingDonationAmount =" << pendingDonationAmount;

    if (pendingDonationAmount <= 0 || selectedCharity.isEmpty()) {
        qDebug() << "Valitse kohde ja summa ensin!";
        return;
    }

    if (accountId <= 0) {
        qDebug() << "Donation blocked: invalid accountId:" << accountId;

        if (ui->btnLanguageFinnish->isChecked()) {
            ui->labelInstruction_Donation->setText("Tilitietoja ei voitu ladata.");
        } else if (ui->btnLanguagePolish->isChecked()) {
            ui->labelInstruction_Donation->setText("Nie udało się wczytać danych konta.");
        } else {
            ui->labelInstruction_Donation->setText("Failed to load account data.");
        }

        return;
    }

    makeWithdrawalRequest(pendingDonationAmount, "DONATION: " + selectedCharity);
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
