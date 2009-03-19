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

#ifndef QGSGEOMETRYANALYZER_H
#define QGSGEOMETRYANALYZER_H
/** \ingroup analysis
 * The QGis class provides vector geometry analysis functions
 */

class ANALYSIS_EXPORT QgsAnalysisExport
{
  public:

bool single_to_multi(  );
bool multi_to_single(  );
bool extract_nodes(  );
bool polygons_to_lines(  );
bool export_geometry_info(  );
bool simplify_geometry(  );
bool polygon_centroids(  );
bool delaunay_triangulation(  );
bool layer_extent(  );
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
bool convertGeometry(  geom_list, vType );

}
