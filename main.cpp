#include "mainwindow.h"
#include <QApplication>
#include <QFile>

// Global Stylesheet
const QString globalStyleSheet = R"(
    /* Main window and default widget background */
    QWidget {
        background-color: #212337;
        color: #DDDDDD;
        font-family: "Meslo LGS NF";
        font-size: 14pt;
    }

    /* === Tab Bar === */
    QTabWidget::pane { border: 1px solid #FF537b; border-top: none; }
    QTabBar::tab {
        background-color: #212337; color: #888; padding: 10px;
        border: 1px solid #FF537b; border-bottom: none; margin-right: 2px;
    }
    QTabBar::tab:selected { background-color: #FF537b; color: #DDDDDD; margin-bottom: -1px; }

    /* === Buttons === */
    QPushButton { color: #DDDDDD; border: 1px solid #FF537b; padding: 8px; min-height: 40px; }
    QPushButton:hover { border-color: #DDDDDD; }

    /* Start/Stop Button Colors */
    QPushButton#startStopButton[filling="false"] { background-color: #0D73CC; }
    QPushButton#startStopButton[filling="true"] { background-color: #FF537b; }
    QPushButton#startStopButton:disabled { background-color: #3a3d52; color: #888; }


    /* Numpad buttons */
    QPushButton.NumpadButton { background-color: #3a3d52; border: 1px solid #5a5d72; }
    QPushButton.NumpadButton:hover { background-color: #5a5d72; }
    
    /* === Back Button (Main Tab) === */
    QPushButton#backButton {
        background-color: #3a3d52; /* Same as numpad */
        border: 1px solid #5a5d72;
    }
    QPushButton#backButton:hover {
        background-color: #5a5d72;
        border-color: #DDDDDD;
    }
    QPushButton#backButton:disabled {
        background-color: #2a2d3a;
        color: #888;
        border-color: #3a3d52;
    }


    /* === Input Fields === */
    QLineEdit, QSpinBox, QComboBox { background-color: #FF537b; color: #DDDDDD; border: none; padding: 8px; }
    QComboBox::drop-down { border: none; }

    /* === Table View (Tags Tab) === */
    QTableWidget { gridline-color: #FF537b; }
    QHeaderView::section { background-color: #3a3d52; padding: 4px; }
    
    /* === GroupBox Borders (Settings Tab) === */
    QGroupBox {
        border: 1px solid #FF537b; border-radius: 4px;
        margin-top: 10px; padding: 10px;
    }
    QGroupBox::title {
        subcontrol-origin: margin; subcontrol-position: top left;
        padding: 0 5px 0 5px; left: 10px; color: #FF537b;
    }

    /* === Labels === */
    QLabel { background-color: transparent; }
    QLabel#clockLabel { color: #DDDDDD; font-size: 16pt; padding-right: 10px; }
    QLabel#waterRemainingLabel { font-size: 16pt; }
    QLabel#connectionStatusLabel { color: #0DCC73; }
    QLabel#cbStatusLabel { color: #0DCC73; } /* Style for Control Board status */

    /* === CheckBox Styling (Ice CheckBox) === */
    QCheckBox#iceCheckBox { margin-left: 10px; }
    QCheckBox#iceCheckBox::indicator {
        width: 30px; height: 30px; border: 2px solid #DDDDDD; background-color: #3a3d52;
    }
    QCheckBox#iceCheckBox::indicator:checked { background-color: #FF537b; }
    QCheckBox#iceCheckBox:disabled { color: #888; }
    QCheckBox#iceCheckBox::indicator:disabled { border-color: #5a5d72; }


    /* === Settings Gear Button (Welcome Screen) === */
    QPushButton#settingsButton {
        background-color: transparent; border: none; color: #0D73CC;
        font-size: 28pt; padding: 5px;
    }
    QPushButton#settingsButton:hover {
       color: #DDDDDD; background-color: #3a3d52; border: 1px solid #FF537b;
    }
)";


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(globalStyleSheet);
    MainWindow w;
    w.show(); // Or w.showFullScreen()
    return a.exec();
}
