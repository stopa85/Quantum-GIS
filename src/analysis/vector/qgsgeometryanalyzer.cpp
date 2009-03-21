/***************************************************************************
    qgsgeometryanalyzer.cpp - QGIS Tools for vector geometry analysis
                             -------------------
    begin                : 19 March 2009
    copyright            : (C) Carson Farmer
    email                : carson.farmer@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id: qgis.h 9774 2008-12-12 05:41:24Z timlinux $ */

#include "qgsgeometryanalyzer.h"

#include "qgsapplication.h"
#include "qgsfield.h"
#include "qgsfeature.h"
#include "qgslogger.h"
#include "qgscoordinatereferencesystem.h"
#include "qgsvectorfilewriter.h"
#include "qgsvectordataprovider.h"



bool QgsGeometryAnalyzer::singlepartsToMultipart( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding,
                             const int fieldIndex )
{
/*
//    vprovider = self.vlayer.dataProvider()
  QgsVectorDataProvider* provider = layer->dataProvider();
//    allAttrs = vprovider.attributeIndexes()
  QgsAttributeList allAttrs = provider->attributeIndexes();
//    vprovider.select( allAttrs )
  provider->select( allAttr, QgsRectangle(), true );
//    fields = vprovider.fields()
//  const QgsFieldsList& fields = provider->fields();
//    writer = QgsVectorFileWriter( self.myName, self.myEncoding, 
//    fields, vprovider.geometryType(), vprovider.crs() )
  QgsVectorFileWriter* writer = new QgsVectorFileWriter( shapefileName,
      fileEncoding, provider->fields(), provider->geometryType(), provider->crs() );

  // check whether file creation was successful
  WriterError err = writer->hasError();
  if ( err != NoError )
  {
    delete writer;
    return err;
  }
//    inFeat = QgsFeature()
  QgsFeature inFeat;
//    outFeat = QgsFeature()
  QgsFeature outFeat;
//    inGeom = QgsGeometry()
  QgsGeometry inGeom;
//    outGeom = QgsGeometry()
  QgsGeometry outGeom;
//    index = vprovider.fieldNameIndex( self.myField )
  int index = provider->fieldNameIndex( fieldName );
//    if not index == -1:

  if ( index == -1 )
  {
    return false;
  }

  QList<QVariant> unique;
  provider->uniqueValues( index, unique )
//      unique = ftools_utils.getUniqueValues( vprovider, int( index ) )
//    else:
//      unique = range( 0, self.vlayer.featureCount() )

//    nFeat = vprovider.featureCount() * len( unique )
//    nElement = 0
//    self.emit( SIGNAL( "runStatus(PyQt_PyObject)" ), 0 )
//    self.emit( SIGNAL( "runRange(PyQt_PyObject)" ), ( 0, nFeat ) )
  if ( unique->size() < layer->featureCount() )
  {
//    if not len( unique ) == self.vlayer.featureCount()
//      for i in unique:
    QList<QgsGeometry> multiGeom;
    bool first;
    QgsAttributeMap atMap;

    for ( int it = unique.begin(); it != unique.end(); ++it )
    {
      provider->select( allAttr, QgsRectangle(), true );
//        vprovider.rewind()
//        multi_feature= []
//        first = True
      first = true;
      while ( provider->nextFeature( inFeat ) )
      {
//        while vprovider.nextFeature( inFeat )

//          atMap = inFeat.attributeMap()

//          idVar = atMap[ index ]
//          if idVar.toString().trimmed() == i.toString().trimmed()
         if ( inFeat.attributeMap()[ index ].toString().trimmed() == it.toString().trimmed() )
         {
//            if first:
//              atts = atMap
//              first = False
           if (first)
           {
             atMap = inFeat.attributeMap();
             first = false;
           }
//            inGeom = QgsGeometry( inFeat.geometry() )
         inGeom = inFeat.geometry();
//            vType = inGeom.type()
         multiGeom << inGeom.asGeometryCollection()
//            feature_list = self.extractAsMulti( inGeom )
//            multi_feature.extend( feature_list )
         }
//          nElement += 1
//          self.emit( SIGNAL( "runStatus(PyQt_PyObject)" ),  nElement )
//        outFeat.setAttributeMap( atts )
          outFeat.setAttributeMap( atMap );
//        outGeom = QgsGeometry( self.convertGeometry( multi_feature, vType ) )
          outGeom = convertGeometry( multifeature, vtype );
          outFeat.setGeometry( outGeom );
          writer.addFeature( outFeat );
//        outFeat.setGeometry( outGeom )
//        writer.addFeature( outFeat )
      }
  }
//      del writer
//    return True
*/
}

