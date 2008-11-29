/***************************************************************************
                         qgsbardiagramfactory.cpp  -  description
                         ------------------------
    begin                : December 2007
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

#include "qgsbardiagramfactory.h"
#include <limits>
#include <QPainter>

QgsBarDiagramFactory::QgsBarDiagramFactory(): QgsWKNDiagramFactory(), mBarWidth(20)
{

}

QgsBarDiagramFactory::~QgsBarDiagramFactory()
{

}

QImage* QgsBarDiagramFactory::createDiagram(int size, const QgsFeature& f, const QgsRenderContext& renderContext) const
{
  QgsAttributeMap dataValues = f.attributeMap();

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

int QgsBarDiagramFactory::getDiagramDimensions(int size, const QgsFeature& f, const QgsRenderContext& context, int& width, int& height) const
{
  height = getHeightBarChart(size, f.attributeMap()) + 2 * mMaximumPenWidth;
  width = mBarWidth * mCategories.size() + 2 * mMaximumPenWidth;
  //consider the gaps
  QList<QgsDiagramCategory>::const_iterator c_it = mCategories.constBegin();
  for(; c_it != mCategories.constEnd(); ++c_it)
    {
      width += (2 * c_it->gap());
    }
  return 0;
}

int QgsBarDiagramFactory::getHeightBarChart(int size, const QgsAttributeMap& featureAttributes) const
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

double QgsBarDiagramFactory::pixelValueRatioBarChart(int size, const QgsAttributeMap& featureAttributes) const
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
