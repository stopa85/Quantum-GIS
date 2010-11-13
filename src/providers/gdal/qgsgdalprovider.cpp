/***************************************************************************
  qgsgdalprovider.cpp  -  QGIS Data provider for
                           GDAL rasters
                             -------------------
    begin                : November, 2010
    copyright            : (C) 2010 by Radim Blazek
    email                : radim dot blazek at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* $Id: qgsgdalprovider.cpp 11522 2009-08-28 14:49:22Z jef $ */

#include "qgslogger.h"
#include "qgsgdalprovider.h"
#include "qgsconfig.h"

#include <math.h>

#include "qgsapplication.h"
#include "qgscoordinatetransform.h"
#include "qgsrectangle.h"
#include "qgscoordinatereferencesystem.h"

#include <QImage>
#include <QSettings>
#include <QColor>
#include <QProcess>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QHash>

#include "gdalwarper.h"
#include "ogr_spatialref.h"
#include "cpl_conv.h"

static QString PROVIDER_KEY = "gdal";
static QString PROVIDER_DESCRIPTION = "GDAL provider";

QgsGdalProvider::QgsGdalProvider( QString const & uri )
    : QgsRasterDataProvider( uri ), mValid( true )
{
  QgsDebugMsg( "QgsGdalProvider: constructing with uri '" + uri + "'." );

  mValid = false;
  mGdalBaseDataset = 0;
  mGdalDataset = 0;

  registerGdalDrivers();

  // To get buildSupportedRasterFileFilter the provider is called with empty uri
  if ( uri.isEmpty() ) return;

  // Initialise the affine transform matrix
  mGeoTransform[0] =  0;
  mGeoTransform[1] =  1;
  mGeoTransform[2] =  0;
  mGeoTransform[3] =  0;
  mGeoTransform[4] =  0;
  mGeoTransform[5] = -1;


  mGdalDataset = NULL;

  mGdalBaseDataset = GDALOpen( QFile::encodeName( uri ).constData(), GA_ReadOnly );

  CPLErrorReset();
  if ( mGdalBaseDataset == NULL )
  {
    QMessageBox::warning( 0, QObject::tr( "Warning" ),
      QObject::tr( "Cannot open GDAL dataset ") + uri + " : "
      + QString::fromUtf8( CPLGetLastErrorMsg()  ) );
     return;
  }

  QgsDebugMsg ("GdalDataset opened" );

  for ( int i = 0; i < GDALGetRasterCount( mGdalDataset ); i++ )
  {
    mMinMaxComputed.append(false);
    mMinimum.append(0);
    mMaximum.append(0);
  }
  // Check if we need a warped VRT for this file.
  if (( GDALGetGeoTransform( mGdalBaseDataset, mGeoTransform ) == CE_None
        && ( mGeoTransform[1] < 0.0
             || mGeoTransform[2] != 0.0
             || mGeoTransform[4] != 0.0
             || mGeoTransform[5] > 0.0 ) )
      || GDALGetGCPCount( mGdalBaseDataset ) > 0 )
  {
    QgsLogger::warning( "Creating Warped VRT." );

    mGdalDataset =
      GDALAutoCreateWarpedVRT( mGdalBaseDataset, NULL, NULL,
                               GRA_NearestNeighbour, 0.2, NULL );
    if ( mGdalDataset == NULL )
    {
      QgsLogger::warning( "Warped VRT Creation failed." );
      mGdalDataset = mGdalBaseDataset;
      GDALReferenceDataset( mGdalDataset );
    }
  }
  else
  {
    mGdalDataset = mGdalBaseDataset;
    GDALReferenceDataset( mGdalDataset );
  }

  //check if this file has pyramids
  CPLErrorReset();
  GDALRasterBandH myGDALBand = GDALGetRasterBand( mGdalDataset, 1 ); //just use the first band
  if ( myGDALBand == NULL )
  {
    QMessageBox::warning( 0, QObject::tr( "Warning" ),
      QObject::tr( "Cannot get GDAL raster band : " )
      + QString::fromUtf8( CPLGetLastErrorMsg()  ) ) ;

    GDALDereferenceDataset( mGdalBaseDataset );
    mGdalBaseDataset = NULL;

    GDALClose( mGdalDataset );
    mGdalDataset = NULL;
    return;
  }

  mHasPyramids = GDALGetOverviewCount( myGDALBand ) > 0;

  //populate the list of what pyramids exist
  // TODO
  //buildPyramidList();

  // Get the layer's projection info and set up the
  // QgsCoordinateTransform for this layer
  // NOTE: we must do this before metadata is called

  QString myWktString;
  myWktString = QString( GDALGetProjectionRef( mGdalDataset ) );
  mCrs.createFromWkt( myWktString );
  if ( !mCrs.isValid() )
  {
    //try to get the gcp srs from the raster layer if available
    myWktString = QString( GDALGetGCPProjection( mGdalDataset ) );
// What is the purpose of this piece of code?
// Sideeffects from validate()?
//    myCRS.createFromWkt(myWktString);
//    if (!myCRS.isValid())
//    {
//      // use force and make CRS valid!
//      myCRS.validate();
//    }

  }

  //set up the coordinat transform - in the case of raster this is mainly used to convert
  //the inverese projection of the map extents of the canvas when zooming in etc. so
  //that they match the coordinate system of this layer
  //QgsDebugMsg( "Layer registry has " + QString::number( QgsMapLayerRegistry::instance()->count() ) + "layers" );

  //metadata();

  // Use the affine transform to get geo coordinates for
  // the corners of the raster
  double myXMax = mGeoTransform[0] +
                  GDALGetRasterXSize( mGdalDataset ) * mGeoTransform[1] +
                  GDALGetRasterYSize( mGdalDataset ) * mGeoTransform[2];
  double myYMin = mGeoTransform[3] +
                  GDALGetRasterXSize( mGdalDataset ) * mGeoTransform[4] +
                  GDALGetRasterYSize( mGdalDataset ) * mGeoTransform[5];

  mExtent.setXMaximum( myXMax );
  // The affine transform reduces to these values at the
  // top-left corner of the raster
  mExtent.setXMinimum( mGeoTransform[0] );
  mExtent.setYMaximum( mGeoTransform[3] );
  mExtent.setYMinimum( myYMin );

  //
  // Set up the x and y dimensions of this raster layer
  //
  mWidth = GDALGetRasterXSize( mGdalDataset );
  mHeight = GDALGetRasterYSize( mGdalDataset );


  GDALGetBlockSize( GDALGetRasterBand( mGdalDataset, 1 ), &mXBlockSize, &mYBlockSize );
  //
  // Determine the nodata value
  //
  mNoDataValue = -9999.0; //Standard default?
  mValidNoDataValue = false;
  int isValid = false;
  double myNoDataValue = GDALGetRasterNoDataValue( GDALGetRasterBand( mGdalDataset, 1 ), &isValid );
  if ( isValid )
  {
    mNoDataValue = myNoDataValue;
    mValidNoDataValue = true;
  }
  QgsDebugMsg( QString("mNoDataValue = %1").arg ( mNoDataValue ) ); 

  mValid = true;
  QgsDebugMsg( "end" );
}

