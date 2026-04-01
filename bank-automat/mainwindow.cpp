#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
        // Clear only works on the PIN page for now
        if (ui->display->currentWidget() == ui->page2_Pin) {
            QString text = ui->pinInput->text();
            text.chop(1); // Remove last character
            ui->pinInput->setText(text);
        }
    });

    // Connect Cancel button
    connect(ui->button_1red_CANCEL, &QPushButton::clicked, this, [this]() {
        // Return from PIN page to Welcome page
        if (ui->display->currentWidget() == ui->page2_Pin) {
            ui->pinInput->clear();
            ui->display->setCurrentWidget(ui->page1_Welcome);
        }
        // Return from Main Menu to Welcome page
        else if (ui->display->currentWidget() == ui->page3_Main) {
            ui->display->setCurrentWidget(ui->page1_Welcome);
        }
    });

    // Connect OK button
    connect(ui->button_3green_OK, &QPushButton::clicked, this, [this]() {
        // Simulate card insertion: Welcome -> PIN page
        if (ui->display->currentWidget() == ui->page1_Welcome) {
            ui->display->setCurrentWidget(ui->page2_Pin);
            currentMode = PinMode;
            ui->pinInput->clear();
            ui->pinInput->setFocus();
        }
        // Check PIN on PIN page
        else if (ui->display->currentWidget() == ui->page2_Pin) {
            if (ui->pinInput->text() == "1234") {
                ui->pinInput->clear();
                ui->display->setCurrentWidget(ui->page3_Main);
            } else {
                // Wrong PIN: clear input and stay on PIN page
                ui->pinInput->clear();
            }
        }
    });

    // Set initial language
    setLanguage("EN");

    // Show Welcome page at startup
    ui->display->setCurrentWidget(ui->page1_Welcome);
}

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
    }
}

void MainWindow::handleDigit(const QString &digit)
{
    // Only allow PIN input on the PIN page
    if (ui->display->currentWidget() == ui->page2_Pin) {
        QString currentText = ui->pinInput->text();

        if (currentText.length() < 4) {
            ui->pinInput->setText(currentText + digit);
        }
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

MainWindow::~MainWindow()
{
    delete ui;
}
