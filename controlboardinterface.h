#ifndef CONTROLBOARDINTERFACE_H
#define CONTROLBOARDINTERFACE_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>

class ControlBoardInterface : public QObject
{
    Q_OBJECT
public:
    explicit ControlBoardInterface(QObject *parent = nullptr);
    ~ControlBoardInterface();

    bool autoConnect();
    bool connectToPort(const QString &portName);
    QString getPortName() const;
    bool isOpen() const;
    static QStringList getAvailablePorts();

signals:
    void portConnected(const QString &portName);
    void tagPlaced(const QString &uid, double preFillMass);
    void dispensedVolumeReceived(double volume);
    void fillStatusReceived(const QString &status);
    void controlBoardError(const QString &message);

public slots:
    void disconnectFromPort();
    void writeCommand(const QByteArray &command);
    void requestFill(double volume);
    void requestStop();

private slots:
    void onReadyRead();

private:
    QSerialPort *m_serialPort;
    QByteArray m_readBuffer;
};

#endif // CONTROLBOARDINTERFACE_H
