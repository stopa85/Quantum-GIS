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
#include "qgssymbologyutils.h"
#include <QDomDocument>
#include <QDomElement>
#include <QImage>
#include <QPainter>
#include <cmath>

QgsWKNDiagramFactory::QgsWKNDiagramFactory(): mBarWidth(20), mMaximumPenWidth(0),mMaximumGap(0)
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
      width = size + 2 * mMaximumPenWidth + 2 * mMaximumGap;
      height = size + 2 * mMaximumPenWidth + 2 * mMaximumGap;
    }
  else if(mDiagramType == "Bar")
    {
      //witdh
      height = getHeightBarChart(size, f.attributeMap()) + 2 * mMaximumPenWidth;
      width = mBarWidth * mCategories.size() + 2 * mMaximumPenWidth;
      //consider the gaps
      QList<QgsDiagramCategory>::const_iterator c_it = mCategories.constBegin();
      for(; c_it != mCategories.constEnd(); ++c_it)
	{
	  width += (2 * c_it->gap());
	}
    }
  
  return 0;
}

QImage* QgsWKNDiagramFactory::createPieChart(int size, const QgsAttributeMap& dataValues) const
{
  //create transparent QImage
  QImage* diagramImage = new QImage(QSize(size + 2 * mMaximumPenWidth + 2 * mMaximumGap, size + 2 * mMaximumPenWidth + 2 * mMaximumGap), QImage::Format_ARGB32_Premultiplied);
  diagramImage->fill(qRgba(0, 0, 0, 0)); //transparent background
  QPainter p;
  p.begin(diagramImage);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(Qt::NoPen);

  //calculate sum of data values
  double sum = 0;
  QList<double> valueList; //cash the values to use them in drawing later

  QgsAttributeMap::const_iterator value_it;
  QList<QgsDiagramCategory>::const_iterator it = mCategories.constBegin();
  for(; it != mCategories.constEnd(); ++it)
    {
      value_it = dataValues.find(it->propertyIndex());
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

  int totalAngle = 0;
  int currentAngle, currentGap;
  int xGapOffset = 0;
  int yGapOffset = 0;

  QList<QgsDiagramCategory>::const_iterator category_it = mCategories.constBegin();
  QList<double>::const_iterator valueList_it = valueList.constBegin();
  
  for(; category_it != mCategories.constEnd() && valueList_it != valueList.constEnd(); ++category_it, ++valueList_it)
    {
      p.setPen(category_it->pen());
      currentAngle = (int)((*valueList_it)/sum*360*16);
      p.setBrush(category_it->brush());

      xGapOffset = 0;
      yGapOffset = 0;
      currentGap = category_it->gap();
      if(currentGap != 0)
	{
	  //qt angles are degrees*16 
	  gapOffsetsForPieSlice(currentGap, totalAngle + currentAngle/2, xGapOffset, yGapOffset);
	}

      p.drawPie(mMaximumPenWidth + mMaximumGap + xGapOffset, mMaximumPenWidth + mMaximumGap - yGapOffset, size, size, totalAngle, currentAngle);
      totalAngle += currentAngle;
    }
  p.end();
  
  return diagramImage;
}

QImage* QgsWKNDiagramFactory::createBarChart(int size, const QgsAttributeMap& dataValues) const
{
  //for barcharts, the specified height is valid for the classification attribute
  //the heights of the other bars are calculated with the same height/value ratio
  //the bar widths are fixed (20 at the moment)
  //int diagramWidth = barWidth * mAttributes.size();

  int h = getHeightBarChart(size, dataValues) + 2 * mMaximumPenWidth;
  int w = mBarWidth * mCategories.size() + 2 * mMaximumPenWidth;
  //consider the gaps todo: take this information from method getDiagramDimensions()
  QList<QgsDiagramCategory>::const_iterator c_it = mCategories.constBegin();
  for(; c_it != mCategories.constEnd(); ++c_it)
    {
      w += (2 * c_it->gap());
    }
    
  QImage* diagramImage = new QImage(QSize(w, h), QImage::Format_ARGB32_Premultiplied);
  diagramImage->fill(0); //transparent background

  //calculate value/pixel ratio
  double pixelValueRatio = pixelValueRatioBarChart(size, dataValues);

  //draw the bars itself
  double currentValue;
  int currentBarHeight;

  QgsAttributeMap::const_iterator att_it;
  QList<QgsDiagramCategory>::const_iterator category_it = mCategories.constBegin();
  
  int currentWidth = mMaximumPenWidth;
  
  QPainter p(diagramImage);
  p.setRenderHint(QPainter::Antialiasing);

  for(; category_it != mCategories.constEnd(); ++category_it)
    {
      att_it = dataValues.find(category_it->propertyIndex());
      if(att_it != dataValues.constEnd())
	{
	  currentWidth += category_it->gap(); //first gap
	  p.setPen(category_it->pen());
	  currentValue = att_it->toDouble();
	  currentBarHeight = (int)(currentValue * pixelValueRatio);
	  p.setBrush(category_it->brush());
	  p.drawRect(QRect(currentWidth, h - currentBarHeight + mMaximumPenWidth, mBarWidth, currentBarHeight));
	  currentWidth += category_it->gap(); //second gap
	  currentWidth += mBarWidth; //go for the next bar...
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

      QList<QgsDiagramCategory>::const_iterator category_it = mCategories.constBegin();
      QgsAttributeMap::const_iterator it;

      for(; category_it != mCategories.constEnd(); ++category_it)
	{
	  it = featureAttributes.find(category_it->propertyIndex());
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

  //diagram categories
  QList<QgsDiagramCategory>::const_iterator c_it = mCategories.constBegin();
  for(; c_it != mCategories.constEnd(); ++c_it)
    {
      QDomElement currentCategoryElem = doc.createElement("category");
      currentCategoryElem.setAttribute("gap", QString::number(c_it->gap()));
      currentCategoryElem.setAttribute("attribute", QString::number(c_it->propertyIndex())) ;
    
      //brush
      QDomElement currentBrushElem = doc.createElement("brush");
      currentBrushElem.setAttribute("red", QString::number(c_it->brush().color().red()));
      currentBrushElem.setAttribute("green", QString::number(c_it->brush().color().green()));
      currentBrushElem.setAttribute("blue", QString::number(c_it->brush().color().blue()));
      currentBrushElem.setAttribute("style", QgsSymbologyUtils::brushStyle2QString(c_it->brush().style()));

      //pen
      QDomElement currentPenElem = doc.createElement("pen");
      currentPenElem.setAttribute("red", QString::number(c_it->pen().color().red()));
      currentPenElem.setAttribute("green", QString::number(c_it->pen().color().green()));
      currentPenElem.setAttribute("blue", QString::number(c_it->pen().color().blue()));
      currentPenElem.setAttribute("width", QString::number(c_it->pen().width()));
      currentPenElem.setAttribute("style", QgsSymbologyUtils::penStyle2QString(c_it->pen().style()));

      currentCategoryElem.appendChild(currentBrushElem);
      currentCategoryElem.appendChild(currentPenElem);
      
      overlayElement.appendChild(currentCategoryElem);
    }
  return true;
}

void QgsWKNDiagramFactory::addCategory(QgsDiagramCategory c)
{
  mCategories.push_back(c);
  
  //update the maximum pen width if necessary (for proper diagram scaling)
  int currentPenWidth = c.pen().width();
  int currentGap = c.gap();

  if(mMaximumPenWidth < currentPenWidth)
    {
      mMaximumPenWidth = currentPenWidth;
    }
  
  if(currentGap > mMaximumGap)
    {
      mMaximumGap = currentGap;
    }
}

int QgsWKNDiagramFactory::gapOffsetsForPieSlice(int gap, int angle, int& xOffset, int& yOffset) const
{
  double rad = angle/2880.0*M_PI;
  xOffset = (int)(cos(rad) * gap);
  yOffset = (int)(sin(rad) * gap);
  
  return 0;
}
