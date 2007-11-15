/***************************************************************************
                             qgsdbsourceselect.cpp  
       Dialog to select PostgreSQL layer(s) and add it to the map canvas
                              -------------------
begin                : Sat Jun 22 2002
copyright            : (C) 2002 by Gary E.Sherman
email                : sherman at mrcc.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#include "qgsdbsourceselect.h"

#include "qgisapp.h"
#include "qgsapplication.h"
#include "qgscontexthelp.h"
#include "qgsnewconnection.h"
#include "qgspgquerybuilder.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextOStream>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QStringList>

#include <cassert>
#include <iostream>

QgsDbSourceSelect::QgsDbSourceSelect(QgisApp *app, Qt::WFlags fl)
  : QDialog(app, fl), mColumnTypeThread(NULL), qgisApp(app), pd(0)
{
  setupUi(this);
  
  //connect 
  
  connect(cmbType,SIGNAL(currentIndexChanged(QString)),this,SLOT(populateConnectionList(QString)));
  btnAdd->setEnabled(false);
  //create connection manager
  mConnMan=new QgsConnectionManager;
  
  //get last connection type used
  QString conType=mConnMan->getSelectedType();
  populateConnectionList(conType);
  setConnectionListPosition();
  //loadConnections("PostgreSQL");
  // connect the double-click signal to the addSingleLayer slot in the parent

  //disable the 'where clause' box for 0.4 release
  //  groupBox3->hide();
  //insert the encoding types available in qt
  mEncodingComboBox->insertItem("BIG5"); 
  mEncodingComboBox->insertItem("BIG5-HKSCS"); 
  mEncodingComboBox->insertItem("EUCJP"); 
  mEncodingComboBox->insertItem("EUCKR"); 
  mEncodingComboBox->insertItem("GB2312"); 
  mEncodingComboBox->insertItem("GBK"); 
  mEncodingComboBox->insertItem("GB18030"); 
  mEncodingComboBox->insertItem("JIS7"); 
  mEncodingComboBox->insertItem("SHIFT-JIS"); 
  mEncodingComboBox->insertItem("TSCII"); 
  mEncodingComboBox->insertItem("UTF-8"); 
  mEncodingComboBox->insertItem("UTF-16"); 
  mEncodingComboBox->insertItem("KOI8-R"); 
  mEncodingComboBox->insertItem("KOI8-U"); 
  mEncodingComboBox->insertItem("ISO8859-1"); 
  mEncodingComboBox->insertItem("ISO8859-2");
  mEncodingComboBox->insertItem("ISO8859-3"); 
  mEncodingComboBox->insertItem("ISO8859-4"); 
  mEncodingComboBox->insertItem("ISO8859-5"); 
  mEncodingComboBox->insertItem("ISO8859-6");
  mEncodingComboBox->insertItem("ISO8859-7"); 
  mEncodingComboBox->insertItem("ISO8859-8"); 
  mEncodingComboBox->insertItem("ISO8859-8-I"); 
  mEncodingComboBox->insertItem("ISO8859-9"); 
  mEncodingComboBox->insertItem("ISO8859-10"); 
  mEncodingComboBox->insertItem("ISO8859-13"); 
  mEncodingComboBox->insertItem("ISO8859-14"); 
  mEncodingComboBox->insertItem("ISO8859-15"); 
  mEncodingComboBox->insertItem("IBM 850"); 
  mEncodingComboBox->insertItem("IBM 866"); 
  mEncodingComboBox->insertItem("CP874"); 
  mEncodingComboBox->insertItem("CP1250"); 
  mEncodingComboBox->insertItem("CP1251"); 
  mEncodingComboBox->insertItem("CP1252"); 
  mEncodingComboBox->insertItem("CP1253"); 
  mEncodingComboBox->insertItem("CP1254"); 
  mEncodingComboBox->insertItem("CP1255"); 
  mEncodingComboBox->insertItem("CP1256"); 
  mEncodingComboBox->insertItem("CP1257"); 
  mEncodingComboBox->insertItem("CP1258"); 
  mEncodingComboBox->insertItem("Apple Roman"); 
  mEncodingComboBox->insertItem("TIS-620"); 

  //read the last encoding from the settings
  //or use local as default
  QSettings settings; 
  QString lastUsedEncoding = settings.readEntry("/UI/encoding");
  if(lastUsedEncoding.isNull()||lastUsedEncoding.isEmpty()||lastUsedEncoding=="\0")
    {
      mEncodingComboBox->setCurrentText(QString(QTextCodec::codecForLocale()->name()));
    }
  else
    {
      mEncodingComboBox->setCurrentText(lastUsedEncoding);
    }

  // Do some things that couldn't be done in designer
  lstTables->horizontalHeader()->setStretchLastSection(true);
  // Set the column count to 3 for the type, name, and sql
  lstTables->setColumnCount(3);
  mColumnLabels += tr("Type"); 
  mColumnLabels += tr("Name"); 
  mColumnLabels += tr("Sql");
  lstTables->setHorizontalHeaderLabels(mColumnLabels);
  lstTables->verticalHeader()->hide();
  
  
}
/** Autoconnected SLOTS **/
// Slot for adding a new connection
void QgsDbSourceSelect::on_btnNew_clicked()
{
  addNewConnection();
}
// Slot for deleting an existing connection
void QgsDbSourceSelect::on_btnDelete_clicked()
{
  deleteConnection();
}
// Slot for performing action when the Add button is clicked
void QgsDbSourceSelect::on_btnAdd_clicked()
{
  addTables();
}

