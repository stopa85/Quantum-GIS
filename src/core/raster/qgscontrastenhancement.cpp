/* **************************************************************************
             qgscontrastenhancement.cpp -  description
   -------------------
begin                : Mon Oct 22 2007
copyright            : (C) 2007 by Peter J. Ersts
email                : ersts@amnh.org

This class contains code that was originally part of the larger QgsRasterLayer 
class originally created circa 2004 by T.Sutton, Gary E.Sherman, Steve Halasz
****************************************************************************/

/* **************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include "qgscontrastenhancement.h"
 
#include <iostream>
 
QgsContrastEnhancement::QgsContrastEnhancement(GDALDataType theDataType)
{

  mLookupTable = NULL;
  mContrastEnhancementAlgorithm = NO_STRETCH;
  mGDALDataType = theDataType;
  mMinimumValue = getMinimumPossibleValue(mGDALDataType);
  mMaximumValue = getMaximumPossibleValue(mGDALDataType);
  mGDALDataTypeRange = mMaximumValue - mMinimumValue;
  
  mLookupTableOffset = mMinimumValue * -1;
  
  if(mGDALDataTypeRange <= 65535)
  {
     mLookupTable = new int[static_cast <int>(mGDALDataTypeRange)];
  }
  
}
 
QgsContrastEnhancement::~QgsContrastEnhancement()
{
}
/*
 *
 * Static methods
 *
 */

/** 
  Simple function to compute the maximum possible value for a GDAL data type. 
  
  This method was created becase, at the time of writing, GDALRasterBand::GetMaximum() 
  would crash with some .img files
*/
double QgsContrastEnhancement::getMaximumPossibleValue(GDALDataType theDataType) 
{
  if(GDT_Byte == theDataType)
  {
    return 255.0;
  }
  else if(GDT_UInt16 == theDataType)
  {
    return 65535.0;
  }
  else if(GDT_Int16 == theDataType || GDT_CInt16 == theDataType)
  {
    return 32767.0;
  }
  else if(GDT_Int32 == theDataType || GDT_CInt32 == theDataType)
  {
    return 2147483647.0;
  }

  return 4294967295.0;
}
/** 
  Simple function to compute the minimum possible value for a GDAL data type. 
  
  This method was created becase, at the time of writing, GDALRasterBand::GetMinimum() 
  would crash with some .img files
*/
double QgsContrastEnhancement::getMinimumPossibleValue(GDALDataType theDataType) 
{
  if(GDT_Byte == theDataType || GDT_UInt16 == theDataType || GDT_UInt32 == theDataType)
  {
    return 0.0;
  }
  else if(GDT_Int16 == theDataType || GDT_CInt16 == theDataType)
  {
    return -32768.0;
  }
  else if(GDT_Int32 == theDataType || GDT_CInt32 == theDataType)
  {
    return -2147483648.0;
  }

  return -4294967295.0;
}

/*
 *
 * Non-Static methods
 *
 */
int QgsContrastEnhancement::calculateContrastEnhancementValue(double theValue)
{
  switch(mContrastEnhancementAlgorithm)
  {
    case CLIP_TO_MINMAX:
      if((theValue - mLookupTableOffset) < mMinimumValue || (theValue - mLookupTableOffset) > mMaximumValue)
      {
        return -1;
      }
      else
      {
        return static_cast<int>((((theValue - mLookupTableOffset) - mMinimumValue)/(mMinimumMaximumRange))*255.0);
      }
      break;
    case STRETCH_TO_MINMAX:
      return static_cast<int>((((theValue - mLookupTableOffset) - mMinimumValue)/(mMinimumMaximumRange))*255.0);
      break;
    default:
      return 0;
  }
}

bool QgsContrastEnhancement::generateLookupTable()
{
  if(NO_STRETCH == mContrastEnhancementAlgorithm) { return false; }
  if(GDT_Int32 == mGDALDataType || GDT_CInt32 == mGDALDataType) { return false; }
  if(!mLookupTable) { return false; }
  
  for(int runner = 0; runner < mGDALDataTypeRange; runner++)
  {
    mLookupTable[runner] = calculateContrastEnhancementValue((double)runner - mLookupTableOffset);
  }
  
  return true;
}

bool QgsContrastEnhancement::isValueInDisplyableRange(double theValue)
{
  if(theValue < mMinimumValue || theValue > mMaximumValue)
  {
    return false;
  }
  
  return true;
}

void QgsContrastEnhancement::setContrastEnhancementAlgorithm(CONTRAST_ENHANCEMENT_ALGORITHM theAlgorithm)
{
  mContrastEnhancementAlgorithm = theAlgorithm;
  
  if(NO_STRETCH != mContrastEnhancementAlgorithm)
  {
    generateLookupTable();
  }
}

void QgsContrastEnhancement::setMaximumValue(double theValue, bool generateTable)
{ 
  if(theValue > getMaximumPossibleValue(mGDALDataType))
  {
    mMaximumValue = getMaximumPossibleValue(mGDALDataType);
  }
  else
  {
    mMaximumValue = theValue; 
  }
 
  mMinimumMaximumRange = mMaximumValue - mMinimumValue;
 
  if(generateTable)
  {
    generateLookupTable();
  }
}

void QgsContrastEnhancement::setMinimumValue(double theValue, bool generateTable)
{ 
  if(theValue < getMinimumPossibleValue(mGDALDataType))
  {
    mMinimumValue = getMinimumPossibleValue(mGDALDataType);
  }
  else
  {
    mMinimumValue = theValue; 
  }
  
  mMinimumMaximumRange = mMaximumValue - mMinimumValue;
  
  if(generateTable)
  {
    generateLookupTable();
  }
}

int QgsContrastEnhancement::stretch(double theValue)
{
  if(mLookupTable)
  {
    return mLookupTable[static_cast <int>(theValue)];
  }
  else
  {
    return calculateContrastEnhancementValue(theValue);
  }
}