bool QgsGeometryAnalyzer::multipartToSingleparts( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding )
{
  /*
    vprovider = self.vlayer.dataProvider()
    allAttrs = vprovider.attributeIndexes()
    vprovider.select( allAttrs )
    fields = vprovider.fields()
    writer = QgsVectorFileWriter( self.myName, self.myEncoding, 
    fields, vprovider.geometryType(), vprovider.crs() )
    inFeat = QgsFeature()
    outFeat = QgsFeature()
    inGeom = QgsGeometry()
    outGeom = QgsGeometry()
    nFeat = vprovider.featureCount()
    nElement = 0
    self.emit( SIGNAL( "runStatus(PyQt_PyObject)" ), 0 )
    self.emit( SIGNAL( "runRange(PyQt_PyObject)" ), ( 0, nFeat ) )
    while vprovider.nextFeature( inFeat )
      nElement += 1  
      self.emit( SIGNAL( "runStatus(PyQt_PyObject)" ), nElement )
      inGeom = inFeat.geometry()
      atMap = inFeat.attributeMap()
      featList = self.extractAsSingle( inGeom )
      outFeat.setAttributeMap( atMap )
      for i in featList:
        outFeat.setGeometry( i )
        writer.addFeature( outFeat )
    del writer
    return True

 */ 
}
bool QgsGeometryAnalyzer::extractNodes( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding )
{
  QgsVectorDataProvider* provider = layer->dataProvider();
  QgsAttributeList allAttrs = provider->attributeIndexes();
  provider->select( allAttr, QgsRectangle(), true );
  QgsVectorFileWriter* writer = new QgsVectorFileWriter( shapefileName,
      fileEncoding, provider->fields(), QGis::WKBPoint, provider->crs() );

  WriterError err = writer->hasError();
  if ( err != NoError )
  {
    delete writer;
    return err;
  }
  
  QgsFeature inFeat;
  QgsFeature outFeat;
  QgsGeometry inGeom;
  QgsGeometry outGeom;
  QList<QgsPoint> pointList;

  while ( provider->nextFeature( inFeat ) )
  {
    pointList = extractPoints( inFeat.geometry() );
    outFeat.setAttributeMap( inFeat.attributeMap() );
    for ( QList<QgsPoint>::const_iterator point = pointList.constBegin(); point != pointList.constEnd(); ++point )
    {
      outFeat.setGeometry( outGeom.fromPoint( point ) )
      writer.addFeature( outFeat )
    }
  }
  delete writer;
  return true;
}

bool QgsGeometryAnalyzer::polygonsToLines( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding )
{
  QgsVectorDataProvider* provider = layer->dataProvider();
  QgsAttributeList allAttrs = provider->attributeIndexes();
  provider->select( allAttr, QgsRectangle(), true );
  QgsVectorFileWriter* writer = new QgsVectorFileWriter( shapefileName,
      fileEncoding, provider->fields(), QGis::WKBPoint, provider->crs() );

  WriterError err = writer->hasError();
  if ( err != NoError )
  {
    delete writer;
    return err;
  }
  
  QgsFeature inFeat;
  QgsFeature outFeat;
  QgsGeometry inGeom;
  QgsGeometry outGeom;
  QList<QgsPolyline> linetList;

  while ( provider->nextFeature( inFeat ) )
  {
    lineList = extractLines( inFeat.geometry() )
    outFeat.setAttributeMap( inFeat.attributeMap() )
    for ( QList<QgsPolyline>::const_iterator line = lineList.constBegin(); line != lineList.constEnd(); ++line )
    {
      outFeat.setGeometry( outGeom.fromPolyline( line ) )
      writer.addFeature( outFeat )
    }
  }
  delete writer;
  return true;
}

