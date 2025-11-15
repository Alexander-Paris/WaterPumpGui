#include "onscreenkeyboard.h"
#include "ui_onscreenkeyboard.h"
#include <QPushButton>
#include <QButtonGroup>

OnScreenKeyboard::OnScreenKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnScreenKeyboard),
    m_inputButtons(new QButtonGroup(this))
{
    ui->setupUi(this);

    // --- Group Input Buttons ---
    // QWERTY Row
    m_inputButtons->addButton(ui->button_q); m_inputButtons->addButton(ui->button_w); m_inputButtons->addButton(ui->button_e);
    m_inputButtons->addButton(ui->button_r); m_inputButtons->addButton(ui->button_t); m_inputButtons->addButton(ui->button_y);
    m_inputButtons->addButton(ui->button_u); m_inputButtons->addButton(ui->button_i); m_inputButtons->addButton(ui->button_o);
    m_inputButtons->addButton(ui->button_p);
    // ASDF Row
    m_inputButtons->addButton(ui->button_a); m_inputButtons->addButton(ui->button_s); m_inputButtons->addButton(ui->button_d);
    m_inputButtons->addButton(ui->button_f); m_inputButtons->addButton(ui->button_g); m_inputButtons->addButton(ui->button_h);
    m_inputButtons->addButton(ui->button_j); m_inputButtons->addButton(ui->button_k); m_inputButtons->addButton(ui->button_l);
    // ZXCV Row
    m_inputButtons->addButton(ui->button_z); m_inputButtons->addButton(ui->button_x); m_inputButtons->addButton(ui->button_c);
    m_inputButtons->addButton(ui->button_v); m_inputButtons->addButton(ui->button_b); m_inputButtons->addButton(ui->button_n);
    m_inputButtons->addButton(ui->button_m);
    // Number Row
    m_inputButtons->addButton(ui->button_1); m_inputButtons->addButton(ui->button_2); m_inputButtons->addButton(ui->button_3);
    m_inputButtons->addButton(ui->button_4); m_inputButtons->addButton(ui->button_5); m_inputButtons->addButton(ui->button_6);
    m_inputButtons->addButton(ui->button_7); m_inputButtons->addButton(ui->button_8); m_inputButtons->addButton(ui->button_9);
    m_inputButtons->addButton(ui->button_0);
    // Symbols
    m_inputButtons->addButton(ui->button_hyphen);
    m_inputButtons->addButton(ui->button_dot);


    // Connect the group's signal to our slot
    connect(m_inputButtons, &QButtonGroup::buttonClicked, this, &OnScreenKeyboard::onInputButtonClicked);

    // Connect special keys
    connect(ui->buttonShift, &QPushButton::clicked, this, &OnScreenKeyboard::onShiftClicked);
    connect(ui->buttonSpace, &QPushButton::clicked, this, &OnScreenKeyboard::spacePressed);
    connect(ui->buttonBackspace, &QPushButton::clicked, this, &OnScreenKeyboard::backspacePressed);
    connect(ui->buttonEnter, &QPushButton::clicked, this, &OnScreenKeyboard::enterPressed);

    // Set initial state (lowercase)
    updateShiftState(false);
}

OnScreenKeyboard::~OnScreenKeyboard()
{
    delete ui;
}

void OnScreenKeyboard::onInputButtonClicked(QAbstractButton *button)
{
    if (button) {
        emit keyPressed(button->text());
    }
    if (m_shiftActive && button->text().length() == 1 && button->text().at(0).isLetter()) {
       updateShiftState(false);
       ui->buttonShift->setChecked(false); 
    }
}

 void OnScreenKeyboard::onShiftClicked()
{
    m_shiftActive = !m_shiftActive; 
    updateShiftState(m_shiftActive);
}

void OnScreenKeyboard::updateShiftState(bool shiftActive)
{
    m_shiftActive = shiftActive;
    for (QAbstractButton *button : m_inputButtons->buttons()) {
        QString currentText = button->text();
        if (currentText.length() == 1) { 
            QChar currentChar = currentText.at(0);
            if (currentChar.isLetter()) {
                button->setText(shiftActive ? currentChar.toUpper() : currentChar.toLower());
            }
        }
    }
}
