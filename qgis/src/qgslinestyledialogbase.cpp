/****************************************************************************
** Form implementation generated from reading ui file 'qgslinestyledialogbase.ui'
**
** Created: Mon Jan 5 12:15:17 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "qgslinestyledialogbase.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a QgsLineStyleDialogBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
QgsLineStyleDialogBase::QgsLineStyleDialogBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "QgsLineStyleDialogBase" );

    cancelbutton = new QPushButton( this, "cancelbutton" );
    cancelbutton->setGeometry( QRect( 320, 200, 80, 50 ) );

    okbutton = new QPushButton( this, "okbutton" );
    okbutton->setGeometry( QRect( 210, 200, 80, 50 ) );

    buttonGroup1 = new QButtonGroup( this, "buttonGroup1" );
    buttonGroup1->setGeometry( QRect( 0, 10, 346, 152 ) );
    buttonGroup1->setExclusive( TRUE );
    buttonGroup1->setColumnLayout(0, Qt::Vertical );
    buttonGroup1->layout()->setSpacing( 6 );
    buttonGroup1->layout()->setMargin( 11 );
    buttonGroup1Layout = new QGridLayout( buttonGroup1->layout() );
    buttonGroup1Layout->setAlignment( Qt::AlignTop );

    solid = new QPushButton( buttonGroup1, "solid" );
    solid->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( solid, 0, 0 );

    dash = new QPushButton( buttonGroup1, "dash" );
    dash->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dash, 1, 0 );

    dot = new QPushButton( buttonGroup1, "dot" );
    dot->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dot, 2, 0 );

    dashdot = new QPushButton( buttonGroup1, "dashdot" );
    dashdot->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dashdot, 0, 1 );

    dashdotdot = new QPushButton( buttonGroup1, "dashdotdot" );
    dashdotdot->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dashdotdot, 1, 1 );
    languageChange();
    resize( QSize(441, 299).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
QgsLineStyleDialogBase::~QgsLineStyleDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QgsLineStyleDialogBase::languageChange()
{
    setCaption( tr( "Select a line style" ) );
    cancelbutton->setText( tr( "cancel" ) );
    okbutton->setText( tr( "ok" ) );
    buttonGroup1->setTitle( tr( "buttonGroup1" ) );
    solid->setText( tr( "solid" ) );
    dash->setText( tr( "dash" ) );
    dot->setText( tr( "dot" ) );
    dashdot->setText( tr( "dashdot" ) );
    dashdotdot->setText( tr( "dashdotdot" ) );
}

