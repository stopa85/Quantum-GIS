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

QgsWKNDiagramFactory::QgsWKNDiagramFactory(): QgsDiagramFactory(), mMaximumPenWidth(0),mMaximumGap(0)
{
  
}

QgsWKNDiagramFactory::~QgsWKNDiagramFactory()
{
  
}

void QgsWKNDiagramFactory::supportedWellKnownNames(std::list<QString>& names)
{
  names.clear();
  names.push_back("Pie");
  names.push_back("Bar");
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
