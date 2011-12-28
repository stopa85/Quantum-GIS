/***************************************************************************
 *   Copyright (C) 2011 by Sergey Yakushev                                 *
 *   yakushevs <at> gmail.com                                              *
 *                                                                         *
 *   This is file define Road graph plugins settings                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ROADGRAPH_RASTER_SETTINGS
#define ROADGRAPH_RASTER_SETTINGS

#include "settings.h"

//  QT includes
#include <qstring.h>

// Qgis includes
#include <qgscoordinatereferencesystem.h>
#include <qgspoint.h>

// standard includes

// forward declaration Qgis-classes
class QWidget;
class QgsGraph;

/**
@author Sergey Yakushev
*/
/**
 * \class RgRasterSettings
 * \brief This class contained settings for QgsRasterLayerDirector
 */

class RgRasterLayerSettings: public RgSettings
{
  public:
    /**
     * default constructor.
     */
    RgRasterLayerSettings();

    /**
     * destructor
     */
    ~RgRasterLayerSettings();
  public:
    /*
     * MANDATORY SETTINGS PROPERTY DECLARATIONS
     */
    void write( QgsProject * );
    void read( const QgsProject * );
    bool test();
    QWidget *getGui( QWidget* Parent );
    void    setFromGui( QWidget* );
    QString name();
   public:
    
    QgsGraph* graph( const QgsCoordinateReferenceSystem& crs, bool crsTransformEnabled, const QVector< QgsPoint >& additionalPoint, QVector< QgsPoint >& tiedPoint );
    
    /**
     * contained Layer name
     */
    QString mLayer;

    /**
     * A speed band number
     */
    int mSpeedBand;

    /**
     * A unit of speed name
     */
    QString mSpeedUnitName;
};
#endif
