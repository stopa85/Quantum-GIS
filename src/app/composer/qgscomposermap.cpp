/***************************************************************************
                         qgscomposermap.cpp
                             -------------------
    begin                : January 2005
    copyright            : (C) 2005 by Radim Blazek
    email                : blazek@itc.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgscomposermap.h"

#include "qgscoordinatetransform.h"
#include "qgslogger.h"
#include "qgsmapcanvas.h"
#include "qgsmaplayer.h"
#include "qgsmaptopixel.h"
#include "qgsproject.h"
#include "qgsmaprender.h"
#include "qgsrendercontext.h"
#include "qgsscalecalculator.h"
#include "qgsvectorlayer.h"

#include "qgslabel.h"
#include "qgslabelattributes.h"

#include <QGraphicsScene>
#include <QPainter>
#include <iostream>
#include <cmath>

QgsComposerMap::QgsComposerMap ( QgsComposition *composition, int x, int y, int width, int height )
  : QgsComposerItem(x, y, width, height, composition)
{
    mComposition = composition;
    mMapCanvas = mComposition->mapCanvas();
    mName = "Map"; //QString(tr("Map %1").arg(mId)); //todo: make static member as counter
    
    // Cache
    mCacheUpdated = false;
    mDrawing = false;

    //calculate mExtent based on width/height ratio and map canvas extent
    mExtent = mMapCanvas->extent();
    setSceneRect(QRectF(x, y, width, height));

    QGraphicsRectItem::setZValue(20);

    connect ( mMapCanvas, SIGNAL(layersChanged()), this, SLOT(mapCanvasChanged()) );

    // Add to scene

    QGraphicsRectItem::show();
}

QgsComposerMap::QgsComposerMap ( QgsComposition *composition)
    : QgsComposerItem(0, 0, 10, 10, composition)
{
    mComposition = composition;
    mMapCanvas = mComposition->mapCanvas();
    mName = "Map"; //QString(tr("Map %1").arg(mId)); //todo: make static member as counter
    QGraphicsRectItem::show();
}

QgsComposerMap::~QgsComposerMap()
{
}

/* This function is called by paint() and cache() to render the map.  It does not override any functions
from QGraphicsItem. */
void QgsComposerMap::draw ( QPainter *painter, const QgsRect& extent, const QSize& size, int dpi)
{
  mMapCanvas->freeze(true);  // necessary ?

  if(!painter)
    {
      return;
    }

  QgsMapRender* canvasMapRender = mMapCanvas->mapRender();
  if(!canvasMapRender)
    {
      return;
    }

  QgsMapRender theMapRender;
  theMapRender.setExtent(extent);
  theMapRender.setOutputSize(size, dpi);
  theMapRender.setLayerSet(canvasMapRender->layerSet());
  theMapRender.setProjectionsEnabled(canvasMapRender->projectionsEnabled());
  theMapRender.setDestinationSrs(canvasMapRender->destinationSrs());
  
  QgsRenderContext* theRenderContext = theMapRender.renderContext();
  if(theRenderContext)
    {
      theRenderContext->setDrawEditingInformation(false);
      theRenderContext->setRenderingStopped(false);
    }

  theMapRender.render(painter);
    
  mMapCanvas->freeze(false);
}

void QgsComposerMap::cache ( void )
{
  int w = rect().width() * horizontalViewScaleFactor();
  int h = rect().height() * horizontalViewScaleFactor();

  if(w > 3000) //limit size of image for better performance
    {
      w = 3000;
    }

  if(h > 3000)
    {
      h = 3000;
    }
      
  mCachePixmap = QPixmap( w, h );
  double mupp = mExtent.width() / w;

  // WARNING: ymax in QgsMapToPixel is device height!!!
  QgsMapToPixel transform(mupp, h, mExtent.yMin(), mExtent.xMin() );
    
  mCachePixmap.fill(QColor(255,255,255));
  
  QPainter p(&mCachePixmap);
  
  draw( &p, mExtent, QSize(w, h), mCachePixmap.logicalDpiX());
  p.end();
  
  mNumCachedLayers = mMapCanvas->layerCount();
  mCacheUpdated = true;
}

