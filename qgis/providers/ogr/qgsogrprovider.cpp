/***************************************************************************
           qgsogrprovider.cpp Data provider for OGR supported formats
                    Formerly known as qgsshapefileprovider.cpp  
begin                : Oct 29, 2003
copyright            : (C) 2003 by Gary E.Sherman
email                : sherman at mrcc.com
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

#include "qgsogrprovider.h"

#ifndef WIN32
#include <netinet/in.h>
#endif

using namespace std;

#include <iostream>
#include <cfloat>
#include <cassert>

#include <ogrsf_frmts.h>
#include <ogr_geometry.h>
#include <ogr_spatialref.h>
#include <cpl_error.h>
#include "ogr_api.h"//only for a test

#include <qfile.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qmap.h>

//TODO Following ifndef can be removed once WIN32 GEOS support
//    is fixed
#ifndef NOWIN32GEOSXXX
//XXX GEOS support on windows is broken until we can get VC++ to
//    tolerate geos.h without throwing a bunch of type errors. It
//    appears that the windows version of GEOS may be compiled with 
//    MINGW rather than VC++.
#endif 


#include "../../src/qgssearchtreenode.h"
#include "../../src/qgsdataprovider.h"
#include "../../src/qgsfeature.h"
#include "../../src/qgsfield.h"
#include "../../src/qgsrect.h"
#include "../../src/qgis.h"


#ifdef WIN32
#define QGISEXTERN extern "C" __declspec( dllexport )
#else
#define QGISEXTERN extern "C"
#endif

static const QString TEXT_PROVIDER_KEY = "ogr";
static const QString TEXT_PROVIDER_DESCRIPTION = "OGR data provider";


/** fill a buffer with the Well Known Binary (WKB) associated with the given
 * OGR feature
 */
static
unsigned char * 
getGeometryPointer_(OGRFeature *fet, OGRwkbByteOrder endianness)
{
  OGRGeometry *geom = fet->GetGeometryRef();

  unsigned char *gPtr = 0;

  // get the wkb representation
  gPtr = new unsigned char[geom->WkbSize()];

  geom->exportToWkb(endianness, gPtr);

  return gPtr;
  
} // getGeometryPointer_




/** internal layer object wrapper round OGRLayer plus provider related
    state.
*/
struct Layer
{
  Layer()
    : mSelectionRectangle(0) // set the selection rectangle pointer to 0
  {
  }

  OGRwkbGeometryType geomType;

  vector<QgsField> attributeFields;

  /// corresponding OGR layer containing our geospatial data
  OGRLayer * ogrLayer;


  long numberFeatures;

  /**Flag indicating, if the minmaxcache should be renewed (true) or not (false)*/
  bool minmaxcachedirty;

  /**Matrix storing the minimum and maximum values*/
  double **minmaxcache;


  //! Selection rectangle 
  OGRPolygon * mSelectionRectangle;


}; // struct Layer



/** internal implementation object containing hidden provider state.
 */
struct QgsOgrProvider::Imp
{
  Imp()
    : minmaxcachedirty( true )
  {
  }

  ~Imp()
  {
    for(int i = 0; i < attributeFields.size(); i++)
    {
      delete [] minmaxcache[i];
    }

    delete [] minmaxcache;

    delete geometryFactory;
    delete wktReader;

  }

    /** these are essentially wrappers for OGR layers

      For each layer associated with a given OGR vector data source there
      should be a corresponding Layer object, which is stored here.  Most
      layer related functions take on an dataSourceLayerNum parameter which
      ultimately serves as an index into this data member.

     */
    vector<Layer> layers;

    OGRDataSource *ogrDataSource;

    /// XXX is this data source or data source layer specific?
    OGREnvelope *extent_;

    // OGR Driver that was actually used to open the layer
    OGRSFDriver *ogrDriver;

    // Friendly name of the OGR Driver that was actually used to open the layer
    QString ogrDriverName;

    // XXX should this be for the entire data source or for each specific layers?
    bool valid;

    //! Flag to indicate that spatial intersect should be used in selecting features
    // XXX should this be for the entire data source or for each specific layers?
    bool mUseIntersect;


    //! The geometry factory
    // XXX should this be for the entire data source or for each specific layers?
    geos::GeometryFactory *geometryFactory;

    //! The well known text reader
    // XXX should this be for the entire data source or for each specific layers?
    geos::WKTReader *wktReader;

}; // struct QgsOgrProvider::Imp



QgsOgrProvider::QgsOgrProvider(QString const & uri)
    : QgsVectorDataProvider(uri),
      imp_( new Imp )
{
  OGRRegisterAll();

  // make connection to the data source
#ifdef QGISDEBUG
  std::cerr << "Data source uri is " << uri.local8Bit() << std::endl;
#endif

  // try to open for update
  imp_->ogrDataSource = OGRSFDriverRegistrar::Open((const char *) uri.local8Bit(), TRUE, &imp_->ogrDriver);

  // if can't open for update, try opening read-only
  if( ! imp_->ogrDataSource )
  {
    imp_->ogrDataSource = OGRSFDriverRegistrar::Open((const char *) uri.local8Bit(),FALSE, &imp_->ogrDriver);

    // TODO: Need to set a flag or something to indicate that the layer
    // TODO: is in read-only mode, otherwise edit ops will fail

    // TODO: capabilities() should now reflect this; need to test.
  }

  if ( imp_->ogrDataSource ) 
  {
#ifdef QGISDEBUG
          QgsDebug( "Data source is valid" );
    std::cerr << "OGR Driver was " << ogrDriver->GetName() << std::endl;
#endif

    imp_->valid = true;

    imp_->ogrDriverName = imp_->ogrDriver->GetName();

    // XXX implement default layer 
    imp_->ogrLayer = imp_->ogrDataSource->GetLayer(0);

    // get the extent_ (envelope) of the layer
    QgsDebug( "Starting get extent" );

    imp_->extent_ = new OGREnvelope();

    imp_->ogrLayer->GetExtent(imp_->extent_);

    QgsDebug( "Finished get extent" );

    // getting the total number of features in the layer
    imp_->numberFeatures = imp_->ogrLayer->GetFeatureCount();   

    // check the validity of the layer
    QgsDebug( "checking validity" );

    loadFields();

    QgsDebug( "Done checking validity" );

  } 
  else
  {
      QgsDebug( "Data source is invalid" );

      QgsDebug( CPLGetLastErrorMsg() );

      imp_->valid = false;
  }

  //resize the cache matrix

  imp_->minmaxcache = new double*[fieldCount()];

  for(int i = 0; i < fieldCount(); i++)
  {
    imp_->minmaxcache[i] = new double[2];
  }

  // create the geos objects
  imp_->geometryFactory = new geos::GeometryFactory();
  assert( imp_->geometryFactory );

  // create the reader
  //    std::cerr << "Creating the wktReader\n";

  imp_->wktReader = new geos::WKTReader(imp_->geometryFactory);

  mNumericalTypes.push_back("Integer");
  mNumericalTypes.push_back("Real");
  mNonNumericalTypes.push_back("String");

} // QgsOgrProvider ctor



