/***************************************************************************
    qgsrasterprojector.cpp - Raster projector
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
/* $Id: qgsrasterprojector.cpp 15005 2011-01-08 16:35:21Z rblazek $ */

#include <cassert>

#include "qgslogger.h"
#include "qgsrasterprojector.h"
#include "qgscoordinatetransform.h"

QgsRasterProjector::QgsRasterProjector ( 
  QgsCoordinateReferenceSystem theSrcCRS, 
  QgsCoordinateReferenceSystem theDestCRS, 
  QgsRectangle theDestExtent, 
  int theDestRows, int theDestCols,
  double theMaxSrcXRes, double theMaxSrcYRes )
    : mSrcCRS ( theSrcCRS )
    , mDestCRS ( theDestCRS )
    , mDestExtent ( theDestExtent )
    , mDestRows ( theDestRows ), mDestCols ( theDestCols )
    , mMaxSrcXRes ( theMaxSrcXRes), mMaxSrcYRes ( theMaxSrcYRes )
{
  QgsDebugMsg("Entered");
  QgsDebugMsg("theDestExtent = " + theDestExtent.toString() );

  // reverse transformation
  mCoordinateTransform = new QgsCoordinateTransform( theDestCRS, theSrcCRS );

  mDestXRes = mDestExtent.width() / (mDestCols);
  mDestYRes = mDestExtent.height() / (mDestRows);

  // Calculate tolerance
  // TODO: Think it over better
  // Note: we are checking on matrix each even point, that means taht the real error 
  // in that moment is approximately half size 
  double myDestRes = mDestXRes < mDestYRes ? mDestXRes : mDestYRes;
  mSqrTolerance = myDestRes * myDestRes;

  // Initialize the matrix by corners and middle points
  mCPCols = mCPRows = 3;
  for ( int i = 0; i < mCPRows; i++) {
    QList<QgsPoint> myRow;
    myRow.append( QgsPoint() );
    myRow.append( QgsPoint() );
    myRow.append( QgsPoint() );
    mCPMatrix.insert(i,  myRow );
  }
  for ( int i = 0; i < mCPRows; i++) {
    calcRow( i );
  }

  while (true) 
  {
    bool myColsOK = checkCols();
    if ( !myColsOK ) {
      insertRows();
    }
    bool myRowsOK = checkRows();
    if ( !myRowsOK ) {
      insertCols();
    }
    if ( myColsOK && myRowsOK ) {
      QgsDebugMsg("CP matrix within tolerance");
      break;
    }
    // What is the maximum reasonable size of transformatio matrix? 
    // TODO: consider better when to break - ratio
    if ( mCPRows * mCPCols > 0.25 * mDestRows * mDestCols ) 
    {
      QgsDebugMsg("Too large CP matrix");
      break;
    }
 
  }
  QgsDebugMsg( QString("CPMatrix size: mCPRows = %1 mCPCols = %2").arg(mCPRows).arg(mCPCols) );
  mDestRowsPerMatrixRow =  (float)mDestRows / (mCPRows - 1);
  mDestColsPerMatrixCol = (float)mDestCols / (mCPCols - 1);

  // Calculate source dimensions
  calcSrcExtent();
  calcSrcRowsCols();
  mSrcXRes = mSrcExtent.height() / mSrcRows;
  mSrcYRes = mSrcExtent.width() / mSrcCols;
}

QgsRasterProjector::~QgsRasterProjector ()
{
  delete mCoordinateTransform;
}



void QgsRasterProjector::calcSrcExtent ()
{
  /* Run around the mCPMatrix and find source extent */
  QgsPoint myPoint = mCPMatrix[0][0];
  mSrcExtent = QgsRectangle ( myPoint.x(), myPoint.y(), myPoint.x(), myPoint.y() );
  for ( int i = 0; i < mCPRows; i++) {
    myPoint = mCPMatrix[i][0];
    mSrcExtent.combineExtentWith ( myPoint.x(), myPoint.y() );
    myPoint = mCPMatrix[i][mCPCols-1];
    mSrcExtent.combineExtentWith ( myPoint.x(), myPoint.y() );
  }
  for ( int i = 1; i < mCPCols-1; i++) {
    myPoint = mCPMatrix[0][i];
    mSrcExtent.combineExtentWith ( myPoint.x(), myPoint.y() );
    myPoint = mCPMatrix[mCPRows-1][i];
    mSrcExtent.combineExtentWith ( myPoint.x(), myPoint.y() );
  }
  // Expand a bit to avoid possible approx coords falling out because of representation error
  //mSrcExtent.setXMinimum();
  QgsDebugMsg(  "mSrcExtent = " + mSrcExtent.toString() );
}

