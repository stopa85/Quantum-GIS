/***************************************************************************
                         qgssisydialog.cpp  -  description
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

#include "qgssisydialog.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include "qcolordialog.h"
#include "qgspatterndialog.h"
#include "qgssymbologyutils.h"
#include "qgslinestyledialog.h"
#include <iostream>
#include "qgsvectorlayer.h"
#include "qpixmap.h"
#include "qgslegenditem.h"
#include "qgslayerproperties.h"
#include "qgsrenderitem.h"
#include "qgssinglesymrenderer.h"
#include "qgsvectorlayerproperties.h"
#include <qlineedit.h>

QgsSiSyDialog::QgsSiSyDialog(): QgsSiSyDialogBase(), m_vectorlayer(0)
{

}

QgsSiSyDialog::QgsSiSyDialog(QgsVectorLayer* layer): QgsSiSyDialogBase(), m_vectorlayer(layer)
{
    //the default settings for single symbol. This is done here because 'single symbol' is the default legend type for a vector layer and can thus be created without calling QgsLayerProperties::alterLayerDialog//already done in qgisapp.cpp
    /*QgsSymbol sy;
    sy.brush().setColor(QColor(0,0,255));
    sy.brush().setStyle(Qt::SolidPattern);
    sy.pen().setStyle(Qt::SolidLine);
    sy.pen().setColor(QColor(0,0,0));
    QgsRenderItem ri(sy,"", "");
    ((QgsSingleSymRenderer*)(layer->renderer()))->addItem(ri);*/

    if(layer)
    {
	//Set the initial display name
	displaynamefield->setText(m_vectorlayer->name());
	outlinecolorbutton->setPaletteBackgroundColor(QColor(0,0,0));
	stylebutton->setText(tr("SolidLine"));
	outlinewidthspinbox->setValue(1);
	fillcolorbutton->setPaletteBackgroundColor(QColor(0,0,255));
	patternbutton->setText(tr("SolidPattern"));

	if(m_vectorlayer&&m_vectorlayer->vectorType()==QGis::Line)
	{
	    fillcolorbutton->unsetPalette();
	    fillcolorbutton->setEnabled(false);
	    patternbutton->setText("");
	    patternbutton->setEnabled(false);
	}

	//do the signal/slot connections
	QObject::connect(outlinecolorbutton,SIGNAL(clicked()),this,SLOT(selectOutlineColor()));
	QObject::connect(stylebutton,SIGNAL(clicked()),this,SLOT(selectOutlineStyle()));
	QObject::connect(fillcolorbutton,SIGNAL(clicked()),this,SLOT(selectFillColor()));
	QObject::connect(patternbutton,SIGNAL(clicked()),this,SLOT(selectFillPattern()));
	QObject::connect(applybutton,SIGNAL(clicked()),this,SLOT(apply()));
	QObject::connect(closebutton,SIGNAL(clicked()),this,SLOT(hide()));
    }
    else
    {
	qWarning("Warning, layer is a null pointer in QgsSiSyDialog::QgsSiSyDialog(QgsVectorLayer)");
    }
}

QgsSiSyDialog::~QgsSiSyDialog()
{
    std::cout << "bin im Destruktor von QgsSiSyDialog" << std::endl;
}

void QgsSiSyDialog::selectOutlineColor()
{
    outlinecolorbutton->setPaletteBackgroundColor(QColorDialog::getColor());
    m_vectorlayer->propertiesDialog()->raise();
    raise();
}
    
void QgsSiSyDialog::selectOutlineStyle()
{
    QgsLineStyleDialog linestyledialog;
    if(linestyledialog.exec()==QDialog::Accepted)
    {
	stylebutton->setText(QgsSymbologyUtils::penStyle2QString(linestyledialog.style()));
    }
    m_vectorlayer->propertiesDialog()->raise();
    raise();
}

void QgsSiSyDialog::selectFillColor()
{
    fillcolorbutton->setPaletteBackgroundColor(QColorDialog::getColor());
    m_vectorlayer->propertiesDialog()->raise();
    raise();
}
    
void QgsSiSyDialog::selectFillPattern()
{
   QgsPatternDialog patterndialog;
    if(patterndialog.exec()==QDialog::Accepted)
    {
	patternbutton->setText(QgsSymbologyUtils::brushStyle2QString(patterndialog.pattern()));
    }
    m_vectorlayer->propertiesDialog()->raise();
    raise();
}

void QgsSiSyDialog::apply()
{
    //query the values of the widgets and set the symbology of the vector layer
    QgsSymbol sy(QColor(255,0,0));
    sy.brush().setColor(fillcolorbutton->paletteBackgroundColor());
    sy.brush().setStyle(QgsSymbologyUtils::qString2BrushStyle(patternbutton->text()));
    sy.pen().setStyle(QgsSymbologyUtils::qString2PenStyle(stylebutton->text()));
    sy.pen().setWidth(outlinewidthspinbox->value());
    sy.pen().setColor(outlinecolorbutton->paletteBackgroundColor());
    QgsRenderItem ri(sy,"blabla", "blabla");
    ((QgsSingleSymRenderer*)(m_vectorlayer->renderer()))->addItem(ri);

    //add a pixmap to the legend item
    QPixmap* pix=m_vectorlayer->legendPixmap();
    int namewidth=10+displaynamefield->text().length()*12;
    int width=(namewidth>60) ? namewidth : 60;
    pix->resize(width,75);
    pix->fill();

    QPainter p(pix);
    p.drawText(10,35,displaynamefield->text());
    m_vectorlayer->setlayerName(displaynamefield->text());
    p.setPen(sy.pen());
    p.setBrush(sy.brush());
    //paint differently in case of point, lines, polygones
    switch(m_vectorlayer->vectorType())
    {
	case QGis::Polygon:
	    p.drawRect(10,45,30,20);
	    break;
	case QGis::Line:
	    p.drawLine(10,55,40,55);
	    break;
	case QGis::Point:
	    p.drawRect(20,55,5,5);
    }
    
    m_vectorlayer->legendItem()->setPixmap(0,(*pix));

    //repaint the map canvas
    m_vectorlayer->triggerRepaint();
}