QgsOgrProvider::~QgsOgrProvider()
{
    // auto_ptr automatically deletes Imp
}


void QgsOgrProvider::setEncoding(const QString& e)
{
    QgsVectorDataProvider::setEncoding(e);
    loadFields();
}

void QgsOgrProvider::loadFields()
{
    //the attribute fields need to be read again when the encoding changes
    imp_->attributeFields.clear();

    OGRFeatureDefn * fdef = imp_->ogrLayer->GetLayerDefn();

    if(fdef)
    {
      imp_->geomType = fdef->GetGeomType();

      for(int i = 0; i < fdef->GetFieldCount(); ++i)
      {
        OGRFieldDefn * fldDef = fdef->GetFieldDefn(i);

        OGRFieldType type = type = fldDef->GetType();

        bool numeric = (type == OFTInteger || type == OFTReal);

        imp_->attributeFields.push_back(QgsField(
              mEncoding->toUnicode(fldDef->GetNameRef()), 
              mEncoding->toUnicode(fldDef->GetFieldTypeName(type)),
              fldDef->GetWidth(),
              fldDef->GetPrecision(),
              numeric));
      }
    }
} // QgsOgrProvider::loadFields()




QString QgsOgrProvider::getProjectionWKT()
{ 
#ifdef QGISDEBUG 
    std::cerr << "QgsOgrProvider::getProjectionWKT()" << std::endl; 
#endif 

  OGRSpatialReference * mySpatialRefSys = imp_->ogrLayer->GetSpatialRef();

  if (! mySpatialRefSys )
  {
      QgsDebug( "QgsOgrProvider::getProjectionWKT() --- no wkt found..returning null" );
      return "";
  }
  else
  {
    // if appropriate, morph the projection from ESRI form
    QFileInfo fileName( imp_->ogrDataSource->GetName() );

#ifdef QGISDEBUG 
    std::cerr << "Data source file name is : " << fileName.filePath().local8Bit() << std::endl; 
#endif 

    if( fileName.extension(false) == ".shp" )
    {
#ifdef QGISDEBUG 
      std::cerr << "Morphing " << fileName.filePath().local8Bit() << " WKT from ESRI" << std::endl; 
#endif 
      // morph it
      mySpatialRefSys->morphFromESRI();
    }

    // get the proj4 text
    char * ppszProj4;
    mySpatialRefSys->exportToProj4( &ppszProj4 );

    std::cout << "vvvvvvvvvvvvvvvvv PROJ4 TEXT vvvvvvvvvvvvvvv" << std::endl; 
    std::cout << ppszProj4 << std::endl; 
    std::cout << "^^^^^^^^^^^^^^^^^ PROJ4 TEXT ^^^^^^^^^^^^^^^" << std::endl; 

    char    *pszWKT = NULL;

    mySpatialRefSys->exportToWkt( &pszWKT );
    QString myWKTString = QString(pszWKT);
    OGRFree(pszWKT);  

    return myWKTString;
  }
} // QgsOgrProvider::getProjectionWKT



QString QgsOgrProvider::storageType() const
{
  // Delegate to the driver loaded in by OGR
  return imp_->ogrDriverName;
} // QgsOgrProvider::storageType()



/**
 * Get the first feature resulting from a select operation
 * @return QgsFeature
 */
QgsFeature * QgsOgrProvider::getFirstFeature(bool fetchAttributes, int dataSourceLayerNum)
{
  QgsFeature * f = 0;

  if( imp_->valid )
  {
    QgsDebug( "getting first feature\n" );

    imp_->ogrLayer->ResetReading();

    OGRFeature * feat = imp_->ogrLayer->GetNextFeature();

    Q_CHECK_PTR( feat  );

    if(feat)
    {
      QgsDebug( "First feature is not null" );
    }
    else
    {
      QgsDebug( "First feature is null" );

      return 0x0; // so return a null indicating that we couldn't get the first feature
    }

    // get the feature type name, if any

    OGRFeatureDefn * featureDefinition = feat->GetDefnRef();
    QString featureTypeName =   
      featureDefinition ? QString(featureDefinition->GetName()) : QString("");

    f = new QgsFeature(feat->GetFID(), featureTypeName );

    Q_CHECK_PTR( f );

    if ( ! f )                 // return null if we can't get a new QgsFeature
    {
      delete feat;

      return 0x0;
    }

    size_t geometry_size = feat->GetGeometryRef()->WkbSize();

    f->setGeometryAndOwnership(getGeometryPointer_(feat,static_cast<OGRwkbByteOrder>(endian())), 
                               geometry_size);

    if(fetchAttributes)
    {
      getFeatureAttributes(feat, f);
    }

    delete feat;

  }

  return f;

} // QgsOgrProvider::getFirstFeature()




bool QgsOgrProvider::getNextFeature(QgsFeature &f, bool fetchAttributes, int dataSourceLayerNum)
{
  bool returnValue;

  if (imp_->valid)
  {
    //std::cerr << "getting next feature\n";
    // skip features without geometry

    OGRFeature *fet;

    while ( (fet = imp_->ogrLayer->GetNextFeature()) ) 
    {
      if ( fet->GetGeometryRef() )
      { break; }
    }

    if (fet)
    {
      OGRGeometry * geom = fet->GetGeometryRef();

      // get the wkb representation
      unsigned char *feature = new unsigned char[geom->WkbSize()];

      geom->exportToWkb((OGRwkbByteOrder) endian(), feature);

      f.setFeatureId(fet->GetFID());
      f.setGeometryAndOwnership(feature, geom->WkbSize());

      OGRFeatureDefn * featureDefinition = fet->GetDefnRef();
      QString featureTypeName =   
        featureDefinition ? QString(featureDefinition->GetName()) : QString("");
      f.typeName( featureTypeName );

      if(fetchAttributes)
      {
        getFeatureAttributes(fet, &f);
      }
      /*   char *wkt = new char[2 * geom->WkbSize()];
           geom->exportToWkt(&wkt);
           f->setWellKnownText(wkt);
           delete[] wkt;  */

      delete fet;

      returnValue = true;

    } else 
    {
#ifdef QGISDEBUG
      QgsDebug( "Feature is null" );
      f.setValid(false);
      returnValue = false;
#endif
      // probably should reset reading here
      imp_->ogrLayer->ResetReading();
    }

  } else
  {
    QgsDebug( "Read attempt on an invalid shapefile data source" );
  }

  return returnValue;

} // QgsOgrProvider::getNextFeature()



