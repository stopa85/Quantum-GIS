/****************************************************************************
** Form interface generated from reading ui file 'qgspatterndialogbase.ui'
**
** Created: Mon Jan 5 12:15:13 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QGSPATTERNDIALOGBASE_H
#define QGSPATTERNDIALOGBASE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QPushButton;

class QgsPatternDialogBase : public QDialog
{
    Q_OBJECT

public:
    QgsPatternDialogBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~QgsPatternDialogBase();

    QButtonGroup* buttonGroup1;
    QPushButton* solid;
    QPushButton* fdiag;
    QPushButton* dense4;
    QPushButton* horizontal;
    QPushButton* dense5;
    QPushButton* diagcross;
    QPushButton* dense1;
    QPushButton* dense6;
    QPushButton* vertical;
    QPushButton* dense7;
    QPushButton* cross;
    QPushButton* dense2;
    QPushButton* bdiag;
    QPushButton* dense3;
    QPushButton* okbutton;
    QPushButton* cancelbutton;

protected:
    QGridLayout* buttonGroup1Layout;

protected slots:
    virtual void languageChange();

};

#endif // QGSPATTERNDIALOGBASE_H
