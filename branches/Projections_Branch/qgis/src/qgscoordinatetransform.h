/***************************************************************************
               QgsCoordinateTransform.h  - Coordinate Transforms
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
#ifndef QGSCOORDINATETRANSFORM_H
#define QGSCOORDINATETRANSFORM_H

//qt includes
#include <qobject.h>

//qgis includes
#include "qgspoint.h"
#include "qgsrect.h"
#include "qgscsexception.h"

//gdal and ogr includes
#include <ogr_api.h>
#include <ogr_spatialref.h>
#include <cpl_error.h>

//non qt includes
#include <iostream>

extern "C"{
#include <proj_api.h>
}
//! Structure for passing around x,y,z points
//typedef struct {double x, double y, double z} qProjPt;
class QString;

/*! \class QgsCoordinateTransform
* \brief Class for doing transforms between two map coordinate systems.
*
* This class can convert map coordinates to a different spatial reference system.
*/
class QgsCoordinateTransform: public QObject
{
  Q_OBJECT
 public:
    QgsCoordinateTransform(QString theSourceWKT, QString theDestWKT  );
     //! destructor
    ~QgsCoordinateTransform();
    enum TransformDirection{
      FORWARD,
      INVERSE
    };
    
    /*! Transform the point from Source Coordinate System to Destination Coordinate System
    * @param p Point to transform
    * @return QgsPoint in Destination Coordinate System
    */    
    QgsPoint transform(QgsPoint p);
    
    /*! Transform the point specified by x,y from Source Coordinate System to Destination Coordinate System
    * @param x x cordinate o point to transform
    * @param y y coordinate of point to transform
    * @return QgsPoint in Destination Coordinate System
    */
    QgsPoint transform(double x, double y);

    /*! Transform a QgsRect to the dest Coordinate system 
    * @param QgsRect rect to transform
    * @return QgsRect in Destination Coordinate System
    */        
    QgsRect transform(QgsRect theRect);
    
    /*! Transform a QgsRect pointer to the dest Coordinate system 
    * @param QgsRect * rect to transform
    * @return QgsRect in Destination Coordinate System
    */        
    QgsRect transform(QgsRect * theRect);    
    
    /*! Inverse Transform the point from Dest Coordinate System to Source Coordinate System
    * @param p Point to transform (in destination coord system)
    * @return QgsPoint in Source Coordinate System
    */    
    QgsPoint inverseTransform(QgsPoint p);
    
    /*! Inverse Transform the point specified by x,y from Dest Coordinate System to Source Coordinate System
    * @param x x cordinate of point to transform (in dest coord sys)
    * @param y y coordinate of point to transform (in dest coord sys)
    * @return QgsPoint in Source Coordinate System
    */
    QgsPoint inverseTransform(double x, double y);

    /*! Inverse Transform a QgsRect to the source Coordinate system 
    * @param QgsRect rect to transform (in dest coord sys)
    * @return QgsRect in Source Coordinate System
    */        
    QgsRect inverseTransform(QgsRect theRect); 
       

    /*! Inverse Transform a QgsRect pointer to the source Coordinate system 
    * @param QgsRect * rect to transform (in dest coord sys)
    * @return QgsRect in Source Coordinate System
    */        
    QgsRect inverseTransform(QgsRect * theRect); 
       
        
    QString showParameters();
    
    //! Accessor and mutator for source WKT
    void setSourceWKT(QString theWKT);
    QString sourceWKT() const {return mSourceWKT;};
    //! Accessor  for dest WKT
    QString destWKT() const {return mDestWKT;};    
    //! Accessor for whether this transoform is properly initialised
    void transformCoords(TransformDirection direction, const int &numPoint, double &x, double &y, double &z);
   bool isInitialised() {return mInitialisedFlag;};
 public slots:
    /** mutator for dest WKT - slot will usually be fired if proj props change and 
        user selects a different coordinate system */
    void setDestWKT(QString theWKT);    
    
 private:
    //!initialise is used to actually create the Transformer instance
    void initialise();
    //! flag to show whether the transform is properly initialised or not
    bool mInitialisedFlag;
    /** Transform definitionsin WKT format */
    QString mSourceWKT,mDestWKT;
    /** Dunno if we need this - XXXXX Delete if unused */
    bool mInputIsDegrees;
    //set to true if src cs  == dest cs
    bool mShortCircuit;
    QString mProj4SrcParms;
    QString mProj4DestParms;
    projPJ mSourceProjection;
    projPJ mDestinationProjection;
};


#endif // QGSCOORDINATETRANSFORM_H
