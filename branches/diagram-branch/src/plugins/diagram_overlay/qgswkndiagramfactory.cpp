/***************************************************************************
                         qgswkndiagramfactory.cpp  -  description
                         ------------------------
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



#include "qgswkndiagramfactory.h"
#include "qgsfeature.h"
#include <QDomDocument>
#include <QDomElement>
#include <QImage>
#include <QPainter>

QgsWKNDiagramFactory::QgsWKNDiagramFactory(): mBarWidth(20)
{

}

QgsWKNDiagramFactory::~QgsWKNDiagramFactory()
{
  
}

QImage* QgsWKNDiagramFactory::createDiagram(int size, const QgsFeature& f) const
{
  QgsAttributeMap featureAttributes = f.attributeMap();
  
  if(mDiagramType == "Pie")
    {
      return createPieChart(size, featureAttributes);
    }
  else if(mDiagramType == "Bar")
    {
      return createBarChart(size, featureAttributes);
    }
  else
    {
      return 0;
    }
}

int QgsWKNDiagramFactory::getDiagramDimensions(int size, const QgsFeature& f, int& width, int& height) const
{
  if(mDiagramType == "Pie") //for pie charts, the size is the pie diameter
    {
      width = size;
      height = size;
    }
  else if(mDiagramType == "Bar")
    {
      //witdh
      width = mBarWidth * mAttributes.size(); 
      height = getHeightBarChart(size, f.attributeMap());
    }
  
  return 0;
}

QImage* QgsWKNDiagramFactory::createPieChart(int size, const QgsAttributeMap& dataValues) const
{
  //create transparent QImage
  QImage* diagramImage = new QImage(QSize(size, size), QImage::Format_ARGB32_Premultiplied);
  diagramImage->fill(qRgba(0, 0, 0, 0)); //transparent background
  QPainter p;
  p.begin(diagramImage);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(Qt::NoPen);

  //calculate sum of data values
  double sum = 0;
  QList<double> valueList; //cash the values to use them in drawing later

  QgsAttributeMap::const_iterator value_it;
  QgsAttributeList::const_iterator it = mAttributes.constBegin();
  for(; it != mAttributes.constEnd(); ++it)
    {
      value_it = dataValues.find(*it);
      valueList.push_back(value_it->toDouble());
      if(value_it != dataValues.constEnd())
	{
	  sum += value_it->toDouble();
	}
    }

  if(sum - 0.0 < 0.000000000000001)
    {
      p.end();
      delete diagramImage;
      return 0;
    }

  //draw pies
  std::list<QColor>::const_iterator color_it = mColorSeries.begin();
  QList<double>::const_iterator valueList_it = valueList.constBegin();

  int totalAngle = 0;
  int currentAngle;

  for(; (color_it != mColorSeries.end() && valueList_it != valueList.constEnd()); ++color_it, ++valueList_it)
    {
      currentAngle = (int)((*valueList_it)/sum*360*16);
      p.setBrush(QBrush(*color_it));
      p.drawPie(0, 0, size, size, totalAngle, currentAngle);
      totalAngle += currentAngle;
    }
  p.end();
  
  return diagramImage;
}

QImage* QgsWKNDiagramFactory::createBarChart(int size, const QgsAttributeMap& dataValues) const
{
  //for barcharts, the specified height is valid for the classification attribute
  //the heights of the other bars are calculated with the same height/value ratio
  //the bar widths are fixed
  //int barWidth = 20;
  //int diagramWidth = barWidth * mAttributes.size();

  int w = mBarWidth * mAttributes.size();
  int h = getHeightBarChart(size, dataValues);
    
  QImage* diagramImage = new QImage(QSize(w, h), QImage::Format_ARGB32_Premultiplied);
  diagramImage->fill(0); //transparent background

  //calculate value/pixel ratio
  double pixelValueRatio = pixelValueRatioBarChart(size, dataValues);

  //draw the bars itself
  double currentValue;
  int currentBarHeight;

  QgsAttributeList::const_iterator it = mAttributes.constBegin();
  std::list<QColor>::const_iterator color_it = mColorSeries.begin();
  QgsAttributeMap::const_iterator att_it;
  int barCounter = 0;
  
  QPainter p(diagramImage);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(Qt::NoPen);

  for(; it != mAttributes.constEnd() && color_it != mColorSeries.end(); ++it, ++color_it)
    {
      att_it = dataValues.find(*it);
      if(att_it != dataValues.constEnd())
	{
	  currentValue = att_it->toDouble();
	  currentBarHeight = (int)(currentValue * pixelValueRatio);
	  p.setBrush(QBrush(*color_it));
	  p.drawRect(QRect(barCounter * mBarWidth, h - currentBarHeight, mBarWidth, currentBarHeight));
	  ++barCounter;
	}
    }

  return diagramImage;
}

void QgsWKNDiagramFactory::supportedWellKnownNames(std::list<QString>& names)
{
  names.clear();
  names.push_back("Pie");
  names.push_back("Bar");
}

int QgsWKNDiagramFactory::getHeightBarChart(int size, const QgsAttributeMap& featureAttributes) const
{     
      //calculate value/pixel ratio
      double pixelValueRatio = pixelValueRatioBarChart(size, featureAttributes); 

      //find maximum attribute value
      double maximumAttValue = -std::numeric_limits<double>::max();
      double currentValue;

      QgsAttributeList::const_iterator att_it = mAttributes.constBegin();
      QgsAttributeMap::const_iterator it;

      for(; att_it != mAttributes.constEnd(); ++att_it)
	{
	  it = featureAttributes.find(*att_it);
	  if(it != featureAttributes.constEnd())
	    {
	      currentValue = it->toDouble();
	      if(currentValue > maximumAttValue)
		{
		  maximumAttValue = currentValue;
		}
	    }
	}
      
      //and calculate height of image based on the maximum attribute value
      int height = (int)(maximumAttValue * pixelValueRatio);
      return height;
}


double QgsWKNDiagramFactory::pixelValueRatioBarChart(int size, const QgsAttributeMap& featureAttributes) const
{
  //find value for scaling attribute
  QList<int>::const_iterator scaling_it = mScalingAttributes.constBegin();
  double scalingValue = 0;

  for(; scaling_it != mScalingAttributes.constEnd(); ++scaling_it)
    {
      QgsAttributeMap::const_iterator it = featureAttributes.find(*scaling_it);
      if(it == featureAttributes.constEnd())
	{
	  continue; //error, scaling attribute not contained in feature attributes
	}
      scalingValue += (it->toDouble());
    }
  
  //calculate value/pixel ratio
  return (size / scalingValue);
}

QgsDiagramFactory::SizeType QgsWKNDiagramFactory::sizeType() const
{
  if(mDiagramType == "Pie")
    {
      return QgsDiagramFactory::DIAMETER;
    }
  else
    {
      return QgsDiagramFactory::HEIGHT;
    }
}

bool QgsWKNDiagramFactory::writeXML(QDomNode& overlay_node, QDomDocument& doc) const
{
  QDomElement overlayElement = overlay_node.toElement();

  //well known name
  QDomElement wellKnownNameElem = doc.createElement("wellknownname");
  QDomText wknText = doc.createTextNode(mDiagramType);
  wellKnownNameElem.appendChild(wknText);
  overlayElement.appendChild(wellKnownNameElem);
	    
  //classification fields
  QList<int>::const_iterator scaling_it = mScalingAttributes.constBegin();
  for(; scaling_it != mScalingAttributes.constEnd(); ++scaling_it)
    {
      QDomElement classificationFieldElem = doc.createElement("classificationfield");
      QDomText classFieldText = doc.createTextNode(QString::number(*scaling_it));
      classificationFieldElem.appendChild(classFieldText);
      overlayElement.appendChild(classificationFieldElem);
    }
	    
  //color tags
  for(std::list<QColor>::const_iterator c_it = mColorSeries.begin(); c_it != mColorSeries.end(); ++c_it)
    {
      QDomElement currentColorElem = doc.createElement("color");
      currentColorElem.setAttribute("red", QString::number(c_it->red()));
      currentColorElem.setAttribute("green", QString::number(c_it->green()));
      currentColorElem.setAttribute("blue", QString::number(c_it->blue()));
      overlayElement.appendChild(currentColorElem);
    }
  
  //attribute tags
  QgsAttributeList::const_iterator a_it;
  
  for(a_it = mAttributes.constBegin(); a_it != mAttributes.constEnd(); ++a_it)
    {
      QDomElement currentAttributeElem = doc.createElement("attribute");
      QDomText currentAttributeText = doc.createTextNode(QString::number(*a_it));
      currentAttributeElem.appendChild(currentAttributeText);
      overlayElement.appendChild(currentAttributeElem);
    }
  return true;
}
