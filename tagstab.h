#ifndef TAGSTAB_H
#define TAGSTAB_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QHash>
#include <QVariant>

// Forward declare UI class
namespace Ui {
class TagsTab;
}

// Forward declare other classes
class QTableWidget;
class ControlBoardInterface; 

class TagsTab : public QWidget
{
    Q_OBJECT

public:
    explicit TagsTab(QWidget *parent = nullptr);
    ~TagsTab();

    bool isTagKnown(const QString &tagID) const;
    void setControlBoardInterface(ControlBoardInterface *interface); 

public slots:
    // Called by MainWindow when tag is placed AND is known to be new
    void onTagScanned(const QString &tagID, double preFillMass); 
    void saveTags();

private slots:
    void onDeleteClicked();

private:
    void addTagRow(const QString &uid, const QString &name, int maxVol, int emptyMass);
    void loadTags();
    QString getDataFilePath() const;

    Ui::TagsTab *ui;
    ControlBoardInterface *m_interface = nullptr;
};

#endif // TAGSTAB_H
