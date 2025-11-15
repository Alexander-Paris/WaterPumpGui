#ifndef ONSCREENKEYBOARD_H
#define ONSCREENKEYBOARD_H

#include <QWidget>
#include <QButtonGroup> 
#include <QAbstractButton>

namespace Ui {
class OnScreenKeyboard;
}

class OnScreenKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit OnScreenKeyboard(QWidget *parent = nullptr);
    ~OnScreenKeyboard();

signals:
    void keyPressed(const QString &key); 
    void backspacePressed();
    void enterPressed();
    void spacePressed();

private slots:
    void onInputButtonClicked(QAbstractButton *button);
    void onShiftClicked();

private:
    void updateShiftState(bool shiftActive);

    Ui::OnScreenKeyboard *ui;
    QButtonGroup *m_inputButtons; 
    bool m_shiftActive = false;
};

#endif // ONSCREENKEYBOARD_H
