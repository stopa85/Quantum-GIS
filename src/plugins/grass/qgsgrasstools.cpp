/***************************************************************************
                              qgsgrasstools.cpp
                             -------------------
    begin                : March, 2005
    copyright            : (C) 2005 by Radim Blazek
    email                : blazek@itc.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <iostream>

#include <QApplication>
#include <QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QStandardItem>
#include <QRegExp>

// Probably we can ditch use of custom delegate
//#include <qgsdetaileditemdelegate.h>
//#include <qgsdetaileditemwidget.h>
//#include <qgsdetaileditemdata.h>

#include "qgis.h"
#include "qgisinterface.h"
#include "qgsapplication.h"
#include "qgslogger.h"

extern "C" {
#include <grass/gis.h>
#include <grass/Vect.h>
}

#include "../../src/providers/grass/qgsgrass.h"
#include "../../src/providers/grass/qgsgrassprovider.h"
#include "qgsgrassattributes.h"
#include "qgsgrasstools.h"
#include "qgsgrassmodule.h"



const int MODULE_DATA_ROLE = Qt::UserRole;
const int MODULE_NAME_ROLE = Qt::UserRole +1;
const int MODULE_DESCRIPTION_ROLE = Qt::UserRole +2;
const int MODULE_FILTER_ROLE = Qt::UserRole +3;


QgsGrassTools::QgsGrassTools ( QgisInterface *iface, 
    QWidget * parent, const char * name, Qt::WFlags f )
: QWidget(parent,f)
{
  setupUi(this);

//  qRegisterMetaType<QgsDetailedItemData>();


  mIface = iface;

  connect( qApp, SIGNAL(aboutToQuit()), 
      this, SLOT(closeTools()) );

  mModelTools= new QStandardItemModel(0,1);
  mModelProxy = new QSortFilterProxyModel(this);
  mModelProxy->setSourceModel(mModelTools);
  mModelProxy->setFilterRole(MODULE_FILTER_ROLE);

  //set the model using our proxy so we can do filtering
  mpListView->setModel(mModelProxy);
  //mpListView->setItemDelegateForColumn(0,new QgsDetailedItemDelegate());

  //
  // Load the modules lists
  //
  // Show before loadConfig() so that user can see loading
  QString conf = QgsApplication::pkgDataPath() + "/grass/config/default.qgc";
  QApplication::setOverrideCursor(Qt::waitCursor);
  loadConfig ( conf );
  QApplication::restoreOverrideCursor();

  // set the dialog title
  QString title = tr("GRASS Tools: ") + QgsGrass::getDefaultLocation()
    + "/" + QgsGrass::getDefaultMapset();


  // Add map browser 
  //mBrowser = new QgsGrassBrowser ( mIface, this );

  //connect( mBrowser, SIGNAL(regionChanged()), 
  //         this, SLOT(emitRegionChanged()) );
}

QgsGrassTools::~QgsGrassTools()
{
  delete mModelTools;
  delete mModelProxy;
}

/* Commented out this - relates to mapbrowser only
   void QgsGrassTools::mapsetChanged()
   {
#ifdef QGISDEBUG
std::cerr << "QgsGrassTools::mapsetChanged()" << std::endl;
#endif

QString title = tr("GRASS Tools: ") + QgsGrass::getDefaultLocation()
+ "/" + QgsGrass::getDefaultMapset();
setCaption(title);

closeTools();
mBrowser->setLocation( QgsGrass::getDefaultGisdbase(), QgsGrass::getDefaultLocation() );
}
*/

/* Move to mapbrowser ?
   void QgsGrassTools::emitRegionChanged()
   {
#ifdef QGISDEBUG
std::cerr << "QgsGrassTools::emitRegionChanged()" << std::endl;
#endif
emit regionChanged();
}
*/

bool QgsGrassTools::loadConfig(QString filePath)
{
#ifdef QGISDEBUG
  std::cerr << "QgsGrassTools::loadConfig(): " << filePath.toLocal8Bit().data() << std::endl;
#endif


  QFile file ( filePath );

  if ( !file.exists() ) {
    QMessageBox::warning( 0, tr("Warning"), tr("The config file (") + filePath + tr(") not found.") );
    return false;
  }
  if ( ! file.open( QIODevice::ReadOnly ) ) {
    QMessageBox::warning( 0, tr("Warning"), tr("Cannot open config file (") + filePath + tr(")") );
    return false;
  }

  QDomDocument doc ( "qgisgrass" );
  QString err;
  int line, column;
  if ( !doc.setContent( &file,  &err, &line, &column ) ) {
    QString errmsg = tr("Cannot read config file (") + filePath + "):\n" + err + tr("\nat line ")  
                                                                      + QString::number(line) + tr(" column ") + QString::number(column);
    std::cerr << errmsg.toLocal8Bit().data() << std::endl;
    QMessageBox::warning( 0, tr("Warning"), errmsg );
    file.close();
    return false;
  }

  QDomElement docElem = doc.documentElement();
  QDomNodeList modulesNodes = docElem.elementsByTagName ( "modules" );

  if ( modulesNodes.count() == 0 ) {
    file.close();
    return false;
  }

  QDomNode modulesNode = modulesNodes.item(0);
  QDomElement modulesElem = modulesNode.toElement();

  // Go through the sections and modules and add them to the list view
  addModules ( modulesElem );

  file.close();
  return true;
}

