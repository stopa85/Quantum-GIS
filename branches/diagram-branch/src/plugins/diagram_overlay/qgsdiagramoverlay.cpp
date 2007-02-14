/***************************************************************************
                         qgsdiagramoverlay.cpp  -  description
                         ---------------------
    begin                : January 2007
    copyright            : (C) 2007 by Marco Hugentobler
    email                : marco dot hugentobler at karto dot baug dot ethz dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsdiagramoverlay.h"
#include "qgscoordinatetransform.h"
#include "qgsdiagramrenderer.h"
#include "qgsfeature.h"
#include "qgsgeometry.h"
#include "qgsmaptopixel.h"
#include "qgsvectordataprovider.h"
#include <QPainter>

QgsDiagramOverlay::QgsDiagramOverlay(QgsVectorLayer* vl): QgsVectorOverlay(vl), mDiagramRenderer(0)
{

}

QgsDiagramOverlay::~QgsDiagramOverlay()
{
  delete mDiagramRenderer;
}

void QgsDiagramOverlay::setDiagramRenderer(QgsDiagramRenderer* r)
{
  delete mDiagramRenderer;
  mDiagramRenderer = r;
}

void QgsDiagramOverlay::createOverlayObjects(const QgsRect& viewExtent)
{
  //memory cleanup
  for(std::multimap<int, QgsOverlayObject*>::iterator it = mOverlayObjects.begin(); it != mOverlayObjects.end(); ++it)
    {
      delete it->second;
    }
  mOverlayObjects.clear();

  //go through all the features and fill the multimap (query mDiagramRenderer for the correct sizes)
  if(mVectorLayer && mDiagramRenderer)
    {
      QgsVectorDataProvider* theProvider = mVectorLayer->getDataProvider();
      if(theProvider)
	{
	  //set spatial filter on data provider
	  theProvider->reset();
	  theProvider->select(viewExtent);

	  QgsFeature currentFeature;
	  int width, height;
	  double value;

	  std::list<unsigned char*> wkbBuffers;
	  std::list<int> wkbSizes;

	  std::list<unsigned char*>::iterator bufferIt;
	  std::list<int>::iterator sizeIt;

	  int multifeaturecounter = 0;

	  while(theProvider->getNextFeature(currentFeature, true, mAttributes))
	    {
	      //todo: insert more objects for multipart features
	      if(mDiagramRenderer->getDiagramSize(width, height, value, currentFeature) != 0)
		{
		  //error
		}

	      if(splitWkb(wkbBuffers, wkbSizes, currentFeature) != 0)
		{
		  continue; //error
		}

	      multifeaturecounter = 0;

	      for(bufferIt = wkbBuffers.begin(), sizeIt = wkbSizes.begin(); bufferIt != wkbBuffers.end(); ++bufferIt, ++sizeIt)
		{
		  mOverlayObjects.insert(std::make_pair(currentFeature.featureId(), new QgsOverlayObject(height, width, 0, false, *bufferIt, *sizeIt, multifeaturecounter)));
		  ++multifeaturecounter;					
		}
	      
	    }
	}
    }
  
}

void QgsDiagramOverlay::drawOverlayObjects(QPainter * p, const QgsRect& viewExtent, QgsMapToPixel * cXf, QgsCoordinateTransform* ct) const
{
  if(mVectorLayer && mDiagramRenderer)
    {
      QgsVectorDataProvider* theProvider = mVectorLayer->getDataProvider();
      if(theProvider)
	{
	  //set spatial filter on data provider
	  theProvider->reset();
	  theProvider->select(viewExtent);

	  QgsFeature currentFeature;
	  QImage* currentDiagramImage = 0;

	  while(theProvider->getNextFeature(currentFeature, false, mAttributes))
	    {
	      //request diagram from renderer
	      currentDiagramImage = mDiagramRenderer->renderDiagram(currentFeature);
	      if(!currentDiagramImage)
		{
		  qWarning("diagram image is 0");
		  continue;
		}

	      //search for overlay object in the multimap, multifeatures still unsolved
	      std::multimap<int, QgsOverlayObject*>::const_iterator lowerIt = mOverlayObjects.lower_bound(currentFeature.featureId());

		if(lowerIt == mOverlayObjects.end())
		{
			return;
		}

	std::multimap<int, QgsOverlayObject*>::const_iterator upperIt = mOverlayObjects.upper_bound(currentFeature.featureId());
	
	std::multimap<int, QgsOverlayObject*>::const_iterator it;


	for(it = lowerIt; it != upperIt; ++it) 
		{
		  //transform coordinates
		  QgsPoint overlayPosition = it->second->position();
		  //first reproject posisiont
		  if(ct)
		    {
		      overlayPosition = ct->transform(overlayPosition);
		    }
		  //then calculate screen point 
		  cXf->transform(&overlayPosition);

		  //place diagram such that the position point is in the middle of the image
		  int shiftX = currentDiagramImage->width()/2;
		  int shiftY = currentDiagramImage->height()/2;
		  p->drawImage((int)overlayPosition.x()-shiftX, (int)overlayPosition.y()-shiftY, *currentDiagramImage);
		}
	      delete currentDiagramImage;
	    }
	}
    }
}

int QgsDiagramOverlay::getOverlayObjectSize(int& width, int& height, double value, const QgsFeature& f) const
{
	return mDiagramRenderer->getDiagramSize(width, height, value, f);
}

