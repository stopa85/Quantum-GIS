/***************************************************************************
    qgsrasterprojector.h - Raster projector
     --------------------------------------
    Date                 : Jan 16, 2011
    Copyright            : (C) 2005 by Radim Blazek
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
/* $Id: qgsrasterprojector.h 15005 2011-01-08 16:35:21Z rblazek $ */

/* This code takes ideas from WarpBuilder in Geotools.
 * Thank you to Ing. Andrea Aime, Ing. Simone Giannecchini and GeoSolutions S.A.S.
 * See : http://geo-solutions.blogspot.com/2011/01/developers-corner-improving.html
 */

#ifndef QGSRASTERPROJECTOR_H
#define QGSRASTERPROJECTOR_H

#include <QVector>
#include <QList>


#include "qgsrectangle.h"
#include "qgscoordinatereferencesystem.h"
#include "qgscoordinatetransform.h"

#include <cmath>

//class QgsRectangle;
class QgsPoint;

//class CORE_EXPORT QgsRasterProjector 
class QgsRasterProjector 
{
//    Q_OBJECT
  public:
    /** \brief QgsRasterProjector implements approximate projection support for 
     * it calculates grid of points in source CRS for target CRS + extent
     * which are used to calculate affine transformation matrices.
     * */
    QgsRasterProjector( 
      QgsCoordinateReferenceSystem theSrcCRS, 
      QgsCoordinateReferenceSystem theDestCRS, 
      QgsRectangle theDestExtent, 
      int theDestRows, int theDestCols,
      double theMaxSrcXRes, double theMaxSrcYRes
    );

    /** \brief The destructor */
    ~QgsRasterProjector();


    /** \brief get destination point for _current_ destination position */
    void destPointOnCPMatrix ( int theRow, int theCol, double *theX, double *theY );

    /** \brief Get matrix upper left row/col indexes for destination row/col */
    int matrixRow ( int theDestRow );
    int matrixCol ( int theDestCol );

    /** \brief destination point x for dest col */
    //double destX ( int theDestCol );

    /** \brief destination point x for dest row */
    //double destY ( int theDestRow );

    /** \brief get destination point for _current_ matrix position */
    QgsPoint srcPoint ( int theRow, int theCol );

    /** \brief Get source row and column indexes for current source extent and resolution */
    void srcRowCol ( int theDestRow, int theDestCol, int *theSrcRow, int *theSrcCol );

    /** \brief insert rows to matrix */
    void insertRows();

    /** \brief insert columns to matrix */
    void insertCols();

    /* calculate single control point in current matrix */
    void calcCP ( int theRow, int theCol );

    /** \brief calculate matrix row */
    bool calcRow( int theRow );

    /** \brief calculate matrix column */
    bool calcCol( int theCol );

    /** \brief calculate source extent */
    void calcSrcExtent();

    /** \brief calculate minimum source width and height */
    void calcSrcRowsCols ();

    /** \brief check error along columns
      * returns true if within threshold */
    bool checkCols ();
  
    /** \brief check error along rows
      * returns true if within threshold */
    bool checkRows ();

    /** \brief get point for matrix position */
    //QgsPoint matrixPoint ( int theRow, int theCol ) { return QgsPoint ( mCPMatrix[theRow][theCol][0], mCPMatrix[theRow][theCol][1] ); }

    /** get source extent */
    QgsRectangle srcExtent() { return mSrcExtent; } 

    /** get/set source width/height */
    int srcRows() { return mSrcRows; }
    int srcCols() { return mSrcCols; }
    void setSrcRows( int theRows ) { mSrcRows = theRows; mSrcXRes = mSrcExtent.height() / mSrcRows; }
    void setSrcCols( int theCols ) { mSrcCols = theCols; mSrcYRes = mSrcExtent.width() / mSrcCols; }

  private:
    /** Source CRS */
    QgsCoordinateReferenceSystem mSrcCRS;

    /** Destination CRS */
    QgsCoordinateReferenceSystem mDestCRS;

    /** Coordinate transform */
    /* QgsCoordinateTransform inherits from QObject and that is not copyable */
    QgsCoordinateTransform *mCoordinateTransform;

    /** Destination extent */
    QgsRectangle mDestExtent;

    /** Source extent */
    QgsRectangle mSrcExtent;

    /** Number of destination rows */
    int mDestRows;

    /** Number of destination columns */
    int mDestCols;

    /** Destination x resolution */
    double mDestXRes;

    /** Destination y resolution */
    double mDestYRes;

    /** Number of source rows */
    int mSrcRows;

    /** Number of source columns */
    int mSrcCols;

    /** Source x resolution */
    double mSrcXRes;

    /** Source y resolution */
    double mSrcYRes;

    /** number of destination rows per matrix row */
    double mDestRowsPerMatrixRow;

    /** number of destination cols per matrix col */
    double mDestColsPerMatrixCol;

    /** Grid of source control points */
    // using QList< QList<double *> > mCPMatrix; takes however a more time (about 20%) than QList< QList<QgsPoint> > mCPMatrix; - why?

    QList< QList<QgsPoint> > mCPMatrix;
    //QList< QList<double *> > mCPMatrix;

    /** Number of mCPMatrix columns */
    int mCPCols;
    /** Number of mCPMatrix rows */
    int mCPRows;

    /** Maximum tolerance in destination units */
    double mSqrTolerance;

    /** Maximum source resolution */
    double mMaxSrcXRes;
    double mMaxSrcYRes;
};

#endif

