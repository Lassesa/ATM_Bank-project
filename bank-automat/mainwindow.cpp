#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>
#include <QSerialPort>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))
{
    ui->setupUi(this);
    defaultStyle = this->styleSheet();

    // Start RFID / serial reader
    setupSerialReader();

    // Set application window icon
    this->setWindowIcon(QIcon(":/logo.svg/logo.svg"));

    // Set default input mode
    currentMode = PinMode;

    // Configure PIN input
    ui->pinInput->clear();
    ui->pinInput->setMaxLength(4);
    ui->pinInput->setEchoMode(QLineEdit::Password);

    // Configure amount input for future use
    ui->amountInput->setText("0 €");

    // Configure language buttons
    ui->btnLanguageEnglish->setCheckable(true);
    ui->btnLanguageFinnish->setCheckable(true);
    ui->btnLanguagePolish->setCheckable(true);

    ui->btnLanguageEnglish->setAutoExclusive(true);
    ui->btnLanguageFinnish->setAutoExclusive(true);
    ui->btnLanguagePolish->setAutoExclusive(true);

    // Set default language button
    ui->btnLanguageEnglish->setChecked(true);

    // Connect language buttons
    connect(ui->btnLanguageEnglish, &QPushButton::clicked, this, [this]() {
        setLanguage("EN");
    });

    connect(ui->btnLanguagePolish, &QPushButton::clicked, this, [this]() {
        setLanguage("PL");
    });

    connect(ui->btnLanguageFinnish, &QPushButton::clicked, this, [this]() {
        setLanguage("FI");
    });

    // Connect numpad buttons
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

    // Connect Clear button
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

    // Connect Cancel button
    connect(ui->button_1red_CANCEL, &QPushButton::clicked, this, [this]() {
        QWidget* current = ui->display->currentWidget();

        // Return from PIN page to Welcome page
        if (current == ui->page2_Pin ||
            current == ui->page1_Welcome) {
            ui->pinInput->clear();
            ui->display->setCurrentWidget(ui->page8_Exit);
        }
        // Return from Main Menu to Welcome page
        else if (current == ui->page3_Main) {
            ui->display->setCurrentWidget(ui->page8_Exit);
        }
        // Return from transaction pages to Main Menu
        else if (
            current == ui->page4_Withdraw ||
            current == ui->page5_Balance ||
            current == ui->page6_Transfer ||
            current == ui->page7_Donation ||
            current == ui->page9_Other
            ) {
            ui->display->setCurrentWidget(ui->page3_Main);
        }
    });


    // Connect OK button
    connect(ui->button_3green_OK, &QPushButton::clicked, this, [this]() {
        // Welcome page -> PIN page
        if (ui->display->currentWidget() == ui->page1_Welcome) {
            ui->display->setCurrentWidget(ui->page2_Pin);
            currentMode = PinMode;
            ui->pinInput->clear();
            ui->pinInput->setFocus();
        }
        // PIN page -> Main page
        else if (ui->display->currentWidget() == ui->page2_Pin) {
            if (ui->pinInput->text() == "1234") {
                ui->pinInput->clear();
                ui->display->setCurrentWidget(ui->page3_Main);
            } else {
                // Wrong PIN: clear input and stay on PIN page
                ui->pinInput->clear();
            }
        }
        // Withdraw page -> process amount
        else if (ui->display->currentWidget() == ui->page4_Withdraw) {
            QString amountText = ui->amountInput->text();
            amountText.remove("€");
            amountText = amountText.trimmed();

            bool ok;
            int amount = amountText.toInt(&ok);

            if (ok && amount > 0) {
                qDebug() << "Withdraw amount:" << amount;

                // Temporary behavior after successful input
                ui->amountInput->setText("0 €");
                ui->display->setCurrentWidget(ui->page3_Main);
            } else {
                // Invalid amount: reset field
                ui->amountInput->setText("0 €");
            }
        }
    });

    // MAIN MENU OPTIONS options
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
        showPage(ui->page9_Other);
    });


    // Set initial language
    setLanguage("EN");

    // Show Welcome page at startup
    ui->display->setCurrentWidget(ui->page1_Welcome);


    // CONTRAST

    connect(ui->btnContrast, &QPushButton::clicked, this, [this]() {
        highContrast = !highContrast;

        if (highContrast) {
            applyHighContrastTheme();
            ui->btnContrast->setText("☀️");
        } else {
            applyDefaultTheme();
            ui->btnContrast->setText("🌙");
        }
    });
}

// LANGUAGES
void MainWindow::setLanguage(const QString &lang)
{
    // Update selected language button state
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
    }
}

void MainWindow::handleDigit(const QString &digit)
{
    // PIN page
    if (ui->display->currentWidget() == ui->page2_Pin) {
        QString currentText = ui->pinInput->text();

        if (currentText.length() < 4) {
            ui->pinInput->setText(currentText + digit);
        }
    }
    // Withdraw page
    else if (ui->display->currentWidget() == ui->page4_Withdraw) {
        QString currentText = ui->amountInput->text();

        // remove euro sign and spaces
        currentText.remove("€");
        currentText = currentText.trimmed();

        if (currentText == "0") {
            currentText = "";
        }

        ui->amountInput->setText(currentText + digit + " €");
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Handle keyboard digits as numpad input
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {
        handleDigit(QString::number(event->key() - Qt::Key_0));
    }
    // Handle backspace like Clear
    else if (event->key() == Qt::Key_Backspace) {
        if (ui->display->currentWidget() == ui->page2_Pin) {
            QString text = ui->pinInput->text();
            text.chop(1);
            ui->pinInput->setText(text);
        }
    }
    // Handle Enter/Return like OK
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        ui->button_3green_OK->click();
    }
    // Handle Escape like Cancel
    else if (event->key() == Qt::Key_Escape) {
        ui->button_1red_CANCEL->click();
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::setupSerialReader()
{
    // Configure the serial port used by the RFID reader
    serial->setPortName("/dev/cu.usbmodem14521201");
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadOnly)) {
        // When new serial data arrives, process it
        connect(serial, &QSerialPort::readyRead,
                this, &MainWindow::readCardData);

        qDebug() << "Serial port opened successfully.";
    } else {
        qDebug() << "Failed to open serial port:" << serial->errorString();
    }
}

