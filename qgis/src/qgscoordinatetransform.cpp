/***************************************************************************
               QgsCoordinateTransform.cpp  - Coordinate Transforms
                             -------------------
    begin                : Dec 2004
    copyright            : (C) 2004 Tim Sutton
    email                : tim at linfiniti.com
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
#include "qgscoordinatetransform.h"

QgsCoordinateTransform::QgsCoordinateTransform( QString theSourceWKT, QString theDestWKT ) : QObject()
{
  mSourceWKT = theSourceWKT;
  mDestWKT = theDestWKT;
  //XXX Who spells initialize initialise?
  initialise();
}

QgsCoordinateTransform::~QgsCoordinateTransform()
{
  delete mSourceToDestXForm;
  delete mDestToSourceXForm;
}

void QgsCoordinateTransform::setSourceWKT(QString theWKT)
{
  mSourceWKT = theWKT;
  initialise();
}
void QgsCoordinateTransform::setDestWKT(QString theWKT)
{
#ifdef QGISDEBUG
  std::cout << "QgsCoordinateTransform::setDestWKT called" << std::endl;
#endif
  mDestWKT = theWKT;
  initialise();
}

void QgsCoordinateTransform::initialise()
{
  mInitialisedFlag=false; //guilty until proven innocent...
  //default to geo / wgs84 for now .... later we will make this user configurable
  QString myGeoWKT =    "GEOGCS[\"WGS 84\", "
    "  DATUM[\"WGS_1984\", "
    "    SPHEROID[\"WGS 84\",6378137,298.257223563, "
    "      AUTHORITY[\"EPSG\",7030]], "
    "    TOWGS84[0,0,0,0,0,0,0], "
    "    AUTHORITY[\"EPSG\",6326]], "
    "  PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",8901]], "
    "  UNIT[\"DMSH\",0.0174532925199433,AUTHORITY[\"EPSG\",9108]], "
    "  AXIS[\"Lat\",NORTH], "
    "  AXIS[\"Long\",EAST], "
    "  AUTHORITY[\"EPSG\",4326]]";
  //default input projection to geo wgs84  
  if (mSourceWKT.isEmpty())
  {
    mSourceWKT = myGeoWKT;
  }
  //but default output projection to be the same as input proj
  //whatever that may be...
  if (mDestWKT.isEmpty())
  {
    mDestWKT = mSourceWKT;
  }  

  if (mSourceWKT == mDestWKT)
  {
    mShortCircuit=true;
    return;
  }
  else
  {
    mShortCircuit=false;
  }

  OGRSpatialReference myInputSpatialRefSys, myOutputSpatialRefSys;
  //this is really ugly but we need to get a QString to a char**
  char *mySourceCharArrayPointer = (char *)mSourceWKT.ascii();
  char *myDestCharArrayPointer = (char *)mDestWKT.ascii();

  /* Here are the possible OGR error codes :
     typedef int OGRErr;

#define OGRERR_NONE                0
#define OGRERR_NOT_ENOUGH_DATA     1    --> not enough data to deserialize 
#define OGRERR_NOT_ENOUGH_MEMORY   2
#define OGRERR_UNSUPPORTED_GEOMETRY_TYPE 3
#define OGRERR_UNSUPPORTED_OPERATION 4
#define OGRERR_CORRUPT_DATA        5
#define OGRERR_FAILURE             6
#define OGRERR_UNSUPPORTED_SRS     7 */

  OGRErr myInputResult = myInputSpatialRefSys.importFromWkt( & mySourceCharArrayPointer );
  if (myInputResult != OGRERR_NONE)
  {
    std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"<< std::endl;
    std::cout << "The source projection for this layer could *** NOT *** be set " << std::endl;
    std::cout << "INPUT: " << std::endl << mSourceWKT << std::endl;
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
    return;
  }

  OGRErr myOutputResult = myOutputSpatialRefSys.importFromWkt( & myDestCharArrayPointer );
  if (myOutputResult != OGRERR_NONE)
  {
    std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"<< std::endl;
    std::cout << "The dest projection for this layer could *** NOT *** be set " << std::endl;
    std::cout << "OUTPUT: " << std::endl << mDestWKT << std::endl;
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
    return;
  }  

  mSourceToDestXForm = OGRCreateCoordinateTransformation( &myInputSpatialRefSys, &myOutputSpatialRefSys );
  mDestToSourceXForm = OGRCreateCoordinateTransformation( &myOutputSpatialRefSys, &myInputSpatialRefSys );
  if ( ! mSourceToDestXForm || ! mDestToSourceXForm)
  {
    std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"<< std::endl;
    std::cout << "The OGR Coordinate transformation for this layer could *** NOT *** be set " << std::endl;
    std::cout << "INPUT: " << std::endl << mSourceWKT << std::endl;
    std::cout << "OUTPUT: " << std::endl << mDestWKT  << std::endl;
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
    return;
  }
  else
  {
    mInitialisedFlag = true;
    std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"<< std::endl;
    std::cout << "The OGR Coordinate transformation for this layer was set to" << std::endl;
    std::cout << "INPUT: " << std::endl << mSourceWKT << std::endl;
    char *proj4src;
    myInputSpatialRefSys.exportToProj4(&proj4src);
    std::cout << "PROJ4: " << std::endl << proj4src << std::endl;  
    std::cout << "OUTPUT: " << std::endl << mDestWKT  << std::endl;
    char *proj4dest;
    myOutputSpatialRefSys.exportToProj4(&proj4dest);
    std::cout << "PROJ4: " << std::endl << proj4dest << std::endl;  
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
  }
  //just a test to see if inverse 
  //inverseTransform(10.0,10.0);
  // Deactivate GDAL error messages.
  //CPLSetErrorHandler( errorHandler );

  // Guess if the source o dest CS is in degrees.
  //Searchf for this phrase in each wkt:  "unit[\"degree\"" 
}

