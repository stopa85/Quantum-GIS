/***************************************************************************
                        qgsrasterlayer.h  -  description
                              -------------------
	begin                : Fri Jun 28 2002
	copyright            : (C) 2004 by T.Sutton, Gary E.Sherman, Steve Halasz
	email                : tim@linfiniti.com
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

/** \file qgsrasterlayer.h
 *  \brief This class provides qgis with the ability to render raster datasets
 *  onto the mapcanvas
 *
 *  The qgsrasterlayer class makes use of gdal for data io, and thus supports
 *  any gdal supported format. The constructor attemtps to infer what type of
 *  file (RASTER_LAYER_TYPE) is being opened - not in terms of the file format (tif, ascii grid etc.)
 *  but rather in terms of whether the image is a GRAYSCALE, PALETTED or MULTIBAND,
 *
 *  Within the three allowable raster layer types, there are 8 permutations of 
 *  how a layer can actually be rendered. These are defined in the DRAWING_STYLE enum
 *  and consist of:
 *
 *  SINGLE_BAND_GRAY -> a GRAYSCALE layer drawn as a range of gray colors (0-255)
 *  SINGLE_BAND_PSEUDO_COLOR -> a GRAYSCALE layer drawn using a pseudocolor algorithm
 *  PALETTED_SINGLE_BAND_GRAY -> a PALLETED layer drawn in gray scale (using only one of the color components)
 *  PALETTED_SINGLE_BAND_PSEUDO_COLOR -> a PALLETED layer having only one of its color components rendered as psuedo color
 *  PALETTED_MULTI_BAND_COLOR -> a PALLETED image where the bands contains 24bit color info and 8 bits is pulled out per color
 *  MULTI_BAND_SINGLE_BAND_GRAY -> a layer containing 2 or more bands, but using only one band to produce a grayscale image
 *  MULTI_BAND_SINGLE_BAND_PSEUDO_COLOR -> a layer containing 2 or more bands, but using only one band to produce a pseudocolor image
 *  MULTI_BAND_COLOR -> a layer containing 2 or more bands, mapped to the three RGBcolors. In the case of a multiband with only two bands, one band will have to be mapped to more than one color
 *
 *  Each of the above mentioned drawing styles is implemented in its own draw* function.
 *  Some of the drawing styles listed above require statistics about the layer such 
 *  as the min / max / mean / stddev etc. Statics for a band can be gathered using the 
 *  getRasterBandStats function. Note that statistics gathering is a slow process and 
 *  evey effort should be made to call this function as few times as possible. For this
 *  reason, qgsraster has a vector class member to store stats for each band. The 
 *  constructor initialises this vector on startup, but only populates the band name and
 *  number fields.
 *  
 *  Note that where bands are of gdal 'undefined' type, their values may exceed the 
 *  renderable range of 0-255. Because of this a linear scaling histogram stretch is
 *  applied to undefined layers to normalise the data into the 0-255 range.
 *
 *  A qgsrasterlayer band can be referred to either by name or by number (base=1). It
 *  should be noted that band names as stored in datafiles may not be uniqe, and 
 *  so the rasterlayer class appends the band number in brackets behind each band name.
 *  
 *  Sample useage of the QgsRasterLayer class:
 *
 *     QString myFileNameQString = "/path/to/file";
 *     QFileInfo myFileInfo(myFileNameQString);
 *     QString myBaseNameQString = myFileInfo.baseName();
 *     QgsRasterLayer *myRasterLayer = new QgsRasterLayer(myFileNameQString, myBaseNameQString);
 *
 *  In order to automate redrawing of a raster layer, you should like it to a map canvas like this :
 *  
 *     QObject::connect( myRasterLayer, SIGNAL(repaintRequested()), mapCanvas, SLOT(refresh()) );
 *
 *  A raster layer can also export its legend as a pixmap:
 *
 *     QPixmap myQPixmap = myRasterLayer->legendPixmap();
 *
 * Once a layer has been created you can find out what type of layer it is (GRAY_OR_UNDEFINED, PALETTE or MULTIBAND):
 *
 *    if (rasterLayer->getRasterLayerType()==QgsRasterLayer::MULTIBAND)
 *    {
 *      //do something
 *    }
 *    else if (rasterLayer->getRasterLayerType()==QgsRasterLayer::PALETTE)
 *    {
 *      //do something
 *    }
 *    else // QgsRasterLayer::GRAY_OR_UNDEFINED
 *    {
 *      //do something.
 *    }
 *
 * You can combine layer type detection with the setDrawingStyle method to override the default drawing style assigned
 * when a layer is loaded.:
 *
 *    if (rasterLayer->getRasterLayerType()==QgsRasterLayer::MULTIBAND)
 *    {
 *       myRasterLayer->setDrawingStyle(QgsRasterLayer::MULTI_BAND_SINGLE_BAND_PSEUDO_COLOR);
 *    }
 *    else if (rasterLayer->getRasterLayerType()==QgsRasterLayer::PALETTE)
 *    {
 *      myRasterLayer->setDrawingStyle(QgsRasterLayer::PALETTED_SINGLE_BAND_PSEUDO_COLOR);
 *    }
 *    else // QgsRasterLayer::GRAY_OR_UNDEFINED
 *    {
 *      myRasterLayer->setDrawingStyle(QgsRasterLayer::SINGLE_BAND_PSEUDO_COLOR);
 *    }
 * 
 *  Raster layers can also have an aribitary level of transparency defined, and have their
 *  colour palettes inverted using the setTransparency and setInvertHistogramFlag methods. 
 * 
 *  Pseudocolour images can have their output adjusted to a given number of standard
 *  deviations using the setStdDevsToPlot method.
 * 
 *  The final area of functionality you may be interested in is band mapping. Band mapping
 *  allows you to choose arbitary band -> colour mappings and is applicable only to PALETTE
 *  and MULTIBAND rasters, There are four mappings that can be made : red, green, blue and gray.
 *  Mappings are non exclusive. That is a given band can be assigned to no, some or all 
 *  colour mappings. The constructor sets sensible defaults for band mappings but these can be
 *  overridden at run time using the setRedBandName,setGreenBandName,setBlueBandName and setGrayBandName 
 *  methods.
 */
 
 