bool QgsGeometryAnalyzer::exportGeometryInformation( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding )
{
  /*
    vprovider = self.vlayer.dataProvider()
    allAttrs = vprovider.attributeIndexes()
    vprovider.select( allAttrs )
    ( fields, index1, index2 ) = self.checkGeometryFields( self.vlayer )
    writer = QgsVectorFileWriter( self.myName, self.myEncoding, 
    fields, vprovider.geometryType(), vprovider.crs() )
    inFeat = QgsFeature()
    outFeat = QgsFeature()
    inGeom = QgsGeometry()
    nFeat = vprovider.featureCount()
    nElement = 0
    self.emit( SIGNAL( "runStatus(PyQt_PyObject)" ), 0)
    self.emit( SIGNAL( "runRange(PyQt_PyObject)" ), ( 0, nFeat ) )
    while vprovider.nextFeature(inFeat)
      self.emit( SIGNAL( "runStatus(PyQt_PyObject)" ),  nElement )
      nElement += 1    
      inGeom = inFeat.geometry()
      ( attr1, attr2 ) = self.simpleMeasure( inGeom )
      outFeat.setGeometry( inGeom )
      atMap = inFeat.attributeMap()
      outFeat.setAttributeMap( atMap )
      outFeat.addAttribute( index1, QVariant( attr1 ) )
      outFeat.addAttribute( index2, QVariant( attr2 ) )
      writer.addFeature( outFeat )
    del writer
    return True

 */ 
}
bool QgsGeometryAnalyzer::simplifyGeometry( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding,
                             const double tolerance )
{
  QgsVectorDataProvider* provider = layer->dataProvider();
  QgsAttributeList allAttrs = provider->attributeIndexes();
  provider->select( allAttr, QgsRectangle(), true );
  QgsVectorFileWriter* writer = new QgsVectorFileWriter( shapefileName,
      fileEncoding, provider->fields(), provider->geometryType(), provider->crs() );

  WriterError err = writer->hasError();
  if ( err != NoError )
  {
    delete writer;
    return err;
  }
  
  QgsFeature inFeat;
  QgsFeature outFeat;
  QgsGeometry inGeom;

  while ( provider->nextFeature( inFeat ) )
  {
    inGeom = inFeat.geometry();
    outFeat.setAttributeMap( inFeat.attributeMap() );
    outFeat.setGeometry( inGeom.simplify( tolerance ) );
    writer.addFeature( outFeat );
  }
  delete writer;
  return true;
}

bool QgsGeometryAnalyzer::polygonCentroids( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding )
{
  /*
    vprovider = self.vlayer.dataProvider()
    allAttrs = vprovider.attributeIndexes()
    vprovider.select( allAttrs )
    fields = vprovider.fields()
    writer = QgsVectorFileWriter( self.myName, self.myEncoding, 
    fields, QGis.WKBPoint, vprovider.crs() )
    inFeat = QgsFeature()
    outfeat = QgsFeature()
    nFeat = vprovider.featureCount()
    nElement = 0
    self.emit( SIGNAL( "runStatus(PyQt_PyObject)" ), 0 )
    self.emit( SIGNAL( "runRange(PyQt_PyObject)" ), ( 0, nFeat ) )
    while vprovider.nextFeature( inFeat )
      geom = inFeat.geometry()
      centroid = geom.centroid()
        outfeat.setGeometry( centroid )
        atMap = inFeat.attributeMap()
        outfeat.setAttributeMap( atMap )
        writer.addFeature( outfeat )
      nElement += 1
      self.emit( SIGNAL( "runStatus(PyQt_PyObject)" ),  nElement )
    del writer
    return True
 */ 
}

bool QgsGeometryAnalyzer::layerExtent( QgsVectorLayer* layer,
                             const QString& shapefileName,
                             const QString& fileEncoding )
{
  QgsFieldMap fields;
  fields.insert( 0 , QgsField( QString( "MINX" ), QVariant.Double ) );
  fields.insert( 1 , QgsField( QString( "MINY" ), QVariant.Double ) );
  fields.insert( 2 , QgsField( QString( "MAXX" ), QVariant.Double ) );
  fields.insert( 3 , QgsField( QString( "MAXY" ), QVariant.Double ) );
  fields.insert( 4 , QgsField( QString( "CNTX" ), QVariant.Double ) );
  fields.insert( 5 , QgsField( QString( "CNTY" ), QVariant.Double ) );
  fields.insert( 6 , QgsField( QString( "AREA" ), QVariant.Double ) );
  fields.insert( 7 , QgsField( QString( "PERIM" ), QVariant.Double ) );
  fields.insert( 8 , QgsField( QString( "HEIGHT" ), QVariant.Double ) );
  fields.insert( 9 , QgsField( QString( "WIDTH" ), QVariant.Double  ) );
  
  QgsVectorFileWriter* writer = new QgsVectorFileWriter( shapefileName,
      fileEncoding, fields, QGis::WKBPolygon, layer->dataProvider()->crs() );

  WriterError err = writer->hasError();
  if ( err != NoError )
  {
    delete writer;
    return err;
  }

  QgsRectangle rect;
  rect = layer->extent();
  double minx = rect.xMinimum();
  double miny = rect.yMinimum();
  double maxx = rect.xMaximum();
  double maxy = rect.yMiaximum();
  double height = rect.height();
  double width = rect.width();
  
  double cntx = minx + ( width / 2.0 );
  double cnty = miny + ( height / 2.0 );
  double area = width * height;
  double perim = ( 2 * width ) + (2 * height );

  QgsFeature feat;
  QgsAttributeMap atMap;
  atMap.insert( 0 , QVariant( minx ) );
  atMap.insert( 1 , QVariant( miny ) );
  atMap.insert( 2 , QVariant( maxx ) );
  atMap.insert( 3 , QVariant( maxy ) );
  atMap.insert( 4 , QVariant( cntx ) );
  atMap.insert( 5 , QVariant( cnty ) );
  atMap.insert( 6 , QVariant( area ) );
  atMap.insert( 7 , QVariant( perim ) );
  atMap.insert( 8 , QVariant( height ) );
  atMap.insert( 9 , QVariant( width ) );
  feat.setAttributeMap( atMap );
  feat.setGeometry( QgsGeometry().fromRect( rect ) );
  writer.addFeature( feat );
  delete writer;
  return true
  
}