//--------------------------------------------------------
// Inlined method implementations for best performance
// 
//--------------------------------------------------------


// ------------------------------------------------------------------
//
//
// --------------- FORWARD PROJECTIONS ------------------------------
//
//
// ------------------------------------------------------------------
QgsPoint QgsCoordinateTransform::transform(QgsPoint thePoint)
{
  if (mShortCircuit || !mInitialisedFlag) return thePoint;
  // transform x
  double x = thePoint.x(); 
  double y = thePoint.y();
  // Number of points to reproject------+
  //                                    | 
  //                                    V 
  if ( ! mSourceToDestXForm->Transform( 1, &x, &y ) )
  {
    //something bad happened....
    throw QgsCsException(QString("Coordinate transform failed"));
  }
  else
  {
#ifdef QGISDEBUG 
    //std::cout << "Point projection...X : " << thePoint.x() << "-->" << x << ", Y: " << thePoint.y() << " -->" << y << std::endl;
#endif        
    return QgsPoint(x, y);
  } 
}

QgsRect QgsCoordinateTransform::transform(QgsRect theRect)
{
  if (mShortCircuit || !mInitialisedFlag) return theRect;
  // transform x
  double x1 = theRect.xMin(); 
  double y1 = theRect.yMin();
  double x2 = theRect.xMax(); 
  double y2 = theRect.yMax();  
  // Number of points to reproject------+
  //                                    | 
  //                                    V 
  if ( ! mSourceToDestXForm->Transform( 1, &x1, &y1 ) || ! mSourceToDestXForm->Transform( 1, &x2, &y2 ) )
  {
    //something bad happened....
    throw QgsCsException(QString("Coordinate transform failed"));
  }
  else
  {
#ifdef QGISDEBUG 
    std::cout << "Rect projection..." 
      << "Xmin : " 
      << theRect.xMin() 
      << "-->" << x1 
      << ", Ymin: " 
      << theRect.yMin() 
      << " -->" << y1
      << "Xmax : " 
      << theRect.xMax() 
      << "-->" << x2 
      << ", Ymax: " 
      << theRect.yMax() 
      << " -->" << y2         
      << std::endl;
#endif        
    return QgsRect(x1, y1, x2 , y2);
  } 
}