#ifndef QGSRASTERLAYER_H
#define QGSRASTERLAYER_H

//
// Includes
// 
 
#include <QColor>
#include <QDateTime>
#include <QVector>
#include <QList>


#include "qgspoint.h"
#include "qgsmaplayer.h"
#include "qgscontrastenhancement.h"

/*
 * 
 * New includes that will convert this class to a data provider interface
 * (B Morley)
 *
 */ 
 
#include "qgsrasterdataprovider.h"

/*
 * END
 */

#include <gdal_priv.h>

//
// Forward declarations
//
class QgsColorTable;
class QgsRect;
class QgsRasterBandStats;
class QgsRasterPyramid;
class QgsRasterLayerProperties;
struct QgsRasterViewPort;
class QImage;
class QPixmap;

class GDALDataset;
class GDALRasterBand;
class QSlider;
class QLibrary;


  
  
/*! \class QgsRasterLayer
 *  \brief This class provides qgis with the ability to render raster datasets
 *  onto the mapcanvas..
 */

class CORE_EXPORT QgsRasterLayer : public QgsMapLayer
{
    Q_OBJECT
public:

    //
    // Structs to hold transparent pixel vlaues
    //
    struct TransparentThreeValuePixel
    {
      double red;
      double green;
      double blue;
      double percentTransparent;
    };

    struct TransparentSingleValuePixel
    {
      double pixelValue;
      double percentTransparent;
    };

    //An entry for classification based upon value.
    //Such a classification is typically used for 
    //single band layers where a pixel value represents
    //not a color but a quantity, e.g. temperature or elevation
    struct ValueClassificationItem
    {
      QString label;
      double value;
      QColor color;
    };

    QList<struct TransparentThreeValuePixel> transparentThreeValuePixelList;
    QList<struct TransparentSingleValuePixel> transparentSingleValuePixelList;

    //
    // Static methods:
    //
    static void buildSupportedRasterFileFilter(QString & fileFilters);
    static bool isSupportedRasterDriver(const QString & driverName);
    static void registerGdalDrivers();

    /** This helper checks to see whether the filename appears to be a valid
       raster file name */
    static bool isValidRasterFileName(const QString & theFileNameQString);

    //
    // Non Static methods:
    //
    /** \brief This is the constructor for the RasterLayer class.
     *
     * The main tasks carried out by the constructor are:
     *
     * -Populate the RasterStatsVector with initial values for each band.
     *
     * -Calculate the layer extents
     *
     * -Determine whether the layer is gray, paletted or multiband.
     *
     * -Assign sensible defaults for the red,green, blue and gray bands.
     *
     * -
     * */
    QgsRasterLayer(const QString & path = QString::null, 
                   const QString &  baseName = QString::null);

    /** \brief The destuctor.  */
    ~QgsRasterLayer();

    /** \brief  A list containing one RasterBandStats struct per raster band in this raster layer.
     * Note that while very RasterBandStats element will have the name and number of its associated
     * band populated, any additional stats are calculated on a need to know basis.*/
    typedef QList<QgsRasterBandStats> RasterStatsList;


    /** \brief  A list containing one RasterPyramid struct per raster band in this raster layer.
     * POTENTIAL pyramid layer. How this works is we divide the height
     * and width of the raster by an incrementing number. As soon as the result
     * of the division is <=256 we stop allowing RasterPyramid stracuts
     * to be added to the list. Each time a RasterPyramid is created
     * we will check to see if a pyramid matching these dimensions already exists
     * in the raster layer, and if so mark the exists flag as true. */
      
    typedef QList<QgsRasterPyramid> RasterPyramidList;
    
    /** \brief A list containing on ContrastEnhancement object per raster band in this raster layer. */
    typedef QList<QgsContrastEnhancement> ContrastEnhancementList;

    /** \brief This typedef is used when the showProgress function is passed to gdal as a function
    pointer. */
    //  typedef  int (QgsRasterLayer::*showTextProgress)( double theProgress,
    //                                      const char *theMessageCharArray,
    //                                      void *theData);

    /** \brief Identify raster value(s) found on the point position 
     *
     * \param point[in]  a coordinate in the CRS of this layer.
     */
    void identify(const QgsPoint & point, std::map<QString,QString>& results);

    /** \brief Identify arbitrary details from the WMS server found on the point position
     *
     * \param point[in]  an image pixel coordinate in the last requested extent of layer.
     *
     * \return  A text document containing the return from the WMS server
     *
     * \note  The arbitraryness of the returned document is enforced by WMS standards
     *        up to at least v1.3.0
     */
    QString identifyAsText(const QgsPoint & point);

