/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef QGSPROJECTIONSELECTOR_H
#define QGSPROJECTIONSELECTOR_H

#include "qgsprojectionselectorbase.h"
#include <qstring.h>


/**
@author Tim Sutton
*/
class QgsProjectionSelector: public QgsProjectionSelectorBase
{
Q_OBJECT
public:
    QgsProjectionSelector( QWidget* parent , const char* name ,WFlags fl =0  );
    ~QgsProjectionSelector();
    
public slots:
    void setSelectedWKT(QString theWKT);
    void setSelectedSRID(QString theSRID);
    QString getCurrentWKT();
    QString getCurrentSRID();
    
private:
    
signals:
    void wktSelected(QString theWKT);
    void sridSelected(QString theSRID);
};

#endif
