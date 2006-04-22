/***************************************************************************
    qgsmaprender.cpp  -  class for rendering map layer set
    ----------------------
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

#include <cmath>
#include <cfloat>

#include "qgscoordinatetransform.h"
#include "qgslogger.h"
#include "qgsmaprender.h"
#include "qgsscalecalculator.h"
#include "qgsmaptopixel.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerregistry.h"

#include <QPixmap>
#include <QPainter>
#include <Q3PaintDeviceMetrics>
#include <QTime>


QgsMapRender::QgsMapRender()
{
  mCoordXForm = new QgsMapToPixel;
  mScaleCalculator = new QgsScaleCalculator;
    
  mDrawing = false;
  mOverview = false;
  
  // set default map units - we use WGS 84 thus use degrees
  setMapUnits(QGis::DEGREES);

  mSize = QSize(0,0);
  
  mProjectionsEnabled = FALSE;
  mDestSRS = new QgsSpatialRefSys(GEOSRS_ID, QgsSpatialRefSys::QGIS_SRSID); // WGS 84

}

QgsMapRender::~QgsMapRender()
{
  delete mCoordXForm;
  delete mScaleCalculator;
  delete mDestSRS;
}


QgsRect QgsMapRender::extent()
{
  return mExtent;
}

void QgsMapRender::updateScale()
{
  mScale = mScaleCalculator->calculate(mExtent, mSize.width());
}

bool QgsMapRender::setExtent(const QgsRect& extent)
{

  // Don't allow zooms where the current extent is so small that it
  // can't be accurately represented using a double (which is what
  // currentExtent uses). Excluding 0 avoids a divide by zero and an
  // infinite loop when rendering to a new canvas. Excluding extents
  // greater than 1 avoids doing unnecessary calculations.

  // The scheme is to compare the width against the mean x coordinate
  // (and height against mean y coordinate) and only allow zooms where
  // the ratio indicates that there is more than about 12 significant
  // figures (there are about 16 significant figures in a double).

  if (extent.width()  > 0 && 
      extent.height() > 0 && 
      extent.width()  < 1 &&
      extent.height() < 1)
  {
    // Use abs() on the extent to avoid the case where the extent is
    // symmetrical about 0.
    double xMean = (fabs(extent.xMin()) + fabs(extent.xMax())) * 0.5;
    double yMean = (fabs(extent.yMin()) + fabs(extent.yMax())) * 0.5;

    double xRange = extent.width() / xMean;
    double yRange = extent.height() / yMean;

    static const double minProportion = 1e-12;
    if (xRange < minProportion || yRange < minProportion)
      return false;
  }

  mExtent = extent;
  if (!mExtent.isEmpty())
    adjustExtentToSize();
  return true;
}



void QgsMapRender::setOutputSize(QSize size, int dpi)
{
  mSize = size;
  mScaleCalculator->setDpi(dpi);
  adjustExtentToSize();
}

void QgsMapRender::adjustExtentToSize()
{
  int myHeight = mSize.height();
  int myWidth = mSize.width();
  
  if (!myWidth || !myHeight)
  {
    mScale = 1;
    mCoordXForm->setParameters(0, 0, 0, 0);
    return;
  }

  // calculate the translation and scaling parameters
  // mupp = map units per pixel
  double muppY = mExtent.height() / myHeight;
  double muppX = mExtent.width() / myWidth;
  mMupp = muppY > muppX ? muppY : muppX;

  // calculate the actual extent of the mapCanvas
  double dxmin, dxmax, dymin, dymax, whitespace;

  if (muppY > muppX)
  {
    dymin = mExtent.yMin();
    dymax = mExtent.yMax();
    whitespace = ((myWidth * mMupp) - mExtent.width()) / 2;
    dxmin = mExtent.xMin() - whitespace;
    dxmax = mExtent.xMax() + whitespace;
  }
  else
  {
    dxmin = mExtent.xMin();
    dxmax = mExtent.xMax();
    whitespace = ((myHeight * mMupp) - mExtent.height()) / 2;
    dymin = mExtent.yMin() - whitespace;
    dymax = mExtent.yMax() + whitespace;
  }

#ifdef QGISDEBUG
  QgsDebugMsg("========== Current Scale ==========");
  QgsDebugMsg("Current extent is " + mExtent.stringRep());
  QgsLogger::debug("MuppX", muppX, 1, __FILE__, __FUNCTION__, __LINE__);
  QgsLogger::debug("MuppY", muppY, 1, __FILE__, __FUNCTION__, __LINE__);
  QgsLogger::debug("Pixmap width", myWidth, 1, __FILE__, __FUNCTION__, __LINE__);
  QgsLogger::debug("Pixmap height", myHeight, 1, __FILE__, __FUNCTION__, __LINE__);
  QgsLogger::debug("Extent width", mExtent.width(), 1, __FILE__, __FUNCTION__, __LINE__);
  QgsLogger::debug("Extent height", mExtent.height(), 1, __FILE__, __FUNCTION__, __LINE__);
  QgsLogger::debug("whitespace: ", whitespace, 1, __FILE__, __FUNCTION__, __LINE__);
#endif


  // update extent
  mExtent.setXmin(dxmin);
  mExtent.setXmax(dxmax);
  mExtent.setYmin(dymin);
  mExtent.setYmax(dymax);
  
  // update the scale
  updateScale();

#ifdef QGISDEBUG
  QgsLogger::debug("Scale (assuming meters as map units) = 1", mScale, 1, __FILE__, __FUNCTION__, __LINE__);
#endif

  mCoordXForm->setParameters(mMupp, dxmin, dymin, myHeight);
}


void QgsMapRender::render(QPainter* painter)
{
  QgsDebugMsg("========== Rendering ==========");

  if (mExtent.isEmpty())
  {
    QgsLogger::warning("empty extent... not rendering");
    return;
  }

  if (mDrawing)
    return;
  
  mDrawing = true;
  
  int myRenderCounter = 0;
  
  QgsCoordinateTransform* ct;

#ifdef QGISDEBUG
  QgsDebugMsg("QgsMapRender::render: Starting to render layer stack.");
  QTime renderTime;
  renderTime.start();
#endif
  // render all layers in the stack, starting at the base
  std::deque<QString>::iterator li = mLayerSet.begin();
  
  while (li != mLayerSet.end())
  {
    QgsDebugMsg("QgsMapRender::render: at layer item '" + (*li));

    // This call is supposed to cause the progress bar to
    // advance. However, it seems that updating the progress bar is
    // incompatible with having a QPainter active (the one that is
    // passed into this function), as Qt produces a number of errors
    // when try to do so. I'm (Gavin) not sure how to fix this, but
    // added these comments and debug statement to help others...
    QgsDebugMsg("If there is a QPaintEngine error here, it is caused by an emit call");

    emit setProgress(myRenderCounter++, mLayerSet.size());
    QgsMapLayer *ml = QgsMapLayerRegistry::instance()->mapLayer(*li);

    if (!ml)
    {
      QgsLogger::warning("QgsMapRender::render: layer not found in registry!");
      li++;
      continue;
    }
        
#ifdef QGISDEBUG
		QgsDebugMsg("QgsMapRender::render: Rendering layer " + ml->name());
		QgsLogger::debug("  Layer minscale ", ml->minScale(), 1, __FILE__, __FUNCTION__, __LINE__);
		QgsLogger::debug("  Layer maxscale ", ml->maxScale(), 1, __FILE__, __FUNCTION__, __LINE__);
		QgsLogger::debug("  Scale dep. visibility enabled? ", ml->scaleBasedVisibility(), 1,\
				 __FILE__, __FUNCTION__, __LINE__);
		QgsLogger::debug("  Input extent: " + ml->extent().stringRep(), 1, __FILE__, __FUNCTION__, __LINE__);
#endif

    if ((ml->scaleBasedVisibility() && ml->minScale() < mScale && ml->maxScale() > mScale)
        || (!ml->scaleBasedVisibility()))
    {
      connect(ml, SIGNAL(drawingProgress(int,int)), this, SLOT(onDrawingProgress(int,int)));        
      
      QgsRect r1 = mExtent, r2;
      bool split = splitLayersExtent(ml, r1, r2);
      //
                  // Now do the call to the layer that actually does
                  // the rendering work!
      //
      
      if (projectionsEnabled())
      {
        ct = new QgsCoordinateTransform(ml->srs(), *mDestSRS);
      }
      else
      {
        ct = NULL;
      }
      
      if (!ml->draw(painter, &r1, mCoordXForm, ct))
        emit drawError(ml);
      
      if (split)
      {
        if (!ml->draw(painter, &r2, mCoordXForm, ct))
          emit drawError(ml);
      }
      
      delete ct;
      
      disconnect(ml, SIGNAL(drawingProgress(int,int)), this, SLOT(onDrawingProgress(int,int)));
    }
    else
    {
      QgsDebugMsg("QgsMapRender::render: Layer not rendered because it is not within the defined "
                  "visibility scale range");
    }

    li++;
    
  } // while (li != end)
      
    QgsDebugMsg("QgsMapRender::render: Done rendering map layers");

  if (!mOverview)
  {
    // render all labels for vector layers in the stack, starting at the base
    li = mLayerSet.begin();
    while (li != mLayerSet.end())
    {
      // TODO: emit setProgress((myRenderCounter++),zOrder.size());
      QgsMapLayer *ml = QgsMapLayerRegistry::instance()->mapLayer(*li);
  
      if (ml && (ml->type() != QgsMapLayer::RASTER))
      {
        // only make labels if the layer is visible
        // after scale dep viewing settings are checked
        if ((ml->scaleBasedVisibility() && ml->minScale() < mScale  && ml->maxScale() > mScale)
            || (!ml->scaleBasedVisibility()))
        {
          QgsRect r1 = mExtent, r2;
          bool split = splitLayersExtent(ml, r1, r2);
      
          if (projectionsEnabled())
          {
            ct = new QgsCoordinateTransform(ml->srs(), *mDestSRS);
          }
          else
          {
            ct = NULL;
          }
      
          ml->drawLabels(painter, &r1, mCoordXForm, ct);
          if (split)
            ml->drawLabels(painter, &r2, mCoordXForm, ct);
          
          delete ct;
        }
      }
      li++;
    }
  } // if (!mOverview)

  // make sure progress bar arrives at 100%!
  emit setProgress(1,1);      
      
#ifdef QGISDEBUG
  QgsLogger::debug("QgsMapRender::render: Rendering done in (seconds)", renderTime.elapsed() / 1000.0, 1,\
		   __FILE__, __FUNCTION__, __LINE__);
#endif

  mDrawing = false;

}

void QgsMapRender::setMapUnits(QGis::units u)
{
  mScaleCalculator->setMapUnits(u);

  // Since the map units have changed, force a recalculation of the scale.
  updateScale();

  emit mapUnitsChanged();
}

QGis::units QgsMapRender::mapUnits() const
{
  return mScaleCalculator->mapUnits();
}

void QgsMapRender::onDrawingProgress(int current, int total)
{
  // TODO: emit signal with progress
  //std::cout << "onDrawingProgress: " << current << " / " << total << std::endl;
  emit updateMap();
}



void QgsMapRender::setProjectionsEnabled(bool enabled)
{
  mProjectionsEnabled = enabled;
  emit projectionsEnabled(enabled);
}

bool QgsMapRender::projectionsEnabled()
{
  return mProjectionsEnabled;
}

void QgsMapRender::setDestinationSrs(const QgsSpatialRefSys& srs)
{
  *mDestSRS = srs;
  emit destinationSrsChanged();
}

const QgsSpatialRefSys& QgsMapRender::destinationSrs()
{
  return *mDestSRS;
}


bool QgsMapRender::splitLayersExtent(QgsMapLayer* layer, QgsRect& extent, QgsRect& r2)
{
  bool split = false;

  if (projectionsEnabled())
  {
    try
    {
      QgsCoordinateTransform tr(layer->srs(), *mDestSRS);
      
#ifdef QGISDEBUG
      QgsLogger::debug<QgsRect>("Getting extent of canvas in layers CS. Canvas is ", extent, __FILE__,\
        __FUNCTION__, __LINE__);
#endif
      // Split the extent into two if the source SRS is
      // geographic and the extent crosses the split in
      // geographic coordinates (usually +/- 180 degrees,
      // and is assumed to be so here), and draw each
      // extent separately.
      static const double splitCoord = 180.0;

      if (tr.sourceSRS().geographicFlag())
      {
        // Note: ll = lower left point
        //   and ur = upper right point
        QgsPoint ll = tr.transform(extent.xMin(), extent.yMin(),
                                   QgsCoordinateTransform::INVERSE);

        QgsPoint ur = tr.transform(extent.xMax(), extent.yMax(), 
                                   QgsCoordinateTransform::INVERSE);

        if (ll.x() > ur.x())
        {
          extent.set(ll, QgsPoint(splitCoord, ur.y()));
          r2.set(QgsPoint(-splitCoord, ll.y()), ur);
          split = true;
        }
        else // no need to split
        {
          extent = tr.transformBoundingBox(extent, QgsCoordinateTransform::INVERSE);
        }
      }
      else // can't cross 180
      {
        extent = tr.transformBoundingBox(extent, QgsCoordinateTransform::INVERSE);
      }
    }
    catch (QgsCsException &cse)
    {
      QgsLogger::warning("Transform error caught in " + QString(__FILE__) + ", line " + QString::number(__LINE__));
      extent = QgsRect(-DBL_MAX, -DBL_MAX, DBL_MAX, DBL_MAX);
      r2     = QgsRect(-DBL_MAX, -DBL_MAX, DBL_MAX, DBL_MAX);
    }
  }
  return split;
}


QgsRect QgsMapRender::layerExtentToOutputExtent(QgsMapLayer* theLayer, QgsRect extent)
{
  if (projectionsEnabled())
  {
    try
    {
      QgsCoordinateTransform tr(theLayer->srs(), *mDestSRS);
      extent = tr.transformBoundingBox(extent);
    }
    catch (QgsCsException &cse)
    {
      qDebug( "Transform error caught in %s line %d:\n%s", __FILE__, __LINE__, cse.what());
    }
  }
  else
  {
    // leave extent unchanged
  }
  
  return extent;
}

QgsPoint QgsMapRender::outputCoordsToLayerCoords(QgsMapLayer* theLayer, QgsPoint point)
{
  if (projectionsEnabled())
  {
    try
    {
      QgsCoordinateTransform tr(theLayer->srs(), *mDestSRS);
      point = tr.transform(point, QgsCoordinateTransform::INVERSE);
    }
    catch (QgsCsException &cse)
    {
      qDebug( "Transform error caught in %s line %d:\n%s", __FILE__, __LINE__, cse.what());
    }
  }
  else
  {
    // leave point without transformation
  }
  return point;
}

QgsRect QgsMapRender::outputCoordsToLayerCoords(QgsMapLayer* theLayer, QgsRect rect)
{
  if (projectionsEnabled())
  {
    try
    {
      QgsCoordinateTransform tr(theLayer->srs(), *mDestSRS);
      rect = tr.transform(rect, QgsCoordinateTransform::INVERSE);
    }
    catch (QgsCsException &cse)
    {
      qDebug( "Transform error caught in %s line %d:\n%s", __FILE__, __LINE__, cse.what());
    }
  }
  return rect;
}


void QgsMapRender::updateFullExtent()
{
  QgsDebugMsg("QgsMapRender::updateFullExtent() called !");
  QgsMapLayerRegistry* registry = QgsMapLayerRegistry::instance();
  
  // reset the map canvas extent since the extent may now be smaller
  // We can't use a constructor since QgsRect normalizes the rectangle upon construction
  mFullExtent.setMinimal();
  
  // iterate through the map layers and test each layers extent
  // against the current min and max values
  std::deque<QString>::iterator it = mLayerSet.begin();
  while(it != mLayerSet.end())
  {
    QgsMapLayer * lyr = registry->mapLayer(*it);
    if (lyr == NULL)
    {
      QgsLogger::warning("WARNING: layer '" + (*it) + "' not found in map layer registry!");
    }
    else
    {
      QgsDebugMsg("Updating extent using " + lyr->name());
      QgsDebugMsg("Input extent: " + lyr->extent().stringRep());
      
      // Layer extents are stored in the coordinate system (CS) of the
      // layer. The extent must be projected to the canvas CS
      QgsRect extent = layerExtentToOutputExtent(lyr, lyr->extent());
      
      QgsDebugMsg("Output extent: " + extent.stringRep());
      mFullExtent.unionRect(extent);

    }
    it++;
  }
  QgsDebugMsg("Full extent: " + mFullExtent.stringRep());
}

void QgsMapRender::setLayerSet(const std::deque<QString>& layers)
{
  mLayerSet = layers;
  updateFullExtent();
}
