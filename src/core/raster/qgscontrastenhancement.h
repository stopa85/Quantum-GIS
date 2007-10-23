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
 
 class CORE_EXPORT QgsContrastEnhancement {
 
 public:
 
    QgsContrastEnhancement();
 
    /** \brief This enumerator describes the types of contrast enhancement algorithms that can be used.  */
    enum CONTRAST_ENHANCEMENT_ALGORITHM
    {
        NO_STRETCH, //this should be the default color scaling algorithm, will allow for the display of images without calling QgsRasterBandStats unless needed
        STRETCH_TO_MINMAX, //linear histogram stretch
        STRETCH_AND_CLIP_TO_MINMAX,
        CLIP_TO_MINMAX
    } mContrastEnhancement;
    
 };
 
#endif