QgsGdalProvider::~QgsGdalProvider()
{
  QgsDebugMsg( "QgsGdalProvider: deconstructing." );
  if ( mGdalBaseDataset )
  {
    GDALDereferenceDataset( mGdalBaseDataset );
  }
  if ( mGdalDataset )
  {
    GDALClose( mGdalDataset );
  }
}


    
QString QgsGdalProvider::metadata()
{
  QString myMetadata ;
  myMetadata += QString( GDALGetDescription( GDALGetDatasetDriver( mGdalDataset ) ) );
  myMetadata += "<br>";
  myMetadata += QString( GDALGetMetadataItem( GDALGetDatasetDriver( mGdalDataset ), GDAL_DMD_LONGNAME, NULL ) );

  // my added code (MColetti)

  myMetadata += "<p class=\"glossy\">";
  myMetadata += tr( "Dataset Description" );
  myMetadata += "</p>\n";
  myMetadata += "<p>";
  myMetadata += QFile::decodeName( GDALGetDescription( mGdalDataset ) );
  myMetadata += "</p>\n";


  char ** GDALmetadata = GDALGetMetadata( mGdalDataset, NULL );

  if ( GDALmetadata )
  {
    QStringList metadata = cStringList2Q_( GDALmetadata );
    myMetadata += QgsRasterDataProvider::makeTableCells( metadata );
  }
  else
  {
    QgsDebugMsg( "dataset has no metadata" );
  }

  for ( int i = 1; i <= GDALGetRasterCount( mGdalDataset ); ++i )
  {
    myMetadata += "<p class=\"glossy\">" + tr( "Band %1" ).arg( i ) + "</p>\n";
    GDALRasterBandH gdalBand = GDALGetRasterBand( mGdalDataset, i );
    GDALmetadata = GDALGetMetadata( gdalBand, NULL );

    if ( GDALmetadata )
    {
      QStringList metadata = cStringList2Q_( GDALmetadata );
      myMetadata += QgsRasterDataProvider::makeTableCells( metadata );
    }
    else
    {
      QgsDebugMsg( "band " + QString::number( i ) + " has no metadata" );
    }

    char ** GDALcategories = GDALGetRasterCategoryNames( gdalBand );

    if ( GDALcategories )
    {
      QStringList categories = cStringList2Q_( GDALcategories );
      myMetadata += QgsRasterDataProvider::makeTableCells( categories );
    }
    else
    {
      QgsDebugMsg( "band " + QString::number( i ) + " has no categories" );
    }

  }

  // end my added code

  myMetadata += "<p class=\"glossy\">";
  myMetadata += tr( "Dimensions:" );
  myMetadata += "</p>\n";
  myMetadata += "<p>";
  myMetadata += tr( "X: %1 Y: %2 Bands: %3" )
                .arg( GDALGetRasterXSize( mGdalDataset ) )
                .arg( GDALGetRasterYSize( mGdalDataset ) )
                .arg( GDALGetRasterCount( mGdalDataset ) );
  myMetadata += "</p>\n";

  //just use the first band
  GDALRasterBandH myGdalBand = GDALGetRasterBand( mGdalDataset, 1 );
  if ( GDALGetOverviewCount( myGdalBand ) > 0 )
  {
    int myOverviewInt;
    for ( myOverviewInt = 0;
          myOverviewInt < GDALGetOverviewCount( myGdalBand );
          myOverviewInt++ )
    {
      GDALRasterBandH myOverview;
      myOverview = GDALGetOverview( myGdalBand, myOverviewInt );
      myMetadata += "<p>X : " + QString::number( GDALGetRasterBandXSize( myOverview ) );
      myMetadata += ",Y " + QString::number( GDALGetRasterBandYSize( myOverview ) ) + "</p>";
    }
  }
  myMetadata += "</p>\n";

  if ( GDALGetGeoTransform( mGdalDataset, mGeoTransform ) != CE_None )
  {
    // if the raster does not have a valid transform we need to use
    // a pixel size of (1,-1), but GDAL returns (1,1)
    mGeoTransform[5] = -1;
  }
  else
  {
    myMetadata += "<p class=\"glossy\">";
    myMetadata += tr( "Origin:" );
    myMetadata += "</p>\n";
    myMetadata += "<p>";
    myMetadata += QString::number( mGeoTransform[0] );
    myMetadata += ",";
    myMetadata += QString::number( mGeoTransform[3] );
    myMetadata += "</p>\n";

    myMetadata += "<p class=\"glossy\">";
    myMetadata += tr( "Pixel Size:" );
    myMetadata += "</p>\n";
    myMetadata += "<p>";
    myMetadata += QString::number( mGeoTransform[1] );
    myMetadata += ",";
    myMetadata += QString::number( mGeoTransform[5] );
    myMetadata += "</p>\n";
  }

  return myMetadata;
}


