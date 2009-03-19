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
    void initTestCase();
  private:
};


void TestQgsVectorAnalyzer::initTestCase()
{
}


QTEST_MAIN( TestQgsVectorAnalyzer )
#include "moc_testqgsvectoranalyzer.cxx"

