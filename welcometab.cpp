#include "welcometab.h"
#include "ui_welcometab.h"
#include <QFont>

WelcomeTab::WelcomeTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WelcomeTab)
{
    ui->setupUi(this);

    // Connect the buttons to their slots
    connect(ui->manualFillButton, &QPushButton::clicked, this, &WelcomeTab::onManualFillClicked);
    connect(ui->settingsButton, &QPushButton::clicked, this, &WelcomeTab::onSettingsClicked);

    QFont font = ui->settingsButton->font();
    font.setPointSize(24);
    ui->settingsButton->setFont(font);
}

WelcomeTab::~WelcomeTab()
{
    delete ui;
}

void WelcomeTab::onManualFillClicked()
{
    emit manualFillRequested();
}

void WelcomeTab::onSettingsClicked()
{
    emit settingsRequested();
}