/**
 * Get the next feature resutling from a select operation
 * Return 0 if there are no features in the selection set
 * @return QgsFeature
 */
QgsFeature * QgsOgrProvider::getNextFeature(bool fetchAttributes, int dataSourceLayerNum)
{
   if( ! imp_->valid )
   {
       QgsDebug( "Read attempt on an invalid shapefile data source" );
       return 0;
   }
   
   OGRFeature * fet;
   OGRGeometry * geom;
   QgsFeature * f = 0;

   while( (fet = imp_->ogrLayer->GetNextFeature()) )
   {
     
     if ( fet->GetGeometryRef() )
     {
         geom = fet->GetGeometryRef();

         // get the wkb representation

         // XXX this happens more than once -- generalize code?
         unsigned char *feature = new unsigned char[geom->WkbSize()];

         geom->exportToWkb((OGRwkbByteOrder) endian(), feature);
         OGRFeatureDefn * featureDefinition = fet->GetDefnRef();
         QString featureTypeName =   
             featureDefinition ? QString(featureDefinition->GetName()) : QString("");

         f = new QgsFeature(fet->GetFID(), featureTypeName);

         f->setGeometryAndOwnership(feature, geom->WkbSize());

         if( fetchAttributes  /*|| !mAttributeFilter.isEmpty()*/ )
         {
             getFeatureAttributes(fet, f);
	   
             // filtering by attribute
             // TODO: improve, speed up!
/*        if (!mAttributeFilter.isEmpty() && !mAttributeFilter.tree()->checkAgainst(f->attributeMap()))
          {
          delete fet;
          continue;
          }    */
         }
    
         if( imp_->mUseIntersect )
         {
             geos::Geometry *geosGeom = 0;

       // XXX This if/else block fixes the endian issue on 
       //     XDR (big endian) platforms for release 0.7. This
       //     code should be revisited to see if there is a more
       //     efficient way to create the geos geometry and to push
       //     the job down to QgsFeature instead of the mix we have
       //     here. The side-effect of this fix is extremely slow
       //     performance on when identifying or selecting multipart
       //     features on XDR platforms.
             if (endian() == QgsDataProvider::XDR)
             {
                 // big endian -- use wkt method
                 geom  =  fet->GetGeometryRef();

                 char *wkt = new char[2 * f->getGeometrySize()];

                 assert(wkt);

                 geom->exportToWkt(&wkt);
                 geosGeom = imp_->wktReader->read(wkt);
             }
             else
             {
                 // little endian -- use QgsFeature method
                 geosGeom = f->geosGeometry();
             }

             assert(geosGeom != 0);
         
             char *sWkt = new char[2 * imp_->mSelectionRectangle->WkbSize()];

             imp_->mSelectionRectangle->exportToWkt(&sWkt);

             geos::Geometry *geosRect = imp_->wktReader->read(sWkt);

             assert(geosGeom);

             if( geosGeom->intersects(geosRect) )
             {
                 QgsDebug("intersection found");

                 delete[] sWkt;
                 delete geosGeom;

                 break;
             }
             else
             {
                 QgsDebug("no intersection found");

                 delete[] sWkt;
                 delete geosGeom;

                 delete f;

                 f = 0;
             }
         }
         else
         {
             break;
         }
     }

     delete fet;

   }

   return f;
} // QgsOgrProvider::getNextFeature()



/**
 * Get the next feature resutling from a select operation
 * Return 0 if there are no features in the selection set
 * @return QgsFeature
 */
/*QgsFeature *QgsOgrProvider::getNextFeature(bool fetchAttributes)
{

  QgsFeature *f = 0;
  if(valid){
    OGRFeature *fet;
    OGRGeometry *geom;
    while ((fet = ogrLayer->GetNextFeature()) != NULL) {
      if (fet->GetGeometryRef())
      {
        if(mUseIntersect)
        {
          geom  =  fet->GetGeometryRef();
          char *wkt = new char[2 * geom->WkbSize()];
          geom->exportToWkt(&wkt);
          geos::Geometry *geosGeom = wktReader->read(wkt);
          assert(geosGeom != 0);
          std::cerr << "Geometry type of geos object is : " << geosGeom->getGeometryType() << std::endl; 
          // get the selection rectangle and create a geos geometry from it
          char *sWkt = new char[2 * mSelectionRectangle->WkbSize()];
          mSelectionRectangle->exportToWkt(&sWkt);
          std::cerr << "Passing " << sWkt << " to goes\n";    
          geos::Geometry *geosRect = wktReader->read(sWkt);
          assert(geosRect != 0);
          std::cerr << "About to apply intersects function\n";
          // test the geometry
          if(geosGeom->intersects(geosRect))
          {
            std::cerr << "Intersection found\n";
            break;
          }
          //XXX For some reason deleting these on win32 causes segfault
          //XXX Someday I'll figure out why...
          //delete[] wkt;  
          //delete[] sWkt;  
        }
        else
        {
          break;
        }
      }
    }
    if(fet){
      geom = fet->GetGeometryRef();

      // get the wkb representation
      unsigned char *feature = new unsigned char[geom->WkbSize()];
      geom->exportToWkb((OGRwkbByteOrder) endian(), feature);

      OGRFeatureDefn * featureDefinition = fet->GetDefnRef();
      QString featureTypeName =   
        featureDefinition ? QString(featureDefinition->GetName()) : QString("");

      f = new QgsFeature(fet->GetFID(), featureTypeName);
      f->setGeometryAndOwnership(feature, geom->WkbSize());

      if(fetchAttributes){
        getFeatureAttributes(fet, f);
      }
      delete fet;
    }else{
#ifdef QGISDEBUG
      std::cerr << "Feature is null\n";
#endif
      // probably should reset reading here
      ogrLayer->ResetReading();
    }


  }else{
#ifdef QGISDEBUG    
    std::cerr << "Read attempt on an invalid shapefile data source\n";
#endif
  }
  return f;
}*/


