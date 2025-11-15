#include "tagstab.h"
#include "ui_tagstab.h"
#include "newbottle.h"
#include "controlboardinterface.h" 
#include <QPushButton>
#include <QHeaderView>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QStringList>
#include <QTableWidget>
#include <QFont>
#include <QHBoxLayout>

const QString TAG_DATA_FILENAME = "bottle_tags.csv";

TagsTab::TagsTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagsTab)
{
    ui->setupUi(this);
    ui->tagsTable->setColumnCount(5);
    ui->tagsTable->setHorizontalHeaderLabels({ "UID", "NAME", "MAX-VOL", "EMPTY-MASS", "DELETE" });
    loadTags();
    ui->tagsTable->resizeColumnsToContents();
    ui->tagsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tagsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tagsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tagsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tagsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->tagsTable->verticalHeader()->setDefaultSectionSize(40);
    ui->tagsTable->verticalHeader()->setVisible(false);
}

TagsTab::~TagsTab()
{
    delete ui;
}

void TagsTab::setControlBoardInterface(ControlBoardInterface *interface) 
{
    m_interface = interface;
}

bool TagsTab::isTagKnown(const QString &tagID) const {
    if (!ui || !ui->tagsTable) {
        qWarning("isTagKnown: Table widget pointer is null!");
        return false;
    }
    for (int i = 0; i < ui->tagsTable->rowCount(); ++i) {
        QTableWidgetItem *item = ui->tagsTable->item(i, 0);
        if (item) {
            QString tableUid = item->text();
            if (QString::compare(tableUid, tagID, Qt::CaseInsensitive) == 0) {
                return true;
            }
        } 
    }
    return false;
}

QString TagsTab::getDataFilePath() const {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) dir.mkpath(".");
    return dataPath + "/" + TAG_DATA_FILENAME;
}

void TagsTab::loadTags() {
    QString filePath = getDataFilePath();
    QFile file(filePath);
    if (!file.exists()) {
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Load Error", "Could not open tag data file for reading:\n" + file.errorString());
        return;
    }
    QTextStream in(&file);
    bool headerSkipped = false;
    ui->tagsTable->setRowCount(0); 
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty()) continue;
        if (!headerSkipped && line.startsWith("\"UID\"")) {
             headerSkipped = true;
             continue;
        }
        QStringList parts = line.split(',');
        if (parts.size() == 4) {
            QString uid = parts[0].trimmed().remove('"');
            QString name = parts[1].trimmed().remove('"');
            bool volOk, massOk;
            int maxVol = parts[2].trimmed().toInt(&volOk);
            int emptyMass = parts[3].trimmed().toInt(&massOk);
            if (volOk && massOk) {
                addTagRow(uid, name, maxVol, emptyMass);
            } 
        } 
    }
    file.close();
}

void TagsTab::saveTags() {
    QString filePath = getDataFilePath();
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QMessageBox::warning(this, "Save Error", "Could not open tag data file for writing:\n" + file.errorString());
        return;
    }
    QTextStream out(&file);
    out << "\"UID\",\"Name\",\"MaxVol\",\"EmptyMass\"\n";
    for (int row = 0; row < ui->tagsTable->rowCount(); ++row) {
        QTableWidgetItem *uidItem = ui->tagsTable->item(row, 0);
        QTableWidgetItem *nameItem = ui->tagsTable->item(row, 1);
        QTableWidgetItem *volItem = ui->tagsTable->item(row, 2);
        QTableWidgetItem *massItem = ui->tagsTable->item(row, 3);
        if (uidItem && nameItem && volItem && massItem) {
            out << "\"" << uidItem->text() << "\","
                << "\"" << nameItem->text() << "\","
                << volItem->text() << ","
                << massItem->text() << "\n"; 
        }
    }
    file.close();
}

void TagsTab::addTagRow(const QString &uid, const QString &name, int maxVol, int emptyMass) {
    int row = ui->tagsTable->rowCount();
    ui->tagsTable->insertRow(row);
    QPushButton *deleteButton = new QPushButton(QString::fromUtf8("\uf014"));
    deleteButton->setProperty("class", "DeleteButton");
    deleteButton->setFont(QFont("Meslo LGS NF", 18));
    deleteButton->setStyleSheet(
        "QPushButton { border: none; background: transparent; color: #FF537b; padding: 0px; margin: 0px; }"
        "QPushButton:hover { color: #DDDDDD; }"
    );
    deleteButton->setCursor(Qt::PointingHandCursor);
    connect(deleteButton, &QPushButton::clicked, this, &TagsTab::onDeleteClicked);
    QWidget *buttonContainer = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(buttonContainer);
    layout->addWidget(deleteButton);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
    buttonContainer->setLayout(layout);
    QTableWidgetItem *uidItem = new QTableWidgetItem(uid);
    QTableWidgetItem *nameItem = new QTableWidgetItem(name);
    QTableWidgetItem *volItem = new QTableWidgetItem(QString::number(maxVol));
    QTableWidgetItem *massItem = new QTableWidgetItem(QString::number(emptyMass));
    uidItem->setFlags(uidItem->flags() & ~Qt::ItemIsEditable);
    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
    volItem->setFlags(volItem->flags() & ~Qt::ItemIsEditable);
    massItem->setFlags(massItem->flags() & ~Qt::ItemIsEditable);
    volItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    massItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tagsTable->setItem(row, 0, uidItem);
    ui->tagsTable->setItem(row, 1, nameItem);
    ui->tagsTable->setItem(row, 2, volItem);
    ui->tagsTable->setItem(row, 3, massItem);
    ui->tagsTable->setCellWidget(row, 4, buttonContainer);
}

void TagsTab::onDeleteClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        for (int row = 0; row < ui->tagsTable->rowCount(); ++row) {
            QWidget *container = ui->tagsTable->cellWidget(row, 4);
            if (container && container->findChild<QPushButton*>() == button) {
                ui->tagsTable->removeRow(row);
                break;
            }
        }
    }
}

void TagsTab::onTagScanned(const QString &tagID, double preFillMass) 
{
    
    if (isTagKnown(tagID)) {
        return;
    }

    NewBottle dialog(tagID, this); 

    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName();
        int maxVol = dialog.getMaxVolume();
        int emptyMass = preFillMass;

        addTagRow(tagID, name, maxVol, emptyMass);
    } 
}
