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
QgsNewConnection::QgsNewConnection(QWidget *parent, const QgsConnection* conn, Qt::WFlags fl)
: QDialog(parent, fl)
{
  setupUi(this);
  if (conn!=NULL)
    {
      qDebug("QgsNewConnection::QgsNewConnection : host "+conn->host);
      txtHost->setText(conn->host);
      txtDatabase->setText(conn->database);
      /*QString port = settings.readEntry(key + "/port");
      if(port.length() ==0){
      	port = "5432";
      }*/
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
            

      txtName->setText(conn->name);
      qDebug("QgsNewConnection::QgsNewConnection : type "+conn->type);
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
  //delete connManager;                                   
}
void QgsNewConnection::testConnection()
{
  // following line uses Qt SQL plugin - currently not used
  // QSqlDatabase *testCon = QSqlDatabase::addDatabase("QPSQL7","testconnection");
  QgsConnectionManager* connMan=new QgsConnectionManager;
  QgsConnection conn;
  
  conn.type=cmbType->currentText();
  conn.name=txtName->text();
  conn.host=txtHost->text();
  conn.database=txtDatabase->text();
  conn.port=txtPort->text();
  conn.user=txtUsername->text();
  conn.password=txtPassword->text();
  conn.geometryColumnsOnly=cb_geometryColumnsOnly->isChecked();
  conn.publicOnly=cb_publicSchemaOnly->isChecked();
  conn.selected=txtName->text();
  conn.save=chkStorePassword->isChecked();
  
  qDebug("QgsNewConnection::testConnection: before connect");
  
  if (connMan->connect(cmbType->currentText(),conn))
    {
      // Database successfully opened; we can now issue SQL commands.
      QMessageBox::information(this, tr("Test connection"), tr("Connection to %1 was successful").arg(txtDatabase->text()));
    } else
    {
      QMessageBox::information(this, tr("Test connection"), tr("Connection failed - Check settings and try again.\n\nExtended error information:\n") +connMan->getError());
    }
 qDebug("QgsNewConnection::testConnection: after connect");
 delete connMan;
}

void QgsNewConnection::saveConnection()
{
  QgsConnection conn;   
  
  conn.type=cmbType->currentText();
  conn.name=txtName->text();
  conn.host=txtHost->text();
  conn.database=txtDatabase->text();
  conn.port=txtPort->text();
  conn.user=txtUsername->text();
  conn.password=txtPassword->text();
  
  conn.geometryColumnsOnly=cb_geometryColumnsOnly->isChecked();
  conn.publicOnly=cb_publicSchemaOnly->isChecked();
  conn.selected=txtName->text();
  conn.save=chkStorePassword->isChecked();
  
  connManager.saveConnection(conn);
 
  accept();
}

void QgsNewConnection::helpInfo()
{
  QgsContextHelp::run(context_id);
}