void MainWindow::readCardData()
{
    // Store incoming serial data until a complete line is received
    static QString buffer;
    buffer += QString::fromUtf8(serial->readAll());

    // Process complete lines from the buffer
    while (buffer.contains('\n')) {
        int endIndex = buffer.indexOf('\n');
        QString line = buffer.left(endIndex).trimmed();
        buffer.remove(0, endIndex + 1);

        qDebug() << "Received line:" << line;

        // Ignore empty lines and prompt characters
        if (line.isEmpty() || line == ">") {
            continue;
        }

        // Treat the received line as the scanned card value
        currentCardUid = line;

        qDebug() << "Scanned card:" << currentCardUid;

        // Stay on the welcome page
        ui->display->setCurrentWidget(ui->page1_Welcome);

        // Show status message
        ui->labelInstruction->setText("Card detected");

        // Show the most recently scanned card value
        ui->CardNumberDisplay->setText(currentCardUid);
        ui->CardNumberDisplay->update();

        qDebug() << "Displayed text:" << ui->CardNumberDisplay->text();
    }
}

void MainWindow::showPage(QWidget *page)
{
    ui->display->setCurrentWidget(page);
}


// Contrast Edit

void MainWindow::applyHighContrastTheme()
{

    applyMonitorStyleToPage(ui->page1_Welcome, true);
    applyMonitorStyleToPage(ui->page2_Pin, true);
    applyMonitorStyleToPage(ui->page3_Main, true);
    applyMonitorStyleToPage(ui->page4_Withdraw, true);
    applyMonitorStyleToPage(ui->page5_Balance, true);
    applyMonitorStyleToPage(ui->page6_Transfer, true);
    applyMonitorStyleToPage(ui->page7_Donation, true);
    applyMonitorStyleToPage(ui->page8_Exit, true);
    applyMonitorStyleToPage(ui->page9_Other, true);
}


void MainWindow::applyDefaultTheme()
{
    // Restore monitor background

    applyMonitorStyleToPage(ui->page1_Welcome, false);
    applyMonitorStyleToPage(ui->page2_Pin, false);
    applyMonitorStyleToPage(ui->page3_Main, false);
    applyMonitorStyleToPage(ui->page4_Withdraw, false);
    applyMonitorStyleToPage(ui->page5_Balance, false);
    applyMonitorStyleToPage(ui->page6_Transfer, false);
    applyMonitorStyleToPage(ui->page7_Donation, false);
    applyMonitorStyleToPage(ui->page8_Exit, false);
    applyMonitorStyleToPage(ui->page9_Other, true);

    ui->display->update();
}

void MainWindow::applyMonitorStyleToPage(QWidget *page, bool highContrastEnabled)
{
    if (!page) return;

    // Page background
    if (highContrastEnabled) {
        page->setStyleSheet("background-color: black;");
    } else {
        page->setStyleSheet("background: transparent;");
    }

    // Labels
    const auto labels = page->findChildren<QLabel *>();
    for (QLabel *label : labels) {
        if (highContrastEnabled) {
            label->setStyleSheet("color: white; background: transparent; border: none;");
        } else {
            label->setStyleSheet("");
        }
    }

    // LineEdits
    const auto edits = page->findChildren<QLineEdit *>();
    for (QLineEdit *edit : edits) {
        if (highContrastEnabled) {
            edit->setStyleSheet(
                "background-color: black;"
                "color: white;"
                "border: 2px solid white;"
                "border-radius: 10px;"
                "padding: 6px;"
                );
        } else {
            edit->setStyleSheet("");
        }
    }

    // ListWidgets
    const auto lists = page->findChildren<QListWidget *>();
    for (QListWidget *list : lists) {
        if (highContrastEnabled) {
            list->setStyleSheet(
                "background-color: black;"
                "color: white;"
                "border: 2px solid white;"
                );
        } else {
            list->setStyleSheet("");
        }
    }

    // Only buttons inside monitor pages
    const auto buttons = page->findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
        const QString name = button->objectName();

        // Do not touch numpad or system buttons
        if (name.startsWith("num_") ||
            name == "button_1red_CANCEL" ||
            name == "button_2yellow_CLEAR" ||
            name == "button_3green_OK") {
            continue;
        }

        if (highContrastEnabled) {
            button->setStyleSheet(
                "background-color: black;"
                "color: white;"
                "border: 2px solid white;"
                "border-radius: 14px;"
                );
        } else {
            button->setStyleSheet("");
        }
    }
}

void MainWindow::selectAmount(int amount)
{
    selectedAmount = amount;
    ui->amountInput->setText(formatAmount(amount));
    showPage(ui->page4_Withdraw);
}

QString MainWindow::formatAmount(int amount)
{
    return QString::number(amount) + " EUR";
}

MainWindow::~MainWindow()
{
    delete ui;
}
