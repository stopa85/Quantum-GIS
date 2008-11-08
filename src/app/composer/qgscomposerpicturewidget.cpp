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
#include "qgsapplication.h"
#include "qgscomposerpicture.h"
#include "qgscomposeritemwidget.h"
#include <QDoubleValidator>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

QgsComposerPictureWidget::QgsComposerPictureWidget( QgsComposerPicture* picture ): QWidget(), mPicture( picture )
{
  setupUi( this );

  //add widget for general composer item properties
  QgsComposerItemWidget* itemPropertiesWidget = new QgsComposerItemWidget( this, picture );
  gridLayout->addWidget( itemPropertiesWidget, 4, 0, 1, 1 );

  mWidthLineEdit->setValidator( new QDoubleValidator( this ) );
  mHeightLineEdit->setValidator( new QDoubleValidator( this ) );

  setGuiElementValues();

  //add preview icons
  addStandardDirectoriesToPreview();

  connect( mPicture, SIGNAL( settingsChanged() ), this, SLOT( setGuiElementValues() ) );
}

QgsComposerPictureWidget::~QgsComposerPictureWidget()
{

}

void QgsComposerPictureWidget::on_mPictureBrowseButton_clicked()
{
  QString openDir;
  QString lineEditText = mPictureLineEdit->text();
  if ( !lineEditText.isEmpty() )
  {
    QFileInfo openDirFileInfo( lineEditText );
    openDir = openDirFileInfo.path();
  }


  //show file dialog
  QString filePath = QFileDialog::getOpenFileName( 0, tr( "Select svg or image file" ), openDir );
  if ( filePath.isEmpty() )
  {
    return;
  }

  //check if file exists
  QFileInfo fileInfo( filePath );
  if ( !fileInfo.exists() || !fileInfo.isReadable() )
  {
    QMessageBox::critical( 0, "Invalid file", "Error, file does not exist or is not readable" );
    return;
  }

  mPictureLineEdit->blockSignals( true );
  mPictureLineEdit->setText( filePath );
  mPictureLineEdit->blockSignals( false );

  //pass file path to QgsComposerPicture
  if ( mPicture )
  {
    mPicture->setPictureFile( filePath );
    mPicture->update();
  }
}

void QgsComposerPictureWidget::on_mPictureLineEdit_editingFinished()
{
  if ( mPicture )
  {
    QString filePath = mPictureLineEdit->text();

    //check if file exists
    QFileInfo fileInfo( filePath );

    if ( !fileInfo.exists() || !fileInfo.isReadable() )
    {
      QMessageBox::critical( 0, "Invalid file", "Error, file does not exist or is not readable" );
      return;
    }

    mPicture->setPictureFile( filePath );
    mPicture->update();
  }
}

void QgsComposerPictureWidget::on_mWidthLineEdit_editingFinished()
{
  if ( mPicture )
  {
    QRectF pictureRect = mPicture->rect();

    bool conversionOk;
    double newWidth = mWidthLineEdit->text().toDouble( &conversionOk );
    if ( conversionOk )
    {
      QRectF newSceneRect( mPicture->transform().dx(), mPicture->transform().dy(), newWidth, pictureRect.height() );
      mPicture->setSceneRect( newSceneRect );
    }
  }
}

void QgsComposerPictureWidget::on_mHeightLineEdit_editingFinished()
{
  if ( mPicture )
  {
    QRectF pictureRect = mPicture->rect();

    bool conversionOk;
    double newHeight = mHeightLineEdit->text().toDouble( &conversionOk );
    if ( conversionOk )
    {
      QRectF newSceneRect( mPicture->transform().dx(), mPicture->transform().dy(), pictureRect.width(), newHeight );
      mPicture->setSceneRect( newSceneRect );
    }
  }
}

void QgsComposerPictureWidget::on_mRotationSpinBox_valueChanged( double d )
{
  if ( mPicture )
  {
    mPicture->setRotation( d );
    mPicture->update();
  }
}

