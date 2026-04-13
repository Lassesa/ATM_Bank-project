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

    // Create network manager for backend API calls
    networkManager = new QNetworkAccessManager(this);

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
    this->setWindowIcon(QIcon(":/logo.svg/logo.svg"));

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
    ui->display->setCurrentWidget(ui->page1_Welcome);

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
    connect(ui->num_0, &QPushButton::clicked, this, [this]() { handleDigit("0"); });
    connect(ui->num_1, &QPushButton::clicked, this, [this]() { handleDigit("1"); });
    connect(ui->num_2, &QPushButton::clicked, this, [this]() { handleDigit("2"); });
    connect(ui->num_3, &QPushButton::clicked, this, [this]() { handleDigit("3"); });
    connect(ui->num_4, &QPushButton::clicked, this, [this]() { handleDigit("4"); });
    connect(ui->num_5, &QPushButton::clicked, this, [this]() { handleDigit("5"); });
    connect(ui->num_6, &QPushButton::clicked, this, [this]() { handleDigit("6"); });
    connect(ui->num_7, &QPushButton::clicked, this, [this]() { handleDigit("7"); });
    connect(ui->num_8, &QPushButton::clicked, this, [this]() { handleDigit("8"); });
    connect(ui->num_9, &QPushButton::clicked, this, [this]() { handleDigit("9"); });

    // -----------------------------
    // Clear button
    // -----------------------------
    connect(ui->button_2yellow_CLEAR, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page2_Pin) {
            QString text = ui->pinInput->text();
            text.chop(1);
            ui->pinInput->setText(text);
        }
        else if (ui->display->currentWidget() == ui->page4_Withdraw) {
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
    // Cancel button
    // -----------------------------
    connect(ui->button_1red_CANCEL, &QPushButton::clicked, this, [this]() {
        QWidget *current = ui->display->currentWidget();

        if (current == ui->page2_Pin || current == ui->page1_Welcome || current == ui->page11_Time) {
            ui->pinInput->clear();
            ui->display->setCurrentWidget(ui->page8_Exit);
        }
        else if (current == ui->page3_Main) {
            ui->display->setCurrentWidget(ui->page8_Exit);
        }
        else if (current == ui->page4_Withdraw ||
                 current == ui->page5_Balance ||
                 current == ui->page6_Transfer ||
                 current == ui->page7_Donation ||
                 current == ui->page9_Other) {
            ui->display->setCurrentWidget(ui->page3_Main);
        }
        else if (current == ui->page12_Accounts ||
                 current == ui->page13_Transactions) {
            ui->display->setCurrentWidget(ui->page5_Balance);
        }
    });

    // -----------------------------
    // OK button
    // -----------------------------
    connect(ui->button_3green_OK, &QPushButton::clicked, this, [this]() {
        if (ui->display->currentWidget() == ui->page1_Welcome) {
            ui->display->setCurrentWidget(ui->page2_Pin);
            ui->pinInput->clear();
            ui->pinInput->setFocus();
        }
        else if (ui->display->currentWidget() == ui->page2_Pin) {
            QString currentCard = ui->CardNumberDisplay->text().trimmed();
            QString currentPin = ui->pinInput->text().trimmed();

            if (currentCard.isEmpty() || currentPin.isEmpty()) {
                qDebug() << "Error: card number or PIN is missing from UI.";
                return;
            }

            qDebug() << "Sending login request with card:" << currentCard << "and PIN:" << currentPin;
            makeLoginRequest(currentCard, currentPin);
        }
        else if (ui->display->currentWidget() == ui->page4_Withdraw) {
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
    });

    // -----------------------------
    // Main menu buttons
    // -----------------------------
    connect(ui->btn_main_choice_1, &QPushButton::clicked, this, [this]() {
        selectAmount(50);
        showPage(ui->page4_Withdraw);
    });

    connect(ui->btn_main_choice_2, &QPushButton::clicked, this, [this]() {
        selectAmount(100);
        showPage(ui->page4_Withdraw);
    });

    connect(ui->btn_main_choice_3, &QPushButton::clicked, this, [this]() {
        selectAmount(0);
        showPage(ui->page4_Withdraw);
    });

    connect(ui->btn_main_choice_4, &QPushButton::clicked, this, [this]() {
        updateBalanceDisplay();
        updateTransactionsDisplay();
        showPage(ui->page5_Balance);
    });

    connect(ui->btn_main_choice_5, &QPushButton::clicked, this, [this]() {
        showPage(ui->page6_Transfer);
    });

    connect(ui->btn_main_choice_6, &QPushButton::clicked, this, [this]() {
        showPage(ui->page7_Donation);
    });

    connect(ui->btn_main_choice_7, &QPushButton::clicked, this, [this]() {
        showPage(ui->page8_Exit);
    });


    connect(ui->btn_main_choice_8, &QPushButton::clicked, this, [this]() {
        showPage(ui->page11_Time);
    });

    connect(ui->Balance_btn_choice_1, &QPushButton::clicked, this, [this]() {
        showPage(ui->page12_Accounts);
    });

    connect(ui->Balance_btn_choice_2, &QPushButton::clicked, this, [this]() {
        showPage(ui->page13_Transactions);
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
    if (ui->display->currentWidget() == ui->page2_Pin) {
        QString currentText = ui->pinInput->text();
        if (currentText.length() < 4) {
            ui->pinInput->setText(currentText + digit);
        }
    }
    else if (ui->display->currentWidget() == ui->page4_Withdraw) {
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
        if (ui->display->currentWidget() == ui->page2_Pin) {
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
    serial->setPortName("/dev/tty.usbmodem146301");

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

        qDebug() << "Scanned card:" << currentCardUid;

        ui->display->setCurrentWidget(ui->page1_Welcome);
        ui->labelInstruction->setText("Card detected");
        ui->CardNumberDisplay->setText(currentCardUid);
        ui->CardNumberDisplay->update();

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
    showPage(ui->page4_Withdraw);
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
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            qDebug() << "RAW BACKEND DATA:" << responseData;

            QJsonDocument resDoc = QJsonDocument::fromJson(responseData);

            if (!resDoc.isNull() && resDoc.isObject()) {
                QJsonObject resObj = resDoc.object();

                sessionToken = resObj.value("token").toString();

                QJsonValue idVal;
                if (resObj.contains("idaccount")) {
                    idVal = resObj.value("idaccount");
                } else if (resObj.contains("id_account")) {
                    idVal = resObj.value("id_account");
                }

                if (idVal.isDouble()) {
                    accountId = idVal.toInt();
                } else {
                    accountId = idVal.toString().toInt();
                }

                qDebug() << "Login successful!";
                qDebug() << "Stored Account ID:" << accountId;
                qDebug() << "Token start:" << sessionToken.left(10) << "...";

                ui->display->setCurrentWidget(ui->page3_Main);
                ui->pinInput->clear();
            } else {
                qDebug() << "Error: backend did not return a valid JSON object.";
            }
        } else {
            QByteArray errorData = reply->readAll();
            qDebug() << "Login error:" << reply->errorString();
            qDebug() << "Backend error message:" << errorData;
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
    QUrl url("http://localhost:3000/transaction/withdrawal");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + sessionToken.toUtf8());

    QJsonObject json;
    json["id_account"] = accountId;
    json["amount"] = amount;
    json["description"] = description;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, description]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Successful transaction:" << description;
            updateBalanceDisplay();
            updateTransactionsDisplay();
            ui->display->setCurrentWidget(ui->page3_Main);
        } else {
            qDebug() << "Transaction error";
        }

        reply->deleteLater();
    });
}


