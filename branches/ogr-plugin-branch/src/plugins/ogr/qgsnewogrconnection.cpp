/***************************************************************************
                              qgsneworacleconnection.cpp  
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
#include <iostream>
#include "qgsnewogrconnection.h"
#include <QSettings>

QgsNewOGRConnection::QgsNewOGRConnection(QWidget *parent, const QString& connName, Qt::WFlags fl): QDialog(parent, fl)
{
  setupUi(this);
  if (!connName.isEmpty())
  {
  	QSettings settings;
	
	QString key = "/Qgis/ogr-connections/" + connName;
	cmbType->setCurrentIndex(cmbType->findText(settings.readEntry(key + "/type")));
	txtServer->setText(settings.readEntry(key + "/server"));
    txtDatabase->setText(settings.readEntry(key + "/database"));
    txtUsername->setText(settings.readEntry(key + "/username"));
    	
	if (settings.readEntry(key + "/save") == "true")
    {
      txtPassword->setText(settings.readEntry(key + "/password"));
      chkStorePassword->setChecked(true);
    }
      txtName->setText(connName);
  }
}

/** Autoconnected SLOTS **/
void QgsNewOGRConnection::on_btnOk_clicked()
{
  saveConnection();
}

void QgsNewOGRConnection::on_btnCancel_clicked(){
  // cancel the dialog
  reject();
}

QgsNewOGRConnection::~QgsNewOGRConnection()
{
}

void QgsNewOGRConnection::saveConnection()
{
  QSettings settings; 
  QString baseKey = "/Qgis/ogr-connections/";
  settings.writeEntry(baseKey + "selected", txtName->text());
  baseKey += txtName->text();
  settings.writeEntry(baseKey + "/type", cmbType->currentText());
  settings.writeEntry(baseKey + "/server", txtServer->text());
  settings.writeEntry(baseKey + "/database", txtDatabase->text());
  settings.writeEntry(baseKey + "/username", txtUsername->text());
  settings.writeEntry(baseKey + "/password", txtPassword->text());
  
  accept();
}