// Not supported by GDAL
QImage* QgsGdalProvider::draw( QgsRectangle  const & viewExtent, int pixelWidth, int pixelHeight )
{
  QgsDebugMsg( "pixelWidth = "  + QString::number( pixelWidth ) );
  QgsDebugMsg( "pixelHeight = "  + QString::number( pixelHeight ) );
  QgsDebugMsg( "viewExtent: " + viewExtent.toString() );

  QImage *image = new QImage( pixelWidth, pixelHeight, QImage::Format_ARGB32 );
  image->fill( QColor( Qt::gray ).rgb() );

  return image;
}


void QgsGdalProvider::readBlock( int theBandNo, int xBlock, int yBlock, void *block )
{
  QgsDebugMsg( "Entered" );
  
  QgsDebugMsg( "yBlock = "  + QString::number( yBlock ) );

  GDALRasterBandH myGdalBand = GDALGetRasterBand( mGdalDataset, theBandNo );
  GDALReadBlock( myGdalBand, xBlock, yBlock, block );
}

void QgsGdalProvider::readBlock( int theBandNo, QgsRectangle  const & theExtent, int thePixelWidth, int thePixelHeight, void *theBlock )
{
  QgsDebugMsg( "thePixelWidth = "  + QString::number( thePixelWidth ) );
  QgsDebugMsg( "thePixelHeight = "  + QString::number( thePixelHeight ) );
  QgsDebugMsg( "theExtent: " + theExtent.toString() );


  GDALRasterBandH myGdalBand = GDALGetRasterBand( mGdalDataset, theBandNo );
  GDALDataType myGdalDataType = GDALGetRasterDataType( myGdalBand );

  //QString myMemDsn = QString( "MEM:::DATAPOINTER=%1,PIXELS=%2,LINES=%3,BANDS=1,DATATYPE=%4,PIXELOFFSET=0,LINEOFFSET=0,BANDOFFSET=0")
    //.arg ( (long) theBlock )  
    //.arg ( thePixelWidth )  
    //.arg ( thePixelHeight )  
    //.arg ( GDALGetDataTypeName( myGdalDataType ) ); 

  QString myMemDsn;
  myMemDsn.sprintf ( "DATAPOINTER = %p", theBlock ); 
  QgsDebugMsg(  myMemDsn );
 
  myMemDsn.sprintf ( "MEM:::DATAPOINTER=%u,PIXELS=%d,LINES=%d,BANDS=1,DATATYPE=%s,PIXELOFFSET=0,LINEOFFSET=0,BANDOFFSET=0", (int)theBlock, thePixelWidth, thePixelHeight,  GDALGetDataTypeName( myGdalDataType ) );

  QgsDebugMsg( "Open GDAL MEM : " + myMemDsn );

  
  CPLErrorReset();
  GDALDatasetH myGdalMemDataset = GDALOpen( myMemDsn.toAscii().constData(),GA_Update);

  if ( !myGdalMemDataset )
  {
    QMessageBox::warning( 0, QObject::tr( "Warning" ),
      QObject::tr( "Cannot open GDAL MEM dataset ") + myMemDsn + " : "
      + QString::fromUtf8( CPLGetLastErrorMsg()  ) );
     return;
  }
  // TODO add CRS to method params
  //GDALSetProjection( myGdalMemDataset, crs().toWkt().toAscii().constData() );
  GDALSetProjection( myGdalMemDataset, GDALGetProjectionRef( mGdalDataset ) ); 

  double myMemGeoTransform[6];
  myMemGeoTransform[0] = theExtent.xMinimum(); /* top left x */
  myMemGeoTransform[1] = theExtent.width()/thePixelWidth; /* w-e pixel resolution */
  myMemGeoTransform[2] = 0; /* rotation, 0 if image is "north up" */
  myMemGeoTransform[3] = theExtent.yMaximum(); /* top left y */
  myMemGeoTransform[4] = 0; /* rotation, 0 if image is "north up" */
  myMemGeoTransform[5] = -1. *  theExtent.height()/thePixelHeight; /* n-s pixel resolution */

  double myGeoTransform[6];
  GDALGetGeoTransform( mGdalDataset, myGeoTransform );
  GDALSetGeoTransform( myGdalMemDataset, myMemGeoTransform );

  for ( int i = 0 ; i < 6; i++ ) 
  {
    QgsDebugMsg ( QString("transform : %1 %2").arg(myGeoTransform[i]).arg ( myMemGeoTransform[i] ) );
  }

  GDALWarpOptions *myWarpOptions = GDALCreateWarpOptions();

  myWarpOptions->hSrcDS = mGdalDataset;
  myWarpOptions->hDstDS = myGdalMemDataset;

  myWarpOptions->nBandCount = 1;
  myWarpOptions->panSrcBands = 
    (int *) CPLMalloc(sizeof(int) * myWarpOptions->nBandCount );
  myWarpOptions->panSrcBands[0] = 1;
  myWarpOptions->panDstBands = 
    (int *) CPLMalloc(sizeof(int) * myWarpOptions->nBandCount );
  myWarpOptions->panDstBands[0] = 1;

  // TODO move here progressCallback and use it
  myWarpOptions->pfnProgress = GDALTermProgress;   

  QgsDebugMsg ( "src wkt: " +  QString (GDALGetProjectionRef(mGdalDataset) ) ); 
  QgsDebugMsg ( "dst wkt: " +  QString (GDALGetProjectionRef(myGdalMemDataset) ) ); 
  myWarpOptions->pTransformerArg = 
      GDALCreateGenImgProjTransformer( 
        mGdalDataset, 
        GDALGetProjectionRef(mGdalDataset), 
        myGdalMemDataset,
        GDALGetProjectionRef(myGdalMemDataset), 
        FALSE, 0.0, 1 
      );
  CPLAssert( myWarpOptions->pTransformerArg  != NULL); 
  myWarpOptions->pfnTransformer = GDALGenImgProjTransform;


  GDALWarpOperation myOperation;

  myOperation.Initialize( myWarpOptions );
  CPLErrorReset();
  CPLErr myErr;
  myErr = myOperation.ChunkAndWarpImage( 0, 0, thePixelWidth, thePixelHeight ); 
  if ( myErr != CPLE_None )
  {
    QMessageBox::warning( 0, QObject::tr( "Warning" ),
      QObject::tr( "Cannot ChunkAndWarpImage : " )
      + QString::fromUtf8( CPLGetLastErrorMsg()  ) );
     return;
  }

  GDALDestroyGenImgProjTransformer( myWarpOptions->pTransformerArg );
  GDALDestroyWarpOptions( myWarpOptions );

  // flush should not be necessary 
  //GDALFlushCache  (  myGdalMemDataset );
  // this was causing crash ???
  // The MEM driver does not free() the memory passed as DATAPOINTER so we can closee the dataset
  GDALClose( myGdalMemDataset ); 

}