/*
 * LIGHT STYLE
 *
 * On success:
 * - Keeping it by deafoult
 * - Changing from Dark
 */

void MainWindow::setupStyles()
{
    lightStyle = R"(

/* =====================================================
   APP BACKGROUND
   ===================================================== */

#centralwidget {
    background-color: #EEF3F9;
}


/* =====================================================
   DISPLAY AREA
   ===================================================== */

#display {
    background-color: qlineargradient(
        x1:0, y1:0, x2:1, y2:1,
        stop:0 #FFE4EC,
        stop:1 #FFFFFF
    );
    border-radius: 28px;
}


/* =====================================================
   STACKED PAGES
   ===================================================== */

#page1_Welcome,
#page2_Pin,
#page3_Main,
#page4_Withdraw,
#page5_Balance,
#page6_Transfer,
#page7_Donation,
#page8_Exit,
#page9_Other,
#page10_Error,
#page11_Time,
#page12_Accounts,
#page13_Transactions {
    background: transparent;
}


/* =====================================================
   GLOBAL TEXT
   ===================================================== */

QLabel {
    color: #1E293B;
}


/* =====================================================
   PAGE TITLES
   ===================================================== */

QLabel#labelWelcome,
QLabel#labelWelcome_PIN,
QLabel#labelWelcome_Main,
QLabel#labelWelcome_Withdraw,
QLabel#labelWelcome_Balance,
QLabel#labelWelcome_Transfer,
QLabel#labelWelcome_Donation,
QLabel#labelWelcome_Exit,
QLabel#labelWelcome_Other,
QLabel#labelWelcome_Error,
QLabel#labelWelcome_Time,
QLabel#labelWelcome_Accounts,
QLabel#labelWelcome_Transactions
 {
    font-size: 40px;
    font-weight: 800;
    color: #0F172A;
}