// Slot for opening the query builder when a layer is double clicked
void QgsDbSourceSelect::on_lstTables_itemDoubleClicked(QTableWidgetItem *item)
{
  setSql(item);
}

// Slot for editing a connection
void QgsDbSourceSelect::on_btnEdit_clicked()
{
  editConnection();
}

// Slot for showing help
void QgsDbSourceSelect::on_btnHelp_clicked()
{
  showHelp();
}
/** End Autoconnected SLOTS **/

//remember what type is selected
void QgsDbSourceSelect::on_cmbType_activated(int)
{
  dbChanged();
}
// Remember which database is selected
void QgsDbSourceSelect::on_cmbConnections_activated(int)
{
  dbChanged();
}

void QgsDbSourceSelect::setLayerType(QString schema, 
                                     QString table, QString column,
                                     QString type)
{
/*                                     
#ifdef QGISDEBUG
  std::cerr << "Received layer type of " << type.toLocal8Bit().data()
            << " for " 
            << (schema+'.'+table+'.'+column).toLocal8Bit().data() 
            << '\n';
#endif

  // Find the right row in the table by searching for the text that
  // was put into the Name column.
  QString full_desc = fullDescription(schema, table, column);

  QList<QTableWidgetItem*> ii = lstTables->findItems(full_desc,
                                                     Qt::MatchExactly);

  if (ii.count() > 0)
  {
    int row = lstTables->row(ii.at(0)); // just use the first one
    QTableWidgetItem* iconItem = lstTables->item(row, 0);

    // Get the icon and tooltip text 
    const QIcon* p;
    QString toolTipText;
    if (mLayerIcons.contains(type))
    {
      p = &(mLayerIcons.value(type).second);
      toolTipText = mLayerIcons.value(type).first;
    }
    else
    {
      qDebug(("Unknown geometry type of '" + type + "'.").toLocal8Bit().data());
      p = &(mLayerIcons.value("UNKNOWN").second);
      toolTipText = mLayerIcons.value("UNKNOWN").first;
    }

    iconItem->setIcon(*p);
    iconItem->setToolTip(toolTipText);
  }*/
}



QgsDbSourceSelect::~QgsDbSourceSelect()
{
    delete mConnMan;                                   
    PQfinish(pd);
}
void QgsDbSourceSelect::populateConnectionList(QString type)
{
  qDebug("QgsDbSourceSelect::populateConnectionList : type "+type);   
  
  QStringList connlist=mConnMan->getConnections(type);
  cmbConnections->clear();
  //add the results to combo
  for (int i = 0; i < connlist.size(); ++i)
     cmbConnections->addItem(connlist.at(i));
  cmbType->setCurrentIndex(cmbType->findText(type)); 
}
void QgsDbSourceSelect::addNewConnection()
{
  QgsNewConnection *nc = new QgsNewConnection(this);

  if (nc->exec())
  {
    populateConnectionList(cmbType->currentText());
  }
}
void QgsDbSourceSelect::editConnection()
{
  QgsConnection conn=mConnMan->getConnectionDetails(cmbType->currentText(),cmbConnections->currentText());
  QgsNewConnection *nc = new QgsNewConnection(this, &conn);

  if (nc->exec())
  {
    nc->saveConnection();
    populateConnectionList(cmbType->currentText());
  }
}

