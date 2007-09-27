/***************************************************************************
                              qgsoraclesourceselect.h 
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
#ifndef QGSOGRSOURCESELECT_H
#define QGSOGRSOURCESELECT_H

#include "ui_qgsogrsourceselect.h"
#include <qgsogrprovider.h>
//#include <gdal.h> 
#include <ogr_api.h>
#include <ogrsf_frmts.h>


class QgisInterface;
//class QListWidgetItem;
//class QgisApp;

class QgsOGRSourceSelect: public QDialog, private Ui::QgsOGRSourceSelect
{
	Q_OBJECT
	public:
	  QgsOGRSourceSelect(QgisInterface* _qI, QWidget* parent = 0, Qt::WFlags fl = 0 );
	  ~QgsOGRSourceSelect();
	  
	private:
	  QgsOGRSourceSelect();
	  QgisInterface* mIface;
	  QString mUri; //uri of the currently connected server
	  QString mType; //type of connection being used
	  QString mParams;
	  void populateConnectionList();
	  int nLayers; 
	  
	 

	
	private slots:
	  void addEntryToServerList();
      void modifyEntryOfServerList();
  	  void deleteEntryOfServerList();
  	  void connectToServer();
  	  void addLayer();	
	  void showHelp();
};

#endif
