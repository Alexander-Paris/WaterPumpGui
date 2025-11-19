#include "controlboardinterface.h"
#include <QRegularExpression>
#include <QDebug>
#include <QSerialPortInfo>

// --- UPDATE THESE IDS FOR YOUR SPECIFIC ARDUINO ---
const quint16 VENDOR_ID = 0x2341; // Standard Arduino VID
const quint16 PRODUCT_ID = 0x0043; // Standard Uno PID

ControlBoardInterface::ControlBoardInterface(QObject *parent)
    : QObject(parent)
{
    m_serialPort = new QSerialPort(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &ControlBoardInterface::onReadyRead);
}

ControlBoardInterface::~ControlBoardInterface() {}

bool ControlBoardInterface::isOpen() const { return m_serialPort->isOpen(); }

QString ControlBoardInterface::getPortName() const {
    return (m_serialPort && m_serialPort->isOpen()) ? m_serialPort->portName() : QString();
}

QStringList ControlBoardInterface::getAvailablePorts() {
    QStringList ports;
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) ports.append(info.portName());
    return ports;
}

bool ControlBoardInterface::autoConnect()
{
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        if (info.hasVendorIdentifier() && info.hasProductIdentifier() &&
            info.vendorIdentifier() == VENDOR_ID && info.productIdentifier() == PRODUCT_ID) {
            return connectToPort(info.portName());
        }
    }
    return false;
}

bool ControlBoardInterface::connectToPort(const QString &portName)
{
    if (m_serialPort->isOpen()) m_serialPort->close();
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        emit portConnected(portName);
        return true;
    }
    qCritical() << "CB: Connection failed:" << m_serialPort->errorString();
    return false;
}

void ControlBoardInterface::disconnectFromPort() { if (m_serialPort->isOpen()) m_serialPort->close(); }

void ControlBoardInterface::writeCommand(const QByteArray &command)
{
    if (isOpen() && m_serialPort->isWritable()) {
        qDebug() << "Pi -> CB:" << command;
        m_serialPort->write(command + "\n");
    }
}

void ControlBoardInterface::requestFill(double volume)
{
    // Protocol: F<FILL VOLUME> (e.g., F651.66)
    writeCommand("F" + QString::number(volume, 'f', 2).toUtf8());
}

void ControlBoardInterface::requestStop()
{
    // Protocol: STOP (Abort/Stop)
    writeCommand("STOP"); 
}

void ControlBoardInterface::onReadyRead()
{
    m_readBuffer.append(m_serialPort->readAll());
    while (m_readBuffer.contains('\n')) {
        int idx = m_readBuffer.indexOf('\n');
        QByteArray line = m_readBuffer.left(idx).trimmed();
        m_readBuffer = m_readBuffer.mid(idx + 1);
        if (line.isEmpty()) continue;
        qDebug() << "CB -> Pi:" << line;

        // --- 1. Tag Placed: TP<UID>PFM<MASS> (e.g., TP9AB05548PFM348.34) ---
        if (line.startsWith("TP")) {
            // Regex captures everything between TP and PFM, and everything after PFM
            static QRegularExpression re("TP(.*)PFM(.*)");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                QString uid = match.captured(1).trimmed();
                double pfm = match.captured(2).toDouble();
                emit tagPlaced(uid, pfm);
            }
        }
        // --- 2. Dispensed Volume: DV<VOL> (e.g., DV135.89) ---
        else if (line.startsWith("DV")) {
            bool ok;
            // Start at index 2 to skip "DV"
            double vol = line.mid(2).toDouble(&ok);
            
            // <<<--- YOUR FILE WAS LIKELY MISSING EVERYTHING AFTER THIS LINE ---<<<
            if (ok) emit dispensedVolumeReceived(vol);
        }
        // --- 3. Fill Status codes ---
        else if (line == "FS" || line == "FP" || line == "FF" || line == "FE" || line == "FA") {
            emit fillStatusReceived(line);
        }
        else if (line.startsWith("ERROR")) {
            emit controlBoardError(line);
        }
    } // <<<--- MISSING THIS BRACE
} // <<<--- MISSING THIS BRACE