double  QgsGdalProvider::noDataValue() const {
  return mNoDataValue;
}

void QgsGdalProvider::computeMinMax ( int theBandNo ) 
{
  if ( mMinMaxComputed[theBandNo-1] ) return; 
  double GDALrange[2];
  GDALRasterBandH myGdalBand = GDALGetRasterBand( mGdalDataset, theBandNo );
  GDALComputeRasterMinMax( myGdalBand, 1, GDALrange ); //Approximate
  mMinimum[theBandNo-1] = GDALrange[0]; 
  mMaximum[theBandNo-1] = GDALrange[1]; 
}

double  QgsGdalProvider::minimumValue( int theBandNo )  
{
  computeMinMax ( theBandNo );
  return  mMinimum[theBandNo-1];
}
double  QgsGdalProvider::maximumValue( int theBandNo ) 
{
  computeMinMax ( theBandNo );
  return  mMaximum[theBandNo-1];
}

QList<QgsColorRampShader::ColorRampItem> QgsGdalProvider::colorTable(int bandNo)const {
  // TODO
  QList<QgsColorRampShader::ColorRampItem> ct;
  
  //double v, r, g, b;
  //for (i = colors.begin(); i != colors.end(); ++i)
  //{
    //QgsColorRampShader::ColorRampItem ctItem2;
    //ctItem2.value = i->value2;
    //ctItem2.color = QColor::fromRgb( i->red2, i->green2, i->blue2);
    //ct.append(ctItem2);
    //QgsDebugMsg( QString("color %1 %2 %3 %4").arg(i->value2).arg(i->red2).arg(i->green2).arg(i->blue2) );

    //v = i->value2; r = i->red2; g = i->green2; b = i->blue2;
  //}
  return ct;
}