/* =====================================================
   PAGE SUBTITLES
   ===================================================== */

QLabel#labelInstruction,
QLabel#labelInstruction_PIN,
QLabel#labelInstruction_Main,
QLabel#labelInstruction_Withdraw,
QLabel#labelInstruction_Balance,
QLabel#labelInstruction_Transfer,
QLabel#labelInstruction_Donation,
QLabel#labelInstruction_Exit,
QLabel#labelInstruction_Other,
QLabel#labelInstruction_Error,
QLabel#labelInstruction_Time,
QLabel#labelInstruction_Accounts,
QLabel#labelInstruction_Transactions  {
    font-size: 20px;
    font-weight: 600;
    color: #475569;
}


/* =====================================================
   FOOTER
   ===================================================== */

QLabel#foundersLabel {
    font-size: 9px;
    font-weight: 600;
    color: #334155;
}


/* =====================================================
   INPUTS
   ===================================================== */

QLineEdit#CardNumberDisplay {
    background-color: #1E293B;
    color: #FFFFFF;
    font-size: 18px;
    font-weight: bold;
    padding: 6px;
    border-radius: 6px;
}

QLineEdit#pinInput {
    background-color: #FFF7FB;
    border: 2px solid #D94680;
    border-radius: 16px;
    color: #9D174D;
    font-size: 28px;
    font-weight: 700;
    font-family: "Segoe UI", "Arial";
    padding: 12px 18px;
    letter-spacing: 12px;
}

QLineEdit#amountInput {
    background-color: transparent;
    border: none;
    color: #AD1457;
    font-size: 48px;
    font-weight: 800;
    font-family: "Segoe UI", "Arial";
    padding: 0px;
    margin: 0px;
}

QLabel#labelAmountCurrency {
    background-color: transparent;
    color: #C2185B;
    font-size: 26px;
    font-weight: 600;
    font-family: "Segoe UI", "Arial";
    padding: 0px;
    margin: 0px;
}


/* =====================================================
   BALANCE PAGE EXTRA
   ===================================================== */

QListWidget {
    background-color: transparent;
    border: none;
    font-size: 16px;
    color: #1E293B;
}

QListWidget#Balance_ListRecentTransactions {
    font-size: 10px;
    color: #0F172A;
}

QLabel#Balance_Amount {
    font-size: 30px;
    font-weight: 800;
    color: #0F172A;
}


/* =====================================================
   GLOBAL BUTTON BASE
   ===================================================== */

QPushButton {
    padding: 10px;
    font-weight: bold;
    border: none;
}


/* =====================================================
   TOP PANEL BUTTONS
   ===================================================== */

QPushButton#btnLanguageEnglish,
QPushButton#btnLanguageFinnish,
QPushButton#btnLanguagePolish,
QPushButton#btnContrast {
    border-radius: 12px;
    padding: 6px 14px;
    font-size: 16px;
    font-weight: 700;
    min-width: 20px;
    min-height: 15px;
}


/* =====================================================
   LANGUAGE BUTTONS
   ===================================================== */

QPushButton#btnLanguageEnglish,
QPushButton#btnLanguageFinnish,
QPushButton#btnLanguagePolish {
    background-color: rgba(255, 255, 255, 0.55);
    color: #1E293B;
    border: 1px solid rgba(30, 41, 59, 0.35);
}

