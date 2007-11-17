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
  mMinimumMaximumRange = mMaximumValue - mMinimumValue;
  mQgsRasterDataTypeRange = mMaximumValue - mMinimumValue;
  
  mLookupTableOffset = mMinimumValue * -1;
  
  //If the data type is larger than 16-bit do not generate a lookup table
  if(mQgsRasterDataTypeRange <= 65535.0)
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
    Simple function to compute the maximum possible value for a data types. 
*/
double QgsContrastEnhancement::getMaximumPossibleValue(QgsRasterDataType theDataType) 
{
  switch(theDataType)
  {
    case QGS_Byte:
      return std::numeric_limits<unsigned char>::max();
      break;
    case QGS_UInt16:
      return std::numeric_limits<unsigned short>::max();
      break;
    case QGS_Int16:
      return std::numeric_limits<short>::max();
      break;
    case QGS_UInt32:
      return std::numeric_limits<unsigned int>::max();
      break;
    case QGS_Int32:
      return std::numeric_limits<int>::max();
      break;
    case QGS_Float32:
      return std::numeric_limits<float>::max();
      break;
    case QGS_Float64:
    return std::numeric_limits<double>::max();
      break;
    case QGS_CInt16:
    return std::numeric_limits<short>::max();
      break;
    case QGS_CInt32:
    return std::numeric_limits<int>::max();
      break;
    case QGS_CFloat32:
    return std::numeric_limits<float>::max();
      break;
    case QGS_CFloat64:
    return std::numeric_limits<double>::max();
      break;
  }

  return std::numeric_limits<double>::max();
}
/** 
    Simple function to compute the minimum possible value for a data type. 
*/
double QgsContrastEnhancement::getMinimumPossibleValue(QgsRasterDataType theDataType) 
{
  switch(theDataType)
  {
    case QGS_Byte:
      return std::numeric_limits<unsigned char>::min();
      break;
    case QGS_UInt16:
      return std::numeric_limits<unsigned short>::min();
      break;
    case QGS_Int16:
      return std::numeric_limits<short>::min();
      break;
    case QGS_UInt32:
      return std::numeric_limits<unsigned int>::min();
      break;
    case QGS_Int32:
      return std::numeric_limits<int>::min();
      break;
    case QGS_Float32:
      return std::numeric_limits<float>::max() * -1.0;
      break;
    case QGS_Float64:
    return std::numeric_limits<double>::max() * -1.0;
      break;
    case QGS_CInt16:
    return std::numeric_limits<short>::min();
      break;
    case QGS_CInt32:
    return std::numeric_limits<int>::min();
      break;
    case QGS_CFloat32:
    return std::numeric_limits<float>::max() * -1.0;
      break;
    case QGS_CFloat64:
    return std::numeric_limits<double>::max() * -1.0;
      break;
  }

  return std::numeric_limits<double>::max() * -1.0;
}

/*
 *
 * Non-Static methods
 *
 */
 
