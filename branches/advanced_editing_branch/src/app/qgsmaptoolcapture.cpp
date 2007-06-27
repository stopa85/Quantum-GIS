/***************************************************************************
    qgsmaptoolcapture.cpp  -  map tool for capturing points, lines, polygons
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

#include "qgsapplication.h"
#include "qgsattributedialog.h"
#include "qgscoordinatetransform.h"
#include "qgsfield.h"
#include "qgsmaptoolcapture.h"
#include "qgsmapcanvas.h"
#include "qgsmaprender.h"
#include "qgsmaptopixel.h"
#include "qgsfeature.h"
#include "qgsproject.h"
#include "qgsrubberband.h"
#include "qgsvectorlayer.h"
#include "qgsvectordataprovider.h"
#include "qgscursors.h"
#include <QCursor>
#include <QPixmap>
#include <QMessageBox>


QgsMapToolCapture::QgsMapToolCapture(QgsMapCanvas* canvas, enum CaptureTool tool)
  : QgsMapTool(canvas), mTool(tool), mRubberBand(0)
{
  mCapturing = FALSE;

  QPixmap mySelectQPixmap = QPixmap((const char **) capture_point_cursor);
  mCursor = QCursor(mySelectQPixmap, 8, 8);

  mSnapper.setMapCanvas(canvas);
}

QgsMapToolCapture::~QgsMapToolCapture()
{
  delete mRubberBand;
  mRubberBand = 0;
}

void QgsMapToolCapture::canvasMoveEvent(QMouseEvent * e)
{
  if (mCapturing)
  {
    QgsPoint mapPoint;
    if(mSnapper.snapToBackgroundLayers(e->pos(), mapPoint) == 0)
      {
	mRubberBand->movePoint(mapPoint);
      }
  }
} // mouseMoveEvent


void QgsMapToolCapture::canvasPressEvent(QMouseEvent * e)
{
  // nothing to be done
}


void QgsMapToolCapture::renderComplete()
{
}

void QgsMapToolCapture::deactivate()
{
  delete mRubberBand;
  mRubberBand = 0;
}

int QgsMapToolCapture::addVertex(const QPoint& p)
{
  QgsVectorLayer *vlayer = dynamic_cast <QgsVectorLayer*>(mCanvas->currentLayer());
  
  if (!vlayer)
    {
      return 1;
    }

  if (!mRubberBand)
    {
      mRubberBand = new QgsRubberBand(mCanvas, mTool == CapturePolygon);
      QgsProject* project = QgsProject::instance();
      QColor color(
		   project->readNumEntry("Digitizing", "/LineColorRedPart", 255),
		   project->readNumEntry("Digitizing", "/LineColorGreenPart", 0),
		   project->readNumEntry("Digitizing", "/LineColorBluePart", 0));
      mRubberBand->setColor(color);
      mRubberBand->setWidth(project->readNumEntry("Digitizing", "/LineWidth", 1));
      mRubberBand->show();
    }

  QgsPoint digitisedPoint;
  try
    {
      digitisedPoint = toLayerCoords(vlayer, p);
    }
  catch(QgsCsException &cse)
    {
      return 2;
    }

  QgsPoint mapPoint;
  QgsPoint layerPoint;

  if(mSnapper.snapToBackgroundLayers(p, mapPoint) == 0)
    {
      try
	{
	  layerPoint = toLayerCoords(vlayer, mapPoint); //transform snapped point back to layer crs
	}
      catch(QgsCsException &cse)
	{
	  return 2;
	}
      mRubberBand->addPoint(mapPoint);
      mCaptureList.push_back(layerPoint); 
    }

  return 0;
}