QgsRect QgsCoordinateTransform::transform(QgsRect * theRect)
{
  if (mShortCircuit || !mInitialisedFlag) return QgsRect(theRect->xMin(),theRect->yMin(),theRect->xMax(),theRect->yMax());
  // transform x
  double x1 = theRect->xMin(); 
  double y1 = theRect->yMin();
  double x2 = theRect->xMax(); 
  double y2 = theRect->yMax();  
  // Number of points to reproject------+
  //                                    | 
  //                                    V 
  if ( ! mSourceToDestXForm->Transform( 1, &x1, &y1 ) || ! mSourceToDestXForm->Transform( 1, &x2, &y2 ) )
  {
    //something bad happened....
    throw QgsCsException(QString("Coordinate transform failed"));
  }
  else
  {
#ifdef QGISDEBUG 
    std::cout << "Rect projection..." 
      << "Xmin : " 
      << theRect->xMin() 
      << "-->" << x1 
      << ", Ymin: " 
      << theRect->yMin() 
      << " -->" << y1
      << "Xmax : " 
      << theRect->xMax() 
      << "-->" << x2 
      << ", Ymax: " 
      << theRect->yMax() 
      << " -->" << y2       
      << std::endl;
#endif        
    return QgsRect(x1, y1, x2 , y2);
  } 
}


QgsPoint QgsCoordinateTransform::transform(double theX, double theY)
{
  if (mShortCircuit || !mInitialisedFlag) return QgsPoint(theX,theY);
  // transform x
  double x = theX; 
  double y = theY;
  if ( ! mSourceToDestXForm->Transform( 1, &x, &y ) )
  {
    //something bad happened....
    throw QgsCsException(QString("Coordinate transform failed"));
  }
  else
  {
#ifdef QGISDEBUG 
    //std::cout << "Point projection...X : " << theX << "-->" << x << ", Y: " << theY << " -->" << y << std::endl;
#endif    
    return QgsPoint(x, y);
  } 
}

// ------------------------------------------------------------------
//
//
// --------------- INVERSE PROJECTIONS ------------------------------
//
//
// ------------------------------------------------------------------


QgsPoint QgsCoordinateTransform::inverseTransform(QgsPoint thePoint)
{
  if (mShortCircuit || !mInitialisedFlag) return thePoint;
  // transform x
  double x = thePoint.x(); 
  double y = thePoint.y();
  // Number of points to reproject------+
  //                                    | 
  //                                    V 
  if ( ! mDestToSourceXForm->Transform( 1, &x, &y ) )
  {
    //something bad happened....
    throw QgsCsException(QString("Coordinate inverse transform failed"));
  }
  else
  {
#ifdef QGISDEBUG 
    //std::cout << "Point inverse projection...X : " << thePoint.x() << "-->" << x << ", Y: " << thePoint.y() << " -->" << y << std::endl;
#endif        
    return QgsPoint(x, y);
  } 
}

QgsRect QgsCoordinateTransform::inverseTransform(QgsRect theRect)
{
  if (mShortCircuit || !mInitialisedFlag) return theRect;
  // transform x
  double x1 = theRect.xMin(); 
  double y1 = theRect.yMin();
  double x2 = theRect.xMax(); 
  double y2 = theRect.yMax();  
  // get the proj parms for source cs
  OGRSpatialReference sr(mSourceWKT);
  char *proj4src;
  sr.exportToProj4(&proj4src);
  // store the src proj parms in a QString because the pointer populated by exportToProj4
  // seems to get corrupted prior to its use in the transform
  const QString qProj4Src(proj4src);
  // get the proj parms for dest cs
  char *pWkt = (char *)mDestWKT.ascii();
  sr.importFromWkt(&pWkt);
  char *proj4dest;
  sr.exportToProj4(&proj4dest);
  // store the dest proj parms in a QString because the pointer populated by exportToProj4
  // seems to get corrupted prior to its use in the transform
  const QString qProj4Dest(proj4dest);
#ifdef QGISDEBUG   

  std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"<< std::endl;
  std::cout << "Rect inverse projection..." << std::endl;
  std::cout << "INPUT: " << std::endl << mSourceWKT << std::endl;
  std::cout << "PROJ4: " << std::endl << qProj4Src << std::endl;  
  std::cout << "OUTPUT: " << std::endl << mDestWKT  << std::endl;
  std::cout << "PROJ4: " << std::endl << qProj4Dest << std::endl;  
  std::cout << "INPUT RECT: " << std::endl << x1 << "," << y1 << ":" << x2 << "," << y2 << std::endl;
  std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
#endif  
  try{
    double z = 0.0;
    /*
    qProjPt pt;
    pt.x = x1;
    pt.y = y1;
    pt.z = 0;
    */
    cs2cs(qProj4Src, qProj4Dest, x1, y1, z);
    cs2cs(qProj4Src, qProj4Dest, x2, y2, z);

  //  XXX old style that fails
  // int myResult1 = mDestToSourceXForm->Transform( 1, &x1, &y1 );
  //int myResult2 = mDestToSourceXForm->Transform( 1, &x2, &y2 );
  //CPLPopErrorHandler();
    }catch(QgsCsException(&cse))
        {
        // rethrow the exception
        throw cse;
        }
#ifdef QGISDEBUG 
    std::cout << "Xmin : " 
      << theRect.xMin() 
      << "-->" << x1 
      << ", Ymin: " 
      << theRect.yMin() 
      << " -->" << y1
      << "Xmax : " 
      << theRect.xMax() 
      << "-->" << x2 
      << ", Ymax: " 
      << theRect.yMax() 
      << " -->" << y2       
      << std::endl;
#endif        
    return QgsRect(x1, y1, x2 , y2);
   
}