QPushButton#btnLanguageEnglish:hover,
QPushButton#btnLanguageFinnish:hover,
QPushButton#btnLanguagePolish:hover {
    background-color: rgba(255, 255, 255, 0.90);
    border: 1px solid rgba(30, 41, 59, 0.55);
    color: #0F172A;
}

QPushButton#btnLanguageEnglish:checked,
QPushButton#btnLanguageFinnish:checked,
QPushButton#btnLanguagePolish:checked {
    background-color: #E83E6D;
    border: 1px solid #E83E6D;
    color: #FFFFFF;
}


/* =====================================================
   CONTRAST BUTTON
   ===================================================== */

QPushButton#btnContrast {
    background-color: #FACC15;
    color: #111827;
    border: 1px solid #EAB308;
}

QPushButton#btnContrast:hover {
    background-color: #FDE047;
    border: 1px solid #EAB308;
    color: #111827;
}

QPushButton#btnContrast:pressed {
    background-color: #EAB308;
    border: 1px solid #CA8A04;
    color: #111827;
}

QPushButton#btnContrast:checked {
    background-color: #111827;
    color: #FACC15;
    border: 1px solid #FACC15;
}


/* =====================================================
   TOUCH PANEL
   ===================================================== */

#TouchPanel {
    background-color: #E5E7EB;
    border-top: 2px solid #CBD5E1;
}


/* =====================================================
   SIDE ACTION BUTTONS
   ===================================================== */

QPushButton#button_1red_CANCEL,
QPushButton#button_2yellow_CLEAR,
QPushButton#button_3green_OK {
    font-size: 20px;
    font-weight: 700;
    border-radius: 12px;
    color: white;
}

QPushButton#button_1red_CANCEL {
    background-color: #C62828;
}
QPushButton#button_1red_CANCEL:hover {
    background-color: #E53935;
}
QPushButton#button_1red_CANCEL:pressed {
    background-color: #8E0000;
}

QPushButton#button_2yellow_CLEAR {
    background-color: #F9A825;
}
QPushButton#button_2yellow_CLEAR:hover {
    background-color: #FDD835;
}
QPushButton#button_2yellow_CLEAR:pressed {
    background-color: #C17900;
}

QPushButton#button_3green_OK {
    background-color: #2E7D32;
}
QPushButton#button_3green_OK:hover {
    background-color: #43A047;
}
QPushButton#button_3green_OK:pressed {
    background-color: #1B5E20;
}


/* =====================================================
   NUMPAD
   ===================================================== */

QPushButton#num_0,
QPushButton#num_1,
QPushButton#num_2,
QPushButton#num_3,
QPushButton#num_4,
QPushButton#num_5,
QPushButton#num_6,
QPushButton#num_7,
QPushButton#num_8,
QPushButton#num_9,
QPushButton#num_left,
QPushButton#num_right {
    background-color: #2F3A4D;
    color: #FFFFFF;
    border-radius: 18px;
    font-size: 26px;
    font-weight: 800;
}

QPushButton#num_0:hover,
QPushButton#num_1:hover,
QPushButton#num_2:hover,
QPushButton#num_3:hover,
QPushButton#num_4:hover,
QPushButton#num_5:hover,
QPushButton#num_6:hover,
QPushButton#num_7:hover,
QPushButton#num_8:hover,
QPushButton#num_9:hover,
QPushButton#num_left:hover,
QPushButton#num_right:hover {
    background-color: #3E4B61;
}

QPushButton#num_0:pressed,
QPushButton#num_1:pressed,
QPushButton#num_2:pressed,
QPushButton#num_3:pressed,
QPushButton#num_4:pressed,
QPushButton#num_5:pressed,
QPushButton#num_6:pressed,
QPushButton#num_7:pressed,
QPushButton#num_8:pressed,
QPushButton#num_9:pressed,
QPushButton#num_left:pressed,
QPushButton#num_right:pressed {
    background-color: #1F2937;
}


/* =====================================================
   MAIN ACTION BUTTONS
   ===================================================== */

