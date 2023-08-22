/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "myslider.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_ir;
    QLabel *label_light;
    QLabel *label_dis;
    QLabel *label_tmp;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_hum;
    QPushButton *pushButton_4;
    QLabel *label_7;
    QLabel *label_people;
    QLabel *label_8;
    QLabel *label_air;
    MySlider *horizontalSlider;
    QLabel *label_10;
    QProgressBar *progressBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 480);
        MainWindow->setMinimumSize(QSize(800, 480));
        MainWindow->setMaximumSize(QSize(800, 480));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(70, 140, 170, 48));
        QFont font;
        font.setPointSize(20);
        pushButton->setFont(font);
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(320, 140, 170, 48));
        pushButton_2->setFont(font);
        pushButton_2->setLayoutDirection(Qt::LeftToRight);
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 50, 801, 71));
        QFont font1;
        font1.setPointSize(24);
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        label->setLayoutDirection(Qt::LeftToRight);
        label->setStyleSheet(QString::fromUtf8("background-color: rgb(114, 159, 207);"));
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(140, 330, 71, 34));
        label_2->setFont(font);
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(340, 330, 81, 34));
        label_3->setFont(font);
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(530, 330, 71, 34));
        label_4->setFont(font);
        label_ir = new QLabel(centralwidget);
        label_ir->setObjectName(QString::fromUtf8("label_ir"));
        label_ir->setGeometry(QRect(230, 330, 71, 34));
        label_ir->setFont(font);
        label_light = new QLabel(centralwidget);
        label_light->setObjectName(QString::fromUtf8("label_light"));
        label_light->setGeometry(QRect(430, 330, 71, 34));
        label_light->setFont(font);
        label_dis = new QLabel(centralwidget);
        label_dis->setObjectName(QString::fromUtf8("label_dis"));
        label_dis->setGeometry(QRect(620, 330, 71, 34));
        label_dis->setFont(font);
        label_tmp = new QLabel(centralwidget);
        label_tmp->setObjectName(QString::fromUtf8("label_tmp"));
        label_tmp->setGeometry(QRect(120, 390, 71, 34));
        label_tmp->setFont(font);
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(30, 390, 71, 34));
        label_5->setFont(font);
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(220, 390, 71, 34));
        label_6->setFont(font);
        label_hum = new QLabel(centralwidget);
        label_hum->setObjectName(QString::fromUtf8("label_hum"));
        label_hum->setGeometry(QRect(310, 390, 71, 34));
        label_hum->setFont(font);
        pushButton_4 = new QPushButton(centralwidget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(560, 140, 170, 48));
        pushButton_4->setFont(font);
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(420, 390, 71, 34));
        label_7->setFont(font);
        label_people = new QLabel(centralwidget);
        label_people->setObjectName(QString::fromUtf8("label_people"));
        label_people->setGeometry(QRect(510, 390, 71, 34));
        label_people->setFont(font);
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(600, 390, 71, 34));
        label_8->setFont(font);
        label_air = new QLabel(centralwidget);
        label_air->setObjectName(QString::fromUtf8("label_air"));
        label_air->setGeometry(QRect(690, 390, 71, 34));
        label_air->setFont(font);
        horizontalSlider = new MySlider(centralwidget);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(200, 260, 500, 30));
        horizontalSlider->setOrientation(Qt::Horizontal);
        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(90, 240, 71, 34));
        label_10->setFont(font);
        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(200, 230, 500, 25));
        QFont font2;
        font2.setPointSize(18);
        font2.setBold(true);
        font2.setWeight(75);
        progressBar->setFont(font2);
        progressBar->setStyleSheet(QString::fromUtf8("color: rgb(239, 41, 41);"));
        progressBar->setValue(0);
        progressBar->setAlignment(Qt::AlignCenter);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "\347\201\257\345\205\211", nullptr));
        pushButton_2->setText(QApplication::translate("MainWindow", "\351\227\250\351\223\203", nullptr));
        label->setText(QApplication::translate("MainWindow", "\345\237\272\344\272\216IMX6ULL\347\232\204\346\231\272\350\203\275\345\256\266\345\261\205\347\263\273\347\273\237", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\347\272\242\345\244\226 :", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "\345\205\211\347\205\247 :", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "\350\267\235\347\246\273 :", nullptr));
        label_ir->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_light->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_dis->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_tmp->setText(QApplication::translate("MainWindow", "26", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "\346\270\251\345\272\246 :", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "\346\271\277\345\272\246 :", nullptr));
        label_hum->setText(QApplication::translate("MainWindow", "75", nullptr));
        pushButton_4->setText(QApplication::translate("MainWindow", "\351\227\250\351\224\201", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "\346\234\211\344\272\272: ", nullptr));
        label_people->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "\347\251\272\346\260\224:", nullptr));
        label_air->setText(QApplication::translate("MainWindow", "300", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "\347\252\227\345\270\230: ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