QList<double> QgsGeometryAnalyzer::simpleMeasure( QgsGeometry& geometry )
{
  /*
    if inGeom.wkbType() == QGis.WKBPoint:
      pt = QgsPoint()
      pt = inGeom.asPoint()
      attr1 = pt.x()
      attr2 = pt.y()
    else:
      measure = QgsDistanceArea()
      attr1 = measure.measure(inGeom)      
      if inGeom.type() == QGis.Polygon:
        attr2 = self.perimMeasure( inGeom, measure )
      else:
        attr2 = attr1
    return ( attr1, attr2 )

 */ 
}
double QgsGeometryAnalyzer::perimeterMeasure( QgsGeometry& geometry )
{
  /*
    value = 0.00
    if inGeom.isMultipart()
      poly = inGeom.asMultiPolygon()
      for k in poly:
        for j in k:
          value = value + measure.measureLine( j )
    else:
      poly = inGeom.asPolygon()
      for k in poly:
        value = value + measure.measureLine( k )
    return value
*/
  
}

QgsFieldMap QgsGeometryAnalyzer::checkGeometryFields( QgsGeometry& geometry, int index1, int index2 )
{
  /*
    vprovider = vlayer.dataProvider()
    nameList = []
    fieldList = vprovider.fields()
    geomType = vlayer.geometryType()
    for i in fieldList.keys()
      nameList.append( fieldList[ i ].name().toLower() )
    if geomType == QGis.Polygon:
      plp = "Poly"
      ( found, index1 ) = self.checkForField( nameList, "AREA" )           
      if not found:
        field = QgsField( "AREA", QVariant.Double, "double", 10, 6, "Polygon area" )
        index1 = len( fieldList.keys() )
        fieldList[ index1 ] = field        
      ( found, index2 ) = self.checkForField( nameList, "PERIMETER" )
        
      if not found:
        field = QgsField( "PERIMETER", QVariant.Double, "double", 10, 6, "Polygon perimeter" )
        index2 = len( fieldList.keys() )
        fieldList[ index2 ] = field         
    elif geomType == QGis.Line:
      plp = "Line"
      (found, index1) = self.checkForField(nameList, "LENGTH")
      if not found:
        field = QgsField("LENGTH", QVariant.Double, "double", 10, 6, "Line length")
        index1 = len(fieldList.keys())
        fieldList[index1] = field
      index2 = index1
    else:
      plp = "Point"
      (found, index1) = self.checkForField(nameList, "XCOORD")
      if not found:
        field = QgsField("XCOORD", QVariant.Double, "double", 10, 6, "Point x coordinate")
        index1 = len(fieldList.keys())
        fieldList[index1] = field
      (found, index2) = self.checkForField(nameList, "YCOORD")
      if not found:
        field = QgsField("YCOORD", QVariant.Double, "double", 10, 6, "Point y coordinate")
        index2 = len(fieldList.keys())
        fieldList[index2] = field
    return (fieldList, index1, index2)
*/
  
}
QgsGeometry QgsGeometryAnalyzer::extractLines( QgsGeometry& geometry )
{
  QGis::WkbType wkbType = geometry.wkbType();
  QList<QgsPolyline> lineList;
  QgsMultiPolygon polyList
  if ( geometry.type() == QGis::Polygon )
  {
    if ( geometry.isMultipart() )
    {
      polyList = geometry.asMultiPolygon()
      for ( QList<QgsPolyline>::const_iterator polygon = polyList.constBegin(); polygon != polyList.constEnd(); ++polygon )
      {
        lineList << line
      }
      else
      {
        lineList = geom.asPolygon()
      }
    }
  }
  return lineList
}
QgsGeometry QgsGeometryAnalyzer::extractAsSingle( QgsGeometry& geometry )
{
/*
    multi_geom = QgsGeometry()
    temp_geom = []
    if geom.type() == 0:
      if geom.isMultipart()
        multi_geom = geom.asMultiPoint()
        for i in multi_geom:
          temp_geom.append( QgsGeometry().fromPoint ( i ) )
      else:
        temp_geom.append( geom )
    elif geom.type() == 1:
      if geom.isMultipart()
        multi_geom = geom.asMultiPolyline()
        for i in multi_geom:
          temp_geom.append( QgsGeometry().fromPolyline( i ) )
      else:
        temp_geom.append( geom )
    elif geom.type() == 2:
      if geom.isMultipart()
        multi_geom = geom.asMultiPolygon()
        for i in multi_geom:
          temp_geom.append( QgsGeometry().fromPolygon( i ) )
      else:
        temp_geom.append( geom )
    return temp_geom
        
*/
  
}

