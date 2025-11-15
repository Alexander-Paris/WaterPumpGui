#include "basestationinterface.h"
#include <QSerialPortInfo>
#include <QDebug>

// Standard Raspberry Pi Pico VID/PID for Serial CDC port
const quint16 PICO_VENDOR_ID = 0x2E8A; // Raspberry Pi Foundation
const quint16 PICO_PRODUCT_ID = 0x000A; // CDC Serial/MicroPython/CircuitPython

BaseStationInterface::BaseStationInterface(QObject *parent)
    : QObject(parent), m_lastPortName("")
{
    m_serialPort = new QSerialPort(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &BaseStationInterface::onReadyRead);
}

BaseStationInterface::~BaseStationInterface() {}

bool BaseStationInterface::isOpen() const { return m_serialPort->isOpen(); }

QString BaseStationInterface::getPortName() const {
    return (m_serialPort && m_serialPort->isOpen()) ? m_serialPort->portName() : QString();
}

QStringList BaseStationInterface::getAvailablePorts() {
    QStringList ports;
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) ports.append(info.portName());
    return ports;
}

bool BaseStationInterface::connectToPort(const QString &portName)
{
    if (m_serialPort->isOpen()) m_serialPort->close();
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        m_lastPortName = portName; // Remember this port on success
        emit portConnected(portName);
        return true;
    }
    return false;
}

bool BaseStationInterface::autoConnect()
{
    // 1. If connected already, we are good.
    if (isOpen()) {
        return true;
    }
    
    // 2. Scan for the specific Pico VID/PID
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        if (info.hasVendorIdentifier() && info.hasProductIdentifier() &&
            info.vendorIdentifier() == PICO_VENDOR_ID && info.productIdentifier() == PICO_PRODUCT_ID) 
        {
            return connectToPort(info.portName());
        }
    }

    // 3. Fallback: If we failed the VID/PID scan, try the last port manually selected (re-healing)
    if (!m_lastPortName.isEmpty()) {
        return connectToPort(m_lastPortName);
    }
    
    return false;
}

void BaseStationInterface::writeCommand(const QByteArray &command)
{
    if (isOpen() && m_serialPort->isWritable()) {
        qDebug() << "Pi -> BS:" << command;
        m_serialPort->write(command + "\n");
    }
}

void BaseStationInterface::requestPeriodicUpdate() { writeCommand("PU"); }
void BaseStationInterface::requestFastUpdate()     { writeCommand("FUR"); }
void BaseStationInterface::requestUpdateStop()     { writeCommand("FUT"); }

void BaseStationInterface::onReadyRead()
{
    m_readBuffer.append(m_serialPort->readAll());
    while (m_readBuffer.contains('\n')) {
        int idx = m_readBuffer.indexOf('\n');
        QByteArray line = m_readBuffer.left(idx).trimmed();
        m_readBuffer = m_readBuffer.mid(idx + 1);
        if (line.isEmpty()) continue;
        qDebug() << "BS -> Pi:" << line;

        // Protocol: WLU<%> or CWL<%> (e.g., WLU65, CWL64) - NO COLONS
        if (line.startsWith("WLU")) {
            // Start at index 3 to skip "WLU"
            emit waterLevelUpdate(line.mid(3).toInt());
        } else if (line.startsWith("CWL")) {
            // Start at index 3 to skip "CWL"
            emit waterLevelUpdate(line.mid(3).toInt());
        }
    }
}
