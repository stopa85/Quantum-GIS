/***************************************************************************
                    qgsnewconnection.cpp  -  description
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
#include <iostream>

#include <QSettings>
#include <QMessageBox>

#include "qgsnewconnection.h"
#include "qgscontexthelp.h"
extern "C"
{
#include <libpq-fe.h>
}
QgsNewConnection::QgsNewConnection(QWidget *parent, const QgsConnectionParameters* conn, Qt::WFlags fl)
: QDialog(parent, fl)
{
  setupUi(this);
  //create member objects
  mConnReg=new QgsConnectionRegistry;
  //set the type of connection
  QString type=mConnReg->selectedType();
  cmbType->setCurrentIndex(cmbType->findText(type)); 
  
  if (conn!=NULL)
    {
      qDebug("QgsNewConnection::QgsNewConnection : host "+conn->host);
      txtHost->setText(conn->host);
      txtDatabase->setText(conn->database);
      if (conn->save)
        txtPassword->setText(conn->password);
      
      txtPort->setText(conn->port);
      txtUsername->setText(conn->user);
      if (conn->publicOnly)
         cb_publicSchemaOnly->setCheckState(Qt::Checked);
      else   
         cb_publicSchemaOnly->setCheckState(Qt::Unchecked);
      
      if (conn->geometryColumnsOnly)
         cb_geometryColumnsOnly->setCheckState(Qt::Checked);
      else   
         cb_geometryColumnsOnly->setCheckState(Qt::Unchecked);
            
      if (conn->save)
         chkStorePassword->setCheckState(Qt::Checked);
      else   
         chkStorePassword->setCheckState(Qt::Unchecked);
      txtName->setText(conn->name);
      cmbType->setCurrentText(conn->type);
    }
  
}

/** Autoconnected SLOTS **/
void QgsNewConnection::on_btnOk_clicked()
{
  saveConnection();
}
void QgsNewConnection::on_btnHelp_clicked()
{
 helpInfo();
}
void QgsNewConnection::on_btnConnect_clicked()
{
  testConnection();
}
void QgsNewConnection::on_btnCancel_clicked(){
  // cancel the dialog
  reject();
}
void QgsNewConnection::on_cb_geometryColumnsOnly_clicked()
{
  if (cb_geometryColumnsOnly->checkState() == Qt::Checked)
    cb_publicSchemaOnly->setEnabled(false);
  else
    cb_publicSchemaOnly->setEnabled(true);
}

/** end  Autoconnected SLOTS **/

QgsNewConnection::~QgsNewConnection()
{
  delete mConnReg;                                   
}
void QgsNewConnection::testConnection()
{
     
  QgsDatabaseConnection* connection=NULL;   
  QgsConnectionParameters connPar;
  
  connPar.type=cmbType->currentText();
  connPar.name=txtName->text();
  connPar.host=txtHost->text();
  connPar.database=txtDatabase->text();
  connPar.port=txtPort->text();
  connPar.user=txtUsername->text();
  connPar.password=txtPassword->text();
  connPar.geometryColumnsOnly=cb_geometryColumnsOnly->isChecked();
  connPar.publicOnly=cb_publicSchemaOnly->isChecked();
  connPar.selected=txtName->text();
  connPar.save=chkStorePassword->isChecked();
  
  if (connPar.type.startsWith("Ogr"))
    {
      connection=new QgsOgrDatabaseConnection(&connPar);
    }
  else
    {
      connection=new QgsPostgresDatabaseConnection(&connPar);               
    }    
  if (connection->connect())
    {
      QMessageBox::information(this, tr("Test connection"), tr("Connection to %1 was successful").arg(txtName->text()));
    } else
    {
      QMessageBox::information(this, tr("Test connection"), tr("Connection failed - Check settings and try again.\n\nExtended error information:\n") +connection->error());
    }
 delete connection;   
  
}

void QgsNewConnection::saveConnection()
{
  QgsConnectionParameters conn;   
  conn.type=cmbType->currentText();
  conn.name=txtName->text();
  conn.host=txtHost->text();
  conn.database=txtDatabase->text();
  conn.port=txtPort->text();
  conn.user=txtUsername->text();
  if (chkStorePassword->isChecked())
    conn.password=txtPassword->text();
  else
    conn.password="";  
  
  conn.geometryColumnsOnly=cb_geometryColumnsOnly->isChecked();
  conn.publicOnly=cb_publicSchemaOnly->isChecked();
  conn.selected=txtName->text();
  conn.save=chkStorePassword->isChecked();
  
  mConnReg->saveConnection(conn);
  mConnReg->setSelectedType(conn.type);
  mConnReg->setSelected(conn.type,conn.name);                       
  accept();
}

void QgsNewConnection::helpInfo()
{
  QgsContextHelp::run(context_id);
}