QgsRect QgsCoordinateTransform::inverseTransform(QgsRect * theRect)
{
  if (mShortCircuit || !mInitialisedFlag) return QgsRect(theRect->xMin(),theRect->yMin(),theRect->xMax(),theRect->yMax());
  // transform x
  double x1 = theRect->xMin(); 
  double y1 = theRect->yMin();
  double x2 = theRect->xMax(); 
  double y2 = theRect->yMax();  
  // Number of points to reproject------+
  //                                    | 
  //                                    V 
  if ( ! mDestToSourceXForm->Transform( 1, &x1, &y1 ) || ! mDestToSourceXForm->Transform( 1, &x2, &y2 ) )
  {
    //something bad happened....
    throw QgsCsException(QString("Inverse Coordinate transform failed"));
  }
  else
  {
#ifdef QGISDEBUG 
    std::cout << "Rect pointer inverse projection..." 
      << "Xmin : " 
      << theRect->xMin() 
      << "-->" << x1 
      << ", Ymin: " 
      << theRect->yMin() 
      << " -->" << y1
      << "Xmax : " 
      << theRect->xMax() 
      << "-->" << x2 
      << ", Ymax: " 
      << theRect->yMax() 
      << " -->" << y2         
      << std::endl;
#endif        
    return QgsRect(x1, y1, x2 , y2);
  } 
}

QgsPoint QgsCoordinateTransform::inverseTransform(double theX, double theY)
{
  if (mShortCircuit || !mInitialisedFlag) return QgsPoint(theX,theY);
  // transform x
  double x = theX; 
  double y = theY;
  if ( ! mDestToSourceXForm->Transform( 1, &x, &y ) )
  {
    //something bad happened....
    throw QgsCsException(QString("Coordinate inverseTransform failed"));
  }
  else
  {
#ifdef QGISDEBUG 
    //std::cout << "Point inverse projection...X : " << theX << "-->" << x << ", Y: " << theY << " -->" << y << std::endl;
#endif    
    return QgsPoint(x, y);
  } 
}
void QgsCoordinateTransform::cs2cs(const QString & dest, const QString& src, double& x, double& y, double& z)
{
  // use proj4 to do the transform from destination to 
  // source CS 
  //XXX might be better to not create these each time through 
  // init the projections (destination and source)
  projPJ pDest = pj_init_plus((const char *)dest);
  projPJ pSrc = pj_init_plus((const char *)src);

  // if the destination projection is lat/long, convert the points to radians
  // prior to transforming
  if(pj_is_latlong(pDest))
  {
    x *= DEG_TO_RAD;
    y *= DEG_TO_RAD;
    z *= DEG_TO_RAD;
  }

  int projResult = pj_transform(pDest, pSrc , 1, 0, &x, &y, &z);

  if (projResult != 0) 
  {
    //something bad happened....
    QString pjErr;
    pjErr += "Failed transform of x, y: ";
    pjErr += pj_strerrno(projResult);
    pjErr += "\n";
    // free the proj objects
    pj_free(pDest);
    pj_free(pSrc);
    throw  QgsCsException(pjErr);
  }
  // if the src is lat/long, convert the results from radians back
  // to degrees
  if(pj_is_latlong(pSrc))
  {
    x *= RAD_TO_DEG;
    y *= RAD_TO_DEG;
    z *= RAD_TO_DEG;
  }
  // free the proj objects
  pj_free(pDest);
  pj_free(pSrc);
}



