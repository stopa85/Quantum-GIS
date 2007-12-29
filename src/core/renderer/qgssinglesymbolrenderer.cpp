/***************************************************************************
                         qgssinglesymbolrenderer.cpp  -  description
                             -------------------
    begin                : Oct 2003
    copyright            : (C) 2003 by Marco Hugentobler
    email                : mhugent@geo.unizh.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id: qgssinglesymbolrenderer.cpp 5371 2006-04-25 01:52:13Z wonder $ */

#include "qgis.h"
#include "qgssinglesymbolrenderer.h"

#include "qgsfeature.h"
#include "qgslogger.h"
#include "qgssymbol.h"
#include "qgssymbologyutils.h"
#include "qgsvectorlayer.h"

#include <QDomNode>
#include <QImage>
#include <QPainter>
#include <QString>
#include <math.h>

QgsSingleSymbolRenderer::QgsSingleSymbolRenderer(QGis::VectorType type)
{
    mVectorType=type;
  
    //initial setting based on random color
    QgsSymbol* sy = new QgsSymbol(mVectorType);
  
    //random fill colors for points and polygons and pen colors for lines
    int red = 1 + (int) (255.0 * rand() / (RAND_MAX + 1.0));
    int green = 1 + (int) (255.0 * rand() / (RAND_MAX + 1.0));
    int blue = 1 + (int) (255.0 * rand() / (RAND_MAX + 1.0));

    if (type == QGis::Line)
    {
	sy->setColor(QColor(red, green, blue));
    } 
    else
    {
	sy->setFillColor(QColor(red, green, blue));
	sy->setFillStyle(Qt::SolidPattern);
	sy->setColor(QColor(0, 0, 0));
    }
    sy->setLineWidth(1);
    mSymbol=sy;

    mAngleClassificationField = -1; // Default is no field for rotation
    mScaleClassificationField = -1; // Default is no field for scale
}

QgsSingleSymbolRenderer::QgsSingleSymbolRenderer(const QgsSingleSymbolRenderer& other)
{
    mVectorType = other.mVectorType;
    mSymbol = new QgsSymbol(*other.mSymbol);
    mAngleClassificationField = other.angleClassificationField();
    mScaleClassificationField = other.scaleClassificationField();
}

QgsSingleSymbolRenderer& QgsSingleSymbolRenderer::operator=(const QgsSingleSymbolRenderer& other)
{
    if(this!=&other)
    {
      mVectorType = other.mVectorType;
      delete mSymbol;
      mSymbol = new QgsSymbol(*other.mSymbol);
      mAngleClassificationField = other.angleClassificationField();
      mScaleClassificationField = other.scaleClassificationField();
    }
    return *this;
}

QgsSingleSymbolRenderer::~QgsSingleSymbolRenderer()
{
    delete mSymbol;
}

void QgsSingleSymbolRenderer::addSymbol(QgsSymbol* sy)
{
    delete mSymbol;
    mSymbol=sy;
}

void QgsSingleSymbolRenderer::renderFeature(QPainter * p, QgsFeature & f, QImage* img, 
	         double* scalefactor, bool selected, double widthScale)
{
	// Point 
	if ( img && mVectorType == QGis::Point) {

          // If scale field is non-negative, use it to scale.
          if (mScaleClassificationField >= 0)
          {
            //first find out the value for the scale classification attribute
            const QgsAttributeMap& attrs = f.attributeMap();
            widthScale *= sqrt(fabs(attrs[mScaleClassificationField].toDouble()));
            QgsDebugMsg(QString("Feature has scale factor %1").arg(*scalefactor));
          }
          *img = mSymbol->getPointSymbolAsImage(  widthScale, 
					 selected, mSelectionColor );
          // If rotation field is non-negative, use it to rotate.
          if (mAngleClassificationField >= 0)
          {
            //first find out the value for the angle classification attribute
            const QgsAttributeMap& attrs = f.attributeMap();
            double angle = attrs[mAngleClassificationField].toDouble();
            int intangle = attrs[mAngleClassificationField].toInt();
            QMatrix rotationMatrix;

#ifdef QGISDEBUG
            QgsLogger::debug("Rendering attribute size ", attrs.size(), 1, __FILE__, __FUNCTION__, __LINE__);
            QgsLogger::debug("Rendering feature field ", mAngleClassificationField, 1, __FILE__, __FUNCTION__, __LINE__);
            QgsLogger::debug("Rendering feature angle ", angle, 1, __FILE__, __FUNCTION__, __LINE__);
            QgsLogger::debug("Rendering feature angle ", intangle, 1, __FILE__, __FUNCTION__, __LINE__);
#endif

            rotationMatrix = rotationMatrix.rotate(angle);

            *img = img->transformed(rotationMatrix, Qt::SmoothTransformation);
          }
	    
          if ( scalefactor )
          { 
            *scalefactor = 1.0;
          }
        }


        // Line, polygon
 	if ( mVectorType != QGis::Point )
	{
	    if( !selected ) 
	    {
		QPen pen=mSymbol->pen();
		pen.setWidthF ( widthScale * pen.width() );
		p->setPen(pen);
		p->setBrush(mSymbol->brush());
	    }
	    else
	    {
		QPen pen=mSymbol->pen();
		pen.setWidthF ( widthScale * pen.width() );
                if (mVectorType == QGis::Line)
                  pen.setColor(mSelectionColor);
		QBrush brush=mSymbol->brush();
		brush.setColor(mSelectionColor);
		p->setPen(pen);
		p->setBrush(brush);
	    }
	}
}