QPushButton#btn_main_choice_1,
QPushButton#btn_main_choice_2,
QPushButton#btn_main_choice_3,
QPushButton#btn_main_choice_4,
QPushButton#btn_main_choice_5,
QPushButton#btn_main_choice_6,
QPushButton#btn_main_choice_7,
QPushButton#btn_main_choice_8,
QPushButton#Balance_btn_choice_1,
QPushButton#Balance_btn_choice_2,
QPushButton#btn_amount_choice_1,
QPushButton#btn_amount_choice_2,
QPushButton#btn_amount_choice_3,
QPushButton#btn_amount_choice_4,
QPushButton#btn_donation_choice_1,
QPushButton#btn_donation_choice_2,
QPushButton#btn_donation_choice_3,
QPushButton#btn_donation_choice_4 {
    background: qlineargradient(
        x1:0, y1:0, x2:1, y2:1,
        stop:0 #F472B6,
        stop:1 #E83E6D
    );
    color: #FFFFFF;
    border-radius: 16px;
    padding: 16px;
    font-weight: 800;
}

QPushButton#btn_main_choice_1,
QPushButton#btn_main_choice_2,
QPushButton#btn_main_choice_3 {
    font-size: 12px;
    font-weight: 800;
}

QPushButton#btn_main_choice_1:hover,
QPushButton#btn_main_choice_2:hover,
QPushButton#btn_main_choice_3:hover,
QPushButton#btn_main_choice_4:hover,
QPushButton#btn_main_choice_5:hover,
QPushButton#btn_main_choice_6:hover,
QPushButton#btn_main_choice_7:hover,
QPushButton#btn_main_choice_8:hover,
QPushButton#Balance_btn_choice_1:hover,
QPushButton#Balance_btn_choice_2:hover,
QPushButton#btn_amount_choice_1:hover,
QPushButton#btn_amount_choice_2:hover,
QPushButton#btn_amount_choice_3:hover,
QPushButton#btn_amount_choice_4:hover,
QPushButton#btn_donation_choice_1:hover,
QPushButton#btn_donation_choice_2:hover,
QPushButton#btn_donation_choice_3:hover,
QPushButton#btn_donation_choice_4:hover {
    background: qlineargradient(
        x1:0, y1:0, x2:1, y2:1,
        stop:0 #FB7185,
        stop:1 #EC4899
    );
}

QPushButton#btn_main_choice_1:pressed,
QPushButton#btn_main_choice_2:pressed,
QPushButton#btn_main_choice_3:pressed,
QPushButton#btn_main_choice_4:pressed,
QPushButton#btn_main_choice_5:pressed,
QPushButton#btn_main_choice_6:pressed,
QPushButton#btn_main_choice_7:pressed,
QPushButton#btn_main_choice_8:pressed,
QPushButton#Balance_btn_choice_1:pressed,
QPushButton#Balance_btn_choice_2:pressed,
QPushButton#btn_amount_choice_1:pressed,
QPushButton#btn_amount_choice_2:pressed,
QPushButton#btn_amount_choice_3:pressed,
QPushButton#btn_amount_choice_4:pressed,
QPushButton#btn_donation_choice_1:pressed,
QPushButton#btn_donation_choice_2:pressed,
QPushButton#btn_donation_choice_3:pressed,
QPushButton#btn_donation_choice_4:pressed {
    background: qlineargradient(
        x1:0, y1:0, x2:1, y2:1,
        stop:0 #DB2777,
        stop:1 #9D174D
    );
}

QPushButton#btn_donation_choice_1:checked,
QPushButton#btn_donation_choice_2:checked,
QPushButton#btn_donation_choice_3:checked,
QPushButton#btn_donation_choice_4:checked,
QPushButton#btn_amount_choice_1:checked,
QPushButton#btn_amount_choice_2:checked,
QPushButton#btn_amount_choice_3:checked,
QPushButton#btn_amount_choice_4:checked {
    background: qlineargradient(
        x1:0, y1:0, x2:1, y2:1,
        stop:0 #DB2777,
        stop:1 #9D174D
    );
}

)";

    /*
 * CONTRAST STYLE
 *
 * On success:
 * - Keeping it false by deafoult
 * - Changing from Light
 */

    contrastStyle = R"(

/* =====================================================
   APP BACKGROUND
   ===================================================== */