void QgsComposerMap::paint ( QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget)
{
  if ( mDrawing ) 
    {
      return;
    }

  if(!mComposition)
    {
      return;
    }

  mDrawing = true;

  QRectF thisPaintRect = QRectF( 0, 0, QGraphicsRectItem::rect().width(), QGraphicsRectItem::rect().height());
  painter->save();
  painter->setClipRect (thisPaintRect);

  double currentScaleFactorX = horizontalViewScaleFactor();

  if( mComposition->plotStyle() == QgsComposition::Preview && mPreviewMode == Render /*&& screen resolution different than last time*/)
    {
      if(currentScaleFactorX != mLastScaleFactorX)
	{
	  mCacheUpdated = false;
	}
    }
    
  if ( mComposition->plotStyle() == QgsComposition::Preview && mPreviewMode != Rectangle) 
    { // Draw from cache
      if ( !mCacheUpdated || mMapCanvas->layerCount() != mNumCachedLayers ) 
	{
	  cache();
	}
      
      // Scale so that the cache fills the map rectangle
      double scale = 1.0 * QGraphicsRectItem::rect().width() / mCachePixmap.width();
      
      painter->save();
      
      painter->translate(0, 0); //do we need this?
      painter->scale(scale,scale);
      
      painter->drawPixmap(0,0, mCachePixmap);
      
      painter->restore();
    } 
  else if ( mComposition->plotStyle() == QgsComposition::Print ||
            mComposition->plotStyle() == QgsComposition::Postscript) 
    {
      QPaintDevice* thePaintDevice = painter->device();
      if(!thePaintDevice)
	{
	  return;
	}
    
      QRectF bRect = boundingRect();
      QSize theSize(bRect.width(), bRect.height());
      draw( painter, mExtent, theSize, 25.4); //scene coordinates seem to be in mm
    } 

  drawFrame(painter);
  if(isSelected())
    {
      drawSelectionBoxes(painter);
    }
  
  painter->restore();

  mLastScaleFactorX =  currentScaleFactorX;
  mDrawing = false;
}

void QgsComposerMap::mapCanvasChanged ( void ) 
{
    mCacheUpdated = false;
    QGraphicsRectItem::update();
}

void QgsComposerMap::setCacheUpdated ( bool u ) 
{
    mCacheUpdated = u;
}    

double QgsComposerMap::scale()
{
  QgsScaleCalculator calculator;
  calculator.setMapUnits(mMapCanvas->mapUnits());
  calculator.setDpi(25.4);  //QGraphicsView units are mm
  return calculator.calculate(mExtent, rect().width());
}

QString QgsComposerMap::name ( void ) 
{
    return mName;
}

void QgsComposerMap::resize(double dx, double dy)
{
  //setRect
  QRectF currentRect = rect();
  QRectF newSceneRect = QRectF(transform().dx(), transform().dy(), currentRect.width() + dx, currentRect.height() + dy);
  setSceneRect(newSceneRect);
}

void QgsComposerMap::setSceneRect(const QRectF& rectangle)
{
  double w = rectangle.width();
  double h = rectangle.height();
  prepareGeometryChange();

  QgsComposerItem::setSceneRect(rectangle);
  
  QGraphicsRectItem::update();
  double newHeight = mExtent.width() * h / w ;
  mExtent = QgsRect(mExtent.xMin(), mExtent.yMin(), mExtent.xMax(), mExtent.yMin() + newHeight);
  mCacheUpdated = false;
  emit extentChanged();
}

void QgsComposerMap::setNewExtent(const QgsRect& extent)
{
  if(mExtent == extent)
    {
      return;
    }
  mExtent = extent;

  //adjust height
  QRectF currentRect = rect();
  
  double newHeight = currentRect.width() * extent.height() / extent.width();

  setSceneRect(QRectF(transform().dx(), transform().dy(), currentRect.width(), newHeight));
}

void QgsComposerMap::setNewScale(double scaleDenominator)
{
  double currentScaleDenominator = scale();

  if(scaleDenominator == currentScaleDenominator)
    {
      return;
    }

  double scaleRatio = scaleDenominator / currentScaleDenominator;

  double newXMax = mExtent.xMin() + scaleRatio * (mExtent.xMax() - mExtent.xMin());
  double newYMax = mExtent.yMin() + scaleRatio * (mExtent.yMax() - mExtent.yMin());

  QgsRect newExtent(mExtent.xMin(), mExtent.yMin(), newXMax, newYMax);
  mExtent = newExtent;
  mCacheUpdated = false;
  emit extentChanged();
  update();
}

double QgsComposerMap::horizontalViewScaleFactor() const
{
  double result = 1;
  if(scene())
    {
      QList<QGraphicsView*> viewList = scene()->views();
      if(viewList.size() > 0)
	{
	  result = viewList.at(0)->transform().m11();
	}
    }
  return result;
}
