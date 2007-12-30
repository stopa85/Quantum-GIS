/***************************************************************************
    qgsmaptooledit.cpp  -  base class for editing map tools
    ---------------------
    begin                : Juli 2007
    copyright            : (C) 2007 by Marco Hugentobler
    email                : marco dot hugentobler at karto dot baug dot ethz dot ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#include "qgsmaptooledit.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"
#include "qgsrubberband.h"
#include "qgsvectorlayer.h"
#include <QKeyEvent>
#include <QSettings>

QgsMapToolEdit::QgsMapToolEdit(QgsMapCanvas* canvas): QgsMapTool(canvas)
{
  mSnapper.setMapCanvas(canvas);
}


QgsMapToolEdit::~QgsMapToolEdit()
{

}

int QgsMapToolEdit::insertSegmentVerticesForSnap(const QList<QgsSnappingResult>& snapResults, QgsVectorLayer* editedLayer)
{
  int returnval = 0;
  QgsPoint layerPoint;

  if(!editedLayer || !editedLayer->isEditable())
    {
      return 1;
    }

  //can only add segment vertices for other features if topological editing is enabled
  //topological editing on?
  int topologicalEditing = QgsProject::instance()->readNumEntry("Digitizing", "/TopologicalEditing", 0);
  if(topologicalEditing == 0)
    {
      return 2;
    }

  QList<QgsSnappingResult>::const_iterator it = snapResults.constBegin();
  for(; it != snapResults.constEnd(); ++it)
    {
      //snap to edited layer?
      if(it->layer == editedLayer)
	{
	  if(it->snappedVertexNr == -1) //segment snap
	    {
	      layerPoint = toLayerCoords(editedLayer, it->snappedVertex);
	      if(!editedLayer->insertVertexBefore(layerPoint.x(), layerPoint.y(), it->snappedAtGeometry, it->afterVertexNr))
		{
		  returnval = 3;
		}
	    }
	}
    }

  return returnval;
}

QgsPoint QgsMapToolEdit::snapPointFromResults(const QList<QgsSnappingResult>& snapResults, const QPoint& screenCoords)
{
  if(snapResults.size() < 1)
    {
      return toMapCoords(screenCoords);
    }
  else
    {
      return snapResults.constBegin()->snappedVertex;
    }
}

QgsRubberBand* QgsMapToolEdit::createRubberBand(bool isPolygon)
{
  QSettings settings;
  QgsRubberBand* rb = new QgsRubberBand(mCanvas, isPolygon);
  QColor color( settings.value("/qgis/digitizing/line_color_red", 255).toInt(),
		settings.value("/qgis/digitizing/line_color_green", 0).toInt(),
		settings.value("/qgis/digitizing/line_color_blue", 0).toInt());
  rb->setColor(color);
  rb->setWidth(settings.value("/qgis/digitizing/line_width", 1).toInt());
  rb->show();
  return rb;
}

QgsVectorLayer* QgsMapToolEdit::currentVectorLayer()
{
  QgsMapLayer* currentLayer = mCanvas->currentLayer();
  if(!currentLayer)
    {
      return 0;
    }

  QgsVectorLayer* vlayer = dynamic_cast<QgsVectorLayer*>(currentLayer);
  if(!vlayer)
    {
      return 0;
    }
  return vlayer;
}


int QgsMapToolEdit::addTopologicalPoints(const QList<QgsPoint>& geom)
{
  if(!mCanvas)
    {
      return 1;
    }

  //find out current vector layer
  QgsVectorLayer *vlayer = currentVectorLayer();
  
  if (!vlayer)
    {
      return 2;
    }

  QList<QgsPoint>::const_iterator list_it = geom.constBegin();
  for(; list_it != geom.constEnd(); ++list_it)
    {
      addTopologicalPoints(*list_it, vlayer);
    }
  
  return 0;
}

int QgsMapToolEdit::addTopologicalPoints(const QgsPoint& p, QgsVectorLayer* vl)
{
  if(!vl)
    {
      return 1;
    }

  QMultiMap<double, QgsSnappingResult> snapResults; //results from the snapper object
  //we also need to snap to vertex to make sure the vertex does not already exist in this geometry
  QMultiMap<double, QgsSnappingResult> vertexSnapResults;

  QList<QgsSnappingResult> filteredSnapResults; //we filter out the results that are on existing vertices

  const double threshold = 0.00000001;

  if(vl->snapWithContext(p, threshold, snapResults, QgsSnapper::SNAP_TO_SEGMENT) != 0)
    {
      return 2;
    }
 
  QMultiMap<double, QgsSnappingResult>::const_iterator snap_it = snapResults.constBegin();
  QMultiMap<double, QgsSnappingResult>::const_iterator vertex_snap_it;

  for(; snap_it != snapResults.constEnd(); ++snap_it)
    { 
      //test if p is already a vertex of this geometry. If yes, don't insert it
      bool vertexAlreadyExists = false;
      if(vl->snapWithContext(p, threshold, vertexSnapResults, QgsSnapper::SNAP_TO_VERTEX) != 0)
	{
	  continue;
	}

      vertex_snap_it = vertexSnapResults.constBegin();
      for(; vertex_snap_it != vertexSnapResults.constEnd(); ++vertex_snap_it)
	{
	  if(snap_it.value().snappedAtGeometry == vertex_snap_it.value().snappedAtGeometry)
	    {
	      vertexAlreadyExists = true; 
	    }
	}
      
      if(!vertexAlreadyExists)
	{
	  filteredSnapResults.push_back(*snap_it);
	}
    }
  insertSegmentVerticesForSnap(filteredSnapResults, vl);
  return 0;
}
