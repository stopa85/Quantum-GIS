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
#include "qgsdiagramfactory.h"
#include "qgsbardiagramfactory.h"
#include "qgspiediagramfactory.h"
#include "qgsdiagramrenderer.h"
#include "qgsfeature.h"
#include "qgsfield.h"
#include "qgsgeometry.h"
#include "qgsmaptopixel.h"
#include "qgsoverlayobject.h"
#include "qgsproject.h"
#include "qgsrendercontext.h"
#include "qgssymbologyutils.h"
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

void QgsDiagramOverlay::createOverlayObjects(const QgsRenderContext& renderContext)
{
    if(!mDisplayFlag)
    {
        return;
    }

  //memory cleanup
  for(QMap<int, QgsOverlayObject*>::iterator it = mOverlayObjects.begin(); it != mOverlayObjects.end(); ++it)
    {
      delete it.value();
    }
  mOverlayObjects.clear();

  //go through all the features and fill the multimap (query mDiagramRenderer for the correct sizes)
  if(mVectorLayer && mDiagramRenderer)
    {
      QgsVectorDataProvider* theProvider = mVectorLayer->dataProvider();
      if(theProvider)
	{
	  //set spatial filter on data provider
      theProvider->select(mAttributes, renderContext.extent());

	  QgsFeature currentFeature;
	  int width, height;

	  std::list<unsigned char*> wkbBuffers;
	  std::list<int> wkbSizes;

	  std::list<unsigned char*>::iterator bufferIt;
	  std::list<int>::iterator sizeIt;

	  int multifeaturecounter = 0;

      while(theProvider->nextFeature(currentFeature))
      {
         //todo: insert more objects for multipart features
       if(mDiagramRenderer->getDiagramDimensions(width, height, currentFeature, renderContext) != 0)
        {
		  //error
        }

       mOverlayObjects.insert(currentFeature.id(), new QgsOverlayObject(height, width, 0, currentFeature.geometryAndOwnership()));
      }
	}
    }
}

void QgsDiagramOverlay::drawOverlayObjects(QgsRenderContext& context) const
{
  if(!mDisplayFlag)
    {
      return;
    }
  if(mVectorLayer && mDiagramRenderer)
    {
      QgsVectorDataProvider* theProvider = mVectorLayer->dataProvider();
      if(theProvider)
	{
	  //set spatial filter on data provider
      theProvider->select(mAttributes, context.extent());

	  QgsFeature currentFeature;
          QImage* currentDiagramImage = 0;

          QPainter* painter = context.painter();

      while(theProvider->nextFeature(currentFeature))
        {
	      //request diagram from renderer
          currentDiagramImage = mDiagramRenderer->renderDiagram(currentFeature, context);
	      if(!currentDiagramImage)
		{
		  qWarning("diagram image is 0");
		  continue;
		}
              //search for overlay object in the map
          QMap<int, QgsOverlayObject*>::const_iterator it = mOverlayObjects.find(currentFeature.id());
          if(it != mOverlayObjects.constEnd())
          {
            if(it.value())
              {
                QgsPoint overlayPosition = it.value()->position();
                const QgsCoordinateTransform* ct = context.coordinateTransform();
                if(ct)
                {
                overlayPosition = ct->transform(overlayPosition);
                }
                context.mapToPixel().transform(&overlayPosition);
                int shiftX = currentDiagramImage->width()/2;
                int shiftY = currentDiagramImage->height()/2;

                if(painter)
                {
                 painter->save();
                 painter->scale( 1.0 / context.rasterScaleFactor(), 1.0 / context.rasterScaleFactor() );
                 painter->drawImage((int)(overlayPosition.x() * context.rasterScaleFactor())-shiftX, (int)(overlayPosition.y() * context.rasterScaleFactor())-shiftY, *currentDiagramImage);
                painter->restore();
                }
            }
          }

          delete currentDiagramImage;
	    }
	}
    }
}

int QgsDiagramOverlay::getOverlayObjectSize(int& width, int& height, double value, const QgsFeature& f, const QgsRenderContext& renderContext) const
{
  return mDiagramRenderer->getDiagramDimensions(width, height, f, renderContext);
}

