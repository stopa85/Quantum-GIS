/***************************************************************************
                         qgsdiagramdialog.cpp  -  description
                         --------------------
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

#include "qgsdiagramdialog.h"
#include "qgsdiagramrenderer.h"
#include "qgsdiagramoverlay.h"
#include "qgsfield.h"
#include "qgslinearlyscalingdialog.h"
#include "qgsvectordataprovider.h"
#include "qgswkndiagramfactory.h"
#include <QColorDialog>


QgsDiagramDialog::QgsDiagramDialog(QgsVectorLayer* vl): mVectorLayer(vl)
{
  setupUi(this);
  QObject::connect(mAddPushButton, SIGNAL(clicked()), this, SLOT(addAttribute()));
  QObject::connect(mRemovePushButton, SIGNAL(clicked()), this, SLOT(removeAttribute()));
  QObject::connect(mClassificationTypeComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(changeClassificationType(const QString&)));
  QObject::connect(mClassificationComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(changeClassificationAttribute(const QString&)));
  QObject::connect(mAttributesTreeWidget, SIGNAL(itemDoubleClicked( QTreeWidgetItem*, int)), this, SLOT(handleItemDoubleClick(QTreeWidgetItem*, int)));

  mDiagramTypeComboBox->insertItem(0, "Bar");
  mDiagramTypeComboBox->insertItem(0, "Pie");

  QStringList headerLabels;
  headerLabels << "Attribute";
  headerLabels << "Color";
  mAttributesTreeWidget->setHeaderLabels(headerLabels);

  if(!mVectorLayer)
    {
      return;
    }

  //insert attributes into combo box
  QgsVectorDataProvider *provider;
  if ((provider = dynamic_cast<QgsVectorDataProvider *>(mVectorLayer->dataProvider())))
    {
      const QgsFieldMap & fields = provider->fields();
      QString str;
      
      int comboIndex = 0;
      for (QgsFieldMap::const_iterator it = fields.begin(); it != fields.end(); ++it)
        {
	  str = (*it).name();
	  mAttributesComboBox->insertItem(comboIndex, str);
	  mClassificationComboBox->insertItem(comboIndex, str);
	  ++comboIndex;
	}
    } 

  mClassificationTypeComboBox->insertItem(0, "linearly scaling");

  //if mVectorLayer already has a diagram overlay, apply its settings to this dialog
  const QgsVectorOverlay* previousOverlay = mVectorLayer->findOverlayByType("diagram");
  if(previousOverlay)
    {
      restoreSettings(previousOverlay);
    }
}

QgsDiagramDialog::QgsDiagramDialog(): mVectorLayer(0)
{

}
 
QgsDiagramDialog::~QgsDiagramDialog()
{

}

void QgsDiagramDialog::addAttribute()
{
  QTreeWidgetItem* newItem = new QTreeWidgetItem(mAttributesTreeWidget);

  //text
  QString currentText = mAttributesComboBox->currentText();
  newItem->setText(0, currentText);
  
  //and icon
  int red = 1 + (int) (255.0 * rand() / (RAND_MAX + 1.0));
  int green = 1 + (int) (255.0 * rand() / (RAND_MAX + 1.0));
  int blue = 1 + (int) (255.0 * rand() / (RAND_MAX + 1.0));
  QColor randomColor(red, green, blue);
  newItem->setBackground(1, QBrush(randomColor));
	    
  if(!currentText.isNull() && !currentText.isEmpty())
    {
      mAttributesTreeWidget->addTopLevelItem(newItem);
    }
}

void QgsDiagramDialog::removeAttribute()
{
  QTreeWidgetItem* currentItem = mAttributesTreeWidget->currentItem();
  if(currentItem)
    {
      delete currentItem;
    }
}

void QgsDiagramDialog::changeClassificationType(const QString& newType)
{
  if(newType == "linearly scaling")
    {
      QWidget* currentWidget = mWidgetStackRenderers->currentWidget();
      if(currentWidget)
	{
	  mWidgetStackRenderers->removeWidget(currentWidget);
	}
      QWidget* newWidget = new QgsLinearlyScalingDialog(mVectorLayer);
      mWidgetStackRenderers->addWidget(newWidget);
      mWidgetStackRenderers->setCurrentWidget(newWidget);
      changeClassificationAttribute(mClassificationComboBox->currentText());
    }
}

void QgsDiagramDialog::changeClassificationAttribute(const QString& newAttribute)
{
  int attributeIndex = QgsDiagramOverlay::indexFromAttributeName(newAttribute, mVectorLayer);
  if(attributeIndex == -1)
    {
      return;
    }
  QgsDiagramRendererWidget* rendererWidget = dynamic_cast<QgsDiagramRendererWidget*>(mWidgetStackRenderers->currentWidget());
  if(!rendererWidget)
    {
      return;
    }
  rendererWidget->changeClassificationField(attributeIndex);
}

void QgsDiagramDialog::apply() const
{
  std::list<QColor> colorList;
  QgsAttributeList attList;

  int topLevelItemCount = mAttributesTreeWidget->topLevelItemCount();
  QTreeWidgetItem* currentItem;
  int currentAttribute;
  
  int classificationField = QgsDiagramOverlay::indexFromAttributeName(mClassificationComboBox->currentText(), mVectorLayer);
  
  for(int i = 0; i < topLevelItemCount; ++i)
    {
      currentItem = mAttributesTreeWidget->topLevelItem(i);
      currentAttribute = QgsDiagramOverlay::indexFromAttributeName(currentItem->text(0), mVectorLayer);
      if(currentAttribute != -1)
	{
	  colorList.push_back(currentItem->background(1).color());
	  attList.push_back(currentAttribute);
	}
    }

  QgsDiagramRendererWidget* rendererWidget = dynamic_cast<QgsDiagramRendererWidget*>(mWidgetStackRenderers->currentWidget());
    if(!rendererWidget)
      {
	return;
      }

  QgsDiagramRenderer* renderer = rendererWidget->createRenderer(mDiagramTypeComboBox->currentText(), classificationField, attList, colorList);
  if(!renderer)
    {
      return;
    }

  //create QgsDiagramOverlay and add the renderer to it
    
    //the overlay may need a different attribute list than the renderer
	if(!attList.contains(classificationField))
	{
		attList.push_back(classificationField);	
	}
    
  QgsDiagramOverlay* diagramOverlay = new QgsDiagramOverlay(mVectorLayer);
  diagramOverlay->setDiagramRenderer(renderer);
  diagramOverlay->setAttributes(attList);
  
  //display flag
  if(mDisplayDiagramsCheckBox->checkState() == Qt::Checked)
    {
      diagramOverlay->setDisplayFlag(true);
    }
  if(mDisplayDiagramsCheckBox->checkState() == Qt::Unchecked)
    {
      diagramOverlay->setDisplayFlag(false);
    }

    //remove already existing diagram overlays
    mVectorLayer-> removeOverlay("diagram");

  //finally add the new overlay to the vector layer
  mVectorLayer->addOverlay(diagramOverlay);
}

void QgsDiagramDialog::handleItemDoubleClick(QTreeWidgetItem * item, int column)
{
  if(column == 1) //change color
    {
      QColor newColor = QColorDialog::getColor();
      if(newColor.isValid())
	{
	  item->setBackground(1, QBrush(newColor));
	}
    }
}

void QgsDiagramDialog::restoreSettings(const QgsVectorOverlay* overlay)
{
  const QgsDiagramOverlay* previousDiagramOverlay = dynamic_cast<const QgsDiagramOverlay*>(overlay);
  if(overlay)
    {
      //set check state according to QgsDiagramOverlay
      if(previousDiagramOverlay->displayFlag())
	{
	  mDisplayDiagramsCheckBox->setCheckState(Qt::Checked);
	}
      else
	{
	  mDisplayDiagramsCheckBox->setCheckState(Qt::Unchecked);
	}
      const QgsDiagramRenderer* previousDiagramRenderer = dynamic_cast<const QgsDiagramRenderer*>(previousDiagramOverlay->diagramRenderer());
     
      if(previousDiagramRenderer && previousDiagramRenderer->factory())
	{
	  QgsWKNDiagramFactory* theFactory = dynamic_cast<QgsWKNDiagramFactory*>(previousDiagramRenderer->factory());
	  if(theFactory)
	    {
	      //well known diagram name
	      mDiagramTypeComboBox->setCurrentIndex(mDiagramTypeComboBox->findText(theFactory->diagramType()));
	      //insert attribute names and colors into mAttributesTreeWidget
	      QList<QgsDiagramCategory> categoryList = theFactory->categories();    
	      QList<QgsDiagramCategory>::const_iterator c_it = categoryList.constBegin();
	      
	      for(; c_it != categoryList.constEnd(); ++c_it)
		{
		  QTreeWidgetItem* newItem = new QTreeWidgetItem(mAttributesTreeWidget);
		  newItem->setText(0, QgsDiagramOverlay::attributeNameFromIndex(c_it->propertyIndex(), mVectorLayer));
		  newItem->setBackground(1, c_it->brush());
		  mAttributesTreeWidget->addTopLevelItem(newItem);		 
		}
	      
	      //classification attribute
	      QString classFieldName = QgsDiagramOverlay::attributeNameFromIndex(theFactory->scalingAttributes().first(), mVectorLayer);
	      mClassificationComboBox->setCurrentIndex(mClassificationComboBox->findText(classFieldName));
	      
	      //classification type (specific for renderer subclass)
	      mClassificationTypeComboBox->setCurrentIndex(mClassificationTypeComboBox->findText("linearly scaling"));
	      
	      //apply the renderer settings to the renderer specific dialog
	      if(mWidgetStackRenderers->count() > 0)
		{
		  QgsDiagramRendererWidget* rendererWidget = dynamic_cast<QgsDiagramRendererWidget*>(mWidgetStackRenderers->currentWidget());
		  if(rendererWidget)
		    {
		      rendererWidget->applySettings(previousDiagramRenderer);
		    }
		}
	    }
	}
    }
}