void QgsDbSourceSelect::deleteConnection()
{
   mConnMan->removeConnection(cmbType->currentText(),cmbConnections->currentText());   
   populateConnectionList(cmbType->currentText());

}
void QgsDbSourceSelect::addTables()
{
  QString type=cmbType->currentText();
  
  //store the table info

  for (int i = 0; i < lstTables->rowCount(); ++i)
  {
    if (lstTables->isItemSelected(lstTables->item(i, 0)))
    {
      QString table;                                               
      //if(type.contains("Ogr",FALSE)>0)
      // table = lstTables->item(i,1)->text(); 
      //else 
      // {                                              
         table = lstTables->item(i,1)->text() + " sql=";
         QTableWidgetItem* sqlItem = lstTables->item(i,2);
         if (sqlItem)
          table += sqlItem->text();
      // }   
      m_selectedTables += table;
    }
  }

  // BEGIN CHANGES ECOS
  if (m_selectedTables.empty() == true)
    QMessageBox::information(this, tr("Select Table"), tr("You must select a table in order to add a Layer."));
  else
    accept();
  // END CHANGES ECOS
  
  
}

void QgsDbSourceSelect::on_btnConnect_clicked()
{
  qDebug("QgsDbSourceSelect::on_btnConnect_clicked");   
  QgsConnection conn=mConnMan->getConnectionDetails(cmbType->currentText(),cmbConnections->currentText());   
  bool makeConnection=true;
  if (conn.password == QString::null)
      {
       // get password from user 
       makeConnection = false;
       QString password = QInputDialog::getText(tr("Password for ") + conn.database + "@" + conn.host,
       tr("Please enter your password:"),
       QLineEdit::Password, QString::null, &makeConnection, this);
       // allow null password entry in case its valid for the database
       password.replace('\\', "\\\\");
       password.replace('\'', "\\'");
       conn.password=password;
       }   
 
   
  if (mConnMan->connect(cmbType->currentText(),conn))
    {
      // Database successfully opened; we can now issue SQL commands.
      // create the pixmaps for the layer types if we haven't already
      // done so.
      m_connInfo="type="+conn.type
                +" host="+conn.host
                +" dbname="+conn.database
                +" port="+conn.port
                +" user="+conn.user
                +" password='"+conn.password+"'"; 
                
     if (mLayerIcons.count() == 0)
       {
         QString myThemePath = QgsApplication::themePath();
                mLayerIcons.insert("POINT",
                                    qMakePair(tr("Point layer"), 
                                    QIcon(myThemePath+"/mIconPointLayer.png")));
                mLayerIcons.insert("MULTIPOINT", 
                                   qMakePair(tr("Multi-point layer"), 
                                   mLayerIcons.value("POINT").second));

                mLayerIcons.insert("LINESTRING",
                                   qMakePair(tr("Linestring layer"), 
                                   QIcon(myThemePath+"/mIconLineLayer.png")));
                mLayerIcons.insert("MULTILINESTRING",
                                   qMakePair(tr("Multi-linestring layer"), 
                                   mLayerIcons.value("LINESTRING").second));

                mLayerIcons.insert("POLYGON",
                                   qMakePair(tr("Polygon layer"), 
                                   QIcon(myThemePath+"/mIconPolygonLayer.png")));
                mLayerIcons.insert("MULTIPOLYGON",
                                   qMakePair(tr("Multi-polygon layer"),
                                   mLayerIcons.value("POLYGON").second));

                mLayerIcons.insert("GEOMETRY",
                                   qMakePair(tr("Mixed geometry layer"), 
                                   QIcon(myThemePath+"/mIconGeometryLayer.png")));
                mLayerIcons.insert("GEOMETRYCOLLECTION",
                                   qMakePair(tr("Geometry collection layer"), 
                                   mLayerIcons.value("GEOMETRY").second));

                mLayerIcons.insert("WAITING",
                                   qMakePair(tr("Waiting for layer type"), 
                                   QIcon(myThemePath+"/mIconWaitingForLayerType.png")));
                mLayerIcons.insert("UNKNOWN",
                                   qMakePair(tr("Unknown layer type"), 
                                   QIcon(myThemePath+"/mIconUnknownLayerType.png")));
       }                          
       lstTables->clear();
       lstTables->setRowCount(0);
       lstTables->setHorizontalHeaderLabels(mColumnLabels);

       QStringList tables=mConnMan->getConnection()->getGeometryTables();
       for (int i = 0; i < tables.size(); ++i)
        { 
            QString toolTipText;
            const QIcon* p;      
            QString geometryType=mConnMan->getConnection()->getTableGeometry(tables.at(i));      
            if (mLayerIcons.contains(geometryType))
             {
               p = &(mLayerIcons.value(geometryType).second);
               toolTipText = mLayerIcons.value(geometryType).first;
               qDebug(geometryType); 
             }
            else
             {
	          qDebug(("Unknown geometry type of '" + geometryType + "'.").toLocal8Bit().data());
              p = &(mLayerIcons.value("UNKNOWN").second);
              toolTipText = mLayerIcons.value("UNKNOWN").first;
             } 
            if (p != 0)
             {
              QTableWidgetItem *iconItem = new QTableWidgetItem();
              iconItem->setIcon(*p);
              iconItem->setToolTip(toolTipText);
              QTableWidgetItem *textItem = new QTableWidgetItem(tables.at(i));
              int row = lstTables->rowCount();
              lstTables->setRowCount(row+1);
              lstTables->setItem(row, 0, iconItem);
              lstTables->setItem(row, 1, textItem);
             }
        }
        

        // And tidy up the columns & rows
        lstTables->resizeColumnsToContents();
        lstTables->resizeRowsToContents();
                    
      
    } else
    {
     QMessageBox::information(this, tr("Connection"), tr("Connection failed - Check settings and try again.\n\nExtended error information:\n") +mConnMan->getError());
    }
    
    if (cmbConnections->count() > 0)
        btnAdd->setEnabled(true);
  
}

