/****************************************************************************
** Form implementation generated from reading ui file 'qgspatterndialogbase.ui'
**
** Created: Mon Jan 5 12:15:18 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "qgspatterndialogbase.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a QgsPatternDialogBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
QgsPatternDialogBase::QgsPatternDialogBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "QgsPatternDialogBase" );
    setModal( TRUE );

    buttonGroup1 = new QButtonGroup( this, "buttonGroup1" );
    buttonGroup1->setGeometry( QRect( 20, 10, 420, 216 ) );
    buttonGroup1->setExclusive( TRUE );
    buttonGroup1->setColumnLayout(0, Qt::Vertical );
    buttonGroup1->layout()->setSpacing( 6 );
    buttonGroup1->layout()->setMargin( 11 );
    buttonGroup1Layout = new QGridLayout( buttonGroup1->layout() );
    buttonGroup1Layout->setAlignment( Qt::AlignTop );

    solid = new QPushButton( buttonGroup1, "solid" );
    solid->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    solid->setBackgroundOrigin( QPushButton::WindowOrigin );
    solid->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( solid, 0, 0 );

    fdiag = new QPushButton( buttonGroup1, "fdiag" );
    fdiag->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    fdiag->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( fdiag, 0, 1 );

    dense4 = new QPushButton( buttonGroup1, "dense4" );
    dense4->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    dense4->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dense4, 0, 2 );

    horizontal = new QPushButton( buttonGroup1, "horizontal" );
    horizontal->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    horizontal->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( horizontal, 1, 0 );

    dense5 = new QPushButton( buttonGroup1, "dense5" );
    dense5->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    dense5->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dense5, 1, 2 );

    diagcross = new QPushButton( buttonGroup1, "diagcross" );
    diagcross->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    diagcross->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( diagcross, 1, 1 );

    dense1 = new QPushButton( buttonGroup1, "dense1" );
    dense1->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    dense1->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dense1, 2, 1 );

    dense6 = new QPushButton( buttonGroup1, "dense6" );
    dense6->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    dense6->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dense6, 2, 2 );

    vertical = new QPushButton( buttonGroup1, "vertical" );
    vertical->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    vertical->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( vertical, 2, 0 );

    dense7 = new QPushButton( buttonGroup1, "dense7" );
    dense7->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    dense7->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dense7, 3, 2 );

    cross = new QPushButton( buttonGroup1, "cross" );
    cross->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    cross->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( cross, 3, 0 );

    dense2 = new QPushButton( buttonGroup1, "dense2" );
    dense2->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    dense2->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dense2, 3, 1 );

    bdiag = new QPushButton( buttonGroup1, "bdiag" );
    bdiag->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    bdiag->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( bdiag, 4, 0 );

    dense3 = new QPushButton( buttonGroup1, "dense3" );
    dense3->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    dense3->setToggleButton( TRUE );

    buttonGroup1Layout->addWidget( dense3, 4, 1 );

    okbutton = new QPushButton( this, "okbutton" );
    okbutton->setGeometry( QRect( 20, 240, 91, 51 ) );

    cancelbutton = new QPushButton( this, "cancelbutton" );
    cancelbutton->setGeometry( QRect( 140, 240, 91, 51 ) );
    languageChange();
    resize( QSize(491, 362).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
QgsPatternDialogBase::~QgsPatternDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QgsPatternDialogBase::languageChange()
{
    setCaption( tr( "select a pattern" ) );
    buttonGroup1->setTitle( QString::null );
    solid->setText( tr( "solid" ) );
    fdiag->setText( tr( "fdiag" ) );
    dense4->setText( tr( "dense4" ) );
    horizontal->setText( tr( "horiz" ) );
    dense5->setText( tr( "dense5" ) );
    diagcross->setText( tr( "diagcross" ) );
    dense1->setText( tr( "dense1" ) );
    dense6->setText( tr( "dense6" ) );
    vertical->setText( tr( "vertical" ) );
    dense7->setText( tr( "dense7" ) );
    cross->setText( tr( "cross" ) );
    dense2->setText( tr( "dense2" ) );
    bdiag->setText( tr( "bdiag" ) );
    dense3->setText( tr( "dense3" ) );
    okbutton->setText( tr( "ok" ) );
    cancelbutton->setText( tr( "cancel" ) );
}