    /** \brief Query gdal to find out the WKT projection string for this layer. This implements the virtual method of the same name defined in QgsMapLayer*/
    QString getProjectionWKT();

    /** \brief Returns the number of raster units per each raster pixel. For rasters with world file, this is normally the first row (without the sign) in that file */
    double rasterUnitsPerPixel();

    /** \brief Draws a thumbnail of the rasterlayer into the supplied pixmap pointer */
     void drawThumbnail(QPixmap * theQPixmap);

    /** \brief Get an 8x8 pixmap of the colour palette. If the layer has no palette a white pixmap will be returned. */
     QPixmap getPaletteAsPixmap();
     
    /** \brief This is called when the view on the rasterlayer needs to be refreshed (redrawn).   
         
        \param drawingToEditingCanvas  Are we drawing to an editable canvas? 
                                       currently not used, but retain to be similar to 
                                       the QgsVectorLayer interface 
     */
    bool draw(QPainter * theQPainter,
              QgsRect & theViewExtent, 
              QgsMapToPixel * theQgsMapToPixel,
              QgsCoordinateTransform* ct,
              bool drawingToEditingCanvas);

    /** \brief This is an overloaded version of the above function that is called by both draw above and drawThumbnail */
    void draw(QPainter * theQPainter, QgsRasterViewPort * myRasterViewPort,
              QgsMapToPixel * theQgsMapToPixel = 0);
    
    //
    // Accessors for image height and width
    //
    /** \brief Accessor that returns the width of the (unclipped) raster  */
    const int getRasterXDim() {return mRasterXDim;};

    /** \brief Accessor that returns the height of the (unclipped) raster  */
    const int getRasterYDim() {return mRasterYDim;};

    //
    // Accessor and mutator for no data double
    //
    /** \brief  Accessor that returns the NO_DATA entry for this raster. */
    const double getNoDataValue() {return mNoDataValue;}

    /** \brief  Mutator that allows the  NO_DATA entry for this raster to be overridden. */
    void setNoDataValue(double theNoData) { mNoDataValue=theNoData; return;};

    /** \brief Simple reset function that set the noDataValue back to the value stored in the first raster band */
    void resetNoDataValue()
    {
      mNoDataValue = -9999;
      if(mGdalDataset != NULL && mGdalDataset->GetRasterCount() > 0)
      {
        int isValid = false;
        double myValue = mGdalDataset->GetRasterBand(1)->GetNoDataValue(&isValid);
        if(isValid)
        {
          mNoDataValue = myValue;
        }
      }
    }
    //
    // Accessor and mutator for mInvertPixelsFlag
    //
    /** \brief Accessor to find out whether the histogram should be inverted.   */
    bool getInvertHistogramFlag()
    {
        return mInvertPixelsFlag;
    }
    /** \brief Mutator to alter the state of the invert histogram flag.  */
    void setInvertHistogramFlag(bool theFlag)
    {
        mInvertPixelsFlag=theFlag;
    }
    //
    // Accessor and mutator for mStandardDeviations
    //
    /** \brief Accessor to find out how many standard deviations are being plotted.  */
    double getStdDevsToPlot()
    {
        return mStandardDeviations;
    };
    /** \brief Mutator to alter the number of standard deviations that should be plotted.  */
    void setStdDevsToPlot(double theStdDevsToPlot)
    {
        mStandardDeviations = theStdDevsToPlot;
    };
    /** \brief Get the number of bands in this layer  */
    const unsigned int getBandCount();
    /** \brief Get RasterBandStats for a band given its number (read only)  */
    const  QgsRasterBandStats getRasterBandStats(int);
    /** \brief  Check whether a given band number has stats associated with it */
    const bool hasStats(int theBandNoInt);
    /** \brief Overloaded method that also returns stats for a band, but uses the band colour name
    *    Note this approach is not recommeneded because it is possible for two gdal raster
    *    bands to have the same name!
    */
    const  QgsRasterBandStats getRasterBandStats(const QString &);
    /** \brief Get the number of a band given its name. Note this will be the rewritten name set 
    *   up in the constructor, and will not necessarily be the same as the name retrieved directly from gdal!
    *   If no matching band is found zero will be returned! */
    const  int getRasterBandNumber (const QString & theBandNameQString);
    /** \brief Get the name of a band given its number.  */
    const  QString getRasterBandName(int theBandNoInt);
    /** \brief Find out whether a given band exists.    */
    bool hasBand(const QString &  theBandName);
    /** \brief Call any inline image manipulation filters */
    void filterLayer(QImage * theQImage);
    /** \brief Accessor for red band name (allows alternate mappings e.g. map blue as red colour). */
    QString getRedBandName()
    {
        return mRedBandName;
    };
    /** \brief Mutator for red band name (allows alternate mappings e.g. map blue as red colour). */
    void setRedBandName(const QString & theBandNameQString);
    // 
    // Accessor and mutator for green band name
    // 
    /** \brief Accessor for green band name mapping.  */
    QString getGreenBandName()
    {
        return mGreenBandName;
    };
    /** \brief Mutator for green band name mapping.  */
    void setGreenBandName(const QString & theBandNameQString);
    //
    // Accessor and mutator for blue band name
    // 
    /** \brief  Accessor for blue band name mapping. */
    QString getBlueBandName()
    {
        return mBlueBandName;
    };
    /** \brief Mutator for blue band name mapping.  */
    void setBlueBandName(const QString & theBandNameQString);
    //
    // Accessor and mutator for transparent band name
    // 
    /** \brief  Accessor for transparent band name mapping. */
    QString getTransparentBandName()
    {
        return mTransparencyBandName;
    };
    /** \brief Mutator for transparent band name mapping.  */
    void setTransparentBandName(const QString & theBandNameQString);
    //
    // Accessor and mutator for transparent band name
    // 
    /** \brief  Accessor for transparent band name mapping. */
    QString getTransparentLayerName()
    {
        return mTransparentLayerName;
    };
    /** \brief Mutator for transparent band name mapping.  */
    void setTransparentLayerName(const QString & theLayerNameQString)
    {
      mTransparentLayerName = theLayerNameQString;
    }
    //
    // Accessor and mutator for gray band name
    //
    /** \brief Accessor for gray band name mapping.  */
    QString getGrayBandName()
    {
        return mGrayBandName;
    };
    /** \brief Mutator for gray band name mapping.  */
    void setGrayBandName(const QString & theBandNameQString);
    // 
    // Accessor and mutator for mDebugOverlayFlag
    // 
    /** \brief Accessor for a flag that determines whether to show some debug info on the image.  */
    bool getShowDebugOverlayFlag()
    {
        return mDebugOverlayFlag;
    };
    /** \brief Mutator for a flag that determines whether to show some debug info on the image.  */
    void setShowDebugOverlayFlag(bool theFlag)
    {
        mDebugOverlayFlag=theFlag;
    };
    
    
    // Accessor and mutator for minimum maximum values 
    //TODO: Move these out of the header file...
    double getMinimumValue(unsigned int theBand) 
    { 
      if(0 < theBand && theBand <= getBandCount()) 
      { 
        return mContrastEnhancementList[theBand - 1].getMinimumValue(); 
      }
      
      return 0.0;
    }
    