/**
  This method calcualted the new value for a pixel based on the contrast
  enhancement algorithm. These case statements could be abstracted more
  so then a USER_DEFINED algorithm could be added and only a pointer to 
  the enhancement algorthm would be needed.
  
  @param theValue The pixel value to enhance
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
        return static_cast<int>((((theValue - getMinimumPossibleValue(mQgsRasterDataType))/(getMaximumPossibleValue(mQgsRasterDataType) - getMinimumPossibleValue(mQgsRasterDataType)))*255.0));
      }
      break;
    }
    case STRETCH_TO_MINMAX:
    {
      int myStretchedValue = static_cast<int>(((theValue - mMinimumValue)/(mMinimumMaximumRange))*255.0);
      if(myStretchedValue < getMinimumPossibleValue(mQgsRasterDataType))
      {
        return 0;
      }
      else if(myStretchedValue > getMaximumPossibleValue(mQgsRasterDataType))
      {
        return static_cast<int>(getMaximumPossibleValue(mQgsRasterDataType));
      }
      return myStretchedValue; 
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
        return static_cast<int>((((theValue - getMinimumPossibleValue(mQgsRasterDataType))/(getMaximumPossibleValue(mQgsRasterDataType) - getMinimumPossibleValue(mQgsRasterDataType)))*255.0));
      }
      break;
    }
    case STRETCH_AND_CLIP_TO_MINMAX:
    {
      if((theValue - mLookupTableOffset) < mMinimumValue || (theValue - mLookupTableOffset) > mMaximumValue)
      {
        return -1;
      }

      int myStretchedValue = static_cast<int>(((theValue - mMinimumValue)/(mMinimumMaximumRange))*255.0);
      if(myStretchedValue < getMinimumPossibleValue(mQgsRasterDataType))
      {
        return 0;
      }
      else if(myStretchedValue > getMaximumPossibleValue(mQgsRasterDataType))
      {
        return static_cast<int>(getMaximumPossibleValue(mQgsRasterDataType));
      }
      return myStretchedValue; 
      break;
    }
    case USER_DEFINED:
    {
      return 0;
    }
    default:
      return 0;
  }
}

/**
    Generate a new lookup table
*/
bool QgsContrastEnhancement::generateLookupTable()
{
  mEnhancementDirty = false;

  if(NO_STRETCH == mContrastEnhancementAlgorithm) { return false; }
  if(QGS_Byte != mQgsRasterDataType && QGS_UInt16 != mQgsRasterDataType && QGS_Int16 != mQgsRasterDataType) { return false; }
  if(!mLookupTable) { return false; }

  for(int myIterator = 0; myIterator < mQgsRasterDataTypeRange; myIterator++)
  {
    mLookupTable[myIterator] = calculateContrastEnhancementValue((double)myIterator - mLookupTableOffset);
  }

  return true;
}

/**
    Determine if a pixel is within in the displayable range.
    
    @param theValue The pixel value to examine
*/
bool QgsContrastEnhancement::isValueInDisplayableRange(double theValue)
{

  //If the contrast enancement has clipping, compare the provided value to the mininimum and maximum values set for the band
  if(CLIP_TO_MINMAX == mContrastEnhancementAlgorithm || STRETCH_AND_CLIP_TO_MINMAX == mContrastEnhancementAlgorithm)
  {
    if(theValue < mMinimumValue || theValue > mMaximumValue)
    {
      return false;
    }
  }

  //Also check to see if the provided value is with the range for the data type
  if(theValue < getMinimumPossibleValue(mQgsRasterDataType) || theValue > getMaximumPossibleValue(mQgsRasterDataType))
  {
    return false;
  }

  return true;
}

/**
    Set the contrast enhancement algorithm. The second parameter is optional and is for performace improvements. If you know you are immediately going to set the Minimum or Maximum value, you can elect to not generate the lookup tale. By default it will be generated.
    
    @param theAlgorithm The new contrast enhancement algorithm
    @param generateTable Flag to overide automatic look up table generation
*/
void QgsContrastEnhancement::setContrastEnhancementAlgorithm(CONTRAST_ENHANCEMENT_ALGORITHM theAlgorithm, bool generateTable)
{
  if(generateTable && theAlgorithm != mContrastEnhancementAlgorithm )
  {
    mContrastEnhancementAlgorithm = theAlgorithm;
    generateLookupTable();
  }
  else if(theAlgorithm != mContrastEnhancementAlgorithm)
  {
    mEnhancementDirty = true;
    mContrastEnhancementAlgorithm = theAlgorithm;
  }
}

/**
    Set the maximum value for the contrast enhancement. The second parameter is option an is for performace improvements. If you know you are immediately going to set the Minimum value or the contrast enhancement algorithm, you can elect to not generate the lookup tale. By default it will be generated.
    
    @param theValue The new maximum value for the band
    @param generateTable Flag to overide automatic look up table generation
*/
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

/**
    Set the maximum value for the contrast enhancement. The second parameter is option an is for performace improvements. If you know you are immediately going to set the Maximum value or the contrast enhancement algorithm, you can elect to not generate the lookup tale. By default it will be generated.
    
    @param theValue The new minimum value for the band
    @param generateTable Flag to overide automatic look up table generation
*/
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

/**
    Public function to generate the enhanced for stretched value for a given input.
    
    @param theValue The pixel value to enhance
*/
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
    // Even if the contrast enhancement algorithms is set to NO_STRETCH
    // The input values will still have to be scaled for all data types
    // greater than 1 byte.
    return calculateContrastEnhancementValue(theValue);
  }
}
