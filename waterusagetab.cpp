#include "waterusagetab.h"
#include "ui_waterusagetab.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QMap>
#include <QHeaderView>
#include <QPushButton>

const QString USAGE_LOG_FILENAME = "water_usage_log.csv";

WaterUsageTab::WaterUsageTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaterUsageTab)
{
    ui->setupUi(this);
    ui->usageTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->refreshButton, &QPushButton::clicked, this, &WaterUsageTab::refreshData);
    refreshData();
}

WaterUsageTab::~WaterUsageTab()
{
    delete ui;
}

QString WaterUsageTab::getLogFilePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath); if (!dir.exists()) dir.mkpath(".");
    return dataPath + "/" + USAGE_LOG_FILENAME;
}

void WaterUsageTab::refreshData()
{
    QFile file(getLogFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->totalLabel->setText("0 mL");
        ui->usageTable->setRowCount(0);
        return;
    }

    QTextStream in(&file);
    QMap<QString, double> bottleTotals;
    double weeklyTotal = 0;
    qint64 sevenDaysAgo = QDateTime::currentMSecsSinceEpoch() - (7LL * 24 * 60 * 60 * 1000);

    // Skip the header line (timestamp, "name", volume)
    if (!in.atEnd()) in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(',');
        if (parts.size() >= 3) {
            bool tsOk, volOk;
            qint64 timestamp = parts[0].toLongLong(&tsOk);
            QString name = parts[1].trimmed().remove('"');
            double volume = parts[2].toDouble(&volOk);

            if (tsOk && volOk && timestamp >= sevenDaysAgo) {
                weeklyTotal += volume;
                bottleTotals[name] += volume;
            }
        }
    }
    file.close();

    ui->totalLabel->setText(QString::number(weeklyTotal, 'f', 0) + " mL");
    ui->usageTable->setRowCount(0);
    for (auto it = bottleTotals.begin(); it != bottleTotals.end(); ++it) {
        int row = ui->usageTable->rowCount();
        ui->usageTable->insertRow(row);
        ui->usageTable->setItem(row, 0, new QTableWidgetItem(it.key()));
        ui->usageTable->setItem(row, 1, new QTableWidgetItem(QString::number(it.value(), 'f', 0)));
    }
}
