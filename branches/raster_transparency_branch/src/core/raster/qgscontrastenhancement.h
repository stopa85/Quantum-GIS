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

#include <limits>

class CORE_EXPORT QgsContrastEnhancement {
 
public:
    
  /** \brief This enumerator describes the types of contrast enhancement algorithms that can be used.  */
  enum CONTRAST_ENHANCEMENT_ALGORITHM
  {
    NO_STRETCH, //this should be the default color scaling algorithm
    STRETCH_TO_MINMAX, //linear histogram stretch
    STRETCH_AND_CLIP_TO_MINMAX,
    CLIP_TO_MINMAX
  };
    
  /*! These are exactly the same as GDAL pixel data types */
  typedef enum QgsRasterDataType {
    QGS_Unknown = 0,
    /*! Eight bit unsigned integer */           QGS_Byte = 1,
    /*! Sixteen bit unsigned integer */         QGS_UInt16 = 2,
    /*! Sixteen bit signed integer */           QGS_Int16 = 3,
    /*! Thirty two bit unsigned integer */      QGS_UInt32 = 4,
    /*! Thirty two bit signed integer */        QGS_Int32 = 5,
    /*! Thirty two bit floating point */        QGS_Float32 = 6,
    /*! Sixty four bit floating point */        QGS_Float64 = 7,
    /*! Complex Int16 */                        QGS_CInt16 = 8,
    /*! Complex Int32 */                        QGS_CInt32 = 9,
    /*! Complex Float32 */                      QGS_CFloat32 = 10,
    /*! Complex Float64 */                      QGS_CFloat64 = 11,
    QGS_TypeCount = 12          /* maximum type # + 1 */
  };
   
  QgsContrastEnhancement(QgsContrastEnhancement::QgsRasterDataType theDatatype=QGS_Byte);
  ~QgsContrastEnhancement();
  
  /*
   *
   * Static methods
   *
   */
  /** \brief Helper function that returns the maximum possible value for a GDAL data type */
  static double getMaximumPossibleValue(QgsRasterDataType);
  /** \brief Helper function that returns the minimum possible value for a GDAL data type */
  static double getMinimumPossibleValue(QgsRasterDataType);
 
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
  bool isValueInDisplayableRange(double);
  void setContrastEnhancementAlgorithm(CONTRAST_ENHANCEMENT_ALGORITHM, bool generatreTable=true);
  /** \brief Set the maximum value for the contrast enhancement range. */
  void setMaximumValue(double, bool generateTable=true);
  /** \brief Return the minimum value for the contrast enhancement range. */
  void setMinimumValue(double, bool generateTable=true);
  /** \brief Apply the contrast enhancement to a value. Return values are 0 - 254, -1 means the pixel was clipped and should not be displayed */
  int stretch(double);
  
private:
  CONTRAST_ENHANCEMENT_ALGORITHM mContrastEnhancementAlgorithm;
  QgsRasterDataType mQgsRasterDataType;
  double mQgsRasterDataTypeRange;
  bool mEnhancementDirty;

  double mMinimumValue;
  double mMaximumValue;
  double mMinimumMaximumRange;

  double mLookupTableOffset;
  int *mLookupTable;

  bool generateLookupTable();
  int calculateContrastEnhancementValue(double);
};
 
#endif
