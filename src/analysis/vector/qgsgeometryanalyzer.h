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
/** \ingroup analysis
 * The QGis class provides vector geometry analysis functions
 */

class ANALYSISEXPORT QgsAnalysisExport
{
  public:

    bool singleToMulti(  );
    bool multiToSingle(  );
    bool extractNodes(  );
    bool polygonsToLines(  );
    bool exportGeometryInfo(  );
    bool simplifyGeometry(  );
    bool polygonCentroids(  );
    bool delaunayTriangulation(  );
    bool layerExtent(  );
    bool extractAsSimple(  geom, tolerance );
    bool simplifyLine(  ln, typ, tol );
    bool recursiveDouglasPeucker(  line, tol, j, k );
    bool shortestDistance(  tline, point);
    bool simpleMeasure(  inGeom );
    bool perimMeasure(  inGeom, measure );
    bool checkForField(  L, e );
    bool checkGeometryFields(  vlayer );
    bool extractAsLine(  geom );
    bool extractAsSingle(  geom );
    bool extractAsMulti(  geom );
    bool convertGeometry(  geomlist, vType );

};
#endif //QGSVECTORANALYZER
