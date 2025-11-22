#include "maintab.h"
#include "ui_maintab.h"
#include <QMessageBox>
#include <QDebug>

MainTab::MainTab(QWidget *parent) : QWidget(parent), ui(new Ui::MainTab)
{
    ui->setupUi(this);
    connect(ui->startStopButton, &QPushButton::clicked, this, &MainTab::onStartStopClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &MainTab::onBackClicked);
    for (auto btn : ui->numpadFrame->findChildren<QPushButton*>())
        connect(btn, &QPushButton::clicked, this, &MainTab::onNumpadClicked);
    ui->waterRemainingLabel->setText(QString::fromUtf8("➔ WATER REMAINING: --- %"));
    onTagRemoved();
}

MainTab::~MainTab() { delete ui; }
void MainTab::setTagsTableWidget(QTableWidget *t) { m_tagsTable = t; }
void MainTab::setIceDensitySpinBox(QSpinBox *s) { m_iceDensitySpinBox = s; }

void MainTab::onTagScanned(const QString &tagID, double preFillMass)
{
    ui->manualVolumeGroup->hide(); ui->numpadFrame->hide(); ui->tagInfoGroup->show();
    ui->startStopButton->show(); ui->backButton->show(); ui->iceContainerWidget->show();
    ui->tagUidLabel->setText(tagID);

    m_currentTagID = tagID;
    m_currentBottleName = "Unknown Tag";
    QString maxVolStr = "N/A"; m_targetVolume = 0.0;
    
    // SAVE: Save the current mass reading from the scale
    m_currentScaleMass = preFillMass; 

    if (m_tagsTable) {
        for (int r = 0; r < m_tagsTable->rowCount(); ++r) {
            if (m_tagsTable->item(r, 0) && QString::compare(m_tagsTable->item(r, 0)->text(), tagID, Qt::CaseInsensitive) == 0) {
                m_currentBottleName = m_tagsTable->item(r, 1)->text();
                // Parse as double
                m_targetVolume = m_tagsTable->item(r, 2)->text().toDouble();
                maxVolStr = QString::number(m_targetVolume, 'f', 2) + " mL";
                break;
            }
        }
    }
    ui->tagNameLabel->setText(m_currentBottleName);
    ui->tagVolLabel->setText(maxVolStr);
    // Display mass with 2 decimal places
    ui->tagEmptyLabel->setText(QString::number(preFillMass, 'f', 2) + " g");
    resetStartButton();
}

void MainTab::onTagRemoved()
{
    ui->manualVolumeGroup->show(); ui->numpadFrame->show(); ui->manualVolumeLineEdit->clear();
    ui->tagInfoGroup->hide(); ui->iceContainerWidget->hide(); ui->startStopButton->show(); ui->backButton->show();
    m_currentBottleName = "Manual Fill"; m_currentTagID = "";
    // Reset the current scale reading
    m_currentScaleMass = 0.0; 
    resetStartButton();
}

void MainTab::onNumpadClicked() {
    QPushButton *b = qobject_cast<QPushButton*>(sender());
    if(b) (b->text()=="<-") ? ui->manualVolumeLineEdit->backspace() : ui->manualVolumeLineEdit->insert(b->text());
}

void MainTab::onStartStopClicked()
{
    if (m_isFilling) {
        emit stopRequested();   // 1. Send STOP command
        setFillingState(false); // 2. Reset UI buttons
        return;
    }
    
    // 1. Get Base Volume
    double vol = ui->numpadFrame->isVisible() ? ui->manualVolumeLineEdit->text().toDouble() : m_targetVolume;
    if (vol <= 0) return;

    // 2. Adaptive Ice Calculation Logic
    if (!m_currentTagID.isEmpty() && ui->iceCheckBox->isChecked()) {
        
        double emptyMass = 0.0;
        if (m_tagsTable) {
            for (int r = 0; r < m_tagsTable->rowCount(); ++r) {
                if (m_tagsTable->item(r, 0) && QString::compare(m_tagsTable->item(r, 0)->text(), m_currentTagID, Qt::CaseInsensitive) == 0) {
                    emptyMass = m_tagsTable->item(r, 3)->text().toDouble(); 
                    break;
                }
            }
        }
        
        double iceMass = m_currentScaleMass - emptyMass;
        
        if (iceMass > 0.0) {
            float densityMultiplier = m_iceDensitySpinBox ? m_iceDensitySpinBox->value() / 100.0f : 0.92f;
            double reductionVolume = iceMass * (1.0 - densityMultiplier);
            vol = vol - reductionVolume;
        }

        if (vol < 0) vol = 0; 
    }

    // 3. Start Fill Process
    m_lastDispensedVolume = 0.0;
    setFillingState(true, "STOP"); 
    emit manualFillRequested(vol); 
}

void MainTab::onFillStatusReceived(const QString &status)
{
    if (!m_isFilling) return;
    setFillingState(false);

    // FIX: Log the volume for Success, Partial, OR Aborted/Stopped
    if (status == "FS" || status == "FP" || status == "FA") {
        
        // Use the last known dispensed volume (prefer m_lastDispensedVolume over target)
        double volumeToLog = (m_lastDispensedVolume > 0) ? m_lastDispensedVolume : m_targetVolume;
        emit fillRecorded(m_currentBottleName, volumeToLog);

        // Show different messages based on the code
        if (status == "FS") {
            QMessageBox::information(this, "Done", "Fill successful!");
        }
        else if (status == "FP") {
            QMessageBox::warning(this, "Status", "Fill Partial (Water run out?)");
        }
        else if (status == "FA") {
            QMessageBox::warning(this, "Status", "Fill Stopped by User.");
        }
    }
    else {
        // Only Fails (FF) or Errors (FE) skip logging
        QMessageBox::critical(this, "Status", "Fill ended with error: " + status);
    }
    
    emit fillComplete();
}

void MainTab::onWaterLevelUpdate(int p) {
    ui->waterRemainingLabel->setText(QString::fromUtf8("➔ WATER REMAINING: %1 %").arg(p));
}

void MainTab::onDispensedVolumeReceived(double v) {
    m_lastDispensedVolume = v;
    
    // Race Condition Fix: Only update text if still filling
    if (m_isFilling) {
        ui->startStopButton->setText(QString::fromUtf8("STOP ✕ (%1 mL)").arg(v, 0, 'f', 2));
    }
}

void MainTab::setFillingState(bool f, const QString &s)
{
    m_isFilling = f; ui->startStopButton->setProperty("filling", f);
    ui->backButton->setEnabled(!f); ui->iceCheckBox->setEnabled(!f);
    ui->numpadFrame->setEnabled(!f); ui->manualVolumeLineEdit->setEnabled(!f);
    emit fillingStateChanged(f);
    if (f) ui->startStopButton->setText(s); else resetStartButton();
    style()->unpolish(ui->startStopButton); style()->polish(ui->startStopButton);
}

void MainTab::resetStartButton()
{
    ui->startStopButton->setText(QString::fromUtf8("START ➔"));
    ui->startStopButton->setProperty("filling", false);
    ui->iceCheckBox->setEnabled(true);
    style()->unpolish(ui->startStopButton); style()->polish(ui->startStopButton);
}
void MainTab::onBackClicked() { emit backRequested(); }