QgsFeature * QgsOgrProvider::getNextFeature(std::list<int> const& attlist, 
                                            int featureQueueSize, 
                                            int dataSourceLayerNum)
{
  QgsFeature * f = 0; 

  if( imp_->valid )
  {
    // skip features without geometry
    OGRFeature *fet;

    while ( (fet = imp_->ogrLayer->GetNextFeature()) ) 
    {

      if (fet->GetGeometryRef())
      {
        if(imp_->mUseIntersect)
        {
          // test this geometry to see if it should be
          // returned 
#ifdef QGISDEBUG2 
            QgsDebug( "Testing geometry using intersect" );
#endif 
        }
        else
        {
#ifdef QGISDEBUG2 
            QgsDebug( "Testing geometry using mbr" );
#endif 
            break;
        }
      }
    }

    if( fet )
    {
      OGRGeometry * geom = fet->GetGeometryRef();

      // get the wkb representation

      // XXX again, redundant?  Generalizable?
      unsigned char *feature = new unsigned char[geom->WkbSize()];
      geom->exportToWkb((OGRwkbByteOrder) endian(), feature);
      OGRFeatureDefn * featureDefinition = fet->GetDefnRef();

      QString featureTypeName =   
        featureDefinition ? QString(featureDefinition->GetName()) : QString("");

      f = new QgsFeature(fet->GetFID(), featureTypeName);
      f->setGeometryAndOwnership(feature, geom->WkbSize());

      for(std::list<int>::const_iterator it=attlist.begin();
          it!=attlist.end();
          ++it)
      {
        getFeatureAttribute(fet,f,*it);
      }

      delete fet;

      //delete [] feature;
    }
    else
    {
      QgsDebug( "Feature is null" );

      // probably should reset reading here
      imp_->ogrLayer->ResetReading();
    }
  }
  else
  {
    QgsDebug( "Read attempt on an invalid shapefile data source" );
  }

  return f;

} // getNextFeature()




/**
 * Select features based on a bounding rectangle. Features can be retrieved
 * with calls to getFirstFeature and getNextFeature.
 * @param mbr QgsRect containing the extent to use in selecting features
 * @param useIntersect If true, an intersect test will be used in selecting
 * features. In OGR, this is a two pass affair. The mUseIntersect value is
 * stored. If true, a secondary filter (using GEOS) is applied to each
 * feature in the getNextFeature function.
 */
void QgsOgrProvider::select(QgsRect *rect, bool useIntersect, int dataSourceLayerNum)
{
  imp_->mUseIntersect = useIntersect;

  // spatial query to select features
  std::cerr << "Selection rectangle is " << *rect << std::endl;

  OGRGeometry * filter = 0;

  filter = new OGRPolygon();

  QString wktExtent = QString("POLYGON ((%1))").arg(rect->asPolygon());

  const char *wktText = (const char *)wktExtent;

  if (useIntersect)
  {
    // store the selection rectangle for use in filtering features during
    // an identify and display attributes
    //    delete mSelectionRectangle;
    imp_->mSelectionRectangle = new OGRPolygon();
    imp_->mSelectionRectangle->importFromWkt((char **)&wktText);
  }

  // reset the extent for the ogr filter
  wktExtent = QString("POLYGON ((%1))").arg(rect->asPolygon());
  wktText = (const char *)wktExtent;

  OGRErr result = ((OGRPolygon *) filter)->importFromWkt((char **)&wktText);
  //TODO - detect an error in setting the filter and figure out what to
  //TODO   about it. If setting the filter fails, all records will be returned
  if (result == OGRERR_NONE) 
  {
    std::cerr << "Setting spatial filter using " << wktExtent.local8Bit() << std::endl;
    imp_->ogrLayer->SetSpatialFilter(filter);
    //ogrLayer->SetSpatialFilterRect(rect->xMin(), rect->yMin(), rect->xMax(), rect->yMax());
  } else 
  {
      QgsDebug( "Setting spatial filter failed!" );
#ifdef QGISDEBUG    
      assert(result == OGRERR_NONE);
#endif
  }
  
  delete filter;
  
} // QgsOgrProvider::select



/**
 * Identify features within the search radius specified by rect
 * @param rect Bounding rectangle of search radius
 * @return std::vector containing QgsFeature objects that intersect rect
 */
void QgsOgrProvider::identify(QgsRect * rect, int dataSourceLayerNum)
{
  // select the features
  select(rect);

#ifdef WIN32
  //TODO fix this later for win32
  // std::vector<QgsFeature> feat;

  // return feat;
#endif

} // QgsOgrProvider::identify() 



// TODO - make this function return the real extent_
QgsRect *QgsOgrProvider::extent()
{
  // TODO: Find out where this new QgsRect is being lost (as reported by valgrind)

  return new QgsRect(imp_->extent_->MinX, imp_->extent_->MinY, imp_->extent_->MaxX, imp_->extent_->MaxY);
} // QgsOgrProvider::extend()



size_t QgsOgrProvider::layerCount() const
{
    return imp_->ogrDataSource->GetLayerCount();
} // QgsOgrProvider::layerCount()


/** 
 * Return the feature type
 */
int QgsOgrProvider::geometryType(int dataSourceLayerNum) const
{
  return imp_->geomType;
} // QgsOgrProvider::geometryType()



/** 
 * Return the feature type
 */
long QgsOgrProvider::featureCount(int dataSourceLayerNum) const
{
  return imp_->numberFeatures;
} // QgsOgrProvider::featureCount()



/**
 * Return the number of fields
 */
int QgsOgrProvider::fieldCount(int dataSourceLayerNum) const
{
  return imp_->attributeFields.size();
} // QgsOgrProvider::fieldCount()




void QgsOgrProvider::getFeatureAttribute(OGRFeature * ogrFet, 
                                         QgsFeature * f, 
                                         int attindex, 
                                         int dataSourceLayerNum)
{
  OGRFieldDefn * fldDef = ogrFet->GetFieldDefnRef(attindex);

  if ( ! fldDef )
  {
    qDebug( "%s:%d ogrFet->GetFieldDefnRef(attindex) returns NULL", 
        __FILE__, __LINE__ );
    return;
  }

  QString fld = fldDef->GetNameRef();
  QCString cstr(ogrFet->GetFieldAsString(attindex));

  bool numeric = imp_->attributeFields[attindex].isNumeric();

  f->addAttribute(fld, mEncoding->toUnicode(cstr), numeric);
} // QgsOgrProvider::getFeatureAttribute()



/**
 * Fetch attributes for a selected feature
 */
void QgsOgrProvider::getFeatureAttributes(OGRFeature *ogrFet, 
                                          QgsFeature *f, 
                                          int dataSourceLayerNum)
{
  for (int i = 0; i < ogrFet->GetFieldCount(); i++) 
  {
    getFeatureAttribute(ogrFet,f,i);
  
    // add the feature attributes to the tree
    /*OGRFieldDefn *fldDef = ogrFet->GetFieldDefnRef(i);
      QString fld = fldDef->GetNameRef();
    //    OGRFieldType fldType = fldDef->GetType();
    QString val;

    val = ogrFet->GetFieldAsString(i);
    f->addAttribute(fld, val);*/
  }
} // QgsOgrProvider::getFeatureAttributes()



