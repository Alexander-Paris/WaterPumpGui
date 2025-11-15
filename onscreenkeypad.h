#ifndef ONSCREENKEYPAD_H
#define ONSCREENKEYPAD_H

#include <QWidget>

namespace Ui {
class OnScreenKeypad;
}

class OnScreenKeypad : public QWidget
{
    Q_OBJECT

public:
    explicit OnScreenKeypad(QWidget *parent = nullptr);
    ~OnScreenKeypad();

signals:
    void keyPressed(const QString &key); // Emitted when 0-9 or '.' is pressed
    void backspacePressed();             // Emitted for backspace
    void enterPressed();                 // Emitted for Enter/OK

private slots:
    void onDigitButtonClicked();

private:
    Ui::OnScreenKeypad *ui;
};

#endif // ONSCREENKEYPAD_H
