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
#ifndef QGSNEWORACLECONNECTION_H
#define QGSNEWORACLECONNECTION_H

#include "ui_qgsnewogrconnection.h"
#include "qgisgui.h"


class QgsNewOGRConnection: public QDialog, private Ui::QgsNewOGRConnection
{
	Q_OBJECT
	public:
		QgsNewOGRConnection(QWidget* parent = 0, const QString& connName = QString::null, Qt::WFlags fl = QgisGui::ModalDialogFlags);
		~QgsNewOGRConnection();
		void saveConnection();
		
	public slots:
		void on_btnOk_clicked();
    	void on_btnCancel_clicked();	 
};

#endif // QGSNEWORACLECONNECTION_H
