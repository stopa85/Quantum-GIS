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

void QgsComposerLegendWidget::on_mBoxCheckBox_stateChanged(int state)
{
  if(mLegend)
    {
      if(state == Qt::Checked)
	{
	  mLegend->setFrame(true);
	}
      else
	{
	  mLegend->setFrame(false);
	}
      mLegend->update();
    }
}


void QgsComposerLegendWidget::on_mBoxSpaceSpinBox_valueChanged(double d)
{
  if(mLegend)
    {
      mLegend->setBoxSpace(d);
      mLegend->adjustBoxSize();
      mLegend->update();
    }
}

void QgsComposerLegendWidget::on_mMoveDownPushButton_clicked()
{
  QStandardItemModel* itemModel = dynamic_cast<QStandardItemModel*>(mItemTreeView->model());
  if(!itemModel)
    {
      return;
    }

  QModelIndex currentIndex = mItemTreeView->currentIndex();
  if(!currentIndex.isValid())
    {
      return;
    }
  
  //is there an older sibling?
  int row = currentIndex.row();
  QModelIndex youngerSibling = currentIndex.sibling(row + 1, 0);

  if(!youngerSibling.isValid())
    {
      return;
    }

  QModelIndex parentIndex = currentIndex.parent();
  if(!parentIndex.isValid())
    {
      return;
    }

  //exchange the items
  QStandardItem* parentItem = itemModel->itemFromIndex(parentIndex);
  QList<QStandardItem*> youngerSiblingItem = parentItem->takeRow(row + 1);
  QList<QStandardItem*> itemToMove = parentItem->takeRow(row);
  
  parentItem->insertRow(row, youngerSiblingItem);
  parentItem->insertRow(row + 1, itemToMove); 
  mItemTreeView->setCurrentIndex(itemModel->indexFromItem(itemToMove.at(0)));

  if(mLegend)
    {
      mLegend->update();
    }
}

void QgsComposerLegendWidget::on_mMoveUpPushButton_clicked()
{
  QStandardItemModel* itemModel = dynamic_cast<QStandardItemModel*>(mItemTreeView->model());
  if(!itemModel)
    {
      return;
    }

  QModelIndex currentIndex = mItemTreeView->currentIndex();
  if(!currentIndex.isValid())
    {
      return;
    }

  //is there an older sibling?
  int row = currentIndex.row();
  QModelIndex olderSibling = currentIndex.sibling(row - 1, 0);

  if(!olderSibling.isValid())
    {
      return;
    }

  QModelIndex parentIndex = currentIndex.parent();
  if(!parentIndex.isValid())
    {
      return;
    }
  
  //exchange the items
  QStandardItem* parentItem = itemModel->itemFromIndex(parentIndex);
  QList<QStandardItem*> itemToMove = parentItem->takeRow(row);
  QList<QStandardItem*> olderSiblingItem = parentItem->takeRow(row - 1);
  
  parentItem->insertRow(row - 1, itemToMove);
  parentItem->insertRow(row, olderSiblingItem);
  mItemTreeView->setCurrentIndex(itemModel->indexFromItem(itemToMove.at(0)));

  if(mLegend)
    {
      mLegend->update();
    }
}

void QgsComposerLegendWidget::on_mRemovePushButton_clicked()
{
  QStandardItemModel* itemModel = dynamic_cast<QStandardItemModel*>(mItemTreeView->model());
  if(!itemModel)
    {
      return;
    }

  QModelIndex currentIndex = mItemTreeView->currentIndex();
  if(!currentIndex.isValid())
    {
      return;
    }

  QModelIndex parentIndex = currentIndex.parent();
  if(!parentIndex.isValid())
    {
      return;
    }
  
  itemModel->removeRow(currentIndex.row(), parentIndex);
  if(mLegend)
    {
      mLegend->update();
    }
}