void QgsRasterProjector::calcSrcRowsCols ()
{
  // Wee need to calculate minimum cell size in the source 
  // TODO: Think it over better, what is the right source resolution?
  //       Taking distances between cell centers projected to source along source 
  //       axis would result in very high resolution
  // TODO: different resolution for rows and cols ?

  // For now, we take cell sizes projected to source but not to source axes
  double myDestColsPerMatrixCell = mDestCols / mCPCols;
  double myDestRowsPerMatrixCell = mDestRows / mCPRows;

  double myMinSize = DBL_MAX;
  
  for ( int i = 0; i < mCPRows-1; i++) {
    for ( int j = 0; j < mCPCols-1; j++) {
      QgsPoint myPointA = mCPMatrix[i][j];
      QgsPoint myPointB = mCPMatrix[i][j+1];
      QgsPoint myPointC = mCPMatrix[i+1][j];
      double mySize = myPointA.sqrDist( myPointB ) / myDestColsPerMatrixCell; 
      if ( mySize < myMinSize ) { myMinSize = mySize; }

      mySize = myPointA.sqrDist( myPointC ) / myDestRowsPerMatrixCell;
      if ( mySize < myMinSize ) { myMinSize = mySize; }
    }
  }
  double myMinXSize = mMaxSrcXRes > myMinSize ? mMaxSrcXRes : myMinSize;
  double myMinYSize = mMaxSrcYRes > myMinSize ? mMaxSrcYRes : myMinSize;
  // TODO: limit by source data resolution
  mSrcRows = (int) ceil ( mSrcExtent.height() / myMinYSize );
  mSrcCols = (int) ceil ( mSrcExtent.width() / myMinXSize );

  QgsDebugMsg( QString("mSrcRows = %1 theSrcCols = %2").arg(mSrcRows).arg(mSrcCols) );
}


inline void QgsRasterProjector::destPointOnCPMatrix ( int theRow, int theCol, double *theX, double *theY )
{
  //QgsDebugMsg( QString("theRow = %1 theCol = %2").arg(theRow).arg(theCol) );
  *theX = mDestExtent.xMinimum() + theCol * mDestExtent.width() / (mCPCols-1);
  *theY = mDestExtent.yMaximum() - theRow * mDestExtent.height() / (mCPRows-1);
  //QgsDebugMsg( QString("x = %1 y = %2").arg(x).arg(y) );
}

inline int QgsRasterProjector::matrixRow ( int theDestRow )
{
  //QgsDebugMsg( QString("theDestRow = %1 mDestRows = %2 mCPRows = %3").arg(theDestRow).arg( mDestRows).arg(mCPRows) );
  //int myRow = (int) ( floor ( (theDestRow + 0.5) / ( (float)mDestRows / (mCPRows - 1) )  ) );
  return (int) ( floor ( (theDestRow + 0.5) / mDestRowsPerMatrixRow  ) );
}
inline int QgsRasterProjector::matrixCol ( int theDestCol )
{
  //QgsDebugMsg( QString("theDestCol = %1 mDestCols = %2 mCPCols = %3").arg(theDestCol).arg( mDestCols).arg(mCPCols) );
  //return (int) ( floor ( (theDestCol + 0.5) / ( (float)mDestCols / (mCPCols - 1) )  ) );
  return (int) ( floor ( (theDestCol + 0.5) / mDestColsPerMatrixCol  ) );
}

QgsPoint QgsRasterProjector::srcPoint ( int theDestRow, int theCol )
{
  return QgsPoint();
}

