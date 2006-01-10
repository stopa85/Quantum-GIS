/***************************************************************************
    qgsmapimage.cpp  -  class for rendering map layer set to pixmap
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

#include "qgsmapimage.h"
#include "qgsscalecalculator.h"
#include "qgsmaptopixel.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerregistry.h"

#include <QPixmap>
#include <QPainter>
#include <Q3PaintDeviceMetrics>



QgsMapImage::QgsMapImage(int width, int height)
{
  mCoordXForm = new QgsMapToPixel;
  mScaleCalculator = new QgsScaleCalculator;
    
  mDrawing = false;
  
  mPixmap = new QPixmap(width, height);
  
  // set the logical dpi
  mScaleCalculator->setDpi(mPixmap->logicalDpiX());

  // set default map units
  mMapUnits = QGis::METERS;
  mScaleCalculator->setMapUnits(mMapUnits);

  // must be initialized at last
  //adjustExtentToPixmap();
}

QgsMapImage::~QgsMapImage()
{
  delete mPixmap;
  delete mCoordXForm;
  delete mScaleCalculator;
}


QgsRect QgsMapImage::extent()
{
  return mExtent;
}


void QgsMapImage::setBgColor(const QColor& color)
{
  mBgColor = color;
}

bool QgsMapImage::setExtent(const QgsRect& extent)
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
    double xMean = (std::abs(extent.xMin()) + std::abs(extent.xMax())) * 0.5;
    double yMean = (std::abs(extent.yMin()) + std::abs(extent.yMax())) * 0.5;

    double xRange = extent.width() / xMean;
    double yRange = extent.height() / yMean;

    static const double minProportion = 1e-12;
    if (xRange < minProportion || yRange < minProportion)
      return false;
  }

  mExtent = extent;
  if (!mExtent.isEmpty())
    adjustExtentToPixmap();
  return true;
}


void QgsMapImage::setPixmapSize(int width, int height)
{
  mPixmap->resize(QSize(width,height));
  
  adjustExtentToPixmap();
}


void QgsMapImage::adjustExtentToPixmap()
{ 
  int myHeight = mPixmap->height();
  int myWidth = mPixmap->width();
  
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
  std::cout << "========== Current Scale ==========" << std::endl;
  std::cout << "Current extent is " << mExtent.stringRep().toLocal8Bit().data() << std::endl;
  std::cout << "MuppX is: " << muppX << "\n" << "MuppY is: " << muppY << std::endl;
  std::cout << "Pixmap width: " << myWidth << ", height: " << myHeight << std::endl;
  std::cout << "Extent width: " << mExtent.width() << ", height: " << mExtent.height() << std::endl;
  std::cout << "whitespace: " << whitespace << std::endl;
#endif

  // update extent
  mExtent.setXmin(dxmin);
  mExtent.setXmax(dxmax);
  mExtent.setYmin(dymin);
  mExtent.setYmax(dymax);
  
  // update the scale
  mScale = mScaleCalculator->calculate(mExtent, myWidth);

#ifdef QGISDEBUG
  std::cout << "Scale (assuming meters as map units) = 1:" << mScale << std::endl;
  std::cout << "------------------------------------------ " << std::endl;
#endif

  mCoordXForm->setParameters(mMupp, dxmin, dymin, myHeight);
}


void QgsMapImage::render()
{
 // TODO: check prerequisities
  // - we have both extent and pixmap, extent is adjusted

#ifdef QGISDEBUG
  std::cout << "========== Rendering ==========" << std::endl;
#endif

  if (mExtent.isEmpty())
  {
    std::cout << "empty extent... not rendering" << endl;
    mPixmap->fill(mBgColor);
    return;
  }


  if (mDrawing)
    return;
  
  mDrawing = true;
  QPainter *paint = new QPainter();

  QPixmap pm(mPixmap->size());
  pm.fill(mBgColor);
  paint->begin(&pm);
  
  // TODO: antialiasing?
  // paint->setRenderHint(QPainter::Antialiasing);

//  int myRenderCounter = 1;
  
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::render: Starting to render layer stack." << std::endl;
#endif
  // render all layers in the stack, starting at the base
  std::deque<QString> layers = mLayers.layerSet();
  std::deque<QString>::iterator li = layers.begin();
  
  while (li != layers.end())
  {
#ifdef QGISDEBUG
    std::cout << "QgsMapImage::render: at layer item '" << (*li).toLocal8Bit().data() << "'." << std::endl;
#endif

    // TODO: emit setProgress(myRenderCounter++,zOrder.size());
    QgsMapLayer *ml = QgsMapLayerRegistry::instance()->mapLayer(*li);

    if (!ml)
    {
#ifdef QGISDEBUG
      std::cout << "QgsMapImage::render: layer not found in registry!" << std::endl;
#endif
      li++;
      continue;
    }
        
#ifdef QGISDEBUG
    std::cout << "QgsMapImage::render: Rendering layer " << ml->name().toLocal8Bit().data() << '\n'
      << "  Layer minscale " << ml->minScale() 
      << ", maxscale " << ml->maxScale() << '.\n' 
      << "  Scale dep. visibility enabled? " 
      << ml->scaleBasedVisibility() << '\n'
      << "  Input extent: " << ml->extent().stringRep().toLocal8Bit().data() 
      << std::endl;
    try
    {
      std::cout << "  Transformed extent: " 
          << ml->coordinateTransform()->transformBoundingBox(ml->extent()).stringRep().toLocal8Bit().data() 
          << std::endl;
    }
    catch (QgsCsException &cse)
    {
      qDebug( "Transform error caught in %s line %d:\n%s", __FILE__, __LINE__, cse.what());
    }
#endif

    if (ml->visible())
    {
      if ((ml->scaleBasedVisibility() && ml->minScale() < mScale && ml->maxScale() > mScale)
          || (!ml->scaleBasedVisibility()))
      {
        QgsRect r1 = mExtent, r2;
        bool split = ml->projectExtent(r1, r2);
        //
                    // Now do the call to the layer that actually does
                    // the rendering work!
        //
        ml->draw(paint, &r1, mCoordXForm, mPixmap);
        if (split)
          ml->draw(paint, &r2, mCoordXForm, mPixmap);
      }
      else
      {
#ifdef QGISDEBUG
        std::cout << "QgsMapImage::render: Layer not rendered because it is not within "
            << "the defined visibility scale range" << std::endl;
#endif
      }
        
    } // if (ml->visible())
    
    li++;
    
  } // while (li != end)
      
#ifdef QGISDEBUG
  std::cout << "QgsMapImage::render: Done rendering map layers...emitting renderComplete(paint)\n";
#endif

  // render all labels for vector layers in the stack, starting at the base
  li = layers.begin();
  while (li != layers.end())
  {
    // TODO: emit setProgress((myRenderCounter++),zOrder.size());
    QgsMapLayer *ml = QgsMapLayerRegistry::instance()->mapLayer(*li);

    if (ml)
    {
      if (ml->visible() && (ml->type() != QgsMapLayer::RASTER))
      {
        // only make labels if the layer is visible
        // after scale dep viewing settings are checked
        if ((ml->scaleBasedVisibility() && ml->minScale() < mScale  && ml->maxScale() > mScale)
            || (!ml->scaleBasedVisibility()))
        {
          QgsRect r1 = mExtent, r2;
          bool split = ml->projectExtent(r1, r2);
      
          ml->drawLabels(paint, &r1, mCoordXForm, mPixmap);
          if (split)
            ml->drawLabels(paint, &r2, mCoordXForm, mPixmap);
        }
      }
      li++;
    }
  }

  //make verys sure progress bar arrives at 100%!
  // TODO: emit setProgress(1,1);
      
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::render: Done rendering map labels...emitting renderComplete(paint)\n";
#endif

  // notify any listeners that rendering is complete
  //note that pmCanvas is not draw to gui yet
  // TODO: emit renderComplete(paint);

  paint->end();
  mDrawing = false;
  delete paint;
  
  *mPixmap = pm;
}

void QgsMapImage::setMapUnits(QGis::units u)
{
  mMapUnits = u;
  mScaleCalculator->setMapUnits(mMapUnits);
  QgsProject::instance()->mapUnits(u); // TODO: sort out
}




////////////////////////////////////////////////////////







void QgsMapLayerSet::updateFullExtent()
{
#ifdef QGISDEBUG
  std::cout << "QgsMapLayerSet::updateFullExtent() called !" << std::endl;
#endif
    
  QgsMapLayerRegistry* registry = QgsMapLayerRegistry::instance();
  bool projectionsEnabled = (QgsProject::instance()->readNumEntry("SpatialRefSys","/ProjectionsEnabled",0)!=0);
  
  // reset the map canvas extent since the extent may now be smaller
  // We can't use a constructor since QgsRect normalizes the rectangle upon construction
  mFullExtent.setMinimal();
  
  // iterate through the map layers and test each layers extent
  // against the current min and max values
  std::deque<QString>::iterator it = mLayerSet.begin();
  while(it != mLayerSet.end())
  {
    QgsMapLayer * lyr = registry->mapLayer(*it);
#ifdef QGISDEBUG 
    std::cout << "Updating extent using " << lyr->name().toLocal8Bit().data() << std::endl;
    std::cout << "Input extent: " << lyr->extent().stringRep().toLocal8Bit().data() << std::endl;
#endif 
    // Layer extents are stored in the coordinate system (CS) of the
    // layer. The extent must be projected to the canvas CS prior to passing
    // on to the updateFullExtent function
    if (projectionsEnabled)
    {
      try
      {
        if ( ! lyr->coordinateTransform() )
          throw QgsCsException( string("NO COORDINATE TRANSFORM FOUND FOR LAYER") );
        
        mFullExtent.unionRect(lyr->coordinateTransform()->transformBoundingBox(lyr->extent()));
      }
      catch (QgsCsException &cse)
      {
        qDebug( "Transform error caught in %s line %d:\n%s", __FILE__, __LINE__, cse.what());
      }
    }
    else
    {
      mFullExtent.unionRect(lyr->extent());
    }
    it++;
  } 
}