bool QgsDiagramOverlay::readXML(const QDomNode& overlayNode)
{
  QDomElement overlayElem = overlayNode.toElement();
  
  //set display flag
  if(overlayElem.attribute("display") == "true")
    {
      mDisplayFlag = true;
    }
  else
    {
      mDisplayFlag = false;
    }

  //create a renderer object
  QgsDiagramRenderer* theDiagramRenderer = 0;
  QDomNodeList rendererList = overlayNode.toElement().elementsByTagName("renderer");
  QDomElement rendererElem;

  QString wellKnownName;
  QgsAttributeList attributeList;
  QList<QBrush> brushList;
  QList<QPen> penList;
  QList<int> classAttrList;

  //wellknownname
  QDomNodeList wknNodeList = overlayElem.elementsByTagName("wellknownname");
  if(wknNodeList.size() < 1)
    {
      return false;
    }
  wellKnownName = wknNodeList.at(0).toElement().text(); 
  QgsWKNDiagramFactory* wknFactory = 0;
  if(wellKnownName == "Pie")
    {
      wknFactory = new QgsPieDiagramFactory();
    }
  else if(wellKnownName == "Bar")
    {
      wknFactory = new QgsBarDiagramFactory();
    }
  else
    {
      return false; //unknown type
    }

  //classificationField
  QDomNodeList classificationFieldList = overlayElem.elementsByTagName("classificationfield");
  for(int i = 0; i < classificationFieldList.size(); ++i)
    {
      classAttrList.push_back(classificationFieldList.at(i).toElement().text().toInt());
    }

  theDiagramRenderer = new QgsDiagramRenderer(classAttrList);
  wknFactory->setScalingAttributes(classAttrList);
  wknFactory->setDiagramType(wellKnownName);

  
  int red, green, blue;
  QDomElement categoryElem, penElem, brushElem;
  QDomNodeList categoryList = overlayElem.elementsByTagName("category");

  for(int i = 0; i < categoryList.size(); ++i)
    {
      categoryElem = categoryList.at(i).toElement();
      
      QgsDiagramCategory newCategory;
      newCategory.setPropertyIndex(categoryElem.attribute("attribute").toInt());
      attributeList.push_back(categoryElem.attribute("attribute").toInt());
      newCategory.setGap(categoryElem.attribute("gap").toInt());

      //pen element
      penElem = categoryElem.namedItem("pen").toElement();
      if(!penElem.isNull())
	{
	  QPen currentPen;
	  red = penElem.attribute("red").toInt();
	  green = penElem.attribute("green").toInt();
	  blue = penElem.attribute("blue").toInt();
	  currentPen.setColor(QColor(red, green, blue));
	  currentPen.setStyle(QgsSymbologyUtils::qString2PenStyle(penElem.attribute("style")));
	  newCategory.setPen(currentPen);
	}

      //brush element
      brushElem = categoryElem.namedItem("brush").toElement();
      if(!brushElem.isNull())
	{
	  QBrush currentBrush;
	  red = brushElem.attribute("red").toInt();
	  green = brushElem.attribute("green").toInt();
	  blue = brushElem.attribute("blue").toInt();
	  currentBrush.setColor(QColor(red, green, blue));
	  currentBrush.setStyle(QgsSymbologyUtils::qString2BrushStyle(brushElem.attribute("style")));
	  newCategory.setBrush(currentBrush);
	}

      wknFactory->addCategory(newCategory);
    }

  if(rendererList.size() < 1)
    {
      return false;
    }
  rendererElem = rendererList.at(0).toElement();

  theDiagramRenderer->setFactory(wknFactory);

  //Read renderer specific settings
  if(theDiagramRenderer)
    {
      theDiagramRenderer->readXML(rendererElem);
      setDiagramRenderer(theDiagramRenderer);
      
      //the overlay may need a different attribute list than the renderer
      QList<int>::const_iterator it = classAttrList.constBegin();
      for(; it != classAttrList.constEnd(); ++it)
	{
	  if(!attributeList.contains(*it))
	    {
	      attributeList.push_back(*it);
	    }
	}
      setAttributes(attributeList);
      return true;
    }
  return false;
}

bool QgsDiagramOverlay::writeXML(QDomNode& layer_node, QDomDocument& doc) const
{
  QDomElement overlayElement = doc.createElement("overlay");
  overlayElement.setAttribute("type", "diagram");
  if(mDisplayFlag)
    {
      overlayElement.setAttribute("display", "true");
    }
  else
    {
      overlayElement.setAttribute("display", "false");
    }

  layer_node.appendChild(overlayElement);
  if(mDiagramRenderer)
    {
      mDiagramRenderer->writeXML(overlayElement, doc);
      QgsDiagramFactory* f = mDiagramRenderer->factory();
      if(f)
	{
	  f->writeXML(overlayElement, doc);
	}
    }
  return true;
}

int QgsDiagramOverlay::createLegendContent(std::list<std::pair<QString, QImage*> >& content) const
{
#if 0
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
#endif //0
  return 1; //todo: adapt to new design
}

int QgsDiagramOverlay::indexFromAttributeName(const QString& name, const QgsVectorLayer* vl)
{
  int error = -1;
  
  if(!vl)
    {
      return error;
    }

  const QgsVectorDataProvider *provider;

  if ((provider = dynamic_cast<const QgsVectorDataProvider *>(vl->dataProvider())))
    {
      return provider->fieldNameIndex(name);
    }
  return error;
}

QString QgsDiagramOverlay::attributeNameFromIndex(int index, const QgsVectorLayer* vl)
{
  if(!vl)
    {
      return "";
    }

  const QgsVectorDataProvider *provider;
  if ((provider = dynamic_cast<const QgsVectorDataProvider *>(vl->dataProvider())))
    {
      const QgsFieldMap & fields = provider->fields();
      QgsFieldMap::const_iterator field_iter = fields.find(index);
      if(field_iter != fields.constEnd())
	{
	  return field_iter->name();
	}
    }
  return "";
}
