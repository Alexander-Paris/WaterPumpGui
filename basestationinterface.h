#ifndef BASESTATIONINTERFACE_H
#define BASESTATIONINTERFACE_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

class BaseStationInterface : public QObject
{
    Q_OBJECT
public:
    explicit BaseStationInterface(QObject *parent = nullptr);
    ~BaseStationInterface();

    bool connectToPort(const QString &portName);
    QString getPortName() const; 
    static QStringList getAvailablePorts();
    bool isOpen() const;
    
    bool autoConnect();

signals:
    void waterLevelUpdate(int percent); // CWL:<%>
    void portConnected(const QString &portName);

public slots:
    void requestPeriodicUpdate(); // Send PU
    void requestFastUpdate();     // Send FUR
    void requestUpdateStop();     // Send FUT

private slots:
    void onReadyRead();

private:
    void writeCommand(const QByteArray &command);
    QSerialPort *m_serialPort;
    QByteArray m_readBuffer;
    
    QString m_lastPortName; // Remembers the last successful port
};

#endif // BASESTATIONINTERFACE_H
