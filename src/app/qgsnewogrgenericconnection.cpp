/***************************************************************************
                    qgsnewogrgenericconnection.cpp  -  description
                             -------------------
    begin                : Wen Jan 08 2008
    copyright            : (C) 2008 by Godofredo Contreras
    email                : frdcn at hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 /* $Id: qgsnewogrconnection.cpp 7413 2007-11-15 19:34:15Z godofredo $ */
 
#include <iostream>

#include <QSettings>
#include <QMessageBox>

#include "qgsnewogrgenericconnection.h"
#include "qgscontexthelp.h"
extern "C"
{
#include <libpq-fe.h>
}
QgsNewOgrGenericConnection::QgsNewOgrGenericConnection(QWidget *parent, const QgsConnectionParameters* conn, Qt::WFlags fl)
: QDialog(parent, fl)
{
  setupUi(this);
 
  //create member objects
  mConnReg=new QgsConnectionRegistry;
  
  if (conn!=NULL)
    {
      txtName->setText(conn->name);
      txtUri->setText(conn->uri);
    } 
      
  
}

/** Autoconnected SLOTS **/
void QgsNewOgrGenericConnection::on_btnOk_clicked()
{
  saveConnection();
}
void QgsNewOgrGenericConnection::on_btnHelp_clicked()
{
 helpInfo();
}
void QgsNewOgrGenericConnection::on_btnConnect_clicked()
{
  testConnection();
}
void QgsNewOgrGenericConnection::on_btnCancel_clicked(){
  // cancel the dialog
  reject();
}


/** end  Autoconnected SLOTS **/

QgsNewOgrGenericConnection::~QgsNewOgrGenericConnection()
{
  delete mConnReg;                                   
}
void QgsNewOgrGenericConnection::testConnection()
{
  QgsConnectionParameters connPar;
  connPar.type="OgrGeneric";
  connPar.name=txtName->text();
  connPar.uri=txtUri->text();
  QgsDatabaseConnection* connection=new QgsOgrDatabaseConnection(&connPar);
  connection->setUri(txtUri->text());
  if (connection->connect())
    {
      // Database successfully opened; we can now issue SQL commands.
      QMessageBox::information(this, tr("Test connection"), tr("Connection to %1 was successful").arg(txtUri->text()));
    } else
    {
      QMessageBox::information(this, tr("Test connection"), tr("Connection failed - Check settings and try again.\n\nExtended error information:\n") +connection->error());
    }
 qDebug("Step three");
 delete connection;
}

void QgsNewOgrGenericConnection::saveConnection()
{
  QgsConnectionParameters conn;   
  
  conn.type="OgrGeneric";
  conn.name=txtName->text();
  conn.uri=txtUri->text();
  
  mConnReg->saveConnection(conn);
  mConnReg->setSelectedType(conn.type);
  mConnReg->setSelected(conn.type,conn.name);                       
  accept();
}

void QgsNewOgrGenericConnection::helpInfo()
{
  QgsContextHelp::run(context_id);
}
