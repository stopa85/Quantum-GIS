/***************************************************************************
     testqgsvectorfilewriter.cpp
     --------------------------------------
    Date                 : Sun Sep 16 12:22:54 AKDT 2007
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
#include <QObject>
#include <QString>
#include <QPainter>
#include <QPixmap>
#include <QStringList>
#include <QObject>
#include <iostream>
#include <QApplication>
#include <QFileInfo>
#include <QDir>

//qgis includes...
#include <qgsmaprender.h> 
#include <qgsmaplayer.h> 
#include <qgsvectorlayer.h> 
#include <qgsapplication.h>
#include <qgsproviderregistry.h>
#include <qgsmaplayerregistry.h>

/** \ingroup UnitTests
 * This is a unit test for the different renderers for vector layers.
 */
class TestQgsRenderers: public QObject
{
  Q_OBJECT;
  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase(){};// will be called after the last testfunction was executed.
    void init(){};// will be called before each testfunction is executed.
    void cleanup(){};// will be called after every testfunction.

    void singleSymbol();
    void uniqueValue();
    void graduatedSymbol();
    void continuousSymbol();
  private:
    bool setQml (QString theFileName);
    bool hashCheck(QString theExpectedHash);
    QgsMapRender * mpMapRenderer;
    QgsMapLayer * mpLayer;
};

void TestQgsRenderers::initTestCase()
{
  // init QGIS's paths - true means that all path will be inited from prefix
  QString qgisPath = QCoreApplication::applicationDirPath ();
  QgsApplication::setPrefixPath(qgisPath, TRUE);
#ifdef Q_OS_LINUX
  QgsApplication::setPkgDataPath(qgisPath + "/../share/qgis");
  QgsApplication::setPluginPath(qgisPath + "/../lib/qgis");
#endif
  // Instantiate the plugin directory so that providers are loaded
  QgsProviderRegistry::instance(QgsApplication::pluginPath());

  //create some objects that will be used in all tests...

  std::cout << "Prefix  PATH: " << QgsApplication::prefixPath().toLocal8Bit().data() << std::endl;
  std::cout << "Plugin  PATH: " << QgsApplication::pluginPath().toLocal8Bit().data() << std::endl;
  std::cout << "PkgData PATH: " << QgsApplication::pkgDataPath().toLocal8Bit().data() << std::endl;
  std::cout << "User DB PATH: " << QgsApplication::qgisUserDbFilePath().toLocal8Bit().data() << std::endl;

  //create a map layer that will be used in all tests...
  QString myFileName (TEST_DATA_DIR); //defined in CmakeLists.txt
  myFileName = myFileName + QDir::separator() + "points.shp";
  QFileInfo myMapFileInfo ( myFileName );
  mpLayer = new QgsVectorLayer ( myMapFileInfo.filePath(),
            myMapFileInfo.completeBaseName(), "ogr" );
  // Register the layer with the registry
  QgsMapLayerRegistry::instance()->addMapLayer(mpLayer);
  //
  // We only need maprender instead of mapcanvas
  // since maprender does not require a qui
  // and is more light weight
  //
  mpMapRenderer = new QgsMapRender();
  QStringList myLayers;
  myLayers << mpLayer->getLayerID();
  mpMapRenderer->setLayerSet(myLayers);
}

void TestQgsRenderers::singleSymbol()
{
  QVERIFY ( setQml("points_single_symbol.qml") );
  QVERIFY ( hashCheck("singlehash"));
}

void TestQgsRenderers::uniqueValue()
{
  QVERIFY ( setQml("points_uniquevalue_symbol.qml") );
  QVERIFY ( hashCheck("uniquehash"));
}

void TestQgsRenderers::graduatedSymbol()
{
  QVERIFY ( setQml("points_graduated_symbol.qml") );
  QVERIFY ( hashCheck("graduatedhash"));
}

void TestQgsRenderers::continuousSymbol()
{
  QVERIFY ( setQml("points_continuous_symbol.qml") );
  QVERIFY ( hashCheck("continuoushash"));
}
//
// Private helper functions not called directly by CTest
//

bool TestQgsRenderers::setQml (QString theFileName)
{
  //load a qml style and apply to our layer
  //the style will correspond to the renderer
  //type we are testing
  if (! mpLayer->isValid() )
  {
    return false;
  }
  QString myFileName (TEST_DATA_DIR); //defined in CmakeLists.txt
  myFileName = myFileName + QDir::separator() + theFileName ;
  bool myStyleFlag=false;
  mpLayer->loadNamedStyle ( myFileName , myStyleFlag );
  return myStyleFlag;
}

bool TestQgsRenderers::hashCheck(QString theExpectedHash)
{
  //
  // Now render our layer onto a pixmap 
  //
  QPixmap myPixmap( 800,800 );
  myPixmap.fill ( QColor ( "#98dbf9" ) );
  QPainter myPainter;
  myPainter.begin( &myPixmap );
  mpMapRenderer->setOutputSize( QSize ( 800,800 ),72 ); 
  mpMapRenderer->setExtent(mpLayer->extent());
  mpMapRenderer->render( &myPainter );
  myPainter.end();
  //
  // Then get the checksum for the pixmap
  // and see if it is what was expected...
  //
  QByteArray myBytes;
  QBuffer myBuffer(&myBytes);
  myBuffer.open(QIODevice::WriteOnly);
  myPixmap.save(&myBuffer, "PNG"); // writes pixmap into bytes in PNG format 
  myPixmap.save ("/tmp/" + theExpectedHash + ".png");
  QByteArray myHash = QCryptographicHash::hash ( myBytes, QCryptographicHash::Sha1);
  QString myHashString = myHash;
  if ( theExpectedHash == myHashString )
  {
    return true;
  }
  else
  {
    return false;
  }
}

QTEST_MAIN(TestQgsRenderers)
#include "moc_testqgsrenderers.cxx"