std::vector<QgsField> const & QgsOgrProvider::fields(int dataSourceLayerNum) const
{
  return imp_->attributeFields;
} // QgsOgrProvider::fields()



void QgsOgrProvider::reset(int dataSourceLayerNum)
{
  // TODO: check whether it supports normal SQL or only that "restricted_sql"
  if (mAttributeFilter.isEmpty())
  {
    imp_->ogrLayer->SetAttributeFilter(NULL);
  }
  else
  {
    imp_->ogrLayer->SetAttributeFilter(mAttributeFilter.string());
  }

  imp_->ogrLayer->SetSpatialFilter(0);
  imp_->ogrLayer->ResetReading();

  // Reset the use intersect flag on a provider reset, otherwise only the last
  // selected feature(s) will be displayed when the attribute table
  // is opened. 
  //XXX In a future release, this "feature" can be used to implement
  // the display of only selected records in the attribute table.
  imp_->mUseIntersect = false;

} // QgsOgrProvider::reset()




QString QgsOgrProvider::minValue(int position, int dataSourceLayerNum)
{
  if ( position >= fieldCount() )
  {
      QgsDebug( "Warning: access requested to invalid position in QgsOgrProvider::minValue(..)" );
  }

  if ( imp_->minmaxcachedirty )
  {
    fillMinMaxCache();
  }

  return QString::number(imp_->minmaxcache[position][0],'f',2);
} // QgsOgrProvider::minValue()



QString QgsOgrProvider::maxValue(int position, int dataSourceLayerNum)
{
  if( position >= fieldCount() )
  {
      QgsDebug( "Warning: access requested to invalid position in QgsOgrProvider::maxValue(..)" );
  }

  if( imp_->minmaxcachedirty )
  {
    fillMinMaxCache();
  }

  return QString::number(imp_->minmaxcache[position][1],'f',2);
} // QgsOgrProvider::maxValue()



void QgsOgrProvider::fillMinMaxCache()
{
  for(int i = 0; i < fieldCount(); i++)
  {
    imp_->minmaxcache[i][0]=DBL_MAX;
    imp_->minmaxcache[i][1]=-DBL_MAX;
  }

  QgsFeature* f = getFirstFeature(true);

  // XXX this could be implemented using std::min() and std::max()
  do
  {
    for(int i = 0; i < fieldCount(); i++)
    {
      double value = (f->attributeMap())[i].fieldValue().toDouble();

      if( value < imp_->minmaxcache[i][0])
      {
        imp_->minmaxcache[i][0]=value;  
      }  
      if(value > imp_->minmaxcache[i][1])
      {
        imp_->minmaxcache[i][1]=value;  
      }
    }

    delete f;

  } while( f = getNextFeature(true) );

  imp_->minmaxcachedirty = false;

} // QgsOgrProvider::fillMinMaxCache



//TODO - add sanity check for shape file layers, to include cheking to
//       see if the .shp, .dbf, .shx files are all present and the layer
//       actually has features
bool QgsOgrProvider::isValid() const
{
  return imp_->valid;
} // QgsOgrProvider::isValid()




bool QgsOgrProvider::addFeature(QgsFeature* f, int dataSourceLayerNum)
{ 
  QgsDebug( "in add Feature" );

  bool returnValue = true;

  OGRFeatureDefn* fdef = imp_->ogrLayer->GetLayerDefn();
  OGRFeature* feature = new OGRFeature(fdef);

  QGis::WKBTYPE ftype;

  memcpy(&ftype, (f->getGeometry()+1), sizeof(int));

  switch(ftype)
  {
    case QGis::WKBPoint:
      {
        OGRPoint * p = new OGRPoint();
        p->importFromWkb(f->getGeometry(),1+sizeof(int)+2*sizeof(double));
        feature->SetGeometry(p);
        break;
      }
    case QGis::WKBLineString:
      {
        OGRLineString* l = new OGRLineString();
        int length;
        memcpy(&length,f->getGeometry()+1+sizeof(int),sizeof(int));
        l->importFromWkb(f->getGeometry(),1+2*sizeof(int)+2*length*sizeof(double));
        feature->SetGeometry(l);
        break;
      }
    case QGis::WKBPolygon:
      {
        OGRPolygon* pol = new OGRPolygon();
        int numrings;
        int totalnumpoints=0;
        int numpoints;//number of points in one ring
        unsigned char* ptr=f->getGeometry()+1+sizeof(int);
        memcpy(&numrings,ptr,sizeof(int));
        ptr+=sizeof(int);
        for(int i=0;i<numrings;++i)
        {
          memcpy(&numpoints,ptr,sizeof(int));
          ptr+=sizeof(int);
          totalnumpoints+=numpoints;
          ptr+=(2*sizeof(double));
        }
        pol->importFromWkb(f->getGeometry(),1+2*sizeof(int)+numrings*sizeof(int)+totalnumpoints*2*sizeof(double));
        feature->SetGeometry(pol);
        break;
      }
    case QGis::WKBMultiPoint:
      {
        OGRMultiPoint* multip= new OGRMultiPoint();
        int count;
        //determine how many points
        memcpy(&count,f->getGeometry()+1+sizeof(int),sizeof(int));
        multip->importFromWkb(f->getGeometry(),1+2*sizeof(int)+count*2*sizeof(double));
        feature->SetGeometry(multip);
        break;
      }
    case QGis::WKBMultiLineString:
      {
        OGRMultiLineString* multil=new OGRMultiLineString();
        int numlines;
        memcpy(&numlines,f->getGeometry()+1+sizeof(int),sizeof(int));
        int totalpoints=0;
        int numpoints;//number of point in one line
        unsigned char* ptr=f->getGeometry()+9;
        for(int i=0;i<numlines;++i)
        {
          memcpy(&numpoints,ptr,sizeof(int));
          ptr+=4;
          for(int j=0;j<numpoints;++j)
          {
            ptr+=16;
            totalpoints+=2;
          }
        }
        int size=1+2*sizeof(int)+numlines*sizeof(int)+totalpoints*2*sizeof(double);
        multil->importFromWkb(f->getGeometry(),size);
        feature->SetGeometry(multil);
        break;
      }
    case QGis::WKBMultiPolygon:
      {
        OGRMultiPolygon* multipol=new OGRMultiPolygon();
        int numpolys;
        memcpy(&numpolys,f->getGeometry()+1+sizeof(int),sizeof(int));
        int numrings;//number of rings in one polygon
        int totalrings=0;
        int totalpoints=0;
        int numpoints;//number of points in one ring
        unsigned char* ptr=f->getGeometry()+9;

        for(int i=0;i<numpolys;++i)
        {
          memcpy(&numrings,ptr,sizeof(int));
          ptr+=4;
          for(int j=0;j<numrings;++j)
          {
            totalrings++;
            memcpy(&numpoints,ptr,sizeof(int));
            for(int k=0;k<numpoints;++k)
            {
              ptr+=16;
              totalpoints+=2;
            }
          }
        }
        int size=1+2*sizeof(int)+numpolys*sizeof(int)+totalrings*sizeof(int)+totalpoints*2*sizeof(double);
        multipol->importFromWkb(f->getGeometry(),size);
        feature->SetGeometry(multipol);
        break;
      }
  }

  //add possible attribute information
  QgsDebug("before attribute commit section");

  for(int i=0;i<f->attributeMap().size();++i)
  {
    QString s=(f->attributeMap())[i].fieldValue();

    QgsDebug( QString("adding attribute: "+s).ascii() );

    if(!s.isEmpty())
    {
      if(fdef->GetFieldDefn(i)->GetType()==OFTInteger)
      {
        feature->SetField(i,s.toInt());

        QgsDebug( QString("OFTInteger, attribute value: "+s.toInt()).ascii() );

      }
      else if(fdef->GetFieldDefn(i)->GetType()==OFTReal)
      {
        feature->SetField(i,s.toDouble());

        QgsDebug( QString("OFTReal, attribute value: "+QString::number(s.toDouble(),'f',3)).ascii() );

      }
      else if(fdef->GetFieldDefn(i)->GetType()==OFTString)
      {
	  feature->SetField(i,s.ascii());
#ifdef QGISDEBUG
        qWarning("OFTString, attribute value: "+QString(s.ascii()));
#endif
      }
      else
      {
        QgsDebug("no type found");
      }
    }
  }

  if(imp_->ogrLayer->CreateFeature(feature)!=OGRERR_NONE)
  {
    //writing failed
    QMessageBox::warning (0, "Warning", "Writing of the feature failed",
        QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );

    returnValue = false;
  }

  ++imp_->numberFeatures;

  delete feature;

  imp_->ogrLayer->SyncToDisk();

  return returnValue;

} // QgsOgrProvider::addFeature()