    double getMinimumValue(QString theBand)
    { 
      return getMinimumValue(getRasterBandNumber(theBand));
    }
    
    void setMinimumValue(unsigned int theBand, double theValue, bool theGenerateLookupTableFlag=true)
    { 
      if(0 < theBand && theBand <= getBandCount())
      { 
        mContrastEnhancementList[theBand - 1].setMinimumValue(theValue, theGenerateLookupTableFlag);
      } 
    }
    
    void setMinimumValue(QString theBand, double theValue, bool theGenerateLookupTableFlag=true)
    { 
      if(theBand != tr("Not Set"))
      {
        setMinimumValue(getRasterBandNumber(theBand),theValue, theGenerateLookupTableFlag);
      }
      
    }
    
    double getMaximumValue(unsigned int theBand)
    {
      if(0 < theBand && theBand <= getBandCount())
      { 
        return mContrastEnhancementList[theBand - 1].getMaximumValue();
      } 
      
      return 0.0;
    }
    
    double getMaximumValue(QString theBand)
    { 
      if(theBand != tr("Not Set"))
      {
        return getMaximumValue(getRasterBandNumber(theBand)); 
      }
      
      return 0.0;
    }
    
    void setMaximumValue(unsigned int theBand, double theValue, bool theGenerateLookupTableFlag=true)
    { 
      if(0 < theBand && theBand <= getBandCount()) 
      { 
        mContrastEnhancementList[theBand - 1].setMaximumValue(theValue, theGenerateLookupTableFlag); 
      } 
    }
    
    void setMaximumValue(QString theBand, double theValue, bool theGenerateLookupTableFlag=true) 
    { 
      if(theBand != tr("Not Set"))
      {
        setMaximumValue(getRasterBandNumber(theBand),theValue, theGenerateLookupTableFlag);
      }
    }
    
    QgsContrastEnhancement* getContrastEnhancement(unsigned int theBand)
    { 
      return &mContrastEnhancementList[theBand - 1]; 
    }

    //
    // Accessor and mutator for the contrast enhancement algorithm
    //
    QgsContrastEnhancement::CONTRAST_ENHANCEMENT_ALGORITHM mContrastEnhancementAlgorithm;
    /** \brief Accessor for contrast enhancement algorithm. */
    QgsContrastEnhancement::CONTRAST_ENHANCEMENT_ALGORITHM getContrastEnhancementAlgorithm()
    {
        return mContrastEnhancementAlgorithm;
    };
    /** \brief Mutator for contrast enhancement algorithm. */
    void setContrastEnhancementAlgorithm(QgsContrastEnhancement::CONTRAST_ENHANCEMENT_ALGORITHM theAlgorithm, bool theGenerateLookupTableFlag=true)
    {
        QList<QgsContrastEnhancement>::iterator myIterator = mContrastEnhancementList.begin();
        while(myIterator !=  mContrastEnhancementList.end())
        {
          (*myIterator).setContrastEnhancementAlgorithm(theAlgorithm, theGenerateLookupTableFlag);
          ++myIterator;
        }

        mContrastEnhancementAlgorithm = theAlgorithm;
    };
    
