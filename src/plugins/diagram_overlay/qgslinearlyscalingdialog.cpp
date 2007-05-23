/***************************************************************************
                         qgslinearlyscalingdialog.cpp  -  description
                         ----------------------------
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

#include "qgslinearlyscalingdialog.h"
#include "qgslinearlyscalingdiagramrenderer.h"
#include "qgsvectordataprovider.h"

QgsLinearlyScalingDialog::QgsLinearlyScalingDialog(QgsVectorLayer* vl): QgsDiagramRendererWidget(vl)
{
  setupUi(this);
  QObject::connect(mFindMaximumValueButton, SIGNAL(clicked()), this, SLOT(insertMaximumAttributeValue()));
}

QgsLinearlyScalingDialog::~QgsLinearlyScalingDialog()
{

}

QgsDiagramRenderer* QgsLinearlyScalingDialog::createRenderer(const QString& type, int classAttr, const QgsAttributeList& attributes, const std::list<QColor>& colors) const
{
  QgsLinearlyScalingDiagramRenderer* renderer = new QgsLinearlyScalingDiagramRenderer(type, attributes, colors);
  renderer->setClassificationField(classAttr);

  double theValue = mValueLineEdit->text().toDouble();
  int size = mSizeSpinBox->value();

  QgsDiagramItem lowerItem(0, 0, 0, 0);
  QgsDiagramItem upperItem(theValue, theValue, size, size);

  renderer->setLowerItem(lowerItem);
  renderer->setUpperItem(upperItem);

  return renderer;
}

void QgsLinearlyScalingDialog::applySettings(const QgsDiagramRenderer* renderer)
{
  const QgsLinearlyScalingDiagramRenderer* linearRenderer = dynamic_cast<const QgsLinearlyScalingDiagramRenderer*>(renderer);
  if(linearRenderer)
    {
      QgsDiagramItem upperItem = linearRenderer->upperItem();
      mValueLineEdit->setText(QString::number(upperItem.upperBound(), 'f'));
      mSizeSpinBox->setValue(upperItem.height());
    }
}

void QgsLinearlyScalingDialog::insertMaximumAttributeValue()
{
  //find the maximum value for this attribute
  if(mVectorLayer)
    {
      QgsVectorDataProvider *provider = dynamic_cast<QgsVectorDataProvider *>(mVectorLayer->getDataProvider());
      if(provider)
	{
	  mValueLineEdit->setText(provider->maxValue(mClassificationField).toString());
	}
    }
}
