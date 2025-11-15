#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QStackedWidget>
#include <QSpinBox> 

// --- FIX: FORWARD DECLARATIONS ONLY ---
// This prevents the compiler from getting stuck in a loop trying to define the classes.
class ControlBoardInterface; 
class BaseStationInterface;
class WelcomeTab;
// -------------------------------------

namespace Ui { class MainWindow; }
class QTabWidget; class MainTab; class SettingsTab; class TagsTab; class WaterUsageTab;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateClock();
    void onFillingStateChanged(bool isFilling);
    // --- NAVIGATION SLOTS ---
    void switchToWelcomeScreen();
    void switchToMainAppTabs();
    void switchToSettingsTab(); 

    // --- LOGGING SLOT ---
    void logFill(const QString &bottleName, double volume);

    // --- HARDWARE SLOTS ---
    void onTagPlaced(const QString &tagID, double preFillMass);
    void onFillStatus(const QString &status);
    void onWaterLevelUpdate(int percent);
    void requestPeriodicWaterLevelUpdate();
    
    void checkConnections(); 

private:
    Ui::MainWindow *ui;
    // Pointers are defined by the forward declarations above:
    ControlBoardInterface *m_controlBoard;
    BaseStationInterface *m_baseStation;
    QTimer *m_clockTimer;
    QLabel *m_clockLabel;
    QTimer *m_periodicUpdateTimer;
    
    QTimer *m_connectionTimer; 
    
    WelcomeTab *m_welcomeTab;
    QStackedWidget *m_stackedWidget;
    QTabWidget *m_mainAppTabs;
    MainTab *m_mainTab;
    SettingsTab *m_settingsTab;
    TagsTab *m_tagsTab;
    WaterUsageTab *m_usageTab;
};
#endif // MAINWINDOW_H
