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
#ifndef QGSNEWOGRGENERICCONNECTION_H
#define QGSNEWOGRGENERICCONNECTION_H
#include "ui_qgsnewogrgenericconnectionbase.h"
#include "qgisgui.h"
#include "qgsconnectionparameters.h"
#include "qgsconnectionregistry.h"
#include "qgsdatabaseconnection.h"
#include "qgsogrdatabaseconnection.h"
/*! \class QgsNewConnection
 * \brief Dialog to allow the user to configure and save connection
 * information for a PostgresQl database
 */
class QgsNewOgrGenericConnection : public QDialog, private Ui::QgsNewOgrGenericConnectionBase 
{
  Q_OBJECT
  public:
    //! Constructor
    QgsNewOgrGenericConnection(QWidget *parent = 0, const QgsConnectionParameters* conn=0, Qt::WFlags fl = QgisGui::ModalDialogFlags);
    //! Destructor
    ~QgsNewOgrGenericConnection();
    //! Tests the connection using the parameters supplied
    void testConnection();
    //! Saves the connection to ~/.qt/qgisrc
    void saveConnection();
    //! Display the context help
    void helpInfo();
  public slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();
    void on_btnHelp_clicked();
    void on_btnConnect_clicked();
    //void on_cb_geometryColumnsOnly_clicked();
  private:
    QgsConnectionRegistry* mConnReg;
    static const int context_id = 929865718;
};

#endif //  QGSNEWCONNECTIONBASE_H
