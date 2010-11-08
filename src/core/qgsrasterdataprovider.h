/***************************************************************************
    qgsrasterdataprovider.h - DataProvider Interface for raster layers
     --------------------------------------
    Date                 : Mar 11, 2005
    Copyright            : (C) 2005 by Brendan Morley
    email                : morb at ozemail dot com dot au
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

/* Thank you to Marco Hugentobler for the original vector DataProvider */

#ifndef QGSRASTERDATAPROVIDER_H
#define QGSRASTERDATAPROVIDER_H

#include "qgslogger.h"
#include "qgsdataprovider.h"
#include "qgscolorrampshader.h"

class QImage;
class QgsPoint;

/** \ingroup core
 * Base class for raster data providers.
 *
 *  \note  This class has been copied and pasted from
 *         QgsVectorDataProvider, and does not yet make
 *         sense for Raster layers.
 */

class CORE_EXPORT QgsRasterDataProvider : public QgsDataProvider
{

    Q_OBJECT

  public:

    //! If you add to this, please also add to capabilitiesString()
    enum Capability
    {
      NoCapabilities =              0,
      Identify =                    1,
      // Draw: the provider is capable to render data on QImage, e.g. WMS, GRASS
      Draw =                   1 << 1,
      // Data: the provider is capable to return data values, so that 
      // QgsRasterLayer can render them using selected rendering mode, e.g. GDAL, GRASS
      Data =          1 << 2
    };

    // This is modified copy of GDALDataType
    enum DataType
    {
      /*! Unknown or unspecified type */          UnknownDataType = 0,
      /*! Eight bit unsigned integer */           Byte = 1,
      /*! Sixteen bit unsigned integer */         UInt16 = 2,
      /*! Sixteen bit signed integer */           Int16 = 3,
      /*! Thirty two bit unsigned integer */      UInt32 = 4,
      /*! Thirty two bit signed integer */        Int32 = 5,
      /*! Thirty two bit floating point */        Float32 = 6,
      /*! Sixty four bit floating point */        Float64 = 7,
      /*! Complex Int16 */                        CInt16 = 8,
      /*! Complex Int32 */                        CInt32 = 9,
      /*! Complex Float32 */                      CFloat32 = 10,
      /*! Complex Float64 */                      CFloat64 = 11,
      TypeCount = 12          /* maximum type # + 1 */
    }; 

    // This is modified copy of GDALColorInterp
    enum ColorInterpretation
    {
      UndefinedColorInterpretation=0,
      /*! Greyscale */                                      GrayIndex=1,
      /*! Paletted (see associated color table) */          PaletteIndex=2,
      /*! Red band of RGBA image */                         RedBand=3,
      /*! Green band of RGBA image */                       GreenBand=4,
      /*! Blue band of RGBA image */                        BlueBand=5,
      /*! Alpha (0=transparent, 255=opaque) */              AlphaBand=6,
      /*! Hue band of HLS image */                          HueBand=7,
      /*! Saturation band of HLS image */                   SaturationBand=8,
      /*! Lightness band of HLS image */                    LightnessBand=9,
      /*! Cyan band of CMYK image */                        CyanBand=10,
      /*! Magenta band of CMYK image */                     MagentaBand=11,
      /*! Yellow band of CMYK image */                      YellowBand=12,
      /*! Black band of CMLY image */                       BlackBand=13,
      /*! Y Luminance */                                    YCbCr_YBand=14,
      /*! Cb Chroma */                                      YCbCr_CbBand=15,
      /*! Cr Chroma */                                      YCbCr_CrBand=16,
      /*! Max current value */                              ColorInterpretationMax=16
    };

    QgsRasterDataProvider();

    QgsRasterDataProvider( QString const & uri );

    virtual ~QgsRasterDataProvider() {};


    /**
     * Add the list of WMS layer names to be rendered by this server
     */
    virtual void addLayers( QStringList const & layers,
                            QStringList  const & styles = QStringList() ) = 0;

    //! get raster image encodings supported by (e.g.) the WMS Server, expressed as MIME types
    virtual QStringList supportedImageEncodings() = 0;

    /**
     * Get the image encoding (as a MIME type) used in the transfer from (e.g.) the WMS server
     */
    virtual QString imageEncoding() const = 0;

    /**
     * Set the image encoding (as a MIME type) used in the transfer from (e.g.) the WMS server
     */
    virtual void setImageEncoding( QString  const & mimeType ) = 0;

    /**
     * Set the image projection (in WMS CRS format) used in the transfer from (e.g.) the WMS server
     */
    virtual void setImageCrs( QString const & crs ) = 0;


    // TODO: Document this better.
    /** \brief   Renders the layer as an image
     */
    virtual QImage* draw( QgsRectangle  const & viewExtent, int pixelWidth, int pixelHeight ) = 0;

    /** Returns a bitmask containing the supported capabilities
        Note, some capabilities may change depending on whether
        a spatial filter is active on this provider, so it may
        be prudent to check this value per intended operation.
      */
    virtual int capabilities() const
    {
      return QgsRasterDataProvider::NoCapabilities;
    }

