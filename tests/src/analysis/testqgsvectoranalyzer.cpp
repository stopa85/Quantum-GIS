/***************************************************************************
  testqgsvectoranalyzer.cpp
  --------------------------------------
Date                 : Sun Sep 16 12:22:49 AKDT 2007
Copyright            : (C) 2007 by Gary E. Sherman
Email                : sherman at mrcc dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QtTest>

//header for class being tested
#include <qgsgeometryanalyzer.h>

class TestQgsVectorAnalyzer: public QObject
{
    Q_OBJECT;
  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init() ;// will be called before each testfunction is executed.
    void cleanup() ;// will be called after every testfunction.
    /** Our tests proper begin here */
    void singleToMulti(  );
    void multiToSingle(  );
    void extractNodes(  );
    void polygonsToLines(  );
    void exportGeometryInfo(  );
    void simplifyGeometry(  );
    void polygonCentroids(  );
    void layerExtent(  );
  private:
};

void  TestQgsVectorAnalyzer::initTestCase()
{
}
void  TestQgsVectorAnalyzer::cleanupTestCase()
{

}
void  TestQgsVectorAnalyzer::init() 
{

}
void  TestQgsVectorAnalyzer::cleanup() 
{

}

void TestQgsVectorAnalyzer::singleToMulti(  )
{

}
void TestQgsVectorAnalyzer::multiToSingle(  )
{

}
void TestQgsVectorAnalyzer::extractNodes(  )
{

}
void TestQgsVectorAnalyzer::polygonsToLines(  )
{

}
void TestQgsVectorAnalyzer::exportGeometryInfo(  )
{

}
void TestQgsVectorAnalyzer::simplifyGeometry(  )
{

}
void TestQgsVectorAnalyzer::polygonCentroids(  )
{

}
void TestQgsVectorAnalyzer::layerExtent(  )
{

}

QTEST_MAIN( TestQgsVectorAnalyzer )
#include "moc_testqgsvectoranalyzer.cxx"

