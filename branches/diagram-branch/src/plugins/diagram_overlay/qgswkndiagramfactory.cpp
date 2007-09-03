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
#include "qgsdiagramitem.h"
#include "qgsfeature.h"
#include <QImage>
#include <QPainter>

QgsWKNDiagramFactory::QgsWKNDiagramFactory()
{

}

QgsWKNDiagramFactory::~QgsWKNDiagramFactory()
{
  
}

QImage* QgsWKNDiagramFactory::createDiagram(int width, int height, const QgsFeature& f) const
{
  //todo: possibility to create a custom diagram
  //return createDiagramKDChart(width, height, f); 

  std::list<double> dataValues;
  QgsAttributeMap featureAttributes = f.attributeMap();
  QgsAttributeList::const_iterator list_iter;
 
  for(list_iter = mAttributes.constBegin(); list_iter != mAttributes.constEnd(); ++list_iter)
    {
      QgsAttributeMap::const_iterator iter = featureAttributes.find(*list_iter);
      if(iter != featureAttributes.constEnd())
	{
	  dataValues.push_back(iter.value().toDouble());
	}
    }
  if(mDiagramType == "Pie")
    {
      return createPieChart(height, dataValues);
    }
  else if(mDiagramType == "Bar")
    {
      return createBarChart(height, dataValues);
    }
  else
    {
      return 0;
    }
}

QImage* QgsWKNDiagramFactory::createPieChart(int height, const std::list<double>& dataValues) const
{
  //create transparent QImage
  QImage* diagramImage = new QImage(QSize(height, height), QImage::Format_ARGB32_Premultiplied);
  diagramImage->fill(qRgba(0, 0, 0, 0)); //transparent background
  QPainter p(diagramImage);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(Qt::NoPen);

  //calculate sum of data values
  double sum = 0;
  for(std::list<double>::const_iterator it = dataValues.begin(); it != dataValues.end(); ++it)
    {
      sum += *it;
    }

  //draw pies
  std::list<double>::const_iterator double_it;
  std::list<QColor>::const_iterator color_it;
  int totalAngle = 0;
  int currentAngle;

  for(double_it = dataValues.begin(), color_it = mColorSeries.begin(); double_it != dataValues.end(); ++double_it, ++color_it)
    {
      currentAngle = (int)((*double_it)/sum*360*16);
      p.setBrush(QBrush(*color_it));
      p.drawPie(0, 0, height, height, totalAngle, currentAngle);
      totalAngle += currentAngle;
    }
  
  return diagramImage;
}

QImage* QgsWKNDiagramFactory::createBarChart(int height, const std::list<double>& dataValues) const
{
  if(height > 2000) //prevent crashes because of to high rectangles 
    {
      return 0;
    }

  int barWidth = 20; //hardcoded width for one bar
  int width = barWidth*dataValues.size();
  QImage* diagramImage = new QImage(QSize(width, height), QImage::Format_ARGB32_Premultiplied);
  diagramImage->fill(0); //transparent background
  QPainter p(diagramImage);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(Qt::NoPen);

  //calculate sum of data values
  double sum = 0;
  for(std::list<double>::const_iterator it = dataValues.begin(); it != dataValues.end(); ++it)
    {
      sum += *it;
    }

  //find max value
  double maxValue = -std::numeric_limits<double>::max();
  for(std::list<double>::const_iterator it = dataValues.begin(); it != dataValues.end(); ++it)
    {
      if (*it > maxValue)
	{
	  maxValue = *it;
	}
    }

  //draw bars
  std::list<double>::const_iterator double_it;
  std::list<QColor>::const_iterator color_it;
  int currentBarHeight;
  int barCounter = 0;

  for(double_it = dataValues.begin(), color_it = mColorSeries.begin(); double_it != dataValues.end(); ++double_it, ++color_it)
    {
      currentBarHeight = (int)((*double_it)/maxValue*height);
      p.setBrush(QBrush(*color_it));
      p.drawRect(QRect(barCounter * barWidth, height - currentBarHeight, barWidth, currentBarHeight));
      ++barCounter;
    }

  return diagramImage;
}

void QgsWKNDiagramFactory::supportedWellKnownNames(std::list<QString>& names)
{
  names.clear();
  names.push_back("Pie");
  names.push_back("Bar");
  names.push_back("Line");
}




