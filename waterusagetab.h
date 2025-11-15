#ifndef WATERUSAGETAB_H
#define WATERUSAGETAB_H

#include <QWidget>

namespace Ui {
class WaterUsageTab;
}

class WaterUsageTab : public QWidget
{
    Q_OBJECT

public:
    explicit WaterUsageTab(QWidget *parent = nullptr);
    ~WaterUsageTab();

public slots:
    void refreshData();

private:
    Ui::WaterUsageTab *ui;
    QString getLogFilePath() const;
};

#endif // WATERUSAGETAB_H
