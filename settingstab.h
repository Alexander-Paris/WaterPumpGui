#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include <QWidget>
#include <QTimer>
#include <QPushButton>


class ControlBoardInterface;
class BaseStationInterface;

namespace Ui {
class SettingsTab;
}

class SettingsTab : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTab(QWidget *parent = nullptr);
    ~SettingsTab();

    void setControlBoardInterface(ControlBoardInterface *interface);
    void setBaseStationInterface(BaseStationInterface *interface);

public slots:
    void onControlBoardConnected(const QString &portName);
    void onBaseStationConnected(const QString &portName);

private slots:
    void updateNetworkInfo();
    void onBaseStationConnectClicked();
    void onControlBoardConnectClicked();
    void onBaseStationPortSelected(const QString &portName); 

private:
    void populateSerialPorts();
    void updateConnectionState();

    Ui::SettingsTab *ui;
    ControlBoardInterface *m_cbInterface = nullptr;
    BaseStationInterface *m_bsInterface = nullptr;
    QTimer *m_networkUpdateTimer;
};

#endif // SETTINGSTAB_H