    /** \brief This enumerator describes the types of histogram colour ramping that can be used.  */
    enum COLOR_RAMPING_TYPE
    {
        BLUE_GREEN_RED, 
        FREAK_OUT //it will scare your granny!
    } colorRampingType;
    //
    // Accessor and mutator for the color ramping type
    //
    /** \brief Accessor for colour ramping type. */
    COLOR_RAMPING_TYPE getColorRampingType()
    {
        return colorRampingType;
    };
    /** \brief Mutator for color scaling algorithm. */
    void setColorRampingType(COLOR_RAMPING_TYPE theRamping)
    {
        colorRampingType=theRamping;
    };
    
    /** \brief This enumerator describes the different kinds of drawing we can do.  */
    enum DRAWING_STYLE
    {
        SINGLE_BAND_GRAY, // a "Gray" or "Undefined" layer drawn as a range of gray colors
        SINGLE_BAND_PSEUDO_COLOR,// a "Gray" or "Undefined" layer drawn using a pseudocolor algorithm
        PALETTED_COLOR, //a "Palette" image drawn using color table
        PALETTED_SINGLE_BAND_GRAY,// a "Palette" layer drawn in gray scale (using only one of the color components)
        PALETTED_SINGLE_BAND_PSEUDO_COLOR, // a "Palette" layer having only one of its color components rendered as psuedo color
        PALETTED_MULTI_BAND_COLOR, // a "Palette" image is decomposed to 3 channels (RGB) and drawn 
	                           // as multiband 
        MULTI_BAND_SINGLE_BAND_GRAY, // a layer containing 2 or more bands, but using only one band to produce a grayscale image
        MULTI_BAND_SINGLE_BAND_PSEUDO_COLOR, //a layer containing 2 or more bands, but using only one band to produce a pseudocolor image
        MULTI_BAND_COLOR //a layer containing 2 or more bands, mapped to the three RGBcolors. In the case of a multiband with only two bands, one band will have to be mapped to more than one color
    } drawingStyle;    
    //
    // Accessor and mutator for drawing style.
    //
    /** \brief Accessor for drawing style.  */
    DRAWING_STYLE getDrawingStyle() {return drawingStyle;};
    /** \brief Returns a string representation of drawing style.
     *
     * Implementaed mainly for serialisation / deserialisation of settings to xml.
     * NOTE: May be deprecated in the future!. Use alternate implementation above rather.
     * */
    QString getDrawingStyleAsQString();
    /** \brief Mutator for drawing style.  */
    void setDrawingStyle(const DRAWING_STYLE &  theDrawingStyle) {drawingStyle=theDrawingStyle;};
    /** \brief Overloaded version of the above function for convenience when restoring from xml.
     *
     * Implementaed mainly for serialisation / deserialisation of settings to xml.
     * NOTE: May be deprecated in the future! Use alternate implementation above rather.
     * */
    void setDrawingStyle(const QString & theDrawingStyleQString);

    /** \brief This enumerator describes the type of raster layer.  */
    enum RASTER_LAYER_TYPE
    {
      GRAY_OR_UNDEFINED,
      PALETTE,
      MULTIBAND    
    } rasterLayerType;
    //
    //accessor and for raster layer type (READ ONLY)
    //
    /** \brief  Accessor for raster layer type (which is a read only property) */
    RASTER_LAYER_TYPE getRasterLayerType() { return rasterLayerType; };
    /** \brief Accessor for hasPyramidsFlag (READ ONLY) */
    bool getHasPyramidsFlag() {return hasPyramidsFlag;};
     
    /** \brief Get a legend image for this layer.  */
    QPixmap getLegendQPixmap();
    /** \brief  Overloaded version of above function that can print layer name onto legend. */
    QPixmap getLegendQPixmap(bool); 
    
    /** \brief Use this method when you want an annotated legend suitable for print output etc. 
     * @param int theLabelCountInt Number of vertical labels to display (defaults to 3)
     * */
    QPixmap getDetailedLegendQPixmap(int theLabelCount);
    
    /**
     * Returns the sublayers of this layer
     *
     * (Useful for providers that manage their own layers, such as WMS)
     *
     */
    QStringList subLayers() const;
    
    /**
     * Reorders the *previously selected* sublayers of this layer from bottom to top
     *
     * (Useful for providers that manage their own layers, such as WMS)
     *
     */
    virtual void setLayerOrder(const QStringList & layers);
    
    /**
     * Set the visibility of the given sublayer name
     */
    virtual void setSubLayerVisibility(const QString & name, bool vis);

    /** \brief Emit a signal asking for a repaint. (inherited from maplayer) */
    void triggerRepaint();
    /** \brief Obtain GDAL Metadata for this layer */
    QString getMetadata(); 
    /** \brief Accessor for ths raster layers pyramid list. A pyramid list defines the 
     * POTENTIAL pyramids that can be in a raster. To know which of the pyramid layers 
     * ACTUALLY exists you need to look at the existsFlag member in each struct stored in the 
     * list.*/
    RasterPyramidList buildRasterPyramidList();
    /** \brief Helper method to retrieve the nth pyramid layer struct from the PyramidList. 
     * If the nth layer does not exist, NULL will be returned. */
//   RasterPyramid getRasterPyramid(int thePyramidNo);

    /**Currently returns always false*/
    bool isEditable() const;
    
    /** Return time stamp for given file name */
    static QDateTime lastModified ( const QString &  name );