#centralwidget {
    background-color: #000000;
}


/* =====================================================
   DISPLAY AREA
   ===================================================== */

#display {
    background-color: #000000;
    border: 2px solid #FFFFFF;
    border-radius: 28px;
}


/* =====================================================
   STACKED PAGES
   ===================================================== */

#page1_Welcome,
#page2_Pin,
#page3_Main,
#page4_Withdraw,
#page5_Balance,
#page6_Transfer,
#page7_Donation,
#page8_Exit,
#page9_Other,
#page10_Error,
#page11_Time,
#page12_Accounts,
#page13_Transactions   {
    background: transparent;
}


/* =====================================================
   GLOBAL TEXT
   ===================================================== */

QLabel {
    color: #FFFFFF;
}


/* =====================================================
   PAGE TITLES
   ===================================================== */

QLabel#labelWelcome,
QLabel#labelWelcome_PIN,
QLabel#labelWelcome_Main,
QLabel#labelWelcome_Withdraw,
QLabel#labelWelcome_Balance,
QLabel#labelWelcome_Transfer,
QLabel#labelWelcome_Donation,
QLabel#labelWelcome_Exit,
QLabel#labelWelcome_Other,
QLabel#labelWelcome_Error,
QLabel#labelWelcome_Time,
QLabel#labelWelcome_Accounts,
QLabel#labelWelcome_Transactions
  {
    font-size: 40px;
    font-weight: 800;
    color: #FFFFFF;
}


/* =====================================================
   PAGE SUBTITLES
   ===================================================== */

QLabel#labelInstruction,
QLabel#labelInstruction_PIN,
QLabel#labelInstruction_Main,
QLabel#labelInstruction_Withdraw,
QLabel#labelInstruction_Balance,
QLabel#labelInstruction_Transfer,
QLabel#labelInstruction_Donation,
QLabel#labelInstruction_Exit,
QLabel#labelInstruction_Other,
QLabel#labelInstruction_Error,
QLabel#labelInstruction_Time,
QLabel#labelInstruction_Accounts,
QLabel#labelInstruction_Transactions   {
    font-size: 20px;
    font-weight: 600;
    color: #FFFFFF;
}

QLabel#foundersLabel {
    font-size: 9px;
    font-weight: 600;
    color: #FFFFFF;
}


/* =====================================================
   INPUTS
   ===================================================== */

QLineEdit#CardNumberDisplay {
    background-color: #000000;
    color: #FFFFFF;
    font-size: 18px;
    font-weight: bold;
    padding: 6px;
    border: 2px solid #FFFFFF;
    border-radius: 6px;
}

QLineEdit#pinInput {
    background-color: #000000;
    border: 2px solid #FFFFFF;
    border-radius: 16px;
    color: #FFFFFF;
    font-size: 28px;
    font-weight: 700;
    font-family: "Segoe UI", "Arial";
    padding: 12px 18px;
    letter-spacing: 12px;
}

QLineEdit#amountInput {
    background-color: transparent;
    border: none;
    color: #FFFFFF;
    font-size: 48px;
    font-weight: 800;
    font-family: "Segoe UI", "Arial";
    padding: 0px;
    margin: 0px;
}

QLabel#labelAmountCurrency {
    background-color: transparent;
    color: #FFFFFF;
    font-size: 26px;
    font-weight: 600;
    font-family: "Segoe UI", "Arial";
    padding: 0px;
    margin: 0px;
}


/* =====================================================
   BALANCE PAGE EXTRA
   ===================================================== */

QListWidget {
    background-color: transparent;
    border: 1px solid #FFFFFF;
    font-size: 16px;
    color: #FFFFFF;
}

QListWidget#Balance_ListRecentTransactions {
    font-size: 10px;
    color: #FFFFFF;
}

QLabel#Balance_Amount {
    font-size: 30px;
    font-weight: 800;
    color: #FFFFFF;
}


/* =====================================================
   GLOBAL BUTTON BASE
   ===================================================== */

QPushButton {
    padding: 10px;
    font-weight: bold;
    border: none;
}


/* =====================================================
   TOP PANEL BUTTONS
   ===================================================== */