void QgsGrassTools::addModules ( QDomElement &element )
{
  QDomNode n = element.firstChild();


  while( !n.isNull() ) 
  {
    QDomElement e = n.toElement();
    if( !e.isNull() ) 
    {
      //std::cout << "tag = " << e.tagName() << std::endl;

      if ( e.tagName() == "section" && e.tagName() == "grass" ) 
      {
        std::cout << "Unknown tag: " << e.tagName().toLocal8Bit().data() << std::endl;
        continue;
      }

      if ( e.tagName() == "section" ) 
      {
        QString label = e.attribute("label");
        QgsDebugMsg( QString("label = %1").arg(label) );

        //
        // Add logic here to delineate sections
        //

        addModules (e );

      } 
      else if ( e.tagName() == "grass" ) 
      { // GRASS module
        QString name = e.attribute("name");
        QgsDebugMsg( QString("name = %1").arg(name) );

        QString path = QgsApplication::pkgDataPath() + "/grass/modules/" + name;
        QString label = QgsGrassModule::label ( path );
        QPixmap pixmap = QgsGrassModule::pixmap ( path, 25 ); 

        //
        // Experimental work by Tim - add this item to our list model
        // 
        QStandardItem * mypDetailItem = new QStandardItem( name );
        mypDetailItem->setData(name,MODULE_NAME_ROLE); //for calling runModule later
        mypDetailItem->setData(label,MODULE_DESCRIPTION_ROLE); //for calling runModule later
        QString mySearchText = name + " - " + label;
        mypDetailItem->setData(mySearchText ,MODULE_FILTER_ROLE); //for filtering later
        mypDetailItem->setData(pixmap,Qt::DecorationRole);
        mypDetailItem->setCheckable(false);
        mypDetailItem->setEditable(false);
        // setData in the delegate with a variantised QgsDetailedItemData
        //QgsDetailedItemData myData;
        //myData.setTitle(name);
        //myData.setDetail(label);
        //myData.setIcon(pixmap);
        //myData.setCheckable(false);
        //myData.setRenderAsWidget(false);
        //QVariant myVariant = qVariantFromValue(myData);
        //mypDetailItem->setData(myVariant,MODULE_DATA_ROLE);
        mModelTools->appendRow(mypDetailItem);
        //
        // End of experimental work by Tim 
        // 
      }
    }
    n = n.nextSibling();
  }
}


//
// Autoconnect slots...
//

void QgsGrassTools::on_mpFilterEdit_textChanged(const QString theText)
{
  QgsDebugMsg("PluginManager filter changed to :" + theText);
  QRegExp::PatternSyntax mySyntax = QRegExp::PatternSyntax(QRegExp::RegExp);
  Qt::CaseSensitivity myCaseSensitivity = Qt::CaseInsensitive;
  QRegExp myRegExp(theText, myCaseSensitivity, mySyntax);
  mModelProxy->setFilterRegExp(myRegExp);
}

void QgsGrassTools::on_mpListView_activated(const QModelIndex &theIndex)
{
  if (theIndex.column() == 0)
  {
    //
    // If the model has been filtered, the index row in the proxy wont match 
    // the index row in the underlying model so we need to jump through this 
    // little hoop to get the correct item
    //
    QStandardItem * mypItem = 
      mModelTools->findItems(theIndex.data(Qt::DisplayRole).toString()).first();

    //show module details in the detail widget
    //QgsDetailedItemData myData = 
    //  qVariantValue<QgsDetailedItemData>(mypItem->data(MODULE_DATA_ROLE));
    //mpDetailsEdit->setText("Description:\n" + myData.detail());
    mpDetailsEdit->setHtml("<b>" + mypItem->data(MODULE_NAME_ROLE).toString() + "</b><br>" + 
        mypItem->data(MODULE_DESCRIPTION_ROLE).toString());
    //dont actually run the module (see double clicked for that)
  }
}

void QgsGrassTools::on_mpListView_doubleClicked(const QModelIndex &theIndex )
{
  if (theIndex.column() == 0)
  {
    //
    // If the model has been filtered, the index row in the proxy wont match 
    // the index row in the underlying model so we need to jump through this 
    // little hoop to get the correct item
    //
    QStandardItem * mypItem = 
      mModelTools->findItems(theIndex.data(Qt::DisplayRole).toString()).first();
    QString myModuleName = mypItem->data(MODULE_NAME_ROLE).toString();

    QString path = QgsApplication::pkgDataPath() + "/grass/modules/" + myModuleName;
    QgsGrassModule * mypModule = new QgsGrassModule ( this, 
        myModuleName, 
        mIface, 
        path, 
        this);
    mypModule->show();
  }
}
