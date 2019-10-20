/********************************************************************************
** Form generated from reading UI file 'log_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOG_DIALOG_H
#define UI_LOG_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_Log_Dialog
{
public:
    QGridLayout *gridLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QPushButton *pushButton_login;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_password;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_username;
    QPushButton *pushButton_register;

    void setupUi(QDialog *Log_Dialog)
    {
        if (Log_Dialog->objectName().isEmpty())
            Log_Dialog->setObjectName(QString::fromUtf8("Log_Dialog"));
        Log_Dialog->resize(400, 300);
        gridLayout_2 = new QGridLayout(Log_Dialog);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 2, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 1, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 0, 1, 1, 1);

        groupBox = new QGroupBox(Log_Dialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton_login = new QPushButton(groupBox);
        pushButton_login->setObjectName(QString::fromUtf8("pushButton_login"));

        gridLayout->addWidget(pushButton_login, 2, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEdit_password = new QLineEdit(groupBox);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));

        horizontalLayout_2->addWidget(lineEdit_password);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        lineEdit_username = new QLineEdit(groupBox);
        lineEdit_username->setObjectName(QString::fromUtf8("lineEdit_username"));

        horizontalLayout->addWidget(lineEdit_username);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        pushButton_register = new QPushButton(groupBox);
        pushButton_register->setObjectName(QString::fromUtf8("pushButton_register"));

        gridLayout->addWidget(pushButton_register, 3, 0, 1, 1);


        gridLayout_2->addWidget(groupBox, 1, 1, 1, 1);


        retranslateUi(Log_Dialog);

        QMetaObject::connectSlotsByName(Log_Dialog);
    } // setupUi

    void retranslateUi(QDialog *Log_Dialog)
    {
        Log_Dialog->setWindowTitle(QApplication::translate("Log_Dialog", "Dialog", nullptr));
        groupBox->setTitle(QApplication::translate("Log_Dialog", "Authentication", nullptr));
        pushButton_login->setText(QApplication::translate("Log_Dialog", "Sign In", nullptr));
        label_2->setText(QApplication::translate("Log_Dialog", "Password:", nullptr));
        label->setText(QApplication::translate("Log_Dialog", "Username:", nullptr));
        pushButton_register->setText(QApplication::translate("Log_Dialog", "Register", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Log_Dialog: public Ui_Log_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOG_DIALOG_H
