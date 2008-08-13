/***************************************************************************
                         qgscomposerpicturewidget.cpp
                         ----------------------------
    begin                : August 13, 2008
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

#include "qgscomposerpicturewidget.h"
#include "qgscomposerpicture.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

QgsComposerPictureWidget::QgsComposerPictureWidget(QgsComposerPicture* picture): QWidget(), mPicture(picture)
{
  setupUi(this);
}

QgsComposerPictureWidget::~QgsComposerPictureWidget()
{

}

void QgsComposerPictureWidget::on_mPictureBrowseButton_clicked()
{
  //show file dialog
  QString filePath = QFileDialog::getOpenFileName(0, tr("Select svg/image file"));
  if(filePath.isEmpty())
    {
      return;
    }

  //check if file exists
  QFileInfo fileInfo(filePath);
  if(!fileInfo.exists() || !fileInfo.isReadable())
    {
      QMessageBox::critical(0, "Invalid file", "Error, file does not exist or is not readable");
      return;
    }

  mPictureLineEdit->blockSignals(true);
  mPictureLineEdit->setText(filePath);
  mPictureLineEdit->blockSignals(false);

  //pass file path to QgsComposerPicture
  if(mPicture)
    {
      mPicture->setPictureFile(filePath);
      mPicture->update();
    }
}

void QgsComposerPictureWidget::on_mPictureLineEdit_editingFinished()
{
  if(mPicture)
    {
      QString filePath = mPictureLineEdit->text();
      
      //check if file exists
      QFileInfo fileInfo(filePath);
      
      if(!fileInfo.exists() || !fileInfo.isReadable())
	{
	  QMessageBox::critical(0, "Invalid file", "Error, file does not exist or is not readable");
	  return;
	}

      mPicture->setPictureFile(filePath);
      mPicture->update();
    }
}

void QgsComposerPictureWidget::on_mRotationSpinBox_valueChanged(double d)
{
  if(mPicture)
    {
      mPicture->setRotation(d);
      mPicture->update();
    }
}
