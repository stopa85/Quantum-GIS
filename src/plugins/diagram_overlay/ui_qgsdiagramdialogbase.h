/********************************************************************************
** Form generated from reading ui file 'qgsdiagramdialogbase.ui'
**
** Created: Mon Feb 12 09:20:23 2007
**      by: Qt User Interface Compiler version 4.2.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QGSDIAGRAMDIALOGBASE_H
#define UI_QGSDIAGRAMDIALOGBASE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

class Ui_QgsDiagramDialogBase
{
public:
    QGridLayout *gridLayout;
    QCheckBox *mDisplayDiagramsCheckBox;
    QHBoxLayout *hboxLayout;
    QLabel *mTypeLabel;
    QComboBox *mDiagramTypeComboBox;
    QHBoxLayout *hboxLayout1;
    QLabel *mAttributesLabel;
    QComboBox *mAttributesComboBox;
    QGridLayout *gridLayout1;
    QTreeWidget *mAttributesTreeWidget;
    QPushButton *mRemovePushButton;
    QPushButton *mAddPushButton;
    QGridLayout *gridLayout2;
    QComboBox *mClassificationTypeComboBox;
    QStackedWidget *mWidgetStackRenderers;
    QWidget *page;
    QWidget *page_2;
    QLabel *mClassificationLabel;
    QComboBox *mClassificationComboBox;
    QLabel *mClassificationTypeLabel;

    void setupUi(QDialog *QgsDiagramDialogBase)
    {
    QgsDiagramDialogBase->setObjectName(QString::fromUtf8("QgsDiagramDialogBase"));
    gridLayout = new QGridLayout(QgsDiagramDialogBase);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    mDisplayDiagramsCheckBox = new QCheckBox(QgsDiagramDialogBase);
    mDisplayDiagramsCheckBox->setObjectName(QString::fromUtf8("mDisplayDiagramsCheckBox"));

    gridLayout->addWidget(mDisplayDiagramsCheckBox, 0, 0, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    mTypeLabel = new QLabel(QgsDiagramDialogBase);
    mTypeLabel->setObjectName(QString::fromUtf8("mTypeLabel"));

    hboxLayout->addWidget(mTypeLabel);

    mDiagramTypeComboBox = new QComboBox(QgsDiagramDialogBase);
    mDiagramTypeComboBox->setObjectName(QString::fromUtf8("mDiagramTypeComboBox"));

    hboxLayout->addWidget(mDiagramTypeComboBox);


    gridLayout->addLayout(hboxLayout, 1, 0, 1, 1);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    mAttributesLabel = new QLabel(QgsDiagramDialogBase);
    mAttributesLabel->setObjectName(QString::fromUtf8("mAttributesLabel"));

    hboxLayout1->addWidget(mAttributesLabel);

    mAttributesComboBox = new QComboBox(QgsDiagramDialogBase);
    mAttributesComboBox->setObjectName(QString::fromUtf8("mAttributesComboBox"));

    hboxLayout1->addWidget(mAttributesComboBox);


    gridLayout->addLayout(hboxLayout1, 2, 0, 1, 1);

    gridLayout1 = new QGridLayout();
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(0);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    mAttributesTreeWidget = new QTreeWidget(QgsDiagramDialogBase);
    mAttributesTreeWidget->setObjectName(QString::fromUtf8("mAttributesTreeWidget"));
    mAttributesTreeWidget->setColumnCount(2);

    gridLayout1->addWidget(mAttributesTreeWidget, 1, 0, 1, 2);

    mRemovePushButton = new QPushButton(QgsDiagramDialogBase);
    mRemovePushButton->setObjectName(QString::fromUtf8("mRemovePushButton"));

    gridLayout1->addWidget(mRemovePushButton, 0, 1, 1, 1);

    mAddPushButton = new QPushButton(QgsDiagramDialogBase);
    mAddPushButton->setObjectName(QString::fromUtf8("mAddPushButton"));

    gridLayout1->addWidget(mAddPushButton, 0, 0, 1, 1);


    gridLayout->addLayout(gridLayout1, 3, 0, 1, 1);

    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    mClassificationTypeComboBox = new QComboBox(QgsDiagramDialogBase);
    mClassificationTypeComboBox->setObjectName(QString::fromUtf8("mClassificationTypeComboBox"));

    gridLayout2->addWidget(mClassificationTypeComboBox, 0, 1, 1, 1);

    mWidgetStackRenderers = new QStackedWidget(QgsDiagramDialogBase);
    mWidgetStackRenderers->setObjectName(QString::fromUtf8("mWidgetStackRenderers"));
    page = new QWidget();
    page->setObjectName(QString::fromUtf8("page"));
    mWidgetStackRenderers->addWidget(page);
    page_2 = new QWidget();
    page_2->setObjectName(QString::fromUtf8("page_2"));
    mWidgetStackRenderers->addWidget(page_2);

    gridLayout2->addWidget(mWidgetStackRenderers, 2, 0, 1, 2);

    mClassificationLabel = new QLabel(QgsDiagramDialogBase);
    mClassificationLabel->setObjectName(QString::fromUtf8("mClassificationLabel"));

    gridLayout2->addWidget(mClassificationLabel, 1, 0, 1, 1);

    mClassificationComboBox = new QComboBox(QgsDiagramDialogBase);
    mClassificationComboBox->setObjectName(QString::fromUtf8("mClassificationComboBox"));

    gridLayout2->addWidget(mClassificationComboBox, 1, 1, 1, 1);

    mClassificationTypeLabel = new QLabel(QgsDiagramDialogBase);
    mClassificationTypeLabel->setObjectName(QString::fromUtf8("mClassificationTypeLabel"));

    gridLayout2->addWidget(mClassificationTypeLabel, 0, 0, 1, 1);


    gridLayout->addLayout(gridLayout2, 4, 0, 1, 1);


    retranslateUi(QgsDiagramDialogBase);

    QSize size(398, 482);
    size = size.expandedTo(QgsDiagramDialogBase->minimumSizeHint());
    QgsDiagramDialogBase->resize(size);


    QMetaObject::connectSlotsByName(QgsDiagramDialogBase);
    } // setupUi

    void retranslateUi(QDialog *QgsDiagramDialogBase)
    {
    QgsDiagramDialogBase->setWindowTitle(QApplication::translate("QgsDiagramDialogBase", "Dialog", 0, QApplication::UnicodeUTF8));
    mDisplayDiagramsCheckBox->setText(QApplication::translate("QgsDiagramDialogBase", "Display diagrams", 0, QApplication::UnicodeUTF8));
    mTypeLabel->setText(QApplication::translate("QgsDiagramDialogBase", "Diagram type:", 0, QApplication::UnicodeUTF8));
    mAttributesLabel->setText(QApplication::translate("QgsDiagramDialogBase", "Attributes:", 0, QApplication::UnicodeUTF8));
    mAttributesTreeWidget->headerItem()->setText(0, QApplication::translate("QgsDiagramDialogBase", "1", 0, QApplication::UnicodeUTF8));
    mAttributesTreeWidget->headerItem()->setText(1, QApplication::translate("QgsDiagramDialogBase", "1", 0, QApplication::UnicodeUTF8));
    mRemovePushButton->setText(QApplication::translate("QgsDiagramDialogBase", "Remove attribute", 0, QApplication::UnicodeUTF8));
    mAddPushButton->setText(QApplication::translate("QgsDiagramDialogBase", "Add attribute", 0, QApplication::UnicodeUTF8));
    mClassificationLabel->setText(QApplication::translate("QgsDiagramDialogBase", "Classification attribute:", 0, QApplication::UnicodeUTF8));
    mClassificationTypeLabel->setText(QApplication::translate("QgsDiagramDialogBase", "Classification type:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(QgsDiagramDialogBase);
    } // retranslateUi

};

namespace Ui {
    class QgsDiagramDialogBase: public Ui_QgsDiagramDialogBase {};
} // namespace Ui

#endif // UI_QGSDIAGRAMDIALOGBASE_H
