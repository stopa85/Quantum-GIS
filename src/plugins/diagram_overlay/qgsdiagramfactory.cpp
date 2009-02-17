/***************************************************************************
                         qgsdiagramfactory.cpp  -  description
                         ---------------------
    begin                : November 2008
    copyright            : (C) 2008 by Marco Hugentobler
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

#include "qgsdiagramfactory.h"
#include "qgsrendercontext.h"

QgsDiagramFactory::QgsDiagramFactory(): mSizeUnit(MM)
 {

 }

 QgsDiagramFactory::~QgsDiagramFactory()
 {

 }

 QgsDiagramFactory::SizeType QgsDiagramFactory::sizeType() const
 {
     return QgsDiagramFactory::HEIGHT;
 }

 double QgsDiagramFactory::diagramSizeScaleFactor(const QgsRenderContext& context) const
 {
     if(mSizeUnit == MM)
     {
        return context.scaleFactor();
     }
     else if(mSizeUnit == MapUnits)
     {
         return 1 / context.mapToPixel().mapUnitsPerPixel(); //pixel based devices
     }

     return 1.0;
 }

 bool QgsDiagramFactory::writeScalingAttributesToXML(QDomElement& factoryElem, QDomDocument& doc) const
 {
    if(factoryElem.isNull())
     {
        return false;
    }

     QgsAttributeList::const_iterator it = mScalingAttributes.constBegin();
     for(; it != mScalingAttributes.constEnd(); ++it)
     {
        QDomElement scalingAttributeElem = doc.createElement("scalingAttribute");
        QDomText scalingAttributeText = doc.createTextNode(QString::number(*it));
        scalingAttributeElem.appendChild(scalingAttributeText);
        factoryElem.appendChild(scalingAttributeElem);
     }
     return false;
 }

 bool QgsDiagramFactory::readScalingAttributesFromXML(const QDomElement& factoryElem)
 {
     if(factoryElem.isNull())
     {
        return false;
     }

     mScalingAttributes.clear();
     QDomNodeList scalingAttributeList = factoryElem.elementsByTagName("scalingAttribute");
     for(int i = 0; i < scalingAttributeList.size(); ++i)
     {
        QDomElement currentScalingElem = scalingAttributeList.at(i).toElement();
        bool conversionSuccess = false;
        int currentIndex = currentScalingElem.text().toInt(&conversionSuccess);
        if(conversionSuccess)
        {
            mScalingAttributes.push_back(currentIndex);
        }
     }
     return true;
 }
