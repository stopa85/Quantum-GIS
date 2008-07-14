/***************************************************************************
                         qgscomposerlegendwidget.cpp
                         ---------------------------
    begin                : July 2008
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

#include "qgscomposerlegendwidget.h"
#include "qgscomposerlegend.h"
#include <QFontDialog>

QgsComposerLegendWidget::QgsComposerLegendWidget(QgsComposerLegend* legend): mLegend(legend)
{
  setupUi(this);

  if(legend)
    {
      mItemTreeView->setModel(legend->model());
    }

  setGuiElements();
}

QgsComposerLegendWidget::QgsComposerLegendWidget(): mLegend(0)
{
  setupUi(this);
}

QgsComposerLegendWidget::~QgsComposerLegendWidget()
{

}

void QgsComposerLegendWidget::setGuiElements()
{
  if(!mLegend)
    {
      return;
    }

  blockSignals(true);
  mTitleLineEdit->setText(mLegend->title());
  mSymbolWidthSpinBox->setValue(mLegend->symbolWidth());
  mSymbolHeightSpinBox->setValue(mLegend->symbolHeight());
  mLayerSpaceSpinBox->setValue(mLegend->layerSpace());
  mSymbolSpaceSpinBox->setValue(mLegend->symbolSpace());
  mIconLabelSpaceSpinBox->setValue(mLegend->iconLabelSpace());
  mBoxSpaceSpinBox->setValue(mLegend->boxSpace());

  if(mLegend->frame())
    { 
      mBoxCheckBox->setCheckState(Qt::Checked);
    }
  else
    {
      mBoxCheckBox->setCheckState(Qt::Unchecked);
    }
  blockSignals(false);
}


void QgsComposerLegendWidget::on_mTitleLineEdit_textChanged(const QString& text)
{
  if(mLegend)
    {
      mLegend->setTitle(text);
      mLegend->adjustBoxSize();
      mLegend->update();
    }
}

void QgsComposerLegendWidget::on_mSymbolWidthSpinBox_valueChanged(double d)
{
  if(mLegend)
    {
      mLegend->setSymbolWidth(d);
      mLegend->adjustBoxSize();
      mLegend->update();
    }
}

void QgsComposerLegendWidget::on_mSymbolHeightSpinBox_valueChanged(double d)
{
  if(mLegend)
    {
      mLegend->setSymbolHeight(d);
      mLegend->adjustBoxSize();
      mLegend->update();
    }
}

void QgsComposerLegendWidget::on_mLayerSpaceSpinBox_valueChanged(double d)
{
  if(mLegend)
    {
      mLegend->setLayerSpace(d);
      mLegend->adjustBoxSize();
      mLegend->update();
    }
}

void QgsComposerLegendWidget::on_mSymbolSpaceSpinBox_valueChanged(double d)
{
  if(mLegend)
    {
      mLegend->setSymbolSpace(d);
      mLegend->adjustBoxSize();
      mLegend->update();
    }
}

void QgsComposerLegendWidget::on_mIconLabelSpaceSpinBox_valueChanged(double d)
{
  if(mLegend)
    {
      mLegend->setIconLabelSpace(d);
      mLegend->adjustBoxSize();
      mLegend->update();
    }
}

void QgsComposerLegendWidget::on_mTitleFontButton_clicked()
{
  if(mLegend)
    {
      bool ok;
      QFont newFont = QFontDialog::getFont(&ok, mLegend->titleFont());
      if(ok)
	{
	  mLegend->setTitleFont(newFont);
	  mLegend->adjustBoxSize();
	  mLegend->update();
	}
    }
}

void QgsComposerLegendWidget::on_mLayerFontButton_clicked()
{
  if(mLegend)
    {
      bool ok;
      QFont newFont = QFontDialog::getFont(&ok, mLegend->layerFont());
      if(ok)
	{
	  mLegend->setLayerFont(newFont);
	  mLegend->adjustBoxSize();
	  mLegend->update();
	}
    }
}

void QgsComposerLegendWidget::on_mItemFontButton_clicked()
{
  if(mLegend)
    {
      bool ok;
      QFont newFont = QFontDialog::getFont(&ok, mLegend->itemFont());
      if(ok)
	{
	  mLegend->setItemFont(newFont);
	  mLegend->adjustBoxSize();
	  mLegend->update();
	}
    }
}
