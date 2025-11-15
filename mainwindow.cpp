#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTableWidget>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QSpinBox> 

// --- FIX: ALL NECESSARY TAB AND INTERFACE HEADERS ARE INCLUDED HERE ---
#include "controlboardinterface.h" 
#include "basestationinterface.h"
#include "welcometab.h"
#include "maintab.h"
#include "settingstab.h"
#include "tagstab.h"
#include "waterusagetab.h"
// ----------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Instantiation now works because ControlBoardInterface is fully defined
    m_controlBoard = new ControlBoardInterface(this);
    if (!m_controlBoard->autoConnect()) {
        qWarning("CB: Auto-connect failed.");
    }
    m_baseStation = new BaseStationInterface(this);

    m_stackedWidget = new QStackedWidget(this); setCentralWidget(m_stackedWidget);
    m_welcomeTab = new WelcomeTab(this); m_stackedWidget->addWidget(m_welcomeTab);
    connect(m_welcomeTab, &WelcomeTab::manualFillRequested, this, &MainWindow::switchToMainAppTabs);
    connect(m_welcomeTab, &WelcomeTab::settingsRequested, this, &MainWindow::switchToSettingsTab);

    m_mainAppTabs = new QTabWidget(this); m_mainAppTabs->setObjectName("mainTabWidget");
    m_mainTab = new MainTab(this);
    m_settingsTab = new SettingsTab(this);
    m_tagsTab = new TagsTab(this);
    m_usageTab = new WaterUsageTab(this);

    m_mainAppTabs->addTab(m_mainTab, "FILL");
    m_mainAppTabs->addTab(m_settingsTab, "SETTINGS");
    m_mainAppTabs->addTab(m_tagsTab, "TAGS");
    m_mainAppTabs->addTab(m_usageTab, "USAGE");
    m_stackedWidget->addWidget(m_mainAppTabs);

    m_mainTab->setTagsTableWidget(m_tagsTab->findChild<QTableWidget*>("tagsTable"));
    m_clockLabel = new QLabel(this); m_clockLabel->setObjectName("clockLabel");
    m_mainAppTabs->setCornerWidget(m_clockLabel, Qt::TopRightCorner);
    m_clockTimer = new QTimer(this); connect(m_clockTimer, &QTimer::timeout, this, &MainWindow::updateClock); m_clockTimer->start(1000); updateClock();
    
    // Setup the 30-second PU timer
    m_periodicUpdateTimer = new QTimer(this); 
    connect(m_periodicUpdateTimer, &QTimer::timeout, this, &MainWindow::requestPeriodicWaterLevelUpdate); 
    m_periodicUpdateTimer->start(30000);
    
    // Send the first PU request immediately on launch.
    requestPeriodicWaterLevelUpdate();

    // Self-healing connection timer (checks every 30 seconds)
    m_connectionTimer = new QTimer(this);
    connect(m_connectionTimer, &QTimer::timeout, this, &MainWindow::checkConnections);
    m_connectionTimer->start(30000); 
    
    // Connect statements now work because the full class definitions are known
    connect(m_controlBoard, &ControlBoardInterface::tagPlaced, this, &MainWindow::onTagPlaced);
    connect(m_controlBoard, &ControlBoardInterface::fillStatusReceived, this, &MainWindow::onFillStatus);
    connect(m_controlBoard, &ControlBoardInterface::dispensedVolumeReceived, m_mainTab, &MainTab::onDispensedVolumeReceived);
    connect(m_baseStation, &BaseStationInterface::waterLevelUpdate, this, &MainWindow::onWaterLevelUpdate);
    connect(m_mainTab, &MainTab::manualFillRequested, m_controlBoard, &ControlBoardInterface::requestFill);
    
    connect(m_mainTab, &MainTab::fillingStateChanged, this, [this](bool f){
        if(f) { m_periodicUpdateTimer->stop(); m_baseStation->requestFastUpdate(); }
        else { m_baseStation->requestUpdateStop(); if(!m_periodicUpdateTimer->isActive()) m_periodicUpdateTimer->start(30000); }
        onFillingStateChanged(f);
    });
    connect(m_mainTab, &MainTab::fillComplete, this, &MainWindow::switchToWelcomeScreen);
    connect(m_mainTab, &MainTab::backRequested, this, &MainWindow::switchToWelcomeScreen);

    connect(m_mainTab, &MainTab::fillRecorded, this, &MainWindow::logFill);

    // Set Interface Pointers for Settings Tab
    m_settingsTab->setBaseStationInterface(m_baseStation);
    m_settingsTab->setControlBoardInterface(m_controlBoard); 
    
    connect(m_controlBoard, &ControlBoardInterface::portConnected, m_settingsTab, &SettingsTab::onControlBoardConnected);
    connect(m_baseStation, &BaseStationInterface::portConnected, m_settingsTab, &SettingsTab::onBaseStationConnected);
    connect(m_controlBoard, &ControlBoardInterface::tagPlaced, m_tagsTab, &TagsTab::onTagScanned);
    connect(qApp, &QApplication::aboutToQuit, m_tagsTab, &TagsTab::saveTags);

    QSpinBox *iceSpinBox = m_settingsTab->findChild<QSpinBox*>("iceDensitySpinBox");
    if (iceSpinBox) m_mainTab->setIceDensitySpinBox(iceSpinBox);

    m_stackedWidget->setCurrentWidget(m_welcomeTab); onFillingStateChanged(false);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::logFill(const QString &bottleName, double volume)
{
    if (volume <= 0) return;
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath); if (!dir.exists()) dir.mkpath(".");
    QFile file(dataPath + "/water_usage_log.csv");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QDateTime::currentMSecsSinceEpoch() << "," << "\"" << bottleName << "\"," << QString::number(volume, 'f', 2) << "\n";
        file.close();
        m_usageTab->refreshData();
    }
}