    /**Copies the symbology settings from another layer. Returns true in case of success*/
    bool copySymbologySettings(const QgsMapLayer& other) {
      //preventwarnings
      if (other.type() < 0) 
      {
        return false;
      }
      return false;
    } //todo

    bool isSymbologyCompatible(const QgsMapLayer& other) const 
    {
      other.type(); //just added to reduce the compiler warnings about unused variables, remove when actually implemented
      return false;
    } //todo

    /**
     * If an operation returns 0 (e.g. draw()), this function
     * returns the text of the error associated with the failure.
     * Interactive users of this provider can then, for example,
     * call a QMessageBox to display the contents.
     */
    QString errorCaptionString();
  
    /**
     * If an operation returns 0 (e.g. draw()), this function
     * returns the text of the error associated with the failure.
     * Interactive users of this provider can then, for example,
     * call a QMessageBox to display the contents.
     */
    QString errorString();

    /** Returns the data provider
     *
     *  \retval 0 if not using the data provider model (i.e. directly using GDAL)
     */
    QgsRasterDataProvider* getDataProvider();

    /** Returns the data provider in a const-correct manner
     *
     *  \retval 0 if not using the data provider model (i.e. directly using GDAL)
     */
    const QgsRasterDataProvider* getDataProvider() const;

     /** \brief Mutator for mUserDefinedRGBMinMaxFlag */
    void setUserDefinedColorMinMax(bool theBool)
    {
      mUserDefinedRGBMinMaxFlag = theBool;
    } 

    /** \brief Accessor for userDefinedMinMax.  */
    bool getUserDefinedColorMinMax()
    {
      return mUserDefinedRGBMinMaxFlag;
    }

    /** \brief Mutator for mUserDefinedRGBMinMaxFlag */
    void setUserDefinedGrayMinMax(bool theBool)
    {
      mUserDefinedGrayMinMaxFlag = theBool;
    } 

    /** \brief Accessor for userDefinedMinMax.  */
    bool getUserDefinedGrayMinMax()
    {
      return mUserDefinedGrayMinMaxFlag;
    }

    //
    // Accessor and mutator for transparency tables.
    //
    /** \brief Mutator for transparentThreeValuePixelList */
    QList<struct TransparentThreeValuePixel> getTransparentThreeValuePixelList()
    {
      return transparentThreeValuePixelList;
    }
    /** \brief Accessor for transparentThreeValuePixelList */
    void setTransparentThreeValuePixelList(QList<struct TransparentThreeValuePixel> newList)
    {
      transparentThreeValuePixelList = newList;
    }
    /** \brief Mutator for transparentSingleValuePixelList */
    QList<struct TransparentSingleValuePixel> getTransparentSingleValuePixelList()
    {
      return transparentSingleValuePixelList;
    }
    /** \brief Accessor for transparentSingleValuePixelList */
    void setTransparentSingleValuePixelList(QList<struct TransparentSingleValuePixel> newList)
    {
      transparentSingleValuePixelList = newList;
    }

    /**Get state of custom classification flag*/
    bool customClassificationEnabled() const {return mCustomClassificationEnabled;}
    /**Set state of custom classification flag*/
    void setCustomClassificationEnabled(bool enabled){mCustomClassificationEnabled = enabled;}
    /**Get custom colormap classification*/
    QList<ValueClassificationItem> valueClassification() const {return mValueClassification;}
    /**Set custom colormap classification*/
    void setValueClassification(const QList<ValueClassificationItem>& classification)
    {mValueClassification = classification;}
    /**Get discrete colors/ interpolated colors for custom classification*/
    bool discreteClassification() const {return mDiscreteClassification;}
    /**Set discrete colors/ interpolated colors for custom classification*/
    void setDiscreteClassification(bool discrete)
    {mDiscreteClassification = discrete;}

public slots:    
    /**
     * Convert this raster to another format
     */
    //void const convertTo();
    /**
     * Mainly inteded for use in propogating progress updates from gdal up to the parent app.
     **/
    void updateProgress(int,int);

    /** \brief Create  gdal pyramid overviews  for this layer.
    * This will speed up performance at the expense of hard drive space.
    * Also, write access to the file is required. If no paramter is passed in
    * it will default to nearest neighbor resampling.
    * \return null string on success, otherwise a string specifying error
    */
    QString buildPyramids(const RasterPyramidList &, 
                          const QString &  theResamplingMethod="NEAREST");
    /** \brief Used at the moment by the above function but hopefully will later
    be useable by any operation that needs to notify the user of its progress. */
/*
    int showTextProgress( double theProgress,
                          const char *theMessageCharArray,
                          void *theData);    
*/

  /** Populate the histogram vector for a given layer
  * @param theBandNoInt - which band to compute the histogram for
  * @param theBinCountInt - how many 'bins' to categorise the data into
  * @param theIgnoreOutOfRangeFlag - whether to ignore values that are out of range (default=true)
  * @param theThoroughBandScanFlag - whether to visit each cell when computing the histogram (default=false)
  */
  void populateHistogram(int theBandNoInt, 
                         int theBinCountInt=256,
                         bool theIgnoreOutOfRangeFlag=true,
                         bool theThoroughBandScanFlag=false);

    /** \brief Color table 
     *  \param band number
     *  \return pointer to color table
     */
    QgsColorTable *colorTable ( int theBandNoInt );
 protected:

