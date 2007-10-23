class QgsContrastEnhancement
{
%TypeHeaderCode
#include <qgscontrastenhancement.h>
%End

public:

    enum CONTRAST_ENHANCEMENT_ALGORITHM
    {
        NO_STRETCH, //this should be the default color scaling algorithm, will allow for the display of images without calling QgsRasterBandStats unless needed
        STRETCH_TO_MINMAX, //linear histogram stretch
        STRETCH_AND_CLIP_TO_MINMAX,
        CLIP_TO_MINMAX
    }; 
    CONTRAST_ENHANCEMENT_ALGORITHM mContrastEnhancement;
    
};