void QgsRasterProjector::srcRowCol ( int theDestRow, int theDestCol, int *theSrcRow, int *theSrcCol )
{
  //QgsDebugMsg( QString("theDestRow = %1 theDestCol = %2").arg( theDestRow).arg(theDestCol) );
  double myDestX = mDestExtent.xMinimum() + ( theDestCol + 0.5 ) * mDestXRes;
  double myDestY = mDestExtent.yMaximum() - ( theDestRow + 0.5 ) * mDestYRes;

  //QgsDebugMsg( QString("myDestX = %1 myDestY = %2").arg( myDestX ).arg( myDestY) );
  int myMatrixRow = matrixRow ( theDestRow );
  int myMatrixCol = matrixCol ( theDestCol );

  //QgsDebugMsg( QString("myMatrixRow = %1 myMatrixCol = %2 mCPRows = %3 mCPCols = %4").arg( myMatrixRow ).arg( myMatrixCol).arg(mCPRows).arg(mCPCols) );

  // See the schema in javax.media.jai.WarpGrid doc (but up side down)
  // TODO: use some kind of cache of values which can be reused
  //QgsPoint myDestLL = destPointOnCPMatrix ( myMatrixRow + 1, myMatrixCol );
  //QgsPoint myDestUR = destPointOnCPMatrix ( myMatrixRow, myMatrixCol + 1 );

  double myDestXMin, myDestYMin, myDestXMax, myDestYMax;

  destPointOnCPMatrix ( myMatrixRow + 1, myMatrixCol, &myDestXMin, &myDestYMin );
  destPointOnCPMatrix ( myMatrixRow, myMatrixCol + 1, &myDestXMax, &myDestYMax );

  //QgsDebugMsg( "myDestLL = " + myDestLL.toString() + " myDestUR = " + myDestUR.toString() );

  QgsPoint *mySrcPoint0 = &(mCPMatrix[myMatrixRow+1][myMatrixCol]);
  QgsPoint *mySrcPoint1 = &(mCPMatrix[myMatrixRow+1][myMatrixCol+1]);
  QgsPoint *mySrcPoint2 = &(mCPMatrix[myMatrixRow][myMatrixCol]);
  QgsPoint *mySrcPoint3 = &(mCPMatrix[myMatrixRow][myMatrixCol+1]);

  //QgsDebugMsg( "mySrcPoint : " + mySrcPoint0.toString() + " " + mySrcPoint1.toString() + " " +mySrcPoint2.toString() + " " + mySrcPoint3.toString() + " " );

  //double xfrac = ( myDestX - myDestLL.x() ) / ( myDestUR.x() - myDestLL.x() );
  //double yfrac = ( myDestY - myDestLL.y() ) / ( myDestUR.y() - myDestLL.y() );
  double xfrac = ( myDestX - myDestXMin ) / ( myDestXMax - myDestXMin );
  double yfrac = ( myDestY - myDestYMin ) / ( myDestYMax - myDestYMin );
  //QgsDebugMsg( QString("xfrac = %1 yfrac = %2").arg(xfrac).arg(yfrac) );
  //assert ( xfrac >= 0 );
  //assert ( yfrac >= 0 );
  
  double s = mySrcPoint0->x() + ( mySrcPoint1->x() - mySrcPoint0->x() ) * xfrac;
  double t = mySrcPoint0->y() + ( mySrcPoint1->y() - mySrcPoint0->y() ) * xfrac; 

  double u = mySrcPoint2->x() + ( mySrcPoint3->x() - mySrcPoint2->x() ) * xfrac;
  double v = mySrcPoint2->y() + ( mySrcPoint3->y() - mySrcPoint2->y() ) * xfrac; 

  double mySrcX = s + (u - s) * yfrac;
  double mySrcY = t + (v - t) * yfrac;

  //QgsDebugMsg( QString("s = %1 t = %2").arg(s).arg(t) );
  //QgsDebugMsg( QString("u = %1 v = %2").arg(u).arg(v) );

  //QgsDebugMsg( QString("mySrcX = %1 mySrcY = %2").arg(mySrcX).arg(mySrcY) );

  // TODO: check again cell selection (coor is in the middle)

  //QgsDebugMsg( QString("mSrcExtent.yMaximum() = %1 mySrcY = %2").arg(mSrcExtent.yMaximum()).arg(mySrcY)) ;
  *theSrcRow = (int) floor ( ( mSrcExtent.yMaximum() - mySrcY ) / mSrcXRes );
  *theSrcCol =  (int) floor ( ( mySrcX - mSrcExtent.xMinimum() ) / mSrcYRes );

  //QgsDebugMsg( QString("( mSrcExtent.yMaximum() - mySrcY ) / ( mSrcExtent.height() / mSrcRows ) = %1") .arg( ( mSrcExtent.yMaximum() - mySrcY ) / ( mSrcExtent.height() / mSrcRows )  ) );
  //QgsDebugMsg( QString("mySrcY = %1 yMaximum = %2").arg(mySrcY).arg(mSrcExtent.yMaximum()) );
  
  //QgsDebugMsg( QString("theSrcRow = %1 theSrcCol = %2 mSrcRows = %3 mSrcCols = %4").arg( *theSrcRow ).arg( *theSrcCol ).arg(mSrcRows).arg(mSrcCols) );

  //assert ( *theSrcRow < mSrcRows );
  //assert ( *theSrcCol < mSrcCols );
}

void QgsRasterProjector::insertRows()
{
  for ( int r = 0; r < mCPRows-1; r++) {
    QList<QgsPoint> myRow;
    for ( int c = 0; c < mCPCols; c++) {
      myRow.append( QgsPoint() );
    }
    QgsDebugMsg( QString("insert new row at %1" ).arg(1+r*2) );
    mCPMatrix.insert( 1+r*2,  myRow );
  }
  mCPRows += mCPRows-1;
  for ( int r = 1; r < mCPRows-1; r += 2) {
    calcRow( r );
  }
}