QPushButton#btnLanguageEnglish,
QPushButton#btnLanguageFinnish,
QPushButton#btnLanguagePolish,
QPushButton#btnContrast {
    border-radius: 12px;
    padding: 6px 14px;
    font-size: 16px;
    font-weight: 700;
    min-width: 20px;
    min-height: 15px;
}


/* =====================================================
   LANGUAGE BUTTONS
   ===================================================== */

QPushButton#btnLanguageEnglish,
QPushButton#btnLanguageFinnish,
QPushButton#btnLanguagePolish {
    background-color: #111111;
    color: #FFFFFF;
    border: 1px solid #FFFFFF;
}

QPushButton#btnLanguageEnglish:hover,
QPushButton#btnLanguageFinnish:hover,
QPushButton#btnLanguagePolish:hover {
    background-color: #222222;
    color: #FFFFFF;
    border: 1px solid #FFFFFF;
}

QPushButton#btnLanguageEnglish:checked,
QPushButton#btnLanguageFinnish:checked,
QPushButton#btnLanguagePolish:checked {
    background-color: #FFFFFF;
    color: #000000;
    border: 1px solid #FFFFFF;
}


/* =====================================================
   CONTRAST BUTTON
   ===================================================== */

QPushButton#btnContrast {
    background-color: #FFFFFF;
    color: #000000;
    border: 2px solid #FFFFFF;
}

QPushButton#btnContrast:hover {
    background-color: #DDDDDD;
    color: #000000;
    border: 2px solid #FFFFFF;
}

QPushButton#btnContrast:pressed {
    background-color: #BBBBBB;
    color: #000000;
    border: 2px solid #FFFFFF;
}

QPushButton#btnContrast:checked {
    background-color: #FACC15;
    color: #000000;
    border: 2px solid #FACC15;
}


/* =====================================================
   TOUCH PANEL
   ===================================================== */

#TouchPanel {
    background-color: #111111;
    border-top: 2px solid #FFFFFF;
}


/* =====================================================
   SIDE ACTION BUTTONS
   ===================================================== */

QPushButton#button_1red_CANCEL,
QPushButton#button_2yellow_CLEAR,
QPushButton#button_3green_OK {
    font-size: 20px;
    font-weight: 700;
    border-radius: 12px;
    color: white;
}

QPushButton#button_1red_CANCEL {
    background-color: #C62828;
}
QPushButton#button_1red_CANCEL:hover {
    background-color: #E53935;
}
QPushButton#button_1red_CANCEL:pressed {
    background-color: #8E0000;
}

QPushButton#button_2yellow_CLEAR {
    background-color: #F9A825;
}
QPushButton#button_2yellow_CLEAR:hover {
    background-color: #FDD835;
}
QPushButton#button_2yellow_CLEAR:pressed {
    background-color: #C17900;
}

QPushButton#button_3green_OK {
    background-color: #2E7D32;
}
QPushButton#button_3green_OK:hover {
    background-color: #43A047;
}
QPushButton#button_3green_OK:pressed {
    background-color: #1B5E20;
}


/* =====================================================
   NUMPAD
   ===================================================== */

QPushButton#num_0,
QPushButton#num_1,
QPushButton#num_2,
QPushButton#num_3,
QPushButton#num_4,
QPushButton#num_5,
QPushButton#num_6,
QPushButton#num_7,
QPushButton#num_8,
QPushButton#num_9,
QPushButton#num_left,
QPushButton#num_right {
    background-color: #2F3A4D;
    color: #FFFFFF;
    border-radius: 18px;
    font-size: 26px;
    font-weight: 800;
    border: 1px solid #FFFFFF;
}

QPushButton#num_0:hover,
QPushButton#num_1:hover,
QPushButton#num_2:hover,
QPushButton#num_3:hover,
QPushButton#num_4:hover,
QPushButton#num_5:hover,
QPushButton#num_6:hover,
QPushButton#num_7:hover,
QPushButton#num_8:hover,
QPushButton#num_9:hover,
QPushButton#num_left:hover,
QPushButton#num_right:hover {
    background-color: #3E4B61;
}

