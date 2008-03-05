/***************************************************************************
    qgsgeometrycolumndescription.h - Description of Spatial Database Column
     --------------------------------------
    Date                 : 26-02-2008
    Copyright            : (C) 2007 by Godofredo Contreras
    Email                : frdc at hotmail dot com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

/* $Id$ */


#ifndef QGSGEOMETRYCOLUMNDESCRIPTION_H
#define QGSGEOMETRYCOLUMNDESCRIPTION_H

#include "qstringlist.h"
#include "qgsconnectionparameters.h"

typedef std::pair<QString, QString> GeometryPair;
typedef std::list<GeometryPair > GeometryColumns;

/**
 * Interface for classes that will connect to spatial databases
 */
class QgsGeometryColumnDescription
{
  public:
	QgsGeometryColumnDescription();
	QgsGeometryColumnDescription(QString type, QString schema, QString table, QString column);
	QgsGeometryColumnDescription(QgsGeometryColumnDescription *geometrycolumn);
	~QgsGeometryColumnDescription();
	void setType(QString type);
	void setSchema(QString schema);
	void setTable(QString table);
    void setColumn(QString column);
    QString type();
    QString schema();
    QString table();
    QString column();
  private:
	QString mType;
	QString mSchema;
	QString mTable;
	QString mColumn;

};
#endif 
