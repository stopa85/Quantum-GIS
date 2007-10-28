
class QgsContrastEnhancement
{
%TypeHeaderCode
#include <qgscontrastenhancement.h>
#include <gdal.h>
%End

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
  double getMaximumValue();
     
  /** \brief Return the minimum value for the contrast enhancement range. */
  double getMinimumValue();
  
  CONTRAST_ENHANCEMENT_ALGORITHM getContrastEnhancementAlgorithm();
  /*
   *
   * Non-Static methods
   *
   */  
  void setContrastEnhancementAlgorithm(CONTRAST_ENHANCEMENT_ALGORITHM);
  /** \brief Set the maximum value for the contrast enhancement range. */
  void setMaximumValue(double, bool);
  /** \brief Return the minimum value for the contrast enhancement range. */
  void setMinimumValue(double, bool);
    
   
};
