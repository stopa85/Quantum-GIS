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
  // free the proj objects
  pj_free(mSourceProjection);
  pj_free(mDestinationProjection);
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
// create the proj4 structs needed for transforming 
  // get the proj parms for source cs
  char *proj4src;
  myInputSpatialRefSys.exportToProj4(&proj4src);
  // store the src proj parms in a QString because the pointer populated by exportToProj4
  // seems to get corrupted prior to its use in the transform
  mProj4SrcParms = proj4src;
  // get the proj parms for dest cs
  char *proj4dest;
  myOutputSpatialRefSys.exportToProj4(&proj4dest);
  // store the dest proj parms in a QString because the pointer populated by exportToProj4
  // seems to get corrupted prior to its use in the transform
  mProj4DestParms = proj4dest;
  // init the projections (destination and source)
   mDestinationProjection = pj_init_plus((const char *)mProj4DestParms);
   mSourceProjection = pj_init_plus((const char *)mProj4SrcParms);

  if ( !mSourceProjection  || ! mDestinationProjection)
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
    std::cout << "PROJ4: " << std::endl << mProj4SrcParms << std::endl;  
    std::cout << "OUTPUT: " << std::endl << mDestWKT  << std::endl;
    std::cout << "PROJ4: " << std::endl << mProj4DestParms << std::endl;  
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
  }
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
  double z = 0.0;
  try
  {

    transformCoords(FORWARD, 1, x, y, z );
  }
  catch(QgsCsException &cse)
  {
    //something bad happened....
    // rethrow the exception
    throw cse;
  }
#ifdef QGISDEBUG 
  //std::cout << "Point projection...X : " << thePoint.x() << "-->" << x << ", Y: " << thePoint.y() << " -->" << y << std::endl;
#endif        
  return QgsPoint(x, y);
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
  try{
    double z = 0.0;
    transformCoords(FORWARD, 1, x1, y1, z);
    transformCoords(FORWARD, 1, x2, y2, z);

  }
  catch(QgsCsException &cse)
  {
    // rethrow the exception
    throw cse;
  }
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

QgsPoint QgsCoordinateTransform::transform(double theX, double theY)
{
  transform(QgsPoint(theX, theY));
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
  double z = 0.0;
  const int pointCount = 1;
  try
  {
    transformCoords(INVERSE, pointCount, x, y, z);
  }
  catch(QgsCsException &cse)
  {
    //something bad happened....
    // rethrow the exception
    throw cse;
  }
#ifdef QGISDEBUG 
  //std::cout << "Point inverse projection...X : " << thePoint.x() << "-->" << x << ", Y: " << thePoint.y() << " -->" << y << std::endl;
#endif        
  return QgsPoint(x, y);
} 

QgsRect QgsCoordinateTransform::inverseTransform(QgsRect theRect)
{
  if (mShortCircuit || !mInitialisedFlag) return theRect;
  // transform x
  double x1 = theRect.xMin(); 
  double y1 = theRect.yMin();
  double x2 = theRect.xMax(); 
  double y2 = theRect.yMax();  
#ifdef QGISDEBUG   

  std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"<< std::endl;
  std::cout << "Rect inverse projection..." << std::endl;
  std::cout << "INPUT: " << std::endl << mSourceWKT << std::endl;
  std::cout << "PROJ4: " << std::endl << mProj4SrcParms << std::endl;  
  std::cout << "OUTPUT: " << std::endl << mDestWKT  << std::endl;
  std::cout << "PROJ4: " << std::endl << mProj4DestParms << std::endl;  
  std::cout << "INPUT RECT: " << std::endl << x1 << "," << y1 << ":" << x2 << "," << y2 << std::endl;
  std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
#endif  
  try{
    double z = 0.0;
    transformCoords(INVERSE, 1, x1, y1, z);
    transformCoords(INVERSE, 1, x2, y2, z);

    }catch(QgsCsException &cse)
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

QgsPoint QgsCoordinateTransform::inverseTransform(double theX, double theY)
{
  inverseTransform(QgsPoint(theX, theY));
}

void QgsCoordinateTransform::transformCoords(TransformDirection direction, const int& numPoints, double& x, double& y, double& z)
{
  // use proj4 to do the transform   
 QString dir;  
  // if the source/destination projection is lat/long, convert the points to radians
  // prior to transforming
  if((pj_is_latlong(mDestinationProjection) && (direction == INVERSE))
        || (pj_is_latlong(mSourceProjection) && (direction == FORWARD)))
  {
    x *= DEG_TO_RAD;
    y *= DEG_TO_RAD;
    z *= DEG_TO_RAD;
    
  }
  int projResult;
if(direction == INVERSE)
{
  projResult = pj_transform(mDestinationProjection, mSourceProjection , numPoints, 0, &x, &y, &z);
  dir = "inverse";
}
else
{
  projResult = pj_transform(mSourceProjection, mDestinationProjection, numPoints, 0, &x, &y, &z);
  dir = "forward";
}

  if (projResult != 0) 
  {
    //something bad happened....
    QString pjErr;
    pjErr += "Failed " + dir + " transform of x, y: ";
    pjErr += pj_strerrno(projResult);
    pjErr += "\n";
    throw  QgsCsException(pjErr);
  }
  // if the result is lat/long, convert the results from radians back
  // to degrees
  if((pj_is_latlong(mDestinationProjection) && (direction == FORWARD))
        || (pj_is_latlong(mSourceProjection) && (direction == INVERSE)))
  {
    x *= RAD_TO_DEG;
    y *= RAD_TO_DEG;
    z *= RAD_TO_DEG;
  }
}



