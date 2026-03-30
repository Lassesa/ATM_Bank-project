#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Select
    ui->btnLanguageEnglish->setCheckable(true);
    ui->btnLanguageFinnish->setCheckable(true);
    ui->btnLanguagePolish->setCheckable(true);

    // OneActive
    ui->btnLanguageEnglish->setAutoExclusive(true);
    ui->btnLanguageFinnish->setAutoExclusive(true);
    ui->btnLanguagePolish->setAutoExclusive(true);

    // Default
    ui->btnLanguageEnglish->setChecked(true);

    // Click
    connect(ui->btnLanguageEnglish, &QPushButton::clicked, this, [this]() {
        setLanguage("EN");
    });

    connect(ui->btnLanguagePolish, &QPushButton::clicked, this, [this]() {
        setLanguage("PL");
    });

    connect(ui->btnLanguageFinnish, &QPushButton::clicked, this, [this]() {
        setLanguage("FI");
    });

    // startowy język
    setLanguage("EN");
}

void MainWindow::setLanguage(const QString &lang)
{
    if (lang == "EN") {
        ui->labelWelcome->setText("Welcome to S/R Bank");
        ui->labelInstruction->setText("Insert your card to begin");
    }
    else if (lang == "PL") {
        ui->labelWelcome->setText("Witamy w S/R Bank");
        ui->labelInstruction->setText("Włóż kartę, aby rozpocząć");
    }
    else if (lang == "FI") {
        ui->labelWelcome->setText("Tervetuloa S/R Pankkiin");
        ui->labelInstruction->setText("Aseta kortti aloittaaksesi");
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