void QgsComposerPictureWidget::on_mPreviewListWidget_currentItemChanged( QListWidgetItem* current, QListWidgetItem* previous )
{
  if(!mPicture || !current)
  {
    return;
  }

  QString absoluteFilePath = current->data(Qt::UserRole).toString();
  mPicture->setPictureFile(absoluteFilePath);
  mPictureLineEdit->setText(absoluteFilePath);
  mPicture->update();
}

void QgsComposerPictureWidget::on_mAddDirectoryButton_clicked()
{
  //let user select a directory
  QString directory = QFileDialog::getExistingDirectory(0, tr("Select new preview directory"));
  if(directory.isNull())
  {
    return; //dialog canceled by user
  }

  //add entry to mSearchDirectoriesComboBox
  mSearchDirectoriesComboBox->addItem(directory);

  //and add icons to the preview
  addDirectoryToPreview(directory);
}

void QgsComposerPictureWidget::on_mRemoveDirectoryButton_clicked()
{
  QString directoryToRemove = mSearchDirectoriesComboBox->currentText();
  mSearchDirectoriesComboBox->removeItem(mSearchDirectoriesComboBox->currentIndex());

  //remove entries from back to front (to have the indices of existing items constant)
  for(int i = (mPreviewListWidget->count() - 1); i >=0; --i)
  {
    QListWidgetItem* currentItem = mPreviewListWidget->item(i);
    if(currentItem && currentItem->data(Qt::UserRole).toString().startsWith(directoryToRemove))
    {
      delete(mPreviewListWidget->takeItem(i));
    }
  }
}

void QgsComposerPictureWidget::setGuiElementValues()
{
  //set initial gui values
  if ( mPicture )
  {
    mWidthLineEdit->blockSignals( true );
    mHeightLineEdit->blockSignals( true );
    mRotationSpinBox->blockSignals( true );
    mPictureLineEdit->blockSignals( true );

    mPictureLineEdit->setText( mPicture->pictureFile() );
    QRectF pictureRect = mPicture->rect();
    mWidthLineEdit->setText( QString::number( pictureRect.width() ) );
    mHeightLineEdit->setText( QString::number( pictureRect.height() ) );
    mRotationSpinBox->setValue( mPicture->rotation() );

    mWidthLineEdit->blockSignals( false );
    mHeightLineEdit->blockSignals( false );
    mRotationSpinBox->blockSignals( false );
    mPictureLineEdit->blockSignals( false );
  }
}

int QgsComposerPictureWidget::addDirectoryToPreview(const QString& path)
{
  //go through all files of a directory
  QDir directory(path);
  if(!directory.exists() || !directory.isReadable())
  {
    return 1; //error
  }

  QFileInfoList fileList = directory.entryInfoList(QDir::Files);
  QFileInfoList::const_iterator fileIt = fileList.constBegin();
  for(; fileIt != fileList.constEnd(); ++fileIt)
  {
    qWarning(fileIt->absoluteFilePath().toLocal8Bit().data());
    QIcon icon(fileIt->absoluteFilePath()); //does this work with svg icons?
    QListWidgetItem * listItem = new QListWidgetItem(mPreviewListWidget);
    listItem->setIcon( icon );
    listItem->setText( "" );
    //store the absolute icon file as user data
    listItem->setData( Qt::UserRole, fileIt->absoluteFilePath());
  }

  return 0;
}

void QgsComposerPictureWidget::addStandardDirectoriesToPreview()
{
  //list all directories in $prefix/share/qgis/svg
  QDir svgDirectory(QgsApplication::svgPath());
  if(!svgDirectory.exists() || !svgDirectory.isReadable())
  {
      return; //error
  }

  QFileInfoList directoryList = svgDirectory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList::const_iterator dirIt = directoryList.constBegin();
  for(; dirIt != directoryList.constEnd(); ++dirIt)
  {
    qWarning(dirIt->absoluteFilePath().toLocal8Bit().data());
    if(addDirectoryToPreview(dirIt->absoluteFilePath()) == 0)
    {
       mSearchDirectoriesComboBox->addItem(dirIt->absoluteFilePath());
    }
  }
}