QgsGeometry QgsGeometryAnalyzer::extractAsMulti( QgsGeometry& geometry )
{
/*
  if ( geometry->mGeos == NULL )
  {
    geometry->exportWkbToGeos();
  }
  if ( !geometry->mGeos )
  {
    return 0;
  }
    return fromGeosGeom( GEOSIntersection( mGeos, geometry->mGeos ) );

  for ( int i = 0; i < geometry.size(); i++ )
    geomarr[i] = geometry->mGeos[i];

  GEOSGeometry *geom = 0;

  try
  {
    geom = GEOSGeom_createCollection( typeId, geomarr, geoms.size() );
  }
  catch ( GEOSException &e )
  {
    Q_UNUSED( e );
  }

  delete [] geomarr;

  return geom;
}  

    temp_geom = []
    if geom.type() == 0:
      if geom.isMultipart()
        return geom.asMultiPoint()
      else:
        return [ geom.asPoint() ]
    elif geom.type() == 1:
      if geom.isMultipart()
        return geom.asMultiPolyline()
      else:
        return [ geom.asPolyline() ]
    else:
      if geom.isMultipart()
        return geom.asMultiPolygon()
      else:
        return [ geom.asPolygon() ]

*/
  
}

QgsGeometry QgsGeometryAnalyzer::convertGeometry( QgsGeometry& geometry )
{
  /*
    if vType == 0:
      return QgsGeometry().fromMultiPoint(geom_list)
    elif vType == 1:
      return QgsGeometry().fromMultiPolyline(geom_list)
    else:
      return QgsGeometry().fromMultiPolygon(geom_list)
  */
}

QList<QgsPoint> QgsGeometryAnalyzer::extractPoints( QgsGeometry& geometry )
{
  QGis::WkbType wkbType = geometry.wkbType();
  QList<QgsPoint> pointList;
  QgsPolyline lineList;
  switch ( wkbType )
  {
    case QGis::WKBPoint25D:
    case QGis::WKBPoint:
    case QGis::WKBMultiLineString25D:
    case QGis::WKBMultiLineString:
    {
      geometry->convertToMultitype()
      pointList = geometry.asMultiPoint()
      break;
    }
    case QGis::WKBLineString25D:
    case QGis::WKBLineString:
    case QGis::WKBMultiLineString25D:
    case QGis::WKBMultiLineString:
    {
      geometry->convertToMultitype()
      lineList = geometry.asMultiPolyline()
      for ( QList<QgsPolyline>::const_iterator line = lineList.constBegin(); line != lineList.constEnd(); ++line )
      {
        pointList << line
      }
      break;
    }
    case QGis::WKBPolygon25D:
    case QGis::WKBPolygon:
    case QGis::WKBMultiPolygon25D:
    case QGis::WKBMultiPolygon:
    {
      geometry->convertToMultitype()
      QgsPolygon polyList = geometry.asMultiPolygon()
      for ( QList<QgsPolygon>::const_iterator lineList = polyList.constBegin(); lineList != polyList.constEnd(); ++lineList )
      {
        for ( QList<QgsPolyline>::const_iterator line = lineList.constBegin(); line != lineList.constEnd(); ++line )
        {
          pointList << line
        }
      }
      break;
    }
    default:
      break;
  }
  return pointList;
}