bool QgsOgrProvider::addFeatures(std::list<QgsFeature*> const flist, int dataSourceLayerNum)
{
  bool returnvalue = true;

  for(std::list<QgsFeature*>::const_iterator it=flist.begin();it!=flist.end();++it)
  {
    if(!addFeature(*it))
    {
      returnvalue = false;
    }
  }
  return returnvalue;
} // QgsOgrProvider::addFeatures()



bool QgsOgrProvider::addAttributes(std::map<QString,QString> const & name, int dataSourceLayerNum)
{
    bool returnvalue = true;

    for(std::map<QString,QString>::const_iterator iter = name.begin();
        iter!=name.end();
        ++iter)
    {
	if(iter->second == "OFTInteger")
	{
	    OGRFieldDefn fielddefn(iter->first,OFTInteger);
	    if(imp_->ogrLayer->CreateField(&fielddefn)!=OGRERR_NONE)
	    {
		QgsDebug("writing of the field failed");	

		returnvalue = false;
	    }
	}
	else if(iter->second == "OFTReal")
	{
	    OGRFieldDefn fielddefn(iter->first,OFTReal);
	    if(imp_->ogrLayer->CreateField(&fielddefn)!=OGRERR_NONE)
	    {
		QgsDebug("writing of the field failed");

		returnvalue = false;
	    }
	}
	else if(iter->second == "OFTString")
	{
	    OGRFieldDefn fielddefn(iter->first,OFTString);
	    if(imp_->ogrLayer->CreateField(&fielddefn)!=OGRERR_NONE)
	    {
		QgsDebug("writing of the field failed");

		returnvalue = false;
	    }
	}
	else
	{
	    QgsDebug("QgsOgrProvider.cpp: type not found");

	    returnvalue = false;
	}
    }

    return returnvalue;

} // QgsOgrProvider::addAttributes


bool QgsOgrProvider::changeAttributeValues(std::map<int,std::map<QString,QString> > const & attr_map, 
                                           int dataSourceLayerNum)
{
#ifdef QGISDEBUG
  std::cerr << "QgsOgrProvider::changeAttributeValues()" << std::endl;
#endif
    
  std::map<int,std::map<QString,QString> > am = attr_map; // stupid, but I don't know other way to convince gcc to compile

  for( std::map<int,std::map<QString,QString> >::iterator it=am.begin();it!=am.end();++it)
  {
    long fid = (long) (*it).first;

    OGRFeature * of = imp_->ogrLayer->GetFeature ( fid );

    if ( !of ) 
    {
        QMessageBox::warning (0, "Warning", "Cannot read feature, cannot change attributes" );
	return false;
    }

    std::map<QString,QString> attr = (*it).second;

    for( std::map<QString,QString>::iterator it2 = attr.begin(); it2!=attr.end(); ++it2 )
    {
	QString name = (*it2).first;
	QString value = (*it2).second;
		
	int fc = of->GetFieldCount();

	for ( int f = 0; f < fc; f++ ) 
        {
	    OGRFieldDefn *fd = of->GetFieldDefnRef ( f );
	    
	    if ( name.compare( fd->GetNameRef() ) == 0 ) 
            {
		OGRFieldType type = fd->GetType();

#ifdef QGISDEBUG
		std::cerr << "set field " << f << " : " << name.local8Bit() << " to " << value.local8Bit() << std::endl;
#endif
		switch ( type ) 
                {
		    case OFTInteger:
		        of->SetField ( f, value.toInt() );
			break;

		    case OFTReal:
		        of->SetField ( f, value.toDouble() );
			break;

		    case OFTString:
		        of->SetField ( f, value.ascii() );
			break;

		    default:
                        QMessageBox::warning(0, "Warning", "Unknown field type, cannot change attribute" );
			break;
		}

		continue;
	    }	
	}

	imp_->ogrLayer->SetFeature ( of );
    }
  }

  imp_->ogrLayer->SyncToDisk();

  return true;
} // QgsOgrProvider::changeAttributeValues()



bool QgsOgrProvider::createSpatialIndex()
{
    //experimental, try to create a spatial index
    QString filename = getDataSourceUri().section('/',-1,-1);//todo: find out the filename from the uri
    QString layername = filename.section('.',0,0);
    QString sql = "CREATE SPATIAL INDEX ON "+layername;

    imp_->ogrDataSource->ExecuteSQL (sql.ascii(), imp_->ogrLayer->GetSpatialFilter(),"");

    //todo: find out, if the .qix file is there
    QString indexname  =  getDataSourceUri();

    indexname.truncate(getDataSourceUri().length()-filename.length());

    indexname = indexname+layername+".qix";

    QFile indexfile(indexname);

    if(indexfile.exists())
    {
	return true;
    }
    else
    {
	return false;
    }
} // QgsOgrProvider::createSpatialIndex()



