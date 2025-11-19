#ifndef WELCOMETAB_H
#define WELCOMETAB_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class WelcomeTab;
}

class WelcomeTab : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeTab(QWidget *parent = nullptr);
    ~WelcomeTab();
    
    void updateWaterLevel(int percent);

signals:
    void manualFillRequested();
    void settingsRequested(); 

private slots:
    void onManualFillClicked();
    void onSettingsClicked(); 

private:
    Ui::WelcomeTab *ui;
};

#endif // WELCOMETAB_H
