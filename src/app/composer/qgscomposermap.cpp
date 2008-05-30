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

// round isn't defined by default in msvc
#ifdef _MSC_VER
 #define round(x)  ((x) >= 0 ? floor((x)+0.5) : floor((x)-0.5))
#endif

QgsComposerMap::QgsComposerMap ( QgsComposition *composition, int id, int x, int y, int width, int height )
  : QgsComposerItem(x, y, width,height,0)
{
    mComposition = composition;
    mId = id;
    mMapCanvas = mComposition->mapCanvas();
    mName = QString(tr("Map %1").arg(mId));
    
    // Cache
    mCacheUpdated = false;

    mCalculate = Scale;
    setPlotStyle ( QgsComposition::Preview );
    mDrawing = false;

    //calculate mExtent based on width/height ratio and map canvas extent
    mExtent = mMapCanvas->extent();
    setRect(QRectF(x, y, width, height));

    QGraphicsRectItem::setZValue(20);

    connect ( mMapCanvas, SIGNAL(layersChanged()), this, SLOT(mapCanvasChanged()) );

    // Add to scene
    mComposition->canvas()->addItem(this);

    QGraphicsRectItem::show();

	    //writeSettings();
}

QgsComposerMap::QgsComposerMap ( QgsComposition *composition, int id )
    : QgsComposerItem(0,0,10,10,0)
{
    mComposition = composition;
    mId = id;
    mMapCanvas = mComposition->mapCanvas();
    mName = QString(tr("Map %1").arg(mId));

    readSettings();

    // Add to scene
    mComposition->canvas()->addItem(this);
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

  mDrawing = true;

  QRectF thisPaintRect = QRectF( 0, 0, QGraphicsRectItem::rect().width(), QGraphicsRectItem::rect().height());
  painter->save();
  painter->setClipRect (thisPaintRect);

  double currentScaleFactorX = horizontalViewScaleFactor();

  if( plotStyle() == QgsComposition::Preview && mPreviewMode == Render /*&& screen resolution different than last time*/)
    {
      if(currentScaleFactorX != mLastScaleFactorX)
	{
	  mCacheUpdated = false;
	}
    }
    
  if ( plotStyle() == QgsComposition::Preview && mPreviewMode != Rectangle) 
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
  else if ( plotStyle() == QgsComposition::Print ||
            plotStyle() == QgsComposition::Postscript) 
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

bool QgsComposerMap::selected( void )
{
    return mSelected;
}

void QgsComposerMap::setCacheUpdated ( bool u ) 
{
    mCacheUpdated = u;
}    

//double QgsComposerMap::scale ( void ) { return mScale; }
double QgsComposerMap::scale()
{
  QgsScaleCalculator calculator;
  calculator.setMapUnits(mMapCanvas->mapUnits());
  calculator.setDpi(25.4);  //QGraphicsView units are mm
  return calculator.calculate(mExtent, rect().width());
}

QWidget *QgsComposerMap::options ( void )
{
  //setOptions ();
    return ( dynamic_cast <QWidget *> (this) );
}

QString QgsComposerMap::name ( void ) 
{
    return mName;
}

bool QgsComposerMap::writeSettings ( void )  
{
  QString path;
  path.sprintf("/composition_%d/map_%d/", mComposition->id(), mId ); 

  QgsProject::instance()->writeEntry( "Compositions", path+"x", mComposition->toMM((int)QGraphicsRectItem::pos().x()) );
  QgsProject::instance()->writeEntry( "Compositions", path+"y", mComposition->toMM((int)QGraphicsRectItem::pos().y()) );


  QgsProject::instance()->writeEntry( "Compositions", path+"width", mComposition->toMM((int)QGraphicsRectItem::rect().width()) );
  QgsProject::instance()->writeEntry( "Compositions", path+"height", mComposition->toMM((int)QGraphicsRectItem::rect().height()) );

  if ( mCalculate == Scale ) {
      QgsProject::instance()->writeEntry( "Compositions", path+"calculate", QString("scale") );
  } else {
      QgsProject::instance()->writeEntry( "Compositions", path+"calculate", QString("extent") );
  }

  QgsProject::instance()->writeEntry( "Compositions", path+"frame", mFrame );

  QgsProject::instance()->writeEntry( "Compositions", path+"previewmode", mPreviewMode );

  return true; 
}

bool QgsComposerMap::readSettings ( void )
{
  bool ok;
  QString path;
  path.sprintf("/composition_%d/map_%d/", mComposition->id(), mId );
    
  double x =  mComposition->fromMM(QgsProject::instance()->readDoubleEntry( "Compositions", path+"x", 0, &ok));
  double y = mComposition->fromMM(QgsProject::instance()->readDoubleEntry( "Compositions", path+"y", 0, &ok));
  int w = mComposition->fromMM(QgsProject::instance()->readDoubleEntry( "Compositions", path+"width", 100, &ok)) ;
  int h = mComposition->fromMM(QgsProject::instance()->readDoubleEntry( "Compositions", path+"height", 100, &ok)) ;
  QGraphicsRectItem::setRect(0, 0, w, h);
  QGraphicsRectItem::setPos(x, y);

  QString calculate = QgsProject::instance()->readEntry("Compositions", path+"calculate", "scale", &ok);
  if ( calculate == "extent" )
  {
    mCalculate = Extent;
  }else
  {
    mCalculate = Scale;
  }
    
  mFrame = QgsProject::instance()->readBoolEntry("Compositions", path+"frame", true, &ok);
    
  mPreviewMode = (PreviewMode) QgsProject::instance()->readNumEntry("Compositions", path+"previewmode", Cache, &ok);

  return true;
}

bool QgsComposerMap::removeSettings ( void )
{
    QString path;
    path.sprintf("/composition_%d/map_%d", mComposition->id(), mId );
    return QgsProject::instance()->removeEntry ( "Compositions", path );
}

bool QgsComposerMap::writeXML( QDomNode & node, QDomDocument & document, bool temp )
{
    return true;
}

bool QgsComposerMap::readXML( QDomNode & node )
{
    return true;
}

void QgsComposerMap::resize(double dx, double dy)
{
  //setRect
  QRectF currentRect = rect();
  QRectF newRect = QRectF(currentRect.x(), currentRect.y(), currentRect.width() + dx, currentRect.height() + dy);
  setRect(newRect); 
}

void QgsComposerMap::setRect(const QRectF& rectangle)
{
  double w = rectangle.width();
  double h = rectangle.height();
  prepareGeometryChange();

  //debug
  qWarning("QgsComposerMap::setRect");
  QgsRect debugRect(rectangle.left(), rectangle.top(), rectangle.right(), rectangle.bottom());
  qWarning(debugRect.stringRep().latin1());

  QgsComposerItem::setRect(rectangle);
  
  QGraphicsRectItem::update();
  double newHeight = mExtent.width() * h / w ;
  mExtent = QgsRect(mExtent.xMin(), mExtent.yMin(), mExtent.xMax(), mExtent.yMin() + newHeight);
  emit extentChanged();
}

void QgsComposerMap::setNewExtent(const QgsRect& extent)
{
  //soon...
}

void QgsComposerMap::setNewScale(double scaleDenominator)
{
  //soon...
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