void QgsSingleSymbolRenderer::readXML(const QDomNode& rnode, QgsVectorLayer& vl)
{
    mVectorType = vl.vectorType();
    QgsSymbol* sy = new QgsSymbol(mVectorType);

    QDomNode classnode = rnode.namedItem("angleclassificationfield");

    int angleClassificationfield = -1;
    int scaleClassificationfield = -1;

    if ( !classnode.isNull() )
    {
      angleClassificationfield = classnode.toElement().text().toInt();
      QgsDebugMsg("Found anglefield: " + QString::number(angleClassificationfield));
    }

    classnode = rnode.namedItem("scaleclassificationfield");
    if ( !classnode.isNull() )
    {
      int scaleClassificationfield = classnode.toElement().text().toInt();
      QgsDebugMsg("Found scalefield: " + QString::number(scaleClassificationfield));
    }

    this->setScaleClassificationField(scaleClassificationfield);
    this->setAngleClassificationField(angleClassificationfield);

    QDomNode synode = rnode.namedItem("symbol");
    
    if ( synode.isNull() )
    {
        QgsDebugMsg("No symbol node in project file's renderitem DOM");
        // XXX abort?
    }
    else
    {
        sy->readXML ( synode );
    }

    //create a renderer and add it to the vector layer
    this->addSymbol(sy);
    vl.setRenderer(this);
}

bool QgsSingleSymbolRenderer::writeXML( QDomNode & layer_node, QDomDocument & document ) const
{
  bool returnval=false;
  QDomElement singlesymbol=document.createElement("singlesymbol");
  layer_node.appendChild(singlesymbol);

  QDomElement angleclassificationfield=document.createElement("angleclassificationfield");
  QDomText angleclassificationfieldtxt=document.createTextNode(QString::number(mAngleClassificationField));
  angleclassificationfield.appendChild(angleclassificationfieldtxt);
  singlesymbol.appendChild(angleclassificationfield);

  QDomElement scaleclassificationfield=document.createElement("scaleclassificationfield");
  QDomText scaleclassificationfieldtxt=document.createTextNode(QString::number(mScaleClassificationField));
  scaleclassificationfield.appendChild(scaleclassificationfieldtxt);
  singlesymbol.appendChild(scaleclassificationfield);

  if(mSymbol)
  {
    returnval=mSymbol->writeXML(singlesymbol,document);
  }
  return returnval;
}


QgsAttributeList QgsSingleSymbolRenderer::classificationAttributes() const
{
  QgsAttributeList list;
  if ( mAngleClassificationField >= 0 )
  {
    list.append(mAngleClassificationField);
  }
  if ( mScaleClassificationField >= 0 )
  {
    list.append(mScaleClassificationField);
  }
  return list;
}

QString QgsSingleSymbolRenderer::name() const
{
  return "Single Symbol";
}

const QList<QgsSymbol*> QgsSingleSymbolRenderer::symbols() const
{
    QList<QgsSymbol*> list;
    list.append(mSymbol);
    return list;
}

QgsRenderer* QgsSingleSymbolRenderer::clone() const
{
    QgsSingleSymbolRenderer* r = new QgsSingleSymbolRenderer(*this);
    return r;
}