void QgsRasterProjector::insertCols()
{
  for ( int r = 0; r < mCPRows; r++) {
    QList<QgsPoint> myRow;
    for ( int c = 0; c < mCPCols-1; c++) {
      mCPMatrix[r].insert( 1+c*2,  QgsPoint() );
    }
  }
  mCPCols += mCPCols-1;
  for ( int c = 1; c < mCPCols-1; c += 2) {
    calcCol( c );
  }

}

void QgsRasterProjector::calcCP ( int theRow, int theCol ) 
{
  QgsDebugMsg( QString("theRow = %1 theCol = %2").arg(theRow).arg(theCol) );
  //QgsPoint myDestPoint = destPointOnCPMatrix ( theRow, theCol );
  double myDestX, myDestY;
  destPointOnCPMatrix ( theRow, theCol, &myDestX, &myDestY  );
  QgsPoint myDestPoint ( myDestX, myDestY ); 
  QgsDebugMsg( "myDestPoint : " + myDestPoint.toString() );

  mCPMatrix[theRow][theCol] = mCoordinateTransform->transform( myDestPoint ); 
  QgsDebugMsg( QString("x = %1 y = %2").arg( mCPMatrix[theRow][theCol].x() ).arg( mCPMatrix[theRow][theCol].y() ) );
}

bool QgsRasterProjector::calcRow( int theRow )
{
  QgsDebugMsg( QString("theRow = %1").arg(theRow) );
  for ( int i = 0; i < mCPCols; i++) {
    calcCP ( theRow, i );
  }
}

bool QgsRasterProjector::calcCol( int theCol )
{
  QgsDebugMsg( QString("theCol = %1").arg(theCol) );
  for ( int i = 0; i < mCPRows; i++) {
    calcCP ( i, theCol );
  }
}

bool QgsRasterProjector::checkCols()
{
  QgsDebugMsg( "Entered" );
  for ( int c = 0; c < mCPCols; c++) {
    for ( int r = 1; r < mCPRows-1; r += 2 ) {
      //QgsPoint myDestPoint = destPointOnCPMatrix ( r, c );
      double myDestX, myDestY;
      destPointOnCPMatrix ( r, c, &myDestX, &myDestY  );
      QgsPoint myDestPoint ( myDestX, myDestY ); 

      QgsPoint mySrcPoint1 = mCPMatrix[r-1][c];
      QgsPoint mySrcPoint2 = mCPMatrix[r][c];
      QgsPoint mySrcPoint3 = mCPMatrix[r+1][c];

      QgsPoint mySrcApprox( ( mySrcPoint1.x() + mySrcPoint3.x() ) / 2,  (mySrcPoint1.y() + mySrcPoint3.y() ) / 2 );
      QgsPoint myDestApprox = mCoordinateTransform->transform( mySrcApprox, QgsCoordinateTransform::ReverseTransform );
      double mySqrDist = myDestApprox.sqrDist( myDestPoint );
      QgsDebugMsg( QString("mySqrDist = %1 mSqrTolerance = %2").arg(mySqrDist).arg(mSqrTolerance) );
      if ( mySqrDist > mSqrTolerance ) { return false; }
    }
  }
  return true;
}

bool QgsRasterProjector::checkRows()
{
  QgsDebugMsg( "Entered" );
  for ( int r = 0; r < mCPRows; r++) {
    for ( int c = 1; c < mCPCols-1; c += 2 ) {
      //QgsPoint myDestPoint = destPointOnCPMatrix ( r, c );
      double myDestX, myDestY;
      destPointOnCPMatrix ( r, c, &myDestX, &myDestY  );

      QgsPoint myDestPoint ( myDestX, myDestY ); 
      QgsPoint mySrcPoint1 = mCPMatrix[r][c-1];
      QgsPoint mySrcPoint2 = mCPMatrix[r][c];
      QgsPoint mySrcPoint3 = mCPMatrix[r][c+1];
      
      QgsPoint mySrcApprox( ( mySrcPoint1.x() + mySrcPoint3.x() ) / 2,  (mySrcPoint1.y() + mySrcPoint3.y() ) / 2 );
      QgsPoint myDestApprox = mCoordinateTransform->transform( mySrcApprox, QgsCoordinateTransform::ReverseTransform );
      double mySqrDist = myDestApprox.sqrDist( myDestPoint );
      QgsDebugMsg( QString("mySqrDist = %1 mSqrTolerance = %2").arg(mySqrDist).arg(mSqrTolerance) );
      if ( mySqrDist > mSqrTolerance ) { return false; }
    }
  }
  return true;
}