    /** reads vector layer specific state from project file DOM node.

        @note

        Called by QgsMapLayer::readXML().

    */
    /* virtual */ bool readXML_( QDomNode & layer_node );



  /** write vector layer specific state to project file DOM node.

      @note

      Called by QgsMapLayer::writeXML().

  */
  /* virtual */ bool writeXML_( QDomNode & layer_node, QDomDocument & doc );
    
private:

    //
    // Private methods
    //
    /** \brief Paint debug information onto the output image.  */
    void showDebugOverlay(QPainter * theQPainter, QgsRasterViewPort * theRasterViewPort);

    //
    // Grayscale Imagery
    //

    /** \brief Drawing routine for single band grayscale image.  */
    void drawSingleBandGray(QPainter * theQPainter, 
                            QgsRasterViewPort * theRasterViewPort,
                            QgsMapToPixel * theQgsMapToPixel,
                            int theBandNoInt);

    /** \brief Drawing routine for single band grayscale image, rendered in pseudocolor.  */
    void drawSingleBandPseudoColor(QPainter * theQPainter, 
                                   QgsRasterViewPort * theRasterViewPort,
                                   QgsMapToPixel * theQgsMapToPixel,
                                   int theBandNoInt);


    //
    // Paletted Layers
    //
    
    /** \brief Drawing routine for paletted image, rendered as a single band image in color.  */
    void drawPalettedSingleBandColor(QPainter * theQPainter,
                                     QgsRasterViewPort * theRasterViewPort,
                                     QgsMapToPixel * theQgsMapToPixel,
                                     int theBandNoInt);
    
    /** \brief Drawing routine for paletted image, rendered as a single band image in grayscale.  */
    void drawPalettedSingleBandGray(QPainter * theQPainter,
                                    QgsRasterViewPort * theRasterViewPort,
                                    QgsMapToPixel * theQgsMapToPixel,
                                    int theBandNoInt,
                                    const QString &  theColorQString);

    /** \brief Drawing routine for paletted image, rendered as a single band image in pseudocolor.  */
    void drawPalettedSingleBandPseudoColor(QPainter * theQPainter,
                                           QgsRasterViewPort * theRasterViewPort,
                                           QgsMapToPixel * theQgsMapToPixel,
                                           int theBandNoInt,
                                           const QString &  theColorQString);

    /** \brief Drawing routine for paletted multiband image.  */
    void drawPalettedMultiBandColor(QPainter * theQPainter,
                                    QgsRasterViewPort * theRasterViewPort,
                                    QgsMapToPixel * theQgsMapToPixel,                                
                                    int theBandNoInt);

    //
    // Multiband Layers
    //
    
    /** \brief Drawing routine for multiband image, rendered as a single band image in grayscale.  */
    void drawMultiBandSingleBandGray(QPainter * theQPainter,
                                     QgsRasterViewPort * theRasterViewPort, 
                                     QgsMapToPixel * theQgsMapToPixel,
                                     int theBandNoInt);

    /** \brief Drawing routine for multiband image, rendered as a single band image in pseudocolor.  */
    void drawMultiBandSingleBandPseudoColor(QPainter * theQPainter, 
                                            QgsRasterViewPort * theRasterViewPort, 
                                            QgsMapToPixel * theQgsMapToPixel,
                                            int theBandNoInt);

    /** \brief Drawing routine for multiband image  */
    void drawMultiBandColor(QPainter * theQPainter, 
                            QgsRasterViewPort * theRasterViewPort,
                            QgsMapToPixel * theQgsMapToPixel);

    /** \brief Read color table from GDAL raster band */
    void readColorTable ( GDALRasterBand *gdalBand, QgsColorTable *theColorTable );

    /** \brief Allocate memory and load data to that allocated memory, data type is the same
     *         as raster band. The memory must be released later!
     *  \return pointer to the memory
     */
    void *readData ( GDALRasterBand *gdalBand, QgsRasterViewPort *viewPort );

    /** \brief Read a raster value on given position from memory block created by readData() 
     *  \param index index in memory block
     */
    inline double readValue ( void *data, GDALDataType type, int index );


    /**
       Load the given raster file

       @returns true if successfully read file

       @note
       
       Called from ctor if a raster image given there
     */
    bool readFile( const QString & fileName );
    
    /** \brief Close data set and release related data */
    void closeDataset ();

    /** \brief Update the layer if it is outdated */
    bool update ();

    /**Gets the color for a pixel value from the classification vector mValueClassification. Assigns the 
color of the lower class for every pixel between two class breaks. Returns 0 in case of success*/
    int getDiscreteColorFromValueClassification(double value, int& red, int& green, int& blue) const;
    /**Gets the color for a pixel value from the classification vector mValueClassification. Interpolates the color between two class breaks linearly. Returns 0 in case of success*/
    int getInterpolatedColorFromValueClassification(double value, int& red, int& green, int& blue) const;

