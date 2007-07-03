/***************************************************************************
    qgsmaptoolvertexedit.h  - tool for adding, moving, deleting vertices
    ---------------------
    begin                : January 2006
    copyright            : (C) 2006 by Martin Dobias
    email                : wonder.sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#ifndef QGSMAPTOOLVERTEXEDIT_H
#define QGSMAPTOOLVERTEXEDIT_H

#include "qgsmapcanvassnapper.h"
#include "qgsmaptool.h"
#include "qgsgeometry.h"
#include "qgsgeometryvertexindex.h"

class QgsRubberBand;
class QgsVertexMarker;

/**Base class for vertex manipulation tools. 
 Inherited by QgsMapToolMoveVertex, QgsMapToolAddVertex, 
QgsMapToolDeleteVertex*/
class QgsMapToolVertexEdit: public QgsMapTool
{
 public:

  QgsMapToolVertexEdit(QgsMapCanvas* canvas);
  
  virtual ~QgsMapToolVertexEdit();

 protected:
  
  QgsMapCanvasSnapper mSnapper;

  QList<QgsSnappingResult> mRecentSnappingResults;

  /**Creates a rubber band with the color/line width from
   the QGIS settings. The caller takes ownership of the 
  returned object*/
  QgsRubberBand* createRubberBand();
};

#endif