QgsCoordinateReferenceSystem QgsGdalProvider::crs()
{
  QgsDebugMsg( "Entered" );
  return mCrs;
}

QgsRectangle QgsGdalProvider::extent()
{
  //TODO
  //mExtent = QgsGdal::extent( mGisdbase, mLocation, mMapset, mMapName, QgsGdal::Raster );
  return mExtent;
}

// this is only called once when statistics are calculated
// TODO
int QgsGdalProvider::xBlockSize() const { 
  return mXBlockSize; 
} 
int QgsGdalProvider::yBlockSize() const { 
  return mYBlockSize; 
}

int QgsGdalProvider::xSize() const { return mWidth; } 
int QgsGdalProvider::ySize() const { return mHeight; }

bool QgsGdalProvider::identify( const QgsPoint& thePoint, QMap<QString, QString>& theResults )
{
  QgsDebugMsg( "Entered" );
  // TODO
  theResults["Error"] = tr( "Not implemented" );
  //theResults["Error"] = tr( "Out of extent" );
  //theResults = QgsGdal::query( mGisdbase, mLocation, mMapset, mMapName, QgsGdal::Raster, thePoint.x(), thePoint.y() );
  return true;
}

int QgsGdalProvider::capabilities() const
{
  int capability = QgsRasterDataProvider::Identify 
                 | QgsRasterDataProvider::Data
                 | QgsRasterDataProvider::EstimatedMinimumMaximum;
  return capability;
}