void MainWindow::requestPeriodicWaterLevelUpdate() { 
    if (m_baseStation && m_baseStation->isOpen()) {
        m_baseStation->requestPeriodicUpdate(); 
    } else {
        qDebug() << "PU Request Blocked: Base Station port closed.";
    }
}

void MainWindow::onTagPlaced(const QString &t, double p) { if (m_tagsTab->isTagKnown(t)) { switchToMainAppTabs(); m_mainTab->onTagScanned(t, p); } else { m_stackedWidget->setCurrentWidget(m_mainAppTabs); m_mainAppTabs->setCurrentIndex(2); } }
void MainWindow::onFillStatus(const QString &s) { m_mainTab->onFillStatusReceived(s); }
void MainWindow::onWaterLevelUpdate(int p) { m_mainTab->onWaterLevelUpdate(p); }
void MainWindow::updateClock() { m_clockLabel->setText(QDateTime::currentDateTime().toString("[HH:mm:ss]")); }
void MainWindow::onFillingStateChanged(bool f) { bool e=!f; if(m_mainAppTabs){ if(m_settingsTab)m_settingsTab->setEnabled(e); if(m_tagsTab)m_tagsTab->setEnabled(e); } }
void MainWindow::switchToMainAppTabs() { if(m_mainAppTabs){ m_stackedWidget->setCurrentWidget(m_mainAppTabs); m_mainAppTabs->setCurrentIndex(0); if(m_mainTab) m_mainTab->onTagRemoved(); } }

void MainWindow::switchToSettingsTab() { if(m_mainAppTabs){ m_stackedWidget->setCurrentWidget(m_mainAppTabs); m_mainAppTabs->setCurrentIndex(1); } }

void MainWindow::switchToWelcomeScreen() { m_baseStation->requestUpdateStop(); if(!m_periodicUpdateTimer->isActive()) m_periodicUpdateTimer->start(30000); if(m_welcomeTab) { m_stackedWidget->setCurrentWidget(m_welcomeTab); if(m_mainTab) m_mainTab->onTagRemoved(); } }

void MainWindow::checkConnections()
{
    // Check Control Board
    if (m_controlBoard && !m_controlBoard->isOpen()) {
        qDebug() << "Connection Timer: Control Board disconnected. Retrying auto-connect...";
        m_controlBoard->autoConnect();
    }

    // Check Base Station
    if (m_baseStation && !m_baseStation->isOpen()) {
        qDebug() << "Connection Timer: Base Station disconnected. Retrying auto-connect...";
        m_baseStation->autoConnect(); 
    }
}
