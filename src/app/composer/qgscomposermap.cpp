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
  : /*QWidget(),*/ QgsComposerItem(x, y, width,height,0)
{
    mComposition = composition;
    mId = id;
    mMapCanvas = mComposition->mapCanvas();
    mName = QString(tr("Map %1").arg(mId));
    
    // Cache
    mCacheUpdated = false;

    mCalculate = Scale;
    //mPlotStyle = QgsComposition::Preview;
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

    init();
    readSettings();
    recalculate();

    // Add to scene
    mComposition->canvas()->addItem(this);
    QGraphicsRectItem::show();
}

void QgsComposerMap::init ()
{
    mNumCachedLayers = 0;
    mSelected = false;
    mUserExtent = mMapCanvas->extent();
    mDrawing = false;

    // Cache
    mCacheUpdated = false;

    // Calculate
    //mCalculateComboBox->insertItem( tr("Extent (calculate scale)"), Scale );
    //mCalculateComboBox->insertItem( tr("Scale (calculate extent)"), Extent );
    mCalculate = Scale;

    setPlotStyle ( QgsComposition::Preview );
    
    // Preview style
    mPreviewMode = Cache;
    //mPreviewModeComboBox->insertItem ( tr("Cache"), Cache );
    //mPreviewModeComboBox->insertItem ( tr("Render"), Render );
    //mPreviewModeComboBox->insertItem ( tr("Rectangle"), Rectangle );
    //mPreviewModeComboBox->setCurrentItem ( Cache );

    mWidthScale = 1.0 / mComposition->scale();
    mSymbolScale = 0.5;
    mFontScale = 1.0;

    mFrame = true;

    QGraphicsRectItem::setZValue(20);

    connect ( mMapCanvas, SIGNAL(layersChanged()), this, SLOT(mapCanvasChanged()) );
}

