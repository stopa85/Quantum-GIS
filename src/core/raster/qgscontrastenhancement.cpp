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
 
QgsContrastEnhancement::QgsContrastEnhancement(QgsRasterDataType theDataType)
{

  mLookupTable = NULL;
  mEnhancementDirty = false;
  mContrastEnhancementAlgorithm = NO_STRETCH;
  mQgsRasterDataType = theDataType;
  mMinimumValue = getMinimumPossibleValue(mQgsRasterDataType);
  mMaximumValue = getMaximumPossibleValue(mQgsRasterDataType);
  mQgsRasterDataTypeRange = mMaximumValue - mMinimumValue;
  
  mLookupTableOffset = mMinimumValue * -1;
  
  if(mQgsRasterDataTypeRange <= 65535)
  {
     mLookupTable = new int[static_cast <int>(mQgsRasterDataTypeRange)];
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
double QgsContrastEnhancement::getMaximumPossibleValue(QgsRasterDataType theDataType) 
{
  if(QGS_Byte == theDataType)
  {
    return 255.0;
  }
  else if(QGS_UInt16 == theDataType)
  {
    return 65535.0;
  }
  else if(QGS_Int16 == theDataType || QGS_CInt16 == theDataType)
  {
    return 32767.0;
  }
  else if(QGS_Int32 == theDataType || QGS_CInt32 == theDataType)
  {
    return 2147483647.0;
  }

  return std::numeric_limits<double>::max();
}
/** 
  Simple function to compute the minimum possible value for a GDAL data type. 
  
  This method was created becase, at the time of writing, GDALRasterBand::GetMinimum() 
  would crash with some .img files
*/
double QgsContrastEnhancement::getMinimumPossibleValue(QgsRasterDataType theDataType) 
{
  if(QGS_Byte == theDataType || QGS_UInt16 == theDataType || QGS_UInt32 == theDataType)
  {
    return 0.0;
  }
  else if(QGS_Int16 == theDataType || QGS_CInt16 == theDataType)
  {
    return -32768.0;
  }
  else if(QGS_Int32 == theDataType || QGS_CInt32 == theDataType)
  {
    return -2147483648.0;
  }

  return std::numeric_limits<double>::min();
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
    case NO_STRETCH:
    {
      if(mQgsRasterDataType == QGS_Byte)
      {
        return static_cast<int>(theValue);
      }
      else
      {
        static_cast<int>((((theValue - getMinimumPossibleValue(mQgsRasterDataType))/(getMaximumPossibleValue(mQgsRasterDataType) - getMinimumPossibleValue(mQgsRasterDataType)))*255.0));
      }
      break;
    }
    case STRETCH_TO_MINMAX:
    {
      int myValue = static_cast<int>(((theValue - mMinimumValue)/(mMinimumMaximumRange))*255.0);
      if(myValue < getMinimumPossibleValue(mQgsRasterDataType))
      {
        return 0;
      }
      else if(myValue > getMaximumPossibleValue(mQgsRasterDataType))
      {
        return static_cast<int>(getMaximumPossibleValue(mQgsRasterDataType));
      }
      return myValue; 
      break;
    }    
    case CLIP_TO_MINMAX:
    {
      if((theValue - mLookupTableOffset) < mMinimumValue || (theValue - mLookupTableOffset) > mMaximumValue)
      {
        return -1;
      }
      
      if(mQgsRasterDataType == QGS_Byte)
      {
        return static_cast<int>(theValue);
      }
      else
      {
        static_cast<int>((((theValue - getMinimumPossibleValue(mQgsRasterDataType))/(getMaximumPossibleValue(mQgsRasterDataType) - getMinimumPossibleValue(mQgsRasterDataType)))*255.0));
      }
      break;
    }
    case STRETCH_AND_CLIP_TO_MINMAX:
    {
      if((theValue - mLookupTableOffset) < mMinimumValue || (theValue - mLookupTableOffset) > mMaximumValue)
      {
        return -1;
      }

      int myValue = static_cast<int>(((theValue - mMinimumValue)/(mMinimumMaximumRange))*255.0);
      if(myValue < getMinimumPossibleValue(mQgsRasterDataType))
      {
        return 0;
      }
      else if(myValue > getMaximumPossibleValue(mQgsRasterDataType))
      {
        return static_cast<int>(getMaximumPossibleValue(mQgsRasterDataType));
      }
      return myValue; 
      break;
    }
    default:
      return 0;
  }
}

bool QgsContrastEnhancement::generateLookupTable()
{
  mEnhancementDirty = false;

  if(NO_STRETCH == mContrastEnhancementAlgorithm) { return false; }
  if(QGS_Int32 == mQgsRasterDataType || QGS_CInt32 == mQgsRasterDataType) { return false; }
  if(!mLookupTable) { return false; }

  for(int myIterator = 0; myIterator < mQgsRasterDataTypeRange; myIterator++)
  {
    mLookupTable[myIterator] = calculateContrastEnhancementValue((double)myIterator - mLookupTableOffset);
    //std::cout << myIterator << " -> " << mLookupTable[myIterator] << std::endl;
  }

  return true;
}

bool QgsContrastEnhancement::isValueInDisplayableRange(double theValue)
{

  if(CLIP_TO_MINMAX == mContrastEnhancementAlgorithm || STRETCH_AND_CLIP_TO_MINMAX == mContrastEnhancementAlgorithm)
  {
    if(theValue < mMinimumValue || theValue > mMaximumValue)
    {
      return false;
    }
  }

  if(theValue < getMinimumPossibleValue(mQgsRasterDataType) || theValue > getMaximumPossibleValue(mQgsRasterDataType))
  {
    return false;
  }

  return true;
}

//Set the contrast enhancement algorithm. The second parameter is option an is for performace improvements.
//If you know you are immediately going to set the Minimum or Maximum value, you can elect to 
// not generate the lookup tale. By default it will be generated.
void QgsContrastEnhancement::setContrastEnhancementAlgorithm(CONTRAST_ENHANCEMENT_ALGORITHM theAlgorithm, bool generateTable)
{
  mContrastEnhancementAlgorithm = theAlgorithm;
  // only call generateLookupTable if we have a contrast algorithm. The third check to is a improve performance, only generate the 
  // lookuptale if the enhancement has changed or a parameter have changed.
  if(generateTable && NO_STRETCH != mContrastEnhancementAlgorithm && theAlgorithm != mContrastEnhancementAlgorithm )
  {
    generateLookupTable();
  }
  else if(!generateTable && NO_STRETCH != mContrastEnhancementAlgorithm && theAlgorithm != mContrastEnhancementAlgorithm)
  {
    mEnhancementDirty;
  }
}

//Set the maximum value for the contrast enhancement. The second parameter is option an is for performace improvements.
//If you know you are immediately going to set the Minimum value or the contrast enhancement algorithm, you can elect to 
//not generate the lookup tale. By default it will be generated.
void QgsContrastEnhancement::setMaximumValue(double theValue, bool generateTable)
{
  if(theValue > getMaximumPossibleValue(mQgsRasterDataType))
  {
    mMaximumValue = getMaximumPossibleValue(mQgsRasterDataType);
  }
  else
  {
    mMaximumValue = theValue; 
  }

  mEnhancementDirty = true;
  mMinimumMaximumRange = mMaximumValue - mMinimumValue;

  if(generateTable)
  {
    generateLookupTable();
  }
}

//Set the maximum value for the contrast enhancement. The second parameter is option an is for performace improvements.
//If you know you are immediately going to set the Maximum value or the contrast enhancement algorithm, you can elect to 
//not generate the lookup tale. By default it will be generated.
void QgsContrastEnhancement::setMinimumValue(double theValue, bool generateTable)
{
  if(theValue < getMinimumPossibleValue(mQgsRasterDataType))
  {
    mMinimumValue = getMinimumPossibleValue(mQgsRasterDataType);
  }
  else
  {
    mMinimumValue = theValue; 
  }

  mEnhancementDirty = true;
  mMinimumMaximumRange = mMaximumValue - mMinimumValue;

  if(generateTable)
  {
    generateLookupTable();
  }
}

int QgsContrastEnhancement::stretch(double theValue)
{
  if(mEnhancementDirty)
  {
    generateLookupTable();
  }

  if(mLookupTable && NO_STRETCH != mContrastEnhancementAlgorithm)
  {
    return mLookupTable[static_cast <int>(theValue)];
  }
  else
  {
    return calculateContrastEnhancementValue(theValue);
  }
}
