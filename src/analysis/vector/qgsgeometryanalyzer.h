/***************************************************************************
    qgsgeometryanalyzer.h - QGIS Tools for vector geometry analysis
                             -------------------
    begin                : 19 March 2009
    copyright            : (C) Carson Farmer
    email                : carson.farmer@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id: qgis.h 9774 2008-12-12 05:41:24Z timlinux $ */

#ifndef QGSGEOMETRYANALYZERH
#define QGSGEOMETRYANALYZERH

#include "qgsvectorlayer.h"
#include "qgsfield.h"


/** \ingroup analysis
 * The QGis class provides vector geometry analysis functions
 */

class ANALYSIS_EXPORT QgsGeometryAnalyzer
{
  public:

bool singlepartsToMultipart( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding,
                             const int fieldIndex );
                      
bool multipartToSingleparts( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding );
                      
bool extractNodes( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding );
                      
bool polygonsToLines( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding );

bool exportGeometryInformation( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding );
                      
bool simplifyGeometry( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding,
                             const double tolerance );
                      
bool polygonCentroids( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding );
                      
bool layerExtent( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding );
                             
  private:
                      
bool simpleMeasure( QgsGeometry& geometry );

bool perimeterMeasure( QgsGeometry& geometry );

bool checkGeometryFields( QgsGeometry& geometry );

bool extractAsLine( QgsGeometry& geometry );

bool extractAsSingle( QgsGeometry& geometry );

bool extractAsMulti( QgsGeometry& geometry );

bool convertGeometry( QgsGeometry& geometry );

bool extractPoints( QgsGeometry& geometry );

};
#endif //QGSVECTORANALYZER
