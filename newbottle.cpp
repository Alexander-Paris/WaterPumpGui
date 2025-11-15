#include "newbottle.h"
#include "ui_newbottle.h" 
#include <QEvent>
#include <QIntValidator>
#include <QLineEdit>
#include "onscreenkeyboard.h" 
#include "onscreenkeypad.h" 
#include <QDebug> 

NewBottle::NewBottle(const QString &tagUid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewBottle)
{
    ui->setupUi(this);
    setWindowTitle("Add New Bottle Tag");
    ui->uidLabel->setText(tagUid);

    ui->maxVolLineEdit->setValidator(new QIntValidator(0, 9999, this));

    ui->nameLineEdit->installEventFilter(this);
    ui->maxVolLineEdit->installEventFilter(this);

    hideInputs(); 

    // Keyboard signals
    connect(ui->keyboardWidget, &OnScreenKeyboard::keyPressed, this, &NewBottle::onInputKeyPressed);
    connect(ui->keyboardWidget, &OnScreenKeyboard::backspacePressed, this, &NewBottle::onInputBackspacePressed);
    connect(ui->keyboardWidget, &OnScreenKeyboard::enterPressed, this, &NewBottle::onInputEnterPressed);
    connect(ui->keyboardWidget, &OnScreenKeyboard::spacePressed, this, &NewBottle::onInputSpacePressed);

    // Keypad signals
    connect(ui->keypadWidget, &OnScreenKeypad::keyPressed, this, &NewBottle::onInputKeyPressed);
    connect(ui->keypadWidget, &OnScreenKeypad::backspacePressed, this, &NewBottle::onInputBackspacePressed);
    connect(ui->keypadWidget, &OnScreenKeypad::enterPressed, this, &NewBottle::onInputEnterPressed);

}

NewBottle::~NewBottle()
{
    delete ui;
}

QString NewBottle::getName() const
{
    return ui ? ui->nameLineEdit->text() : QString();
}

int NewBottle::getMaxVolume() const
{
    return ui ? ui->maxVolLineEdit->text().toInt() : 0;
}

bool NewBottle::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
        if (lineEdit == ui->nameLineEdit) {
            m_focusedLineEdit = lineEdit;
            showKeyboard(); // Show alphanumeric keyboard
        } else if (lineEdit == ui->maxVolLineEdit) {
            m_focusedLineEdit = lineEdit;
            showKeypad(); // Show numeric keypad
        }
    }

    // Pass event along
    return QDialog::eventFilter(obj, event);
}

void NewBottle::showKeyboard() {
    ui->inputStackedWidget->setCurrentWidget(ui->keyboardWidget);
}

void NewBottle::showKeypad() {
    ui->inputStackedWidget->setCurrentWidget(ui->keypadWidget);
}

void NewBottle::hideInputs() {
    if (ui->inputStackedWidget->widget(0) == ui->placeholderPage) {
        ui->inputStackedWidget->setCurrentIndex(0); 
    } else {
        qWarning("Placeholder page not found at index 0 in inputStackedWidget!");
    }
}

void NewBottle::onInputKeyPressed(const QString &key)
{
    if (m_focusedLineEdit) {
        if (m_focusedLineEdit == ui->nameLineEdit && key == ".") {
            return;
        }
        m_focusedLineEdit->insert(key);
    }
}

void NewBottle::onInputBackspacePressed()
{
    if (m_focusedLineEdit) {
        m_focusedLineEdit->backspace();
    }
}

void NewBottle::onInputSpacePressed()
{
    if (m_focusedLineEdit == ui->nameLineEdit) {
        m_focusedLineEdit->insert(" ");
    }
}

void NewBottle::onInputEnterPressed()
{
    hideInputs();
    m_focusedLineEdit = nullptr;
    accept(); 
}
