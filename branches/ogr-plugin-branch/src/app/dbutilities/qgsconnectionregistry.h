/***************************************************************************
 qgsconnectionregistry.cpp   Class to save and load connection parameter
                            from qgis settings. 
                              -------------------
  begin                : October 2, 2007
  copyright            : (C) 2007 by Godofredo Contreras
  email                : frdcn at hotmail dot com
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
 
#ifndef QGSCONNECTIONREGISTRY_H
#define QGSCONNECTIONREGISTRY_H


#include "qgsconnectionparameters.h"
#include "qstringlist.h"

/**
 * Class to save, modify and delete qgis database connection params wich are
 * stored in qgis registry. Also this class have methods to obtain a list of
 * connections of certain type and to set and get the last connection used and its
 * type.
 */
class QgsConnectionRegistry
{

public:
	QgsConnectionRegistry();
	~QgsConnectionRegistry();
	QgsConnectionParameters connectionParameters(QString type, QString name);
	QStringList connectionParametersList(QString type);
	QString selected(QString type);
	QString selectedType();
	bool removeConnectionParameters(QString type, QString name);
	bool modifyConnectionParameters(QgsConnectionParameters conn);
	bool saveConnection(QgsConnectionParameters conn);
	void setSelected(QString type, QString name);
	void setSelectedType(QString type);

};
#endif //define QGSCONNECTIONREGISTRY_H
