/* **************************************************************************
             qgscontrastenhancement.h -  description
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
 
#ifndef QGSCONTRASTENHANCEMENT_H
#define QGSCONTRASTENHANCEMENT_H
 
#include <gdal_priv.h>

class CORE_EXPORT QgsContrastEnhancement {
 
public:
 
  QgsContrastEnhancement(GDALDataType theDatatype=GDT_Byte);
  ~QgsContrastEnhancement();
    
  /** \brief This enumerator describes the types of contrast enhancement algorithms that can be used.  */
  enum CONTRAST_ENHANCEMENT_ALGORITHM
  {
    NO_STRETCH, //this should be the default color scaling algorithm
    STRETCH_TO_MINMAX, //linear histogram stretch
    STRETCH_AND_CLIP_TO_MINMAX,
    CLIP_TO_MINMAX
  };
    
  /*
   *
   * Static methods
   *
   */
  /** \brief Helper function that returns the maximum possible value for a GDAL data type */
  static double getMaximumPossibleValue(GDALDataType);
  /** \brief Helper function that returns the minimum possible value for a GDAL data type */
  static double getMinimumPossibleValue(GDALDataType);
 
  /*
   *
   * Non-Static Inline methods
   *
   */
  /** \brief Return the maximum value for the contrast enhancement range. */
  double getMaximumValue() { return mMaximumValue; }
     
  /** \brief Return the minimum value for the contrast enhancement range. */
  double getMinimumValue() { return mMinimumValue; }
  
  CONTRAST_ENHANCEMENT_ALGORITHM getContrastEnhancementAlgorithm() { return mContrastEnhancementAlgorithm; }
  
  /*
   *
   * Non-Static methods
   *
   */  
  /** \brief Return true if pixel is in stretable range, false if pixel is outside of range (i.e., clipped) */
  bool isValueInDisplyableRange(double);
  void setContrastEnhancementAlgorithm(CONTRAST_ENHANCEMENT_ALGORITHM);
  /** \brief Set the maximum value for the contrast enhancement range. */
  void setMaximumValue(double, bool generateTable=true);
  /** \brief Return the minimum value for the contrast enhancement range. */
  void setMinimumValue(double, bool generateTable=true);
  /** \brief Apply the contrast enhancement to a value. Return values are 0 - 254, -1 means the pixel was clipped and should not be displayed */
  int stretch(double);
  
private:
  CONTRAST_ENHANCEMENT_ALGORITHM mContrastEnhancementAlgorithm;
  GDALDataType mGDALDataType;
  double mGDALDataTypeRange;
  
  double mMinimumValue;
  double mMaximumValue;
  double mMinimumMaximumRange;
    
  double mLookupTableOffset;
  int *mLookupTable;
    
  bool generateLookupTable();
  int calculateContrastEnhancementValue(double);
};
 
#endif