QgsComposerMap::~QgsComposerMap()
{
#ifdef QGISDEBUG
     std::cerr << "QgsComposerMap::~QgsComposerMap" << std::endl;
#endif
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

void QgsComposerMap::setUserExtent ( QgsRect const & rect )
{
    mUserExtent = rect;
    recalculate();
    
    QGraphicsRectItem::update();
    QGraphicsRectItem::scene()->update();
}

void QgsComposerMap::cache ( void )
{
    int w = rect().width();
    int h = rect().height();
      
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

  painter->setClipRect (QRectF( 0, 0, QGraphicsRectItem::rect().width(), QGraphicsRectItem::rect().height() ));

#ifdef QGISDEBUG
  std::cout << "QgsComposerMapt::paint mPlotStyle = " << plotStyle() 
            << " mPreviewMode = " << mPreviewMode << std::endl;
#endif
    
  if ( plotStyle() == QgsComposition::Preview &&  mPreviewMode == Cache ) 
    { // Draw from cache
      if ( !mCacheUpdated || mMapCanvas->layerCount() != mNumCachedLayers ) 
	{
	  cache();
	}
      
      // Scale so that the cache fills the map rectangle
      double scale = 1.0 * QGraphicsRectItem::rect().width() / mCachePixmap.width();
#ifdef QGISDEBUG
      std::cout << "scale = " << scale << std::endl;
#endif
      
      painter->save();
      
      painter->translate(0, 0); //do we need this?
      painter->scale(scale,scale);
      
      painter->drawPixmap(0,0, mCachePixmap);
      
      painter->restore();
    } 
  else if ( (plotStyle() == QgsComposition::Preview && mPreviewMode == Render) || 
            plotStyle() == QgsComposition::Print ||
            plotStyle() == QgsComposition::Postscript ) 
    {
      QgsDebugMsg("render")

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
  
  mDrawing = false;
}

/*
void QgsComposerMap::sizeChanged ( void ) 
{
    int w, h;
    w = mComposition->fromMM ( mWidthLineEdit->text().toDouble() );
    h = mComposition->fromMM ( mHeightLineEdit->text().toDouble() );

    QGraphicsRectItem::setRect(0, 0, w, h);
    recalculate();

    QGraphicsRectItem::update();
    QGraphicsRectItem::scene()->update();
    
    writeSettings();
    }*/

/*
void QgsComposerMap::on_mWidthLineEdit_editingFinished ( void ) { sizeChanged(); }
void QgsComposerMap::on_mHeightLineEdit_editingFinished ( void ) { sizeChanged(); }

void QgsComposerMap::on_mCalculateComboBox_activated( int )
{
    mCalculate = mCalculateComboBox->currentItem();
    
    if ( mCalculate == Scale )
    {
	  recalculate();
      mCacheUpdated = false;
      QGraphicsRectItem::scene()->update();
      mComposition->emitMapChanged ( mId );
    }
    setOptions();
    writeSettings();
    }*/

double QgsComposerMap::scaleFromUserScale ( double us ) 
{
  double s=0;
   
  switch ( mComposition->mapCanvas()->mapUnits() ) {
    case QGis::METERS :
      s = 1000. * mComposition->scale() / us;
      break;
    case QGis::FEET :
      s = 304.8 * mComposition->scale() / us;
      break;
    case QGis::UNKNOWN :
    case QGis::DEGREES :
      s = mComposition->scale() / us;
      break;
  }
  return s;
}

double QgsComposerMap::userScaleFromScale ( double s )
{ 
  double us=0;
    
  switch ( mComposition->mapCanvas()->mapUnits() ) {
    case QGis::METERS :
      us = 1000. * mComposition->scale() / s; 
      break;
    case QGis::FEET :
      us = 304.8 * mComposition->scale() / s; 
      break;
    case QGis::DEGREES :
    case QGis::UNKNOWN :
      us = mComposition->scale() / s;
      break;
  }
    
  return us;
}

/*
void QgsComposerMap::on_mScaleLineEdit_editingFinished()
{
#ifdef QGISDEBUG
    std::cout << "QgsComposerMap::on_mScaleLineEdit_editingFinished" << std::endl;
#endif
    mCalculate = mCalculateComboBox->currentItem();

    mUserScale = mScaleLineEdit->text().toDouble();

    mScale = scaleFromUserScale ( mUserScale );

    recalculate();

    mCacheUpdated = false;
    QGraphicsRectItem::update();
    QGraphicsRectItem::scene()->update();
    
    writeSettings();
    mComposition->emitMapChanged ( mId );
    }*/

 /*
void QgsComposerMap::scaleChanged ( void ) 
{
    mWidthScale = mWidthScaleLineEdit->text().toDouble();
    mSymbolScale = mSymbolScaleLineEdit->text().toDouble();
    mFontScale = mFontScaleLineEdit->text().toDouble();

    mCacheUpdated = false;
    QGraphicsRectItem::update();
    QGraphicsRectItem::scene()->update();
    
    writeSettings();
    mComposition->emitMapChanged ( mId );
    }*/

/*
void QgsComposerMap::on_mFontScaleLineEdit_editingFinished ( void ) { scaleChanged(); }
void QgsComposerMap::on_mSymbolScaleLineEdit_editingFinished ( void ) { scaleChanged(); }
void QgsComposerMap::on_mWidthScaleLineEdit_editingFinished ( void ) { scaleChanged(); }
*/

void QgsComposerMap::mapCanvasChanged ( void ) 
{
#ifdef QGISDEBUG
    std::cout << "QgsComposerMap::canvasChanged" << std::endl;
#endif
    mCacheUpdated = false;
    QGraphicsRectItem::update();
}

/*
void QgsComposerMap::on_mPreviewModeComboBox_activated ( int i )
{
    mPreviewMode = (PreviewMode) i;
    writeSettings();
    }*/

void QgsComposerMap::recalculate ( void ) 
{
#ifdef QGISDEBUG
  std::cout << "QgsComposerMap::recalculate mCalculate = " << mCalculate << std::endl;
#endif
  if ( mCalculate == Scale ) 
  {
    // Calculate scale from extent and rectangle
    double xscale = QGraphicsRectItem::rect().width() / mUserExtent.width();
    double yscale = QGraphicsRectItem::rect().height() / mUserExtent.height();

    mExtent = mUserExtent;

    if ( xscale < yscale )
    {
      mScale = xscale;
      // extend y
      double d = ( 1. * QGraphicsRectItem::rect().height() / mScale - mUserExtent.height() ) / 2 ;
      mExtent.setYmin ( mUserExtent.yMin() - d );
      mExtent.setYmax ( mUserExtent.yMax() + d );
    }
    else
    {
      mScale = yscale;
      // extend x
      double d = ( 1.* QGraphicsRectItem::rect().width() / mScale - mUserExtent.width() ) / 2 ;
      mExtent.setXmin ( mUserExtent.xMin() - d );
      mExtent.setXmax ( mUserExtent.xMax() + d );
    }

    mUserScale = userScaleFromScale ( mScale );
  } 
  else
  {
    // Calculate extent
    double xc = ( mUserExtent.xMax() + mUserExtent.xMin() ) / 2;
    double yc = ( mUserExtent.yMax() + mUserExtent.yMin() ) / 2;
  
    double width = QGraphicsRectItem::rect().width() / mScale;
    double height = QGraphicsRectItem::rect().height() / mScale;
  
    mExtent.setXmin ( xc - width/2  );
    mExtent.setXmax ( xc + width/2  );
    mExtent.setYmin ( yc - height/2  );
    mExtent.setYmax ( yc + height/2  );
  }

#ifdef QGISDEBUG
  std::cout << "mUserExtent = " << mUserExtent.stringRep().toLocal8Bit().data() << std::endl;
  std::cout << "mScale = " << mScale << std::endl;
  std::cout << "mExtent = " << mExtent.stringRep().toLocal8Bit().data() << std::endl;
#endif

  //setOptions();
  mCacheUpdated = false;
}

/*
void QgsComposerMap::on_mFrameCheckBox_clicked ( )
{
    mFrame = mFrameCheckBox->isChecked();
    QGraphicsRectItem::update();
    QGraphicsRectItem::scene()->update();

    writeSettings();
    }*/

 /*
void QgsComposerMap::setOptions ( void )
{
#ifdef QGISDEBUG
  std::cout << "QgsComposerMap::setOptions" << std::endl;
#endif
    
  //mNameLabel->setText ( mName );
    
  //mCalculateComboBox->setCurrentItem( mCalculate );
    
  //mWidthLineEdit->setText ( QString("%1").arg( mComposition->toMM((int)QGraphicsRectItem::rect().width()), 0,'g') );
  //mHeightLineEdit->setText ( QString("%1").arg( mComposition->toMM((int)QGraphicsRectItem::rect().height()),0,'g') );
    
  // Scale
  switch ( mComposition->mapCanvas()->mapUnits() ) {
    case QGis::METERS :
    case QGis::FEET :
      //mScaleLineEdit->setText ( QString("%1").arg((int)mUserScale) );
      break;
    case QGis::DEGREES :
    case QGis::UNKNOWN :
      //mScaleLineEdit->setText ( QString("%1").arg(mUserScale,0,'f') );
      break;
  }
  if ( mCalculate == Scale ) {
    //mScaleLineEdit->setEnabled(false);  
  } else {
    //mScaleLineEdit->setEnabled(true); 
  }
    
  mWidthScaleLineEdit->setText ( QString("%1").arg(mWidthScale,0,'g',2) );
  mSymbolScaleLineEdit->setText ( QString("%1").arg(mSymbolScale,0,'g',2) );
  mFontScaleLineEdit->setText ( QString("%1").arg(mFontScale,0,'g',2) );

  mFrameCheckBox->setChecked ( mFrame );
    
  mPreviewModeComboBox->setCurrentItem( mPreviewMode );
  }*/

/*
void QgsComposerMap::on_mSetCurrentExtentButton_clicked ( void )
{ 
    mUserExtent = mMapCanvas->extent();
    recalculate();
    QGraphicsRectItem::update();
    QGraphicsRectItem::scene()->update();
    setOptions();
    writeSettings();
    mComposition->emitMapChanged ( mId );
    }*/

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

double QgsComposerMap::widthScale (void ) { return mWidthScale ; }
double QgsComposerMap::symbolScale (void ) { return mSymbolScale ; }
double QgsComposerMap::fontScale (void ) { return mFontScale ; }

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
    
  QgsProject::instance()->writeEntry( "Compositions", path+"north", mUserExtent.yMax() );
  QgsProject::instance()->writeEntry( "Compositions", path+"south", mUserExtent.yMin() );
  QgsProject::instance()->writeEntry( "Compositions", path+"east", mUserExtent.xMax() );
  QgsProject::instance()->writeEntry( "Compositions", path+"west", mUserExtent.xMin() );

  QgsProject::instance()->writeEntry( "Compositions", path+"scale", mUserScale );

  QgsProject::instance()->writeEntry( "Compositions", path+"widthscale", mWidthScale );
  QgsProject::instance()->writeEntry( "Compositions", path+"symbolscale", mSymbolScale );
  QgsProject::instance()->writeEntry( "Compositions", path+"fontscale", mFontScale );

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

  mUserExtent.setYmax ( QgsProject::instance()->readDoubleEntry( "Compositions", path+"north", 100, &ok) );
  mUserExtent.setYmin ( QgsProject::instance()->readDoubleEntry( "Compositions", path+"south", 0, &ok) );
  mUserExtent.setXmax ( QgsProject::instance()->readDoubleEntry( "Compositions", path+"east", 100, &ok) );
  mUserExtent.setXmin ( QgsProject::instance()->readDoubleEntry( "Compositions", path+"west", 0, &ok) );

  mUserScale =  QgsProject::instance()->readDoubleEntry( "Compositions", path+"scale", 1000., &ok);
  mScale = scaleFromUserScale ( mUserScale );

  mWidthScale = QgsProject::instance()->readDoubleEntry("Compositions", path+"widthscale", 1., &ok);
  mSymbolScale = QgsProject::instance()->readDoubleEntry("Compositions", path+"symbolscale", 1., &ok);
  mFontScale = QgsProject::instance()->readDoubleEntry("Compositions", path+"fontscale", 1., &ok);
    
  mFrame = QgsProject::instance()->readBoolEntry("Compositions", path+"frame", true, &ok);
    
  mPreviewMode = (PreviewMode) QgsProject::instance()->readNumEntry("Compositions", path+"previewmode", Cache, &ok);
    
  recalculate();

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
