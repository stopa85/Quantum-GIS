/***************************************************************************
                              qgsoraclesourceselect.cpp  
                              -------------------
  begin                : September 28, 2006
  copyright            : (C) 2006 by Stephan Holl
  email                : stephan at holl minus land dot de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 

#include "qgisinterface.h"
#include "qgsogrsourceselect.h"
#include "qgsnewogrconnection.h"
//#include "qgisapp.h"
//#include "qgsapplication.h"


#include <QTableWidgetItem>
#include <QTextOStream>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>

#include <cassert>
#include <iostream>



QgsOGRSourceSelect::QgsOGRSourceSelect(QgisInterface* _qI, QWidget* parent, Qt::WFlags fl )
: QDialog(parent) , mIface(_qI)
{
  setupUi(this);
  
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
  connect(btnAdd, SIGNAL(clicked()), this, SLOT(addLayer()));
  connect(btnNew, SIGNAL(clicked()), this, SLOT(addEntryToServerList()));
  connect(btnEdit, SIGNAL(clicked()), this, SLOT(modifyEntryOfServerList()));
  connect(btnDelete, SIGNAL(clicked()), this, SLOT(deleteEntryOfServerList()));
  connect(btnConnect,SIGNAL(clicked()), this, SLOT(connectToServer()));
  connect(btnHelp,SIGNAL(clicked()), this, SLOT(showHelp()));
  
  populateConnectionList();
  
  
}
  
QgsOGRSourceSelect::~QgsOGRSourceSelect()
{
	
}

void QgsOGRSourceSelect::populateConnectionList()
{
  QSettings settings;
  QStringList keys = settings.subkeyList("/Qgis/ogr-connections");
  QStringList::Iterator it = keys.begin();
  cmbConnections->clear();
  while (it != keys.end())
  {
    cmbConnections->insertItem(*it);
    ++it;
  }
  
  if (keys.begin() != keys.end())
  {
    // Connections available - enable various buttons
    btnConnect->setEnabled(TRUE);
    btnEdit->setEnabled(TRUE);
    btnDelete->setEnabled(TRUE);
  }
  else
  {
    // No connections available - disable various buttons
    btnConnect->setEnabled(FALSE);
    btnEdit->setEnabled(FALSE);
    btnDelete->setEnabled(FALSE);
  }
  
}

void QgsOGRSourceSelect::addEntryToServerList()
{
	QgsNewOGRConnection *nc = new QgsNewOGRConnection(this, "New OGR Connection");

	if (nc->exec())
	{
		populateConnectionList();
	}
}

void QgsOGRSourceSelect::modifyEntryOfServerList()
{
	QgsNewOGRConnection *nc = new QgsNewOGRConnection(this, cmbConnections->currentText());
	
	if (nc->exec())
	{
		nc->saveConnection();
	}
	populateConnectionList();
}

void QgsOGRSourceSelect::deleteEntryOfServerList()
{
	QSettings settings;
	QString key = "/Qgis/ogr-connections/" + cmbConnections->currentText();
	QString msg =
		tr("Are you sure you want to remove the ") + cmbConnections->currentText() + tr(" connection and all associated settings?");
	int result = QMessageBox::information(this, tr("Confirm Delete"), msg, tr("Yes"), tr("No"));
	if (result == 0)
	{
		settings.remove(key);
		cmbConnections->removeItem(cmbConnections->currentItem());
	}
	
	populateConnectionList();
}

void QgsOGRSourceSelect::connectToServer()
{
   // populate the table list
  QSettings settings;
  QString key = "/Qgis/ogr-connections/" + cmbConnections->currentText();
  //get type of connection
  QString connType = settings.readEntry(key + "/type");
  mType=connType;
  QString password;
  QString connString;
  QString database;
  QString username;
  QString host;
  QString ogrProviderParams;
  //type mysql
  qDebug("Connection-type is: " + connType);
  ogrProviderParams="type="+connType+","
                    +"server="+settings.readEntry(key + "/server")+","
                    +"database="+settings.readEntry(key + "/database")+","
                    +"user="+settings.readEntry(key + "/username")+",";
  password = settings.readEntry(key + "/password");           
             
  if (connType=="MySQL")
   {
     connString = "MySQL:";
     database = settings.readEntry(key + "/database");
     connString+=database;
     username = settings.readEntry(key + "/username");
     connString +=",user="+username;
     password = settings.readEntry(key + "/password");
     connString += ",password=" + password;
     host = settings.readEntry(key + "/server");
     connString += ",host="+ host;
   } 
   else if (connType=="PostgreSQL")
   {
     connString = "PG:";
     database = settings.readEntry(key + "/database");
     connString+="dbname="+database;
     username = settings.readEntry(key + "/username");
     connString +=" user="+username;
     password = settings.readEntry(key + "/password");
     connString += " password=" + password;
     host = settings.readEntry(key + "/server");
     connString += " host="+ host;
   }
  

  bool makeConnection = true;
  
  if (password == QString::null) {
    // get password from user 
    makeConnection = false;
    QString password = QInputDialog::getText(tr("Password for ") + username + "/<password>@" + database,
        tr("Please enter your password:"),
        QLineEdit::Password, QString::null, &makeConnection, this);

    //  allow null password entry in case its valid for the database
  }
  ogrProviderParams+="password="+password;

#ifdef QGISDEBUG
  std::cout << "Connection info: " << connString.toLocal8Bit().data() << std::endl;
#endif
  if (makeConnection) {
	mParams = ogrProviderParams;
    mUri = connString;
    qDebug("Connection-String is: " + mUri);

    // Register all OGR-drivers
    OGRRegisterAll();
	
	OGRDataSource *poDS;
	
	poDS = OGRSFDriverRegistrar::Open( mUri, FALSE );
	if( poDS == NULL ) {
        printf( "Open failed.\n" );
		QMessageBox::information(this, tr("Open failed"), tr("The connection to " + mUri + " failed. Please verify your connection parameters. Make sure you have compiled the OCI-bindungs into GDAL/OGR."));
        return;
    } 
	
    // get layercount
    nLayers = poDS->GetLayerCount(); // poLayerCount;
	
	// create a pointer to the layer
	OGRLayer  *poLayer;
	// create a pointer to the feature
    //OGRFeature *poFeature;
	// create a pointer to the feature-definition
    OGRFeatureDefn *poFDefn;
        
	//layer in lstTables
	// Here's an annoying thing... calling clear() removes the
	// header items too, so we need to reinstate them after calling
	// clear(). 
	lstTables->clear();
	
    // loop through all available layers
	for (int j = 0; j < nLayers  ; j++) {
    
 		poLayer = poDS->GetLayer( j );
		poLayer->ResetReading();
	
		poFDefn = poLayer->GetLayerDefn();
        #ifdef QGISDEBUG			
		// show names of the layer
		std::cout << "Name of Layer: " << poFDefn->GetName() << std::endl;
		// show geometry of layer
		std::cout << "Geometry of Layer: " << poFDefn->GetGeomType() << std::endl;
				
        #endif
        //limit tables shown to those who have a geometry column
        if (wkbNone!=poFDefn->GetGeomType())
          {
		    QListWidgetItem *textItem = new QListWidgetItem(poFDefn->GetName());
		    lstTables->addItem(textItem);
          }  
		
	} // end of for()
	
	// destroy datasource again when finished
	OGRDataSource::DestroyDataSource( poDS );
  }
} // END QgsOracleSourceSelect::connectToServer()

void QgsOGRSourceSelect::addLayer()
{
	
	//get selected entry in lstWidget
	QListWidgetItem* cItem = lstTables->currentItem();
	if(!cItem)
	{
		QMessageBox::information(this, tr("Select Table"), tr("You must select a table in order to add a Layer."));
		return;
	}
	QString table = cItem->text();
	
	//add a ogr layer to the map
	if(mIface)
	{
        if ((mType=="MySQL")||(mType=="PostgreSQL"))
          {
            #ifdef QGISDEBUG			
		    std::cout << "Layer uri: " << mUri.toLocal8Bit().data() << " " << table.toLocal8Bit().data() << std::endl;
		    std::cout << "Layer params: " << mParams.toLocal8Bit().data() << ",table=" << table.toLocal8Bit().data() << std::endl;
	        #endif      
	        qDebug("Layer uri: " + mUri + " " + table);
		    mIface->addVectorLayer(mParams+ ",table=" + table, table, "ogr");
          }
	}
	accept();
}

void QgsOGRSourceSelect::showHelp()
{
	// implement me
}

