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
#include "qgsfield.h"
#include "qgsgeometry.h"
#include "qgslinearlyscalingdiagramrenderer.h"
#include "qgsmaptopixel.h"
#include "qgsproject.h"
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
  if(!mDisplayFlag)
    {
      return;
    }
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

bool QgsDiagramOverlay::readXML(const QDomNode& overlayNode)
{
  QDomElement overlayElem = overlayNode.toElement();

  //create a renderer object
  QgsDiagramRenderer* theDiagramRenderer = 0;
  QDomNodeList rendererList = overlayNode.toElement().elementsByTagName("renderer");
  QDomElement rendererElem;

  QString wellKnownName;
  QgsAttributeList attributeList;
  std::list<QColor> colorList;
  int classificationField;

  //classificationField
  QDomNodeList classificationFieldList = overlayElem.elementsByTagName("classificationfield");
  if(classificationFieldList.size() < 1)
    {
      return false;
    }
  classificationField = classificationFieldList.at(0).toElement().text().toInt();

  //attributes
  QDomNodeList attributeNodeList = overlayElem.elementsByTagName("attribute");
  for(int i = 0; i < attributeNodeList.size(); ++i)
    {
      attributeList.push_back(attributeNodeList.at(i).toElement().text().toInt());
    }

  //colors
  QDomNodeList colorNodeList = overlayElem.elementsByTagName("color");
  QDomElement currentColorElem;
  for(int i = 0; i < colorNodeList.size(); ++i)
    {
      currentColorElem = colorNodeList.at(i).toElement();
      //todo: look at attributes red, green, blue
    }

  if(rendererList.size() < 1)
    {
      return false;
    }
  rendererElem = rendererList.at(0).toElement();

  QString type = rendererElem.attribute("type");
  if(type == "linearly_scaling")
    {
      theDiagramRenderer = new QgsLinearlyScalingDiagramRenderer(wellKnownName, attributeList, colorList);
      theDiagramRenderer->setClassificationField(classificationField);
    }
  else
    {
      return false;
    }
  
  //add classification field, colors, attributes to the renderer

  //call renderer->readXML(rendererNode) to read the renderer specific settings
  if(theDiagramRenderer)
    {
      theDiagramRenderer->readXML(rendererElem);
      return true;
    }
  return false;
}

bool QgsDiagramOverlay::writeXML(QDomNode& layer_node, QDomDocument& doc) const
{
  QDomElement overlayElement = doc.createElement("overlay");
  overlayElement.setAttribute("type", "diagram");
  layer_node.appendChild(overlayElement);

  if(mDiagramRenderer)
    {
      //classification field
      QDomElement classificationFieldElem = doc.createElement("classificationfield");
      QDomText classFieldText = doc.createTextNode(QString::number(mDiagramRenderer->classificationField()));
      classificationFieldElem.appendChild(classFieldText);
      overlayElement.appendChild(classificationFieldElem);
      
      //color tags
      std::list<QColor> colorList = mDiagramRenderer->colors();
      for(std::list<QColor>::const_iterator c_it = colorList.begin(); c_it != colorList.end(); ++c_it)
	{
	  QDomElement currentColorElem = doc.createElement("color");
	  currentColorElem.setAttribute("red", QString::number(c_it->red()));
	  currentColorElem.setAttribute("green", QString::number(c_it->green()));
	  currentColorElem.setAttribute("blue", QString::number(c_it->blue()));
	  overlayElement.appendChild(currentColorElem);
	}

      //attribute tags
      QgsAttributeList attributeList = mDiagramRenderer->attributes();
      QgsAttributeList::const_iterator a_it;

      for(a_it = attributeList.constBegin(); a_it != attributeList.constEnd(); ++a_it)
	{
	  QDomElement currentAttributeElem = doc.createElement("attribute");
	  QDomText currentAttributeText = doc.createTextNode(QString::number(*a_it));
	  currentAttributeElem.appendChild(currentAttributeText);
	  overlayElement.appendChild(currentAttributeElem);
	}

      //write settings specific to the particular renderer type
      mDiagramRenderer->writeXML(overlayElement, doc);
    }

  return true;
}

int QgsDiagramOverlay::createLegendContent(std::list<std::pair<QString, QImage*> >& content) const
{
  //first make sure the list is clean
  std::list<std::pair<QString, QImage*> >::iterator it;
  for(it = content.begin(); it != content.end(); ++it)
    {
      delete (it->second);
    }
  content.clear();
  
  if(mDiagramRenderer)
    {
      //first item: name of the classification attribute
      QString classificationName = QgsDiagramOverlay::attributeNameFromIndex(mDiagramRenderer->classificationField(), mVectorLayer);
      content.push_back(std::make_pair(classificationName, (QImage*)0));

      //then a descriptive symbol (must come from diagram renderer)
      QString legendSymbolText;
      QImage* legendSymbolImage = mDiagramRenderer->getLegendImage(legendSymbolText);
      content.push_back(std::make_pair(legendSymbolText, legendSymbolImage));

      //then color/attribute pairs
      std::list<QColor> colorList = mDiagramRenderer->colors();
      std::list<QColor>::const_iterator color_it = colorList.begin();
      QgsAttributeList attributeList = mDiagramRenderer->attributes();
      QgsAttributeList::const_iterator att_it = attributeList.begin();
      QString attributeName;
      QImage* colorImage;
      QPainter p;

      for(; att_it != attributeList.constEnd() && color_it != colorList.end(); ++color_it, ++att_it)
	{
	  colorImage = new QImage(15, 15, QImage::Format_ARGB32_Premultiplied);
	  colorImage->fill(QColor(255,255,255,0).rgba());
	  p.begin(colorImage);
	  p.setPen(Qt::NoPen);
	  p.setBrush(*color_it);
	  p.drawRect(0, 0, 15, 15);
	  p.end();
	  attributeName = QgsDiagramOverlay::attributeNameFromIndex(*att_it, mVectorLayer);
	  content.push_back(std::make_pair(attributeName, colorImage));
	}

      
      
      return 0;
    }
  else
    {
      return 1;
    }
}

int QgsDiagramOverlay::indexFromAttributeName(const QString& name, const QgsVectorLayer* vl)
{
  int notFound = -1;
  
  if(!vl)
    {
      return notFound;
    }

  const QgsVectorDataProvider *provider;

  if ((provider = dynamic_cast<const QgsVectorDataProvider *>(vl->getDataProvider())))
    {
      const QgsFieldMap & fields = provider->fields();
      for (QgsFieldMap::const_iterator it = fields.begin(); it != fields.end(); ++it)
        {
	  if((*it).name() == name)
	    {
	      return it.key();
	    }
        }
    }
  return notFound;
}

QString QgsDiagramOverlay::attributeNameFromIndex(int index, const QgsVectorLayer* vl)
{
  if(!vl)
    {
      return "";
    }

  const QgsVectorDataProvider *provider;
  if ((provider = dynamic_cast<const QgsVectorDataProvider *>(vl->getDataProvider())))
    {
      const QgsFieldMap & fields = provider->fields();
      QgsFieldMap::const_iterator it = fields.find(index);
      if(it != fields.constEnd())
	{
	  return it.value().name();
	}
    }
  return "";
}
