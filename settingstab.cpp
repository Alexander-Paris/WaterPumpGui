#include "settingstab.h"
#include "ui_settingstab.h"
#include "controlboardinterface.h"
#include "basestationinterface.h"
#include <QMessageBox>
#include <QDebug>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QProcess>
#include <QStringList>
#include <QPushButton>
#include <QComboBox>

SettingsTab::SettingsTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsTab)
{
    ui->setupUi(this);

    populateSerialPorts();
    updateConnectionState(); 

    m_networkUpdateTimer = new QTimer(this);
    connect(m_networkUpdateTimer, &QTimer::timeout, this, &SettingsTab::updateNetworkInfo);
    updateNetworkInfo();
    m_networkUpdateTimer->start(5000);

    // Connect Buttons
    connect(ui->connectButton_BS, &QPushButton::clicked, this, &SettingsTab::onBaseStationConnectClicked);
    connect(ui->connectButton_CB, &QPushButton::clicked, this, &SettingsTab::onControlBoardConnectClicked);
    
    // Connect Dropdown
    connect(ui->baseStationCombo, &QComboBox::currentTextChanged, this, &SettingsTab::onBaseStationPortSelected);


    ui->cbStatusLabel->setText("Disconnected");
    ui->bsStatusLabel->setText("Disconnected");
}

SettingsTab::~SettingsTab()
{
    delete ui;
}

void SettingsTab::setControlBoardInterface(ControlBoardInterface *interface)
{
    m_cbInterface = interface;
    if (m_cbInterface && m_cbInterface->isOpen()) {
        ui->controlBoardCombo->setCurrentText(m_cbInterface->getPortName());
    }
}

void SettingsTab::setBaseStationInterface(BaseStationInterface *interface)
{
    m_bsInterface = interface;
}

void SettingsTab::updateConnectionState()
{
    // Control Board Check
    if (m_cbInterface && m_cbInterface->isOpen()) {
        ui->cbStatusLabel->setText(QString("Connected to %1").arg(m_cbInterface->getPortName()));
        ui->controlBoardCombo->setCurrentText(m_cbInterface->getPortName());
    } else {
        ui->cbStatusLabel->setText("Disconnected");
    }

    // Base Station Check
    if (m_bsInterface && m_bsInterface->isOpen()) {
        ui->bsStatusLabel->setText(QString("Connected to %1").arg(m_bsInterface->getPortName()));
        ui->baseStationCombo->setCurrentText(m_bsInterface->getPortName());
    } else {
        ui->bsStatusLabel->setText("Disconnected");
    }
}


void SettingsTab::populateSerialPorts()
{
    ui->baseStationCombo->clear();
    ui->controlBoardCombo->clear();
    QStringList ports = BaseStationInterface::getAvailablePorts();
    ui->baseStationCombo->addItems(ports);
    ui->controlBoardCombo->addItems(ports);
    ui->baseStationCombo->insertItem(0, "");
    ui->controlBoardCombo->insertItem(0, "");
}

// Slot for Control Board (Arduino/RFID) connection
void SettingsTab::onControlBoardConnected(const QString &portName)
{
    updateConnectionState();
}

// Slot for Base Station (Water Level) connection
void SettingsTab::onBaseStationConnected(const QString &portName)
{
    updateConnectionState();
}

// Slot for when the dropdown selection changes
void SettingsTab::onBaseStationPortSelected(const QString &portName)
{
    // Connection is only initiated by the button click.
}

// Slot for the "Connect Base Station" button
void SettingsTab::onBaseStationConnectClicked()
{
    if (!m_bsInterface) {
        QMessageBox::warning(this, "Error", "Base Station interface is not initialized.");
        return;
    }
    QString portName = ui->baseStationCombo->currentText();
    if (portName.isEmpty()) {
        QMessageBox::warning(this, "Error", "No Base Station port selected.");
        return;
    }

    if (m_bsInterface->connectToPort(portName)) {
         QMessageBox::information(this, "Success", "Base Station port connected to " + portName);
    } else {
         QMessageBox::critical(this, "Error", "Failed to open Base Station port.");
    }
}

// Slot for the "Connect Control Board" button (Manual Override)
void SettingsTab::onControlBoardConnectClicked()
{
    if (!m_cbInterface) {
        QMessageBox::warning(this, "Error", "Control Board interface is not initialized.");
        return;
    }
    QString portName = ui->controlBoardCombo->currentText();
    if (portName.isEmpty()) {
        QMessageBox::warning(this, "Error", "No Control Board port selected.");
        return;
    }

    if (m_cbInterface->connectToPort(portName)) {
         QMessageBox::information(this, "Success", "Control Board port connected to " + portName);
    } else {
         QMessageBox::critical(this, "Error", "Failed to open Control Board port.");
    }
}

void SettingsTab::updateNetworkInfo()
{
    QString interfaceName = "N/A";
    QString ipAddress = "N/A";
    QString ssid = "N/A";
    QString hostname = QHostInfo::localHostName();
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsUp) &&
            iface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !iface.flags().testFlag(QNetworkInterface::IsLoopBack) &&
            !iface.flags().testFlag(QNetworkInterface::IsPointToPoint))
        {
            QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for (const QNetworkAddressEntry &entry : entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol && !entry.ip().isNull()) {
                    interfaceName = iface.name();
                    ipAddress = entry.ip().toString();
                    break;
                }
            }
            if (ipAddress != "N/A") break;
        }
    }
    if (interfaceName.startsWith("wlan")) {
        QProcess process;
        process.start("iwgetid", QStringList() << interfaceName << "-r");
        process.waitForFinished(1000);
        QString output = process.readAllStandardOutput().trimmed();
        if (!output.isEmpty()) ssid = output;
        else if (ipAddress != "N/A") ssid = "(Connected)";
        else ssid = "(Not Connected)";
    } else if (interfaceName.startsWith("eth")) {
        ssid = "(Wired)";
    }
    ui->ifaceLabel->setText(interfaceName);
    ui->ipLabel->setText(ipAddress);
    ui->ssidLabel->setText(ssid);
    ui->hostnameLabel->setText(hostname);
}