    /**
     *  Returns the above in friendly format.
     */
    QString capabilitiesString() const;


    // TODO: Get the supported formats by this provider

    // TODO: Get the file masks supported by this provider, suitable for feeding into the file open dialog box

    /** Returns data type for the band specified by number */
    virtual int dataType ( int bandNo ) const
    {
      return QgsRasterDataProvider::UnknownDataType;
    }

    int typeSize ( int dataType ) const
    {
      // modified copy from GDAL
      switch( dataType )
      {
        case Byte:
          return 8;

        case UInt16:
        case Int16:
          return 16;

        case UInt32:
        case Int32:
        case Float32:
        case CInt16:
          return 32;

        case Float64:
        case CInt32:
        case CFloat32:
          return 64;

        case CFloat64:
          return 128;

        default:
          return 0;
      }
    }
    int dataTypeSize ( int bandNo ) const
    {
      return typeSize ( dataType ( bandNo ) );
    }

    /** Get numbur of bands */
    virtual int bandCount() const {
      return 0;
    }

    /** Returns data type for the band specified by number */
    virtual int colorInterpretation ( int bandNo ) const {
      return QgsRasterDataProvider::UndefinedColorInterpretation;
    }

    /** Get block size */
    virtual int xBlockSize() const { return 0; }
    virtual int yBlockSize() const { return 0; }
    
    /** Get raster size */
    virtual int xSize() const { return 0; }
    virtual int ySize() const { return 0; }

    /** read block of data  */
    // TODO clarify what happens on the last block (the part outside raster)
    virtual void readBlock( int bandNo, int xBlock, int yBlock, void *data ){}

    /** read block of data using give extent and size */
    virtual void readBlock( int bandNo, QgsRectangle  const & viewExtent, int width, int height, void *data ) {};

    /** value representing null data */
    virtual double noDataValue() const { return 0; }

    virtual double minimumValue(int bandNo)const { return 0; }
    virtual double maximumValue(int bandNo)const { return 0; }

    virtual QList<QgsColorRampShader::ColorRampItem> colorTable(int bandNo)const { return QList<QgsColorRampShader::ColorRampItem>(); }

    /**
     * Get metadata in a format suitable for feeding directly
     * into a subset of the GUI raster properties "Metadata" tab.
     */
    virtual QString metadata() = 0;

    /** \brief Identify raster value(s) found on the point position */
    virtual bool identify( const QgsPoint & point, QMap<QString, QString>& results );

    /**
     * \brief Identify details from a server (e.g. WMS) from the last screen update
     *
     * \param[in] point  The pixel coordinate (as it was displayed locally on screen)
     *
     * \return  A text document containing the return from the WMS server
     *
     * \note WMS Servers prefer to receive coordinates in image space, therefore
     *       this function expects coordinates in that format.
     *
     * \note  The arbitraryness of the returned document is enforced by WMS standards
     *        up to at least v1.3.0
     */
    virtual QString identifyAsText( const QgsPoint& point ) = 0;

    /**
     * \brief Identify details from a server (e.g. WMS) from the last screen update
     *
     * \param[in] point  The pixel coordinate (as it was displayed locally on screen)
     *
     * \return  A html document containing the return from the WMS server
     *
     * \note WMS Servers prefer to receive coordinates in image space, therefore
     *       this function expects coordinates in that format.
     *
     * \note  The arbitraryness of the returned document is enforced by WMS standards
     *        up to at least v1.3.0
     *
     * \note  added in 1.5
     */
    virtual QString identifyAsHtml( const QgsPoint& point ) = 0;

    /**
     * \brief   Returns the caption error text for the last error in this provider
     *
     * If an operation returns 0 (e.g. draw()), this function
     * returns the text of the error associated with the failure.
     * Interactive users of this provider can then, for example,
     * call a QMessageBox to display the contents.
     *
     */
    virtual QString lastErrorTitle() = 0;

    /**
     * \brief   Returns the verbose error text for the last error in this provider
     *
     * If an operation returns 0 (e.g. draw()), this function
     * returns the text of the error associated with the failure.
     * Interactive users of this provider can then, for example,
     * call a QMessageBox to display the contents.
     *
     */
    virtual QString lastError() = 0;

    /**
     * \brief   Returns the format of the error text for the last error in this provider
     *
     * \note added in 1.6
     */
    virtual QString lastErrorFormat();


    /**Returns the dpi of the output device.
      @note: this method was added in version 1.2*/
    int dpi() const {return mDpi;}

    /**Sets the output device resolution.
      @note: this method was added in version 1.2*/
    void setDpi( int dpi ) {mDpi = dpi;}


  protected:
    /**Dots per intch. Extended WMS (e.g. QGIS mapserver) support DPI dependent output and therefore
    are suited for printing. A value of -1 means it has not been set
    @note: this member has been added in version 1.2*/
    int mDpi;

};

#endif