int QgsGdalProvider::dataType( int bandNo ) const
{
  GDALRasterBandH myGdalBand = GDALGetRasterBand( mGdalDataset, bandNo ); 
  GDALDataType myGdalDataType = GDALGetRasterDataType( myGdalBand );

  switch ( myGdalDataType ) {
    case GDT_Unknown:
      return QgsRasterDataProvider::UnknownDataType;
      break;
    case GDT_Byte:
      return QgsRasterDataProvider::Byte;
      break;
    case GDT_UInt16:
      return QgsRasterDataProvider::UInt16;
      break;
    case GDT_Int16:
      return QgsRasterDataProvider::Int16;
      break;
    case GDT_UInt32:
      return QgsRasterDataProvider::UInt32;
      break;
    case GDT_Int32:
      return QgsRasterDataProvider::Int32;
      break;
    case GDT_Float32:
      return QgsRasterDataProvider::Float32;
      break;
    case GDT_Float64:
      return QgsRasterDataProvider::Float64;
      break;
    case GDT_CInt16:
      return QgsRasterDataProvider::CInt16;
      break;
    case GDT_CInt32:
      return QgsRasterDataProvider::CInt32;
      break;
    case GDT_CFloat32:
      return QgsRasterDataProvider::CFloat32;
      break;
    case GDT_CFloat64:
      return QgsRasterDataProvider::CFloat64;
      break;
  }
  return QgsRasterDataProvider::UnknownDataType;
}

int QgsGdalProvider::bandCount() const
{
  return GDALGetRasterCount( mGdalDataset);
}

int QgsGdalProvider::colorInterpretation ( int bandNo ) const {
  // TODO
  return QgsRasterDataProvider::GrayIndex;
}

void QgsGdalProvider::registerGdalDrivers()
{
  if ( GDALGetDriverCount() == 0 )
    GDALAllRegister();
}


bool QgsGdalProvider::isValid()
{
  QgsDebugMsg ( QString("valid = %1").arg(mValid)); 
  return mValid;
}

