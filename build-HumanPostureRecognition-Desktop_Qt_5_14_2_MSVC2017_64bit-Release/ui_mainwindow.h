/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *lab_cam1;
    QLabel *lab_cam2;
    QPushButton *pb_open;
    QPushButton *pb_close;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(777, 788);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMaximumSize(QSize(777, 788));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        lab_cam1 = new QLabel(centralwidget);
        lab_cam1->setObjectName(QString::fromUtf8("lab_cam1"));
        lab_cam1->setGeometry(QRect(10, 10, 640, 360));
        lab_cam1->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        lab_cam2 = new QLabel(centralwidget);
        lab_cam2->setObjectName(QString::fromUtf8("lab_cam2"));
        lab_cam2->setGeometry(QRect(10, 380, 640, 360));
        lab_cam2->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        pb_open = new QPushButton(centralwidget);
        pb_open->setObjectName(QString::fromUtf8("pb_open"));
        pb_open->setGeometry(QRect(660, 10, 111, 41));
        pb_close = new QPushButton(centralwidget);
        pb_close->setObjectName(QString::fromUtf8("pb_close"));
        pb_close->setGeometry(QRect(660, 70, 111, 41));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 777, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        lab_cam1->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        lab_cam2->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        pb_open->setText(QCoreApplication::translate("MainWindow", "open", nullptr));
        pb_close->setText(QCoreApplication::translate("MainWindow", "close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