int QgsOgrProvider::capabilities() const
{
  int ability = NoCapabilities;

  // collect abilities reported by OGR
  if (imp_->ogrLayer)
  {
    // Whilst the OGR documentation (e.g. at
    // http://www.gdal.org/ogr/classOGRLayer.html#a17) states "The capability
    // codes that can be tested are represented as strings, but #defined
    // constants exists to ensure correct spelling", we always use strings
    // here.  This is because older versions of OGR don't always have all
    // the #defines we want to test for here.

    if (imp_->ogrLayer->TestCapability("RandomRead"))
    // TRUE if the GetFeature() method works for this layer.
    {
      // TODO: Perhaps influence if QGIS caches into memory (vs read from disk every time) based on this setting.
    }

    if (imp_->ogrLayer->TestCapability("SequentialWrite"))
    // TRUE if the CreateFeature() method works for this layer.
    {
      ability |= QgsVectorDataProvider::AddFeatures;
    }

    if (imp_->ogrLayer->TestCapability("RandomWrite"))
    // TRUE if the SetFeature() method is operational on this layer.
    {
      ability |= QgsVectorDataProvider::ChangeAttributeValues;

      // TODO According to http://shapelib.maptools.org/ (Shapefile C Library V1.2)
      // TODO "You can't modify the vertices of existing structures".
      // TODO Need to work out versions of shapelib vs versions of GDAL/OGR
      // TODO And test appropriately.

      // This provider can't change geometries yet anyway (cf. Postgres provider)
      // ability |= QgsVectorDataProvider::ChangeGeometries;
    }

    if (imp_->ogrLayer->TestCapability("FastSpatialFilter"))
    // TRUE if this layer implements spatial filtering efficiently.
    // Layers that effectively read all features, and test them with the 
    // OGRFeature intersection methods should return FALSE.
    // This can be used as a clue by the application whether it should build
    // and maintain it's own spatial index for features in this layer.
    {
      // TODO: Perhaps use as a clue by QGIS whether it should build and maintain it's own spatial index for features in this layer.
    }

    if (imp_->ogrLayer->TestCapability("FastFeatureCount"))
    // TRUE if this layer can return a feature count
    // (via OGRLayer::GetFeatureCount()) efficiently ... ie. without counting
    // the features. In some cases this will return TRUE until a spatial
    // filter is installed after which it will return FALSE.
    {
      // TODO: Perhaps use as a clue by QGIS whether it should spawn a thread to count features.
    }

    if (imp_->ogrLayer->TestCapability("FastGetExtent"))
    // TRUE if this layer can return its data extent 
    // (via OGRLayer::GetExtent()) efficiently ... ie. without scanning
    // all the features. In some cases this will return TRUE until a
    // spatial filter is installed after which it will return FALSE.
    {
      // TODO: Perhaps use as a clue by QGIS whether it should spawn a thread to calculate extent.
    }

    if (imp_->ogrLayer->TestCapability("FastSetNextByIndex"))
    // TRUE if this layer can perform the SetNextByIndex() call efficiently.
    {
      // No use required for this QGIS release.
    }

    QgsDebug( "QgsOgrProvider::capabilities: GDAL Version Num is 'GDAL_VERSION_NUM'." );

    if (1)
    {
      // Ideally this should test for Shapefile type and GDAL >= 1.2.6
      // In reality, createSpatialIndex() looks after itself.
      ability |= QgsVectorDataProvider::CreateSpatialIndex;
    }

  }

  return ability;

/*
    return (QgsVectorDataProvider::AddFeatures
	    | QgsVectorDataProvider::ChangeAttributeValues
	    | QgsVectorDataProvider::CreateSpatialIndex);
*/
} // QgsOgrProvider::capabilities()




QString  QgsOgrProvider::name() const
{
    return TEXT_PROVIDER_KEY;
} // ::name()



QString  QgsOgrProvider::description() const
{
    return TEXT_PROVIDER_DESCRIPTION;
} //  QgsOgrProvider::name()



/**
 * Class factory to return a pointer to a newly created 
 * QgsOgrProvider object
 */
QGISEXTERN QgsOgrProvider * classFactory(const QString *uri)
{
  return new QgsOgrProvider(*uri);
}



/** Required key function (used to map the plugin to a data store type)
*/
QGISEXTERN QString providerKey()
{
  return TEXT_PROVIDER_KEY;
}


/**
 * Required description function 
 */
QGISEXTERN QString description()
{
    return TEXT_PROVIDER_DESCRIPTION;
} 




/**

  Convenience function for readily creating file filters.

  Given a long name for a file filter and a regular expression, return
  a file filter string suitable for use in a QFileDialog::OpenFiles()
  call.  The regular express, glob, will have both all lower and upper
  case versions added.

  @note

  Copied from qgisapp.cpp.  

  @todo XXX This should probably be generalized and moved to a standard
            utility type thingy.

*/
static QString createFileFilter_(QString const &longName, QString const &glob)
{
    return "[OGR] " + 
           longName + " (" + glob.lower() + " " + glob.upper() + ");;";
} // createFileFilter_




