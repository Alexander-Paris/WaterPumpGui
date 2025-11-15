#ifndef NEWBOTTLE_H
#define NEWBOTTLE_H

#include <QDialog>
#include <QLineEdit> 

// Forward declare UI class
namespace Ui {
class NewBottle;
}

class NewBottle : public QDialog
{
    Q_OBJECT

public:
    explicit NewBottle(const QString &tagUid, QWidget *parent = nullptr);
    ~NewBottle();

    QString getName() const;
    int getMaxVolume() const;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onInputKeyPressed(const QString &key);
    void onInputBackspacePressed();
    void onInputEnterPressed();
    void onInputSpacePressed(); 

private:
    Ui::NewBottle *ui;
    QLineEdit *m_focusedLineEdit = nullptr;

    void showKeyboard();
    void showKeypad();
    void hideInputs();
};

#endif // NEWBOTTLE_H
