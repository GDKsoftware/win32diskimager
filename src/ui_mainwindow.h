/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Sun Apr 19 07:19:02 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QGroupBox *groupBox;
    QHBoxLayout *hboxLayout1;
    QLineEdit *leFile;
    QToolButton *tbBrowse;
    QGroupBox *groupBox_2;
    QHBoxLayout *hboxLayout2;
    QComboBox *cboxDevice;
    QGroupBox *groupBox_5;
    QHBoxLayout *hboxLayout3;
    QProgressBar *progressbar;
    QHBoxLayout *hboxLayout4;
    QSpacerItem *spacerItem;
    QPushButton *bRip;
    QPushButton *bBurn;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(298, 137);
        QIcon icon;
        icon.addPixmap(QPixmap(QString::fromUtf8(":/images/images/icon-ubuntu.png")), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        vboxLayout = new QVBoxLayout(centralwidget);
        vboxLayout->setSpacing(0);
        vboxLayout->setMargin(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        hboxLayout1 = new QHBoxLayout(groupBox);
        hboxLayout1->setSpacing(3);
        hboxLayout1->setMargin(0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        leFile = new QLineEdit(groupBox);
        leFile->setObjectName(QString::fromUtf8("leFile"));

        hboxLayout1->addWidget(leFile);

        tbBrowse = new QToolButton(groupBox);
        tbBrowse->setObjectName(QString::fromUtf8("tbBrowse"));

        hboxLayout1->addWidget(tbBrowse);


        hboxLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        hboxLayout2 = new QHBoxLayout(groupBox_2);
        hboxLayout2->setSpacing(0);
        hboxLayout2->setMargin(0);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        cboxDevice = new QComboBox(groupBox_2);
        cboxDevice->setObjectName(QString::fromUtf8("cboxDevice"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cboxDevice->sizePolicy().hasHeightForWidth());
        cboxDevice->setSizePolicy(sizePolicy);

        hboxLayout2->addWidget(cboxDevice);


        hboxLayout->addWidget(groupBox_2);


        vboxLayout->addLayout(hboxLayout);

        groupBox_5 = new QGroupBox(centralwidget);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        hboxLayout3 = new QHBoxLayout(groupBox_5);
        hboxLayout3->setSpacing(0);
        hboxLayout3->setMargin(0);
        hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
        progressbar = new QProgressBar(groupBox_5);
        progressbar->setObjectName(QString::fromUtf8("progressbar"));
        progressbar->setValue(0);

        hboxLayout3->addWidget(progressbar);


        vboxLayout->addWidget(groupBox_5);

        hboxLayout4 = new QHBoxLayout();
        hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout4->addItem(spacerItem);

        bRip = new QPushButton(centralwidget);
        bRip->setObjectName(QString::fromUtf8("bRip"));

        hboxLayout4->addWidget(bRip);

        bBurn = new QPushButton(centralwidget);
        bBurn->setObjectName(QString::fromUtf8("bBurn"));

        hboxLayout4->addWidget(bBurn);


        vboxLayout->addLayout(hboxLayout4);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Win32 Disk Imager", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Image File", 0, QApplication::UnicodeUTF8));
        tbBrowse->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Device", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Progress", 0, QApplication::UnicodeUTF8));
        progressbar->setFormat(QApplication::translate("MainWindow", "%p%", 0, QApplication::UnicodeUTF8));
        bRip->setText(QApplication::translate("MainWindow", "Rip", 0, QApplication::UnicodeUTF8));
        bBurn->setText(QApplication::translate("MainWindow", "Burn", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