QPushButton#num_0:pressed,
QPushButton#num_1:pressed,
QPushButton#num_2:pressed,
QPushButton#num_3:pressed,
QPushButton#num_4:pressed,
QPushButton#num_5:pressed,
QPushButton#num_6:pressed,
QPushButton#num_7:pressed,
QPushButton#num_8:pressed,
QPushButton#num_9:pressed,
QPushButton#num_left:pressed,
QPushButton#num_right:pressed {
    background-color: #1F2937;
}


/* =====================================================
   MAIN ACTION BUTTONS
   ===================================================== */

QPushButton#btn_main_choice_1,
QPushButton#btn_main_choice_2,
QPushButton#btn_main_choice_3,
QPushButton#btn_main_choice_4,
QPushButton#btn_main_choice_5,
QPushButton#btn_main_choice_6,
QPushButton#btn_main_choice_7,
QPushButton#btn_main_choice_8,
QPushButton#Balance_btn_choice_1,
QPushButton#Balance_btn_choice_2,
QPushButton#btn_amount_choice_1,
QPushButton#btn_amount_choice_2,
QPushButton#btn_amount_choice_3,
QPushButton#btn_amount_choice_4,
QPushButton#btn_donation_choice_1,
QPushButton#btn_donation_choice_2,
QPushButton#btn_donation_choice_3,
QPushButton#btn_donation_choice_4 {
    background: #000000;
    color: #FFFFFF;
    border: 2px solid #FFFFFF;
    border-radius: 16px;
    padding: 16px;
    font-weight: 800;
}

QPushButton#btn_main_choice_1,
QPushButton#btn_main_choice_2,
QPushButton#btn_main_choice_3 {
    font-size: 12px;
    font-weight: 800;
}

QPushButton#btn_main_choice_1:hover,
QPushButton#btn_main_choice_2:hover,
QPushButton#btn_main_choice_3:hover,
QPushButton#btn_main_choice_4:hover,
QPushButton#btn_main_choice_5:hover,
QPushButton#btn_main_choice_6:hover,
QPushButton#btn_main_choice_7:hover,
QPushButton#btn_main_choice_8:hover,
QPushButton#Balance_btn_choice_1:hover,
QPushButton#Balance_btn_choice_2:hover,
QPushButton#btn_amount_choice_1:hover,
QPushButton#btn_amount_choice_2:hover,
QPushButton#btn_amount_choice_3:hover,
QPushButton#btn_amount_choice_4:hover,
QPushButton#btn_donation_choice_1:hover,
QPushButton#btn_donation_choice_2:hover,
QPushButton#btn_donation_choice_3:hover,
QPushButton#btn_donation_choice_4:hover {
    background: #222222;
    color: #FFFFFF;
    border: 2px solid #FFFFFF;
}

QPushButton#btn_main_choice_1:pressed,
QPushButton#btn_main_choice_2:pressed,
QPushButton#btn_main_choice_3:pressed,
QPushButton#btn_main_choice_4:pressed,
QPushButton#btn_main_choice_5:pressed,
QPushButton#btn_main_choice_6:pressed,
QPushButton#btn_main_choice_7:pressed,
QPushButton#btn_main_choice_8:pressed,
QPushButton#Balance_btn_choice_1:pressed,
QPushButton#Balance_btn_choice_2:pressed,
QPushButton#btn_amount_choice_1:pressed,
QPushButton#btn_amount_choice_2:pressed,
QPushButton#btn_amount_choice_3:pressed,
QPushButton#btn_amount_choice_4:pressed,
QPushButton#btn_donation_choice_1:pressed,
QPushButton#btn_donation_choice_2:pressed,
QPushButton#btn_donation_choice_3:pressed,
QPushButton#btn_donation_choice_4:pressed {
    background: #FFFFFF;
    color: #000000;
    border: 2px solid #FFFFFF;
}

QPushButton#btn_donation_choice_1:checked,
QPushButton#btn_donation_choice_2:checked,
QPushButton#btn_donation_choice_3:checked,
QPushButton#btn_donation_choice_4:checked,
QPushButton#btn_amount_choice_1:checked,
QPushButton#btn_amount_choice_2:checked,
QPushButton#btn_amount_choice_3:checked,
QPushButton#btn_amount_choice_4:checked {
    background: #FFFFFF;
    color: #000000;
    border: 2px solid #FFFFFF;
}

)";
}


/*
 * Managing STYLES
 *
 */

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

