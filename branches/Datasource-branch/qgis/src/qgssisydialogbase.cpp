/****************************************************************************
** Form implementation generated from reading ui file 'qgssisydialogbase.ui'
**
** Created: Tue Jan 6 18:52:15 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "qgssisydialogbase.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a QgsSiSyDialogBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
QgsSiSyDialogBase::QgsSiSyDialogBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "QgsSiSyDialogBase" );

    QWidget* privateLayoutWidget = new QWidget( this, "layout1" );
    privateLayoutWidget->setGeometry( QRect( 10, 70, 630, 80 ) );
    layout1 = new QGridLayout( privateLayoutWidget, 1, 1, 11, 6, "layout1"); 

    outlinecolorlabel = new QLabel( privateLayoutWidget, "outlinecolorlabel" );

    layout1->addWidget( outlinecolorlabel, 0, 0 );

    outlinestylelabel = new QLabel( privateLayoutWidget, "outlinestylelabel" );

    layout1->addWidget( outlinestylelabel, 0, 2 );

    outlinewidthspinbox = new QSpinBox( privateLayoutWidget, "outlinewidthspinbox" );

    layout1->addWidget( outlinewidthspinbox, 0, 6 );

    patternbutton = new QPushButton( privateLayoutWidget, "patternbutton" );

    layout1->addMultiCellWidget( patternbutton, 1, 1, 4, 5 );

    outlinewidthlabel = new QLabel( privateLayoutWidget, "outlinewidthlabel" );

    layout1->addWidget( outlinewidthlabel, 0, 5 );

    fillcolorbutton = new QPushButton( privateLayoutWidget, "fillcolorbutton" );

    layout1->addWidget( fillcolorbutton, 1, 1 );

    stylebutton = new QPushButton( privateLayoutWidget, "stylebutton" );

    layout1->addMultiCellWidget( stylebutton, 0, 0, 3, 4 );

    fillcolorlabel = new QLabel( privateLayoutWidget, "fillcolorlabel" );

    layout1->addWidget( fillcolorlabel, 1, 0 );

    fillpaternlabel = new QLabel( privateLayoutWidget, "fillpaternlabel" );

    layout1->addMultiCellWidget( fillpaternlabel, 1, 1, 2, 3 );

    outlinecolorbutton = new QPushButton( privateLayoutWidget, "outlinecolorbutton" );

    layout1->addWidget( outlinecolorbutton, 0, 1 );

    applybutton = new QPushButton( this, "applybutton" );
    applybutton->setGeometry( QRect( 270, 180, 104, 34 ) );
    applybutton->setAutoDefault( TRUE );

    closebutton = new QPushButton( this, "closebutton" );
    closebutton->setGeometry( QRect( 400, 180, 104, 34 ) );
    closebutton->setAutoDefault( TRUE );

    displaynamelabel = new QLabel( this, "displaynamelabel" );
    displaynamelabel->setGeometry( QRect( 13, 25, 132, 30 ) );
    displaynamelabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, displaynamelabel->sizePolicy().hasHeightForWidth() ) );
    displaynamelabel->setMinimumSize( QSize( 0, 30 ) );
    QFont displaynamelabel_font(  displaynamelabel->font() );
    displaynamelabel->setFont( displaynamelabel_font ); 

    displaynamefield = new QLineEdit( this, "displaynamefield" );
    displaynamefield->setGeometry( QRect( 151, 25, 450, 30 ) );
    displaynamefield->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, displaynamefield->sizePolicy().hasHeightForWidth() ) );
    displaynamefield->setMinimumSize( QSize( 0, 30 ) );
    languageChange();
    resize( QSize(870, 255).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
QgsSiSyDialogBase::~QgsSiSyDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QgsSiSyDialogBase::languageChange()
{
    setCaption( tr( "single symbol" ) );
    outlinecolorlabel->setText( tr( "outline color" ) );
    outlinestylelabel->setText( tr( "style" ) );
    patternbutton->setText( QString::null );
    outlinewidthlabel->setText( tr( "width" ) );
    fillcolorbutton->setText( QString::null );
    stylebutton->setText( QString::null );
    fillcolorlabel->setText( tr( "fill color" ) );
    fillpaternlabel->setText( tr( "pattern" ) );
    outlinecolorbutton->setText( QString::null );
    applybutton->setText( tr( "apply" ) );
    applybutton->setAccel( QKeySequence( QString::null ) );
    closebutton->setText( tr( "close" ) );
    closebutton->setAccel( QKeySequence( QString::null ) );
    displaynamelabel->setText( tr( "Display Name" ) );
}

