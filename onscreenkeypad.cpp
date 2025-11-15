#include "onscreenkeypad.h"
#include "ui_onscreenkeypad.h"
#include <QPushButton>
#include <QDebug>

OnScreenKeypad::OnScreenKeypad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnScreenKeypad)
{
    ui->setupUi(this);

    // Connect all digit/dot buttons to the same slot
    connect(ui->button0, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->button1, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->button2, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->button3, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->button4, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->button5, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->button6, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->button7, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->button8, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->button9, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);
    connect(ui->buttonDot, &QPushButton::clicked, this, &OnScreenKeypad::onDigitButtonClicked);

    // Connect Backspace and Enter directly to their signals
    connect(ui->buttonBackspace, &QPushButton::clicked, this, &OnScreenKeypad::backspacePressed);
    connect(ui->buttonEnter, &QPushButton::clicked, this, &OnScreenKeypad::enterPressed);
}

OnScreenKeypad::~OnScreenKeypad()
{
    delete ui;
}

void OnScreenKeypad::onDigitButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        emit keyPressed(button->text());
    }
}