QStringList QgsDbSourceSelect::selectedTables()
{
  return m_selectedTables;
}


void QgsDbSourceSelect::setSql(QTableWidgetItem *item)
{
/*  int row = lstTables->row(item);
  QString tableText = lstTables->item(row, 1)->text();

  QTableWidgetItem* sqlItem = lstTables->item(row, 2);
  QString sqlText;
  if (sqlItem)
    sqlText = sqlItem->text();
  // Parse out the table name
  QString table = tableText.left(tableText.find("(")); */
  /*assert(pd != 0);
  // create a query builder object
  QgsPgQueryBuilder * pgb = new QgsPgQueryBuilder(table, pd, this);
  // set the current sql in the query builder sql box
  pgb->setSql(sqlText);
  // set the PG connection object so it can be used to fetch the
  // fields for the table, get sample values, and test the query
  pgb->setConnection(pd);
  // show the dialog
  if(pgb->exec())
  {
    // if user accepts, store the sql for the layer so it can be used
    // if and when the layer is added to the map
    if (!sqlItem)
    {
      sqlItem = new QTableWidgetItem();
      lstTables->setItem(row, 2, sqlItem);
    }
    sqlItem->setText(pgb->sql());
    // Ensure that the current row remains selected
    lstTables->setItemSelected(lstTables->item(row,0), true);
    lstTables->setItemSelected(lstTables->item(row,1), true);
    lstTables->setItemSelected(lstTables->item(row,2), true);
  }
  // delete the query builder object
  delete pgb;*/
}


QString QgsDbSourceSelect::encoding()
{
  return mEncodingComboBox->currentText();
}
void QgsDbSourceSelect::showHelp()
{
  QgsContextHelp::run(context_id);
}

void QgsDbSourceSelect::dbChanged()
{
  // Remember which database was selected.
  mConnMan->setSelectedType(cmbType->currentText());
  mConnMan->setSelected(cmbType->currentText(),cmbConnections->currentText());
  qDebug("Saving last connection position");
}

void QgsDbSourceSelect::setConnectionListPosition()
{
  qDebug("Setting connection list position"); 
  QString toSelect = mConnMan->getSelected(cmbType->currentText());
  // Does toSelect exist in cmbConnections?
  bool set = false;
  for (int i = 0; i < cmbConnections->count(); ++i)
    if (cmbConnections->text(i) == toSelect)
    {
      cmbConnections->setCurrentItem(i);
      set = true;
      break;
    }
  // If we couldn't find the stored item, but there are some, 
  // default to the last item (this makes some sense when deleting
  // items as it allows the user to repeatidly click on delete to
  // remove a whole lot of items).
  if (!set && cmbConnections->count() > 0)
  {
    // If toSelect is null, then the selected connection wasn't found
    // by QSettings, which probably means that this is the first time
    // the user has used qgis with database connections, so default to
    // the first in the list of connetions. Otherwise default to the last.
    if (toSelect.isNull())
      cmbConnections->setCurrentItem(0);
    else
      cmbConnections->setCurrentItem(cmbConnections->count()-1);
  }
}


QString QgsDbSourceSelect::connInfo()
{
  return m_connInfo;
}
