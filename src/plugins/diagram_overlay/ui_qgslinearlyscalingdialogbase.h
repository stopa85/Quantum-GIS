/********************************************************************************
** Form generated from reading ui file 'qgslinearlyscalingdialogbase.ui'
**
** Created: Mon Feb 12 09:20:23 2007
**      by: Qt User Interface Compiler version 4.2.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QGSLINEARLYSCALINGDIALOGBASE_H
#define UI_QGSLINEARLYSCALINGDIALOGBASE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

class Ui_QgsLinearlyScalingDialogBase
{
public:
    QGridLayout *gridLayout;
    QLabel *mSizeLabel;
    QSpinBox *mSizeSpinBox;
    QLabel *mTitleLabel;
    QLineEdit *mValueLineEdit;
    QPushButton *mFindMaximumValueButton;

    void setupUi(QWidget *QgsLinearlyScalingDialogBase)
    {
    QgsLinearlyScalingDialogBase->setObjectName(QString::fromUtf8("QgsLinearlyScalingDialogBase"));
    gridLayout = new QGridLayout(QgsLinearlyScalingDialogBase);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    mSizeLabel = new QLabel(QgsLinearlyScalingDialogBase);
    mSizeLabel->setObjectName(QString::fromUtf8("mSizeLabel"));

    gridLayout->addWidget(mSizeLabel, 1, 2, 1, 1);

    mSizeSpinBox = new QSpinBox(QgsLinearlyScalingDialogBase);
    mSizeSpinBox->setObjectName(QString::fromUtf8("mSizeSpinBox"));
    mSizeSpinBox->setMaximum(999);

    gridLayout->addWidget(mSizeSpinBox, 1, 3, 1, 1);

    mTitleLabel = new QLabel(QgsLinearlyScalingDialogBase);
    mTitleLabel->setObjectName(QString::fromUtf8("mTitleLabel"));

    gridLayout->addWidget(mTitleLabel, 0, 0, 1, 3);

    mValueLineEdit = new QLineEdit(QgsLinearlyScalingDialogBase);
    mValueLineEdit->setObjectName(QString::fromUtf8("mValueLineEdit"));

    gridLayout->addWidget(mValueLineEdit, 1, 1, 1, 1);

    mFindMaximumValueButton = new QPushButton(QgsLinearlyScalingDialogBase);
    mFindMaximumValueButton->setObjectName(QString::fromUtf8("mFindMaximumValueButton"));

    gridLayout->addWidget(mFindMaximumValueButton, 1, 0, 1, 1);


    retranslateUi(QgsLinearlyScalingDialogBase);

    QSize size(609, 91);
    size = size.expandedTo(QgsLinearlyScalingDialogBase->minimumSizeHint());
    QgsLinearlyScalingDialogBase->resize(size);


    QMetaObject::connectSlotsByName(QgsLinearlyScalingDialogBase);
    } // setupUi

    void retranslateUi(QWidget *QgsLinearlyScalingDialogBase)
    {
    QgsLinearlyScalingDialogBase->setWindowTitle(QApplication::translate("QgsLinearlyScalingDialogBase", "Form", 0, QApplication::UnicodeUTF8));
    mSizeLabel->setText(QApplication::translate("QgsLinearlyScalingDialogBase", "Size:", 0, QApplication::UnicodeUTF8));
    mTitleLabel->setText(QApplication::translate("QgsLinearlyScalingDialogBase", "Scale linearly between 0 and the following attribute value/ diagram size:", 0, QApplication::UnicodeUTF8));
    mFindMaximumValueButton->setText(QApplication::translate("QgsLinearlyScalingDialogBase", "find maximum Value:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(QgsLinearlyScalingDialogBase);
    } // retranslateUi

};

namespace Ui {
    class QgsLinearlyScalingDialogBase: public Ui_QgsLinearlyScalingDialogBase {};
} // namespace Ui

#endif // UI_QGSLINEARLYSCALINGDIALOGBASE_H
