/****************************************************************************
** Form interface generated from reading ui file 'qgssisydialogbase.ui'
**
** Created: Mit Jan 7 09:05:03 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QGSSISYDIALOGBASE_H
#define QGSSISYDIALOGBASE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QSpinBox;
class QPushButton;
class QLineEdit;

class QgsSiSyDialogBase : public QDialog
{
    Q_OBJECT

public:
    QgsSiSyDialogBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~QgsSiSyDialogBase();

    QLabel* outlinecolorlabel;
    QLabel* outlinestylelabel;
    QSpinBox* outlinewidthspinbox;
    QPushButton* patternbutton;
    QLabel* outlinewidthlabel;
    QPushButton* fillcolorbutton;
    QPushButton* stylebutton;
    QLabel* fillcolorlabel;
    QLabel* fillpaternlabel;
    QPushButton* outlinecolorbutton;
    QPushButton* applybutton;
    QPushButton* closebutton;
    QLabel* displaynamelabel;
    QLineEdit* displaynamefield;

protected:
    QGridLayout* layout1;

protected slots:
    virtual void languageChange();

};

#endif // QGSSISYDIALOGBASE_H
