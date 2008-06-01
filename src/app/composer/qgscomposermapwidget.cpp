/***************************************************************************
                         qgscomposermapwidget.cpp
                         ------------------------
    begin                : May 26, 2008
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

#include "qgscomposermapwidget.h"
#include "qgscomposermap.h"

QgsComposerMapWidget::QgsComposerMapWidget(QgsComposerMap* composerMap): QWidget(), mComposerMap(composerMap)
{
  setupUi(this);
  mWidthLineEdit->setValidator(new QDoubleValidator(0));
  mHeightLineEdit->setValidator(new QDoubleValidator(0));
  mScaleLineEdit->setValidator(new QDoubleValidator(0));

  mCalculateComboBox->insertItem( 0, tr("Extent (calculate scale)"));
  mCalculateComboBox->insertItem( 1, tr("Scale (calculate extent)"));

  mPreviewModeComboBox->insertItem(0, tr("Cache"));
  mPreviewModeComboBox->insertItem(1, tr("Render"));
  mPreviewModeComboBox->insertItem(2, tr("Rectangle"));

  mFrameCheckBox->setCheckState(Qt::Checked);

  if(composerMap)
    {
      connect(composerMap, SIGNAL(extentChanged()), this, SLOT(updateSettingsNoSignals()));
    }

  updateGuiElements();
}

QgsComposerMapWidget::~QgsComposerMapWidget()
{
  
}

void QgsComposerMapWidget::on_mWidthLineEdit_editingFinished()
{
  if(mComposerMap)
    {
      bool conversionSuccess = true;
      double newWidth = mWidthLineEdit->text().toDouble(&conversionSuccess);
      if(!conversionSuccess)
	{
	  return;
	}
      QRectF composerMapRect = mComposerMap->rect();
      QRectF newRect(composerMapRect.x(), composerMapRect.y(), newWidth, composerMapRect.height());
      mComposerMap->setRect(newRect);
    }
}

void QgsComposerMapWidget::on_mHeightLineEdit_editingFinished()
{
  if(mComposerMap)
    {
      bool conversionSuccess = true;
      double newHeight = mHeightLineEdit->text().toDouble(&conversionSuccess);
      if(!conversionSuccess)
	{
	  return;
	}
      QRectF composerMapRect = mComposerMap->rect();
      QRectF newRect(composerMapRect.x(), composerMapRect.y(), composerMapRect.width(), newHeight);
      mComposerMap->setRect(newRect);
    }
}

void QgsComposerMapWidget::on_mPreviewModeComboBox_activated(int i)
{
  if(!mComposerMap)
    {
      return;
    }

  QString comboText = mPreviewModeComboBox->currentText();
  if(comboText == tr("Cache"))
    {
      mComposerMap->setPreviewMode(QgsComposerMap::Cache);
    }
  else if(comboText == tr("Render"))
    {
      mComposerMap->setPreviewMode(QgsComposerMap::Render);
    }
  else if(comboText == tr("Rectangle"))
    {
      mComposerMap->setPreviewMode(QgsComposerMap::Rectangle);
    }
}

void QgsComposerMapWidget::on_mCalculateComboBox_activated(int i)
{
  if(!mComposerMap)
    {
      return;
    }
  
  QString comboText = mCalculateComboBox->currentText();
  if(comboText == tr("Extent (calculate scale)"))
    {
      mComposerMap->setCalculationMode(QgsComposerMap::Scale);
      mHeightLineEdit->setEnabled(true);
      mWidthLineEdit->setEnabled(true);
      mScaleLineEdit->setEnabled(false);
    }
  else if(comboText == tr("Scale (calculate extent)"))
    {
      mComposerMap->setCalculationMode(QgsComposerMap::Extent);
      mHeightLineEdit->setEnabled(false);
      mWidthLineEdit->setEnabled(false);
      mScaleLineEdit->setEnabled(true);
    }
}

void QgsComposerMapWidget::on_mFrameCheckBox_stateChanged(int state)
{
  if(!mComposerMap)
    {
      return;
    }

  if(state == Qt::Checked)
    {
      if(mComposerMap->frame())
	{
	  return;
	}
      mComposerMap->setFrame(true);
    }
  else
    {
      if(!mComposerMap->frame())
	{
	  return;
	}
      mComposerMap->setFrame(false);
    }
  mComposerMap->update();
}

void QgsComposerMapWidget::updateSettingsNoSignals()
{
  mHeightLineEdit->blockSignals(true);
  mWidthLineEdit->blockSignals(true);
  mScaleLineEdit->blockSignals(true);
  mCalculateComboBox->blockSignals(true);
  mPreviewModeComboBox->blockSignals(true);

  updateGuiElements();
  
  mHeightLineEdit->blockSignals(false);
  mWidthLineEdit->blockSignals(false);
  mScaleLineEdit->blockSignals(false);
  mCalculateComboBox->blockSignals(false);
  mPreviewModeComboBox->blockSignals(false);
}

void QgsComposerMapWidget::updateGuiElements()
{
  if(mComposerMap)
    {
      //width, height, scale
      QRectF composerMapRect = mComposerMap->rect();
      mWidthLineEdit->setText(QString::number(composerMapRect.width()));
      mHeightLineEdit->setText(QString::number(composerMapRect.height()));
      mScaleLineEdit->setText(QString::number(mComposerMap->scale()));

      //calculation mode
      QgsComposerMap::Calculate calculationMode = mComposerMap->calculationMode();
      int index = -1;
      if(calculationMode == QgsComposerMap::Scale)
	{
	  index = mCalculateComboBox->findText(tr("Extent (calculate scale)"));
	  mHeightLineEdit->setEnabled(true);
	  mWidthLineEdit->setEnabled(true);
	  mScaleLineEdit->setEnabled(false);
	}
      else if(calculationMode == QgsComposerMap::Extent)
	{
	  index = mCalculateComboBox->findText(tr("Scale (calculate extent)"));
	  mHeightLineEdit->setEnabled(true);
	  mWidthLineEdit->setEnabled(true);
	  mScaleLineEdit->setEnabled(false);
	}
      if(index != -1)
	{
	  mCalculateComboBox->setCurrentIndex(index);
	}

      //preview mode
      QgsComposerMap::PreviewMode previewMode = mComposerMap->previewMode();
      index = -1;
      if(previewMode == QgsComposerMap::Cache)
	{
	  index = mPreviewModeComboBox->findText(tr("Cache"));
	}
      else if(previewMode == QgsComposerMap::Render)
	{
	  index = mPreviewModeComboBox->findText(tr("Render"));
	}
      else if(previewMode == QgsComposerMap::Rectangle)
	{
	  index = mPreviewModeComboBox->findText(tr("Rectangle"));
	}
      if(index != -1)
	{
	  mPreviewModeComboBox->setCurrentIndex(index);
	}
    }
}