QString QgsGdalProvider::identifyAsText( const QgsPoint& point )
{
  return  QString( "Not implemented" );
}

QString QgsGdalProvider::identifyAsHtml( const QgsPoint& point )
{
  return  QString( "Not implemented" );
}

QString QgsGdalProvider::lastErrorTitle()
{
  return  QString( "Not implemented" );
}

QString QgsGdalProvider::lastError()
{
  return  QString( "Not implemented" );
}

QString  QgsGdalProvider::name() const
{
  return PROVIDER_KEY;
}

QString  QgsGdalProvider::description() const
{
  return PROVIDER_DESCRIPTION;
}
/**
  Builds the list of file filter strings to later be used by
  QgisApp::addRasterLayer()

  We query GDAL for a list of supported raster formats; we then build
  a list of file filter strings from that list.  We return a string
  that contains this list that is suitable for use in a
  QFileDialog::getOpenFileNames() call.

*/
void QgsGdalProvider::buildSupportedRasterFileFilter( QString & theFileFiltersString )
{
  QgsDebugMsg("Entered");
  // first get the GDAL driver manager
  //registerGdalDrivers();

  // then iterate through all of the supported drivers, adding the
  // corresponding file filter

  GDALDriverH myGdalDriver;           // current driver

  char **myGdalDriverMetadata;        // driver metadata strings

  QString myGdalDriverLongName( "" ); // long name for the given driver
  QString myGdalDriverExtension( "" );  // file name extension for given driver
  QString myGdalDriverDescription;    // QString wrapper of GDAL driver description

  QStringList metadataTokens;   // essentially the metadata string delimited by '='

  QStringList catchallFilter;   // for Any file(*.*), but also for those
  // drivers with no specific file filter

  GDALDriverH jp2Driver = NULL; // first JPEG2000 driver found

  // Grind through all the drivers and their respective metadata.
  // We'll add a file filter for those drivers that have a file
  // extension defined for them; the others, well, even though
  // theoreticaly we can open those files because there exists a
  // driver for them, the user will have to use the "All Files" to
  // open datasets with no explicitly defined file name extension.
  // Note that file name extension strings are of the form
  // "DMD_EXTENSION=.*".  We'll also store the long name of the
  // driver, which will be found in DMD_LONGNAME, which will have the
  // same form.

  // start with the default case
  theFileFiltersString = QObject::tr( "[GDAL] All files (*)" );

  for ( int i = 0; i < GDALGetDriverCount(); ++i )
  {
    myGdalDriver = GDALGetDriver( i );

    Q_CHECK_PTR( myGdalDriver );

    if ( !myGdalDriver )
    {
      QgsLogger::warning( "unable to get driver " + QString::number( i ) );
      continue;
    }
    // now we need to see if the driver is for something currently
    // supported; if not, we give it a miss for the next driver

    myGdalDriverDescription = GDALGetDescription( myGdalDriver );

    // QgsDebugMsg(QString("got driver string %1").arg(myGdalDriverDescription));

    myGdalDriverMetadata = GDALGetMetadata( myGdalDriver, NULL );

    // presumably we know we've run out of metadta if either the
    // address is 0, or the first character is null
    while ( myGdalDriverMetadata && '\0' != myGdalDriverMetadata[0] )
    {
      metadataTokens = QString( *myGdalDriverMetadata ).split( "=", QString::SkipEmptyParts );
      // QgsDebugMsg(QString("\t%1").arg(*myGdalDriverMetadata));

      // XXX add check for malformed metadataTokens

      // Note that it's oddly possible for there to be a
      // DMD_EXTENSION with no corresponding defined extension
      // string; so we check that there're more than two tokens.

      if ( metadataTokens.count() > 1 )
      {
        if ( "DMD_EXTENSION" == metadataTokens[0] )
        {
          myGdalDriverExtension = metadataTokens[1];

        }
        else if ( "DMD_LONGNAME" == metadataTokens[0] )
        {
          myGdalDriverLongName = metadataTokens[1];

          // remove any superfluous (.*) strings at the end as
          // they'll confuse QFileDialog::getOpenFileNames()

          myGdalDriverLongName.remove( QRegExp( "\\(.*\\)$" ) );
        }
      }
      // if we have both the file name extension and the long name,
      // then we've all the information we need for the current
      // driver; therefore emit a file filter string and move to
      // the next driver
      if ( !( myGdalDriverExtension.isEmpty() || myGdalDriverLongName.isEmpty() ) )
      {
        // XXX add check for SDTS; in that case we want (*CATD.DDF)
        QString glob = "*." + myGdalDriverExtension.replace( "/", " *." );
        // Add only the first JP2 driver found to the filter list (it's the one GDAL uses)
        if ( myGdalDriverDescription == "JPEG2000" ||
             myGdalDriverDescription.startsWith( "JP2" ) ) // JP2ECW, JP2KAK, JP2MrSID
        {
          if ( jp2Driver )
            break; // skip if already found a JP2 driver

          jp2Driver = myGdalDriver;   // first JP2 driver found
          glob += " *.j2k";           // add alternate extension
        }
        else if ( myGdalDriverDescription == "GTiff" )
        {
          glob += " *.tiff";
        }
        else if ( myGdalDriverDescription == "JPEG" )
        {
          glob += " *.jpeg";
        }

        theFileFiltersString += ";;[GDAL] " + myGdalDriverLongName + " (" + glob.toLower() + " " + glob.toUpper() + ")";

        break;            // ... to next driver, if any.
      }

      ++myGdalDriverMetadata;

    }                       // each metadata item

    if ( myGdalDriverExtension.isEmpty() && !myGdalDriverLongName.isEmpty() )
    {
      // Then what we have here is a driver with no corresponding
      // file extension; e.g., GRASS.  In which case we append the
      // string to the "catch-all" which will match all file types.
      // (I.e., "*.*") We use the driver description intead of the
      // long time to prevent the catch-all line from getting too
      // large.

      // ... OTOH, there are some drivers with missing
      // DMD_EXTENSION; so let's check for them here and handle
      // them appropriately

      // USGS DEMs use "*.dem"
      if ( myGdalDriverDescription.startsWith( "USGSDEM" ) )
      {
        QString glob = "*.dem";
        theFileFiltersString += ";;[GDAL] " + myGdalDriverLongName + " (" + glob.toLower() + " " + glob.toUpper() + ")";
      }
      else if ( myGdalDriverDescription.startsWith( "DTED" ) )
      {
        // DTED use "*.dt0, *.dt1, *.dt2"
        QString glob = "*.dt0";
        glob += " *.dt1";
        glob += " *.dt2";
        theFileFiltersString += ";;[GDAL] " + myGdalDriverLongName + " (" + glob.toLower() + " " + glob.toUpper() + ")";
      }
      else if ( myGdalDriverDescription.startsWith( "MrSID" ) )
      {
        // MrSID use "*.sid"
        QString glob = "*.sid";
        theFileFiltersString += ";;[GDAL] " + myGdalDriverLongName + " (" + glob.toLower() + " " + glob.toUpper() + ")";
      }
      else
      {
        catchallFilter << QString( GDALGetDescription( myGdalDriver ) );
      }
    }

    myGdalDriverExtension = myGdalDriverLongName = "";  // reset for next driver

  }                           // each loaded GDAL driver

  QgsDebugMsg( "Raster filter list built: " + theFileFiltersString );
}                               // buildSupportedRasterFileFilter_()

/**
 * Class factory to return a pointer to a newly created
 * QgsGdalProvider object
 */
QGISEXTERN QgsGdalProvider * classFactory( const QString *uri )
{
  return new QgsGdalProvider( *uri );
}
/** Required key function (used to map the plugin to a data store type)
*/
QGISEXTERN QString providerKey()
{
  return PROVIDER_KEY;
}
/**
 * Required description function
 */
QGISEXTERN QString description()
{
  return PROVIDER_DESCRIPTION;
}
/**
 * Required isProvider function. Used to determine if this shared library
 * is a data provider plugin
 */
QGISEXTERN bool isProvider()
{
  return true;
}