    //
    // Private member vars
    //
    /** \brief  Raster width. */
    int mRasterXDim;
    /** \brief  Raster Height. */
    int mRasterYDim;
    /** \brief Cell value representing no data. e.g. -9999  */
    double mNoDataValue;
    /** \brief Flag to indicate whether debug infor overlay should be rendered onto the raster.  */
    bool mDebugOverlayFlag;
    /** \brief Pointer to the gdaldataset.  */
    GDALDataset * mGdalDataset;
    /** \brief Values for mapping pixel to world coordinates. Contents of
     * this array are the same as the gdal adfGeoTransform */
    double mGeoTransform[6];
    /** \brief Flag indicating whether the colour of pixels should be inverted or not.  */
    bool mInvertPixelsFlag;
    /** \brief Number of stddev to plot (0) to ignore. Not applicable to all layer types.  */
    double mStandardDeviations;
    /** \brief A collection of stats - one for each band in the layer.
     * The typedef for this is defined above before class declaration
     */
    RasterStatsList mRasterStatsList;
    /** \brief List containging the contrast enhancements for each band */
    ContrastEnhancementList mContrastEnhancementList;
    /** \brief The band to be associated with the color red - usually 1.  */
    QString mRedBandName;
    /** \brief The band to be associated with the color green - usually 2.  */
    QString mGreenBandName;
    /** \brief The band to be associated with the color blue - usually 3.  */
    QString mBlueBandName;
    /** \brief The band to be associated with transparency.  */
    QString mTransparencyBandName;
    /** \brief The Layer to be associated with transparency.  */
    QString mTransparentLayerName;
    /** \brief The band to be associated with the grayscale only ouput - usually 1.  */
    QString mGrayBandName;
    /** \brief Minimum red value - used in scaling procedure.  */
    double mRedMinimum;
    /** \brief Maximum red value - used in scaling procedure.  */
    double mRedMaximum;
    /** \brief Minimum green value - used in scaling procedure.  */
    double mGreenMinimum;
    /** \brief Maximum green value - used in scaling procedure.  */
    double mGreenMaximum;
    /** \brief Minimum blue value - used in scaling procedure.  */
    double mBlueMinimum;
    /** \brief Maximum blue value - used in scaling procedure.  */
    double mBlueMaximum;
    /** \brief Minimum gray value - used in scaling procedure.  */
    double mGrayMinimum;
    /** \brief Maximum gray value - used in scaling procedure.  */
    double mGrayMaximum;
    /** \brief Whether this raster has overviews / pyramids or not */
    bool hasPyramidsFlag;
    //Since QgsRasterBandStats deos not set the mRedMinimum mRedMaximum etc., it is benificial to know if the user as set these values. Default = false
    bool mUserDefinedRGBMinMaxFlag;
    bool mUserDefinedGrayMinMaxFlag;
    /** \brief This list holds a series of RasterPyramid structs
     * which store infomation for each potential pyramid level for this raster.*/
    RasterPyramidList mPyramidList;
    /**This flag holds if custom classification is enabled or not*/
    bool mCustomClassificationEnabled;
    /**This vector holds the information for classification based on values. 
Each item holds a value, a label and a color. The member mDiscreteClassification holds 
if one color is applied for all values between two class breaks (true) or if the item values are 
(linearly) interpolated for values between the item values (false)*/
    QList<ValueClassificationItem> mValueClassification;
    /**This member holds if one color is applied for all values between two class breaks (true) or if the item values are (linearly) interpolated for values between the item values (false)*/
    bool mDiscreteClassification;


/*
 * 
 * New functions that will convert this class to a data provider interface
 * (B Morley)
 *
 */ 
 
public:
 
  //! Constructor in provider mode
  // TODO Rename into a general constructor when the old raster interface is retired
  // \param  dummy  is just there to distinguish this function signature from the old non-provider one.
  QgsRasterLayer(int dummy, 
                 const QString & baseName = QString(),
                 const QString & path = QString(),
                 const QString & providerLib = QString(),
                 const QStringList & layers = QStringList(),
                 const QStringList & styles = QStringList(),
                 const QString & format = QString(),
                 const QString & crs = QString(),
                 const QString & proxyHost = QString(),
                 int proxyPort = 80,
                 const QString & proxyUser = QString(),
                 const QString & proxyPass = QString());

  void setDataProvider( const QString & provider,
                        const QStringList & layers,
                        const QStringList & styles,
                        const QString & format,
                        const QString & crs,
                        const QString & proxyHost,
                        int proxyPort,
                        const QString & proxyUser,
                        const QString & proxyPass );

  //! Does this layer use a provider for setting/retrieving data?
  bool usesProvider();

  /**
   * Sets a proxy for the path given in the constructor
   *
   * \retval TRUE if proxy setting is successful (if indeed it is supported)
   */
  bool setProxy(const QString & host = 0,
                            int port = 80,
                const QString & user = 0,
                const QString & pass = 0);

  //! Which provider is being used for this Raster Layer?
  QString providerKey();

public slots:

  void showStatusMessage(const QString & theMessage);


private:
  
  //! Data provider key
  QString mProviderKey;
  
  //! pointer for loading the provider library
  QLibrary *mLib;

  //! Pointer to data provider derived from the abstract base class QgsDataProvider
  QgsRasterDataProvider *mDataProvider;

  /**Flag indicating wheter the layer is in editing mode or not*/
  bool mEditable;
  
  /**Flag indicating wheter the layer has been modified since the last commit*/
  bool mModified;

  //! Timestamp, the last modified time of the data source when the layer was created
  QDateTime mLastModified;

  /**
   * The error caption associated with the last error.
   */
  QString mErrorCaption;

  /**
   * The error message associated with the last error.
   */
  QString mError;

};

#endif
