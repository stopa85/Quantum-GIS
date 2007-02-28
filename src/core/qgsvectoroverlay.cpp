/***************************************************************************
                         qgsvectoroverlay.h  -  description
                         ------------------
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

#include "qgsvectoroverlay.h"
#include "qgsgeometry.h"

QgsVectorOverlay::QgsVectorOverlay(QgsVectorLayer* vl): mVectorLayer(vl), mDisplayFlag(true)
{
  
}

QgsVectorOverlay::~QgsVectorOverlay()
{
  for(std::multimap<int, QgsOverlayObject*>::iterator it = mOverlayObjects.begin(); it != mOverlayObjects.end(); ++it)
    {
      delete it->second;
    }
}

int QgsVectorOverlay::splitWkb(std::list<unsigned char*>& wkbGeometries, std::list<int> wkbSizes, QgsFeature& f) const
{
  wkbGeometries.clear();
  wkbSizes.clear();

  QGis::WKBTYPE wkbType;
  memcpy(&wkbType, &((f.geometry()->wkbBuffer())[1]), sizeof(int));
  //= mVectorLayer->geometryType();
  switch(wkbType)
    {
      //todo: handle these cases
    case QGis::WKBPoint25D:
    case QGis::WKBLineString25D:
    case QGis::WKBPolygon25D:
    case QGis::WKBMultiPoint25D:
    case QGis::WKBMultiLineString25D:
    case QGis::WKBMultiPolygon25D:
    case QGis::WKBUnknown:
      {
	return 1;
      }

    case QGis::WKBPoint:
    case QGis::WKBLineString:
    case QGis::WKBPolygon:
      {
	//do a simple copy
	int size = f.geometry()->wkbSize();
	unsigned char* buffer = new unsigned char[size];
	memcpy(buffer, f.geometry()->wkbBuffer(), size);
	wkbGeometries.push_back(buffer);
	wkbSizes.push_back(size);
	break;
      }
    case QGis::WKBMultiPoint:
      {
	//todo
      }
    case QGis::WKBMultiLineString:
      {
	return splitWkbMultiLine(wkbGeometries, wkbSizes, f);
      }
    case QGis::WKBMultiPolygon:
      {
	return splitWkbMultiPolygon(wkbGeometries, wkbSizes, f);
      }
    }
  return 0;
}

int QgsVectorOverlay::splitWkbMultiPolygon(std::list<unsigned char*>& wkbGeometries, std::list<int> wkbSizes, QgsFeature& f) const
{
  int currentPosition = 1 + sizeof(int);
  int numberOfPolygons;
  int numberOfRings;
  int numberOfPoints;
  int startSection; //start byte of single feature
  int endSection; //end byte of single feature
  int sectionSize;//end byte - start byte

  unsigned char* featureWKB = f.geometry()->wkbBuffer();

  memcpy(&numberOfPolygons, &(featureWKB[currentPosition]), sizeof(int));
  currentPosition += sizeof(int);

  for(int i = 0; i < 1 /*numberOfPolygons*/; ++i)
    {
      startSection = currentPosition;
      currentPosition += (1 + sizeof(int));
      memcpy(&numberOfRings, &(featureWKB[currentPosition]), sizeof(int));
      currentPosition += sizeof(int);
      for(int j = 0; j < numberOfRings; ++j)
	{
	  memcpy(&numberOfPoints, &(featureWKB[currentPosition]), sizeof(int));
	  currentPosition += sizeof(int);
	  for(int k = 0; k < numberOfPoints; ++k)
	    {
	      currentPosition += 2* sizeof(double);
	    }
	}
      endSection = currentPosition;
      sectionSize = endSection - startSection + 1;
      unsigned char* newFeature = new unsigned char[sectionSize];
      memcpy(newFeature, &(featureWKB[startSection]), sectionSize);
      wkbGeometries.push_back(newFeature);
      wkbSizes.push_back(sectionSize);
    }
  return 0;
}

int QgsVectorOverlay::splitWkbMultiLine(std::list<unsigned char*>& wkbGeometries, std::list<int> wkbSizes, QgsFeature& f) const
{
	int currentPosition = 1 + sizeof(int);
  	int numberOfLines;
  	int numberOfPoints;
  	int startSection; //start byte of single feature
  	int endSection; //end byte of single feature
  	int sectionSize;//end byte - start byte

  	unsigned char* featureWKB = f.geometry()->wkbBuffer();

  	memcpy(&numberOfLines, &(featureWKB[currentPosition]), sizeof(int));
  	currentPosition += sizeof(int);
	
	for(int i = 0; i < 1 /*numberOfLines*/; ++i)
	{
		currentPosition += (1 + sizeof(int)); //skip endian and type
		startSection = currentPosition;
		memcpy(&numberOfPoints, &(featureWKB[currentPosition]), sizeof(int));
		currentPosition += sizeof(int);
		for(int j = 0; j < numberOfPoints; ++j)
		{
			currentPosition += 2*sizeof(double);
		}
		endSection = currentPosition;
		sectionSize = endSection - startSection + 1;
      		unsigned char* newFeature = new unsigned char[sectionSize];
      		memcpy(newFeature, &(featureWKB[startSection]), sectionSize);
      		wkbGeometries.push_back(newFeature);
      		wkbSizes.push_back(sectionSize);
	}
	return 0;
}

int QgsVectorOverlay::splitWkbMultiPoint(std::list<unsigned char*>& wkbGeometries, std::list<int> wkbSizes, QgsFeature& f) const
{
	return 1; //soon...
}
