/***************************************************************************
                           qgscomposerpicture.cpp
                             -------------------
    begin                : September 2005
    copyright            : (C) 2005 by Radim Blazek
    email                : radim.blazek@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#include "qgscomposerpicture.h"
#include <QFileInfo>
#include <QImageReader>
#include <QPainter>
#include <QSvgRenderer>

QgsComposerPicture::QgsComposerPicture(QgsComposition *composition): QgsComposerItem(composition), mMode(UNKNOWN), mSvgCacheUpToDate(false), mCachedDpi(0)
{
}

QgsComposerPicture::QgsComposerPicture(): QgsComposerItem(0), mMode(UNKNOWN), mSvgCacheUpToDate(false)
{

}

QgsComposerPicture::~QgsComposerPicture()
{

}

void QgsComposerPicture::paint (QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget)
{
  if(!painter)
    {
      return;
    }

  if(mMode == SVG)
    {
      int newDpi = (painter->device()->logicalDpiX() + painter->device()->logicalDpiY()) / 2;
      if(newDpi != mCachedDpi)
	{
	  mSvgCacheUpToDate = false;
	  mCachedDpi = newDpi;
	  mImage = QImage(rect().width() * newDpi/25.4, rect().height() * newDpi/25.4, QImage::Format_ARGB32);
	}

      if(!mSvgCacheUpToDate)
	{
	  updateImageFromSvg();
	}
    }

  painter->save();
  painter->rotate(mRotation);

  if(mMode != UNKNOWN)
    {
      double widthRatio = mImage.width() / rect().width();
      double heightRatio = mImage.height() / rect().height();
      double targetWidth, targetHeight;
      if(widthRatio > heightRatio)
	{
	  targetWidth = rect().width();
	  targetHeight = mImage.height() / widthRatio;
	}
      else
	{
	  targetHeight = rect().height();
	  targetWidth = mImage.width() / heightRatio;
	}
      painter->drawImage(QRectF(0, 0, targetWidth, targetHeight), mImage, QRectF(0, 0, mImage.width(), mImage.height()));
    }

  //frame and selection boxes
  drawFrame(painter);
  if(isSelected())
    {
      drawSelectionBoxes(painter);
    }

  painter->restore();
}

void QgsComposerPicture::setPictureFile(const QString& path)
{
  mSourceFile.setFileName(path);
  if(!mSourceFile.exists())
    {
      mMode = UNKNOWN;
    }
  
  QFileInfo sourceFileInfo(mSourceFile);
  QString sourceFileSuffix = sourceFileInfo.suffix();
  if(sourceFileSuffix.compare("svg", Qt::CaseInsensitive) == 0)
    {
      //try to open svg
      QSvgRenderer validTestRenderer(mSourceFile.fileName());
      if(validTestRenderer.isValid())
	{
	  mMode = SVG;
	  mDefaultSvgSize = validTestRenderer.defaultSize();
	}
      else
	{
	  mMode = UNKNOWN;
	}
    }
  else
    {
      //try to open raster with QImageReader
      QImageReader imageReader(mSourceFile.fileName());
      if(imageReader.read(&mImage))
	{
	  mMode = RASTER;
	}
      else
	{
	  mMode = UNKNOWN;
	}
    }
}

void QgsComposerPicture::updateImageFromSvg()
{
  mImage.fill(0);
  QPainter p(&mImage);
  QSvgRenderer theRenderer(mSourceFile.fileName());
  if(theRenderer.isValid())
    {
      theRenderer.render(&p);
    }
  mSvgCacheUpToDate = true;
}

void QgsComposerPicture::setSceneRect(const QRectF& rectangle)
{
  mSvgCacheUpToDate = false;
  if(mMode == SVG)
    {
      //keep aspect ratio
      double widthRatio = rectangle.width() / mDefaultSvgSize.width();
      double heightRatio = rectangle.height() / mDefaultSvgSize.height();

      double newImageWidth;
      double newImageHeight;

      if(widthRatio > heightRatio)
	{
	  newImageWidth = rectangle.width() * mCachedDpi / 25.4;
	  newImageHeight = mDefaultSvgSize.height() * widthRatio * mCachedDpi / 25.4;
	}
      else
	{
	  newImageHeight = rectangle.height() * mCachedDpi / 25.4;
	  newImageWidth = mDefaultSvgSize.width() * heightRatio * mCachedDpi / 25.4;
	}
      mImage = QImage(newImageWidth, newImageHeight, QImage::Format_ARGB32);
    }
  QgsComposerItem::setSceneRect(rectangle);
}

void QgsComposerPicture::setRotation(double rotation)
{
  if(rotation > 360)
    {
      mRotation = ((int)rotation) % 360;
    }
  else
    {
      mRotation = rotation;
    }
}

bool QgsComposerPicture::writeXML(QDomElement& elem, QDomDocument & doc)
{
  return false; //soon...
}

bool QgsComposerPicture::readXML(const QDomElement& itemElem, const QDomDocument& doc)
{
  return false; //soon...
}
