#ifndef MAINTAB_H
#define MAINTAB_H

#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>

namespace Ui { class MainTab; }

class MainTab : public QWidget
{
    Q_OBJECT
public:
    explicit MainTab(QWidget *parent = nullptr);
    ~MainTab();
    void setTagsTableWidget(QTableWidget *tableWidget);
    void setIceDensitySpinBox(QSpinBox *spinBox);

signals:
    void fillingStateChanged(bool isFilling);
    void fillComplete();
    void manualFillRequested(double volume);
    void backRequested();
    void fillRecorded(const QString &bottleName, double volume);
    void stopRequested();

public slots:
    void onTagScanned(const QString &tagID, double preFillMass);
    void onTagRemoved();
    void onFillStatusReceived(const QString &status);
    void onWaterLevelUpdate(int percent);
    void onDispensedVolumeReceived(double volume);

private slots:
    void onNumpadClicked();
    void onStartStopClicked();
    void onBackClicked();

private:
    void setFillingState(bool filling, const QString &statusText = "");
    void resetStartButton();
    Ui::MainTab *ui;
    QTableWidget *m_tagsTable = nullptr;
    double m_targetVolume = 0.0;
    bool m_isFilling = false;
    QSpinBox *m_iceDensitySpinBox = nullptr;
    QString m_currentBottleName = "Manual";
    QString m_currentTagID = "";
    double m_lastDispensedVolume = 0.0;
    double m_currentScaleMass = 0.0; 
};
#endif // MAINTAB_H
