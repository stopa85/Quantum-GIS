/****************************************************************************
** Form interface generated from reading ui file 'qgslinestyledialogbase.ui'
**
** Created: Mon Jan 5 12:15:13 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QGSLINESTYLEDIALOGBASE_H
#define QGSLINESTYLEDIALOGBASE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QButtonGroup;

class QgsLineStyleDialogBase : public QDialog
{
    Q_OBJECT

public:
    QgsLineStyleDialogBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~QgsLineStyleDialogBase();

    QPushButton* cancelbutton;
    QPushButton* okbutton;
    QButtonGroup* buttonGroup1;
    QPushButton* solid;
    QPushButton* dash;
    QPushButton* dot;
    QPushButton* dashdot;
    QPushButton* dashdotdot;

protected:
    QGridLayout* buttonGroup1Layout;

protected slots:
    virtual void languageChange();

};

#endif // QGSLINESTYLEDIALOGBASE_H