QGISEXTERN QString fileVectorFilters()
{
    static QString myFileFilters;

    // if we've already built the supported vector string, just return what
    // we've already built
    if ( ! ( myFileFilters.isEmpty() || myFileFilters.isNull() ) )
    {
        return myFileFilters;
    }

    // first get the GDAL driver manager

    OGRSFDriverRegistrar *driverRegistrar = OGRSFDriverRegistrar::GetRegistrar();

    if (!driverRegistrar)
    {
        QMessageBox::warning(0,"OGR Driver Manager","unable to get OGRDriverManager");
        return "";              // XXX good place to throw exception if we
    }                           // XXX decide to do exceptions

    // then iterate through all of the supported drivers, adding the
    // corresponding file filter

    OGRSFDriver *driver;          // current driver

    QString driverName;           // current driver name

    // Grind through all the drivers and their respective metadata.
    // We'll add a file filter for those drivers that have a file
    // extension defined for them; the others, welll, even though
    // theoreticaly we can open those files because there exists a
    // driver for them, the user will have to use the "All Files" to
    // open datasets with no explicitly defined file name extension.
#ifdef QGISDEBUG

    std::cerr << "Driver count: " << driverRegistrar->GetDriverCount() << std::endl;
#endif

    for (int i = 0; i < driverRegistrar->GetDriverCount(); ++i)
    {
        driver = driverRegistrar->GetDriver(i);

        Q_CHECK_PTR(driver);

        if (!driver)
        {
            qWarning("unable to get driver %d", i);
            continue;
        }

        driverName = driver->GetName();


        if (driverName.startsWith("ESRI"))
        {
            myFileFilters += createFileFilter_("ESRI Shapefiles", "*.shp");
        }
        else if (driverName.startsWith("UK"))
        {
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("SDTS"))
        {
            myFileFilters += createFileFilter_( "Spatial Data Transfer Standard",
                                                "*catd.ddf" );
        }
        else if (driverName.startsWith("TIGER"))
        {
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("S57"))
        {
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("MapInfo"))
        {
            myFileFilters += createFileFilter_("MapInfo", "*.mif *.tab");
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("DGN"))
        {
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("VRT"))
        {
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("AVCBin"))
        {
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("REC"))
        {
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("Memory"))
        {
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("Jis"))
        {
            // XXX needs file filter extension
        }
        else if (driverName.startsWith("GML"))
        {
            // XXX not yet supported; post 0.1 release task
            myFileFilters += createFileFilter_( "Geography Markup Language",
                                                "*.gml" );
        }
        else
        {
            // NOP, we don't know anything about the current driver
            // with regards to a proper file filter string
            qDebug( "%s:%d unknown driver %s", __FILE__, __LINE__, (const char *)driverName.local8Bit() );
        }

    }                           // each loaded GDAL driver

    std::cout << myFileFilters.local8Bit() << std::endl;

    // can't forget the default case

    myFileFilters += "All files (*.*)";

    return myFileFilters;

} // fileVectorFilters() const



QString QgsOgrProvider::fileVectorFilters() const
{
    return fileVectorFilters();
} // QgsOgrProvider::fileVectorFilters() const



/**
 * Required isProvider function. Used to determine if this shared library
 * is a data provider plugin
 */
QGISEXTERN bool isProvider()
{
  return true;
}

QGISEXTERN bool createEmptyDataSource(const QString& uri,const QString& format, QGis::WKBTYPE vectortype)
{
  //hard coded for the moment
  OGRwkbGeometryType geomtype=(OGRwkbGeometryType)((int)vectortype);
  QString mOutputFormat = "ESRI Shapefile";
  QString mOutputFileName = uri;
#ifdef WIN32 
  QString outname=mOutputFileName.mid(mOutputFileName.findRev("\\")+1,mOutputFileName.length());
#else
  QString outname=mOutputFileName.mid(mOutputFileName.findRev("/")+1,mOutputFileName.length());
#endif
  OGRSFDriverRegistrar* driverregist = OGRSFDriverRegistrar::GetRegistrar();

  if(driverregist==0)
  {
    return false;
  }
  OGRSFDriver* driver = driverregist->GetDriverByName(mOutputFormat);
  if(driver==0)
  {
    return false;
  }
  OGRDataSource* datasource = driver->CreateDataSource(mOutputFileName,NULL);
  if(datasource==0)
  {
    return false;
  }

  OGRSpatialReference reference;
  OGRLayer* layer = datasource->CreateLayer(outname.latin1(),&reference,geomtype,NULL);
  if(layer==0)
  {
    return false;
  }

  //create a dummy field
  OGRFieldDefn fielddef("dummy",OFTReal);
  fielddef.SetWidth(1);
  fielddef.SetPrecision(1);
  if(layer->CreateField(&fielddef,FALSE)!=OGRERR_NONE)
  {
    return false;
  }

  int count=layer->GetLayerDefn()->GetFieldCount();
#ifdef QGISDEBUG
  qWarning("Field count is: "+QString::number(count));
#endif
  //just for a test: create a dummy featureO
  /*OGRFeatureDefn* fdef=layer->GetLayerDefn();
    OGRFeature* feature=new OGRFeature(fdef);
    OGRPoint* p=new OGRPoint();
    p->setX(700000);
    p->setY(300000);
    feature->setGeometryAndOwnership(p);
    if(layer->CreateFeature(feature)!=OGRERR_NONE)
    {
    qWarning("errrrrrrrrrror!");
    }*/

  if(layer->SyncToDisk()!=OGRERR_NONE)
  {
    return false;
  }

  return true;

  /*OGRLayerH mLayerHandle;
    OGRRegisterAll();
    OGRSFDriverH myDriverHandle = OGRGetDriverByName( mOutputFormat );

    if( myDriverHandle == NULL )
    {
    std::cout << "Unable to find format driver named " << mOutputFormat << std::endl;
    return false;
    }

    OGRDataSourceH mDataSourceHandle = OGR_Dr_CreateDataSource( myDriverHandle, mOutputFileName, NULL );
    if( mDataSourceHandle == NULL )
    {
    std::cout << "Datasource handle is null! " << std::endl;
    return false;
    }

  //define the spatial ref system
  OGRSpatialReferenceH mySpatialReferenceSystemHandle = NULL;

  QString myWKT = NULL; //WKT = Well Known Text
  //sample below shows how to extract srs from a raster
  //    const char *myWKT = GDALGetProjectionRef( hBand );

  if( myWKT != NULL && strlen(myWKT) != 0 )
  {
  mySpatialReferenceSystemHandle = OSRNewSpatialReference( myWKT );
  }
  //change 'contour' to something more useful!
#ifdef QGISDEBUG
qWarning("mOutputFileName: "+mOutputFileName);
#endif //QGISDEBUG


#ifdef QGISDEBUG
qWarning("outname: "+outname);
#endif //QGISDEBUG

mLayerHandle = OGR_DS_CreateLayer( mDataSourceHandle, outname, 
mySpatialReferenceSystemHandle, geomtype, NULL );

if( mLayerHandle == NULL )
{
std::cout << "Error layer handle is null!" << std::endl;
return false;
}
else
{
std::cout << "File handle created!" << std::endl;
}

OGRFieldDefnH myFieldDefinitionHandle;
myFieldDefinitionHandle = OGR_Fld_Create( "dummy",OFTReal);
OGR_Fld_SetWidth( myFieldDefinitionHandle,1);
OGR_Fld_SetPrecision( myFieldDefinitionHandle,1);
OGR_L_CreateField( mLayerHandle, myFieldDefinitionHandle, FALSE );
OGR_Fld_Destroy( myFieldDefinitionHandle );

return true;*/
  }



list<QgsMapLayer*> 
QgsOgrProvider::createLayers()
{
    list<QgsMapLayer*> layers;

    // for each OGR layer create a QgsVectorLayer
    if ( ! imp_->ogrDataSource )
    {
        QgsDebug( "no OGR data source" );
        return layers;
    }

    size_t numLayers = layerCount();

    

    // TODO finish rest

    return layers;

} // QgsOgrProvider::createLayers()

