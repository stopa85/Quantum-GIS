/* QGIS data provider for PostgreSQL/PostGIS layers */
/* $Id$ */
#include <iostream>
#include <qtextstream.h> 
#include "../../src/qgis.h"
#include "../../src/qgsfeature.h"
#include "../../src/qgsfield.h"
#include "../../src/qgsrect.h"

#include "qgspostgresprovider.h"
QgsPostgresProvider::QgsPostgresProvider(QString uri):dataSourceUri(uri)
{
		
	// make connection to the data source
  // for postgres, the connection information is passed as a space delimited
  // string:
  //  host=192.168.1.5 dbname=test user=gsherman password=xxx table=tablename
	std::cout << "Data source uri is " << uri << std::endl;
  // strip the table name off
  QString tableName = uri.mid(uri.find("table=") +6);
  QString connInfo = uri.left(uri.find("table="));
  std::cout << "Table name is " << tableName << std::endl;
  std::cout << "Connection info is " << connInfo << std::endl;
    // calculate the schema if specified
    QString schema = "";
    if(tableName.find(".") > -1){
    	schema = tableName.left(tableName.find("."));
    }
    QString geometryColumn = tableName.mid(tableName.find(" (") +2);
    geometryColumn.truncate(geometryColumn.length()-1);
    tableName = tableName.mid(tableName.find(".")+1, tableName.find(" (") - (tableName.find(".")+1));
 
    qWarning("Geometry column is: " + geometryColumn);
    qWarning("Schema is: " + schema);
    qWarning("Table name is: " + tableName);

   PGconn *pd =PQconnectdb((const char *)connInfo);
	// check the connection status
  if(PQstatus(pd) == CONNECTION_OK){
    std::cout << "Connection to the database was successful\n";
   
    // check the geometry column
    QString sql = "select f_geometry_column,type,srid from geometry_columns where f_table_name='"
    	+ tableName + "' and f_geometry_column = '" + geometryColumn + 
      "' and f_table_schema = '" + schema + "'";
      qWarning("Getting geometry column: " + sql);
      PGresult * result = PQexec(pd, (const char *)sql);
      if(PQresultStatus(result) == PGRES_TUPLES_OK){
        // this is a valid layer
         valid = true;
         std::cout << "geometry column query returned " << PQntuples(result) << std::endl;
         // store the srid 
         std::cout << "column number of srid is " << PQfnumber(result,"srid")  << std::endl;
         srid =  PQgetvalue(result, 0, PQfnumber(result,"srid"));
			std::cout << "SRID is " << srid << std::endl;
      //std::cout << "Format is " << PQfformat(result, PQfnumber(result,"srid")) << std::endl;
      // set the type
      	// set the simple type for use with symbology operations
			QString fType = PQgetvalue(result, 0, PQfnumber(result,"type"));
			if (fType == "POINT" || fType == "MULTIPOINT")
				geomType = QGis::WKBPoint;
			else if (fType == "LINESTRING" || fType == "MULTILINESTRING")
				geomType = QGis::WKBLineString;
			else if (fType == "POLYGON" || fType == "MULTIPOLYGON")
				geomType = QGis::WKBPolygon;
      std::cout << "Feature type is " << geomType << std::endl;
      std::cout << "Feature type name is " << QGis::qgisFeatureTypes[geomType] << std::endl;
      // free the result
      PQclear(result);
         // get the extents
       qWarning("Getting extents using schema.table: " + sql);
       sql = "select xmax(extent(" + geometryColumn + ")) as xmax,"
			  "xmin(extent(" + geometryColumn + ")) as xmin,"
			  "ymax(extent(" + geometryColumn + ")) as ymax," "ymin(extent(" + geometryColumn + 
        ")) as ymin" " from " + tableName;
       result = PQexec(pd, (const char *)sql);
       	layerExtent.setXmax(QString(PQgetvalue(result, 0, PQfnumber(result,"xmax"))).toDouble());
				layerExtent.setXmin(QString(PQgetvalue(result, 0, PQfnumber(result,"xmin"))).toDouble());
				layerExtent.setYmax(QString(PQgetvalue(result, 0, PQfnumber(result,"ymax"))).toDouble());
				layerExtent.setYmin(QString(PQgetvalue(result, 0, PQfnumber(result,"ymin"))).toDouble());
				QString xMsg;
				QTextOStream(&xMsg).precision(18);
				QTextOStream(&xMsg).width(18);
				QTextOStream(&xMsg) << "Set extents to: " << layerExtent.
				  xMin() << ", " << layerExtent.yMin() << " " << layerExtent.xMax() << ", " << layerExtent.yMax();
         qWarning(xMsg);
       // clear query result
       PQclear(result);
       // get total number of features
       sql = "select count(*) from " + tableName;
       result = PQexec(pd, (const char *)sql);
       numberFeatures = QString(PQgetvalue(result, 0, 0)).toLong();
       std::cout << "Feature count is " << numberFeatures << std::endl;
       PQclear(result);
        // Populate the field vector for this layer. The field vector contains
      // field name, type, length, and precision (if numeric)
      sql = "select * from " + tableName + " limit 1";
      result = PQexec(pd, (const char *)sql);
      
      for (int i = 0; i < PQnfields(result); i++) {
        QString fieldName = PQfname(result, i);
        QString fieldType = getFieldTypeName(pd,PQftype(result, i));
        int fieldModifier = PQfmod(result, i);
        int fieldSize = PQfsize(result, i);
        std::cout << "Field: Name, Type, Size, Modifier:" << std::endl;
        std::cout << "Field: " << fieldName << ", " << fieldType << ", "  << fieldSize << ", " << fieldModifier <<  std::endl;
         /*  attributeFields.push_back(QgsField(
            fieldName, 
            fieldType,
            fieldSize,
            fieldModifier)); */
      }
      }else{
        // the table is not a geometry table
        valid = false;
      }
//		reset tableName to include schema
		tableName.prepend(schema+".");

    
    
    
    
    
		// get the extent_ (envelope) of the layer
    std::cout << "Starting get extent\n";
    
	//	extent_ = new OGREnvelope();
		
    std::cout << "Finished get extent\n";
    // getting the total number of features in the layer
   // numberFeatures = ogrLayer->GetFeatureCount();
		// check the validity of the layer
    std::cout << "checking validity\n";
	 // set valid = true or false here
      // Populate the field vector for this layer. The field vector contains
      // field name, type, length, and precision (if numeric)
     /*  for (int i = 0; i < feat->GetFieldCount(); i++) {
          OGRFieldDefn *fldDef = feat->GetFieldDefnRef(i);
          attributeFields.push_back(QgsField(
            fldDef->GetNameRef(), 
            fldDef->GetFieldTypeName(fldDef->GetType()),
            fldDef->GetWidth(),
            fldDef->GetPrecision()));
      } */

		 } else {
			valid = false;
      std::cout << "Connection to database failed\n";
		}
    
        std::cout << "Done checking validity\n";
	/* } else {
		std::cout << "Data source is invalid" << std::endl;
		valid = false;
	} */

	//create a boolean vector and set every entry to false

/* 	if (valid) {
		selected = new std::vector < bool > (ogrLayer->GetFeatureCount(), false);
	} else {
		selected = 0;
	} */
//  tabledisplay=0;
	//draw the selected features in yellow
//  selectionColor.setRgb(255,255,0);

}

QgsPostgresProvider::~QgsPostgresProvider()
{
}

/**
	* Get the first feature resutling from a select operation
	* @return QgsFeature
	*/
QgsFeature *QgsPostgresProvider::getFirstFeature(bool fetchAttributes)
{
	QgsFeature *f = 0;
	if(valid){
		std::cout << "getting first feature\n";
	
		f = new QgsFeature();
	/* 	f->setGeometry(getGeometryPointer(feat));
     if(fetchAttributes){
       getFeatureAttributes(feat, f);
      } */
	}
	return f;
}

	/**
	* Get the next feature resutling from a select operation
    * Return 0 if there are no features in the selection set
	* @return QgsFeature
	*/
QgsFeature *QgsPostgresProvider::getNextFeature(bool fetchAttributes)
{
    
	QgsFeature *f = 0;
/* 	if(valid){
		//std::cout << "getting next feature\n";
		OGRFeature *fet = ogrLayer->GetNextFeature();
		if(fet){
            OGRGeometry *geom = fet->GetGeometryRef();
			
			// get the wkb representation
			unsigned char *feature = new unsigned char[geom->WkbSize()];
			geom->exportToWkb((OGRwkbByteOrder) endian(), feature);
            f = new QgsFeature();
            f->setGeometry(feature);
            if(fetchAttributes){
              getFeatureAttributes(fet, f);
            }
         
            delete fet;
		}else{
			std::cout << "Feature is null\n";
            // probably should reset reading here
            ogrLayer->ResetReading();
		}
    
		
	}else{
        std::cout << "Read attempt on an invalid shapefile data source\n";
    } */
	return f;
}

	/**
	* Select features based on a bounding rectangle. Features can be retrieved
	* with calls to getFirstFeature and getNextFeature.
	* @param mbr QgsRect containing the extent to use in selecting features
	*/
void QgsPostgresProvider::select(QgsRect *rect)
{
    // spatial query to select features
  //  std::cout << "Selection rectangle is " << *rect << std::endl;
/*     OGRGeometry *filter = 0;
	filter = new OGRPolygon();
	QString wktExtent = QString("POLYGON ((%1))").arg(rect->stringRep());
	const char *wktText = (const char *)wktExtent;

	OGRErr result = ((OGRPolygon *) filter)->importFromWkt((char **)&wktText);
    //TODO - detect an error in setting the filter and figure out what to
    //TODO   about it. If setting the filter fails, all records will be returned
	if (result == OGRERR_NONE) {
  //      std::cout << "Setting spatial filter using " << wktExtent    << std::endl;
		ogrLayer->SetSpatialFilter(filter);
  //      std::cout << "Feature count: " << ogrLayer->GetFeatureCount() << std::endl;

	}else{
        std::cout << "Setting spatial filter failed!" << std::endl;
    } */
}

	/**
		* Set the data source specification. This may be a path or database
	* connection string
	* @uri data source specification
	*/
void QgsPostgresProvider::setDataSourceUri(QString uri)
{
	dataSourceUri = uri;
}

		/**
	* Get the data source specification. This may be a path or database
	* connection string
	* @return data source specification
	*/
QString QgsPostgresProvider::getDataSourceUri()
{
	return dataSourceUri;
}

	/**
	* Identify features within the search radius specified by rect
	* @param rect Bounding rectangle of search radius
	* @return std::vector containing QgsFeature objects that intersect rect
	*/
std::vector < QgsFeature > QgsPostgresProvider::identify(QgsRect * rect)
{
  features.clear();
  // select the features
  select(rect);
  
	return features;
}

/* unsigned char * QgsPostgresProvider::getGeometryPointer(OGRFeature *fet){
//	OGRGeometry *geom = fet->GetGeometryRef();
	unsigned char *gPtr=0;
		// get the wkb representation
	 	gPtr = new unsigned char[geom->WkbSize()];
      
		geom->exportToWkb((OGRwkbByteOrder) endian(), gPtr);
	return gPtr;

} */

int QgsPostgresProvider::endian()
{
	char *chkEndian = new char[4];
	memset(chkEndian, '\0', 4);
	chkEndian[0] = 0xE8;

	int *ce = (int *) chkEndian;
	int retVal;
	if (232 == *ce)
		retVal = NDR;
	else
		retVal = XDR;
	delete[]chkEndian;
	return retVal;
}

// TODO - make this function return the real extent_
QgsRect *QgsPostgresProvider::extent()
{
  return new QgsRect();//extent_->MinX, extent_->MinY, extent_->MaxX, extent_->MaxY);
}

/** 
* Return the feature type
*/
int QgsPostgresProvider::geometryType(){
    return geomType;
}
/** 
* Return the feature type
*/
long QgsPostgresProvider::featureCount(){
    return numberFeatures;
}

/**
* Return the number of fields
*/
int QgsPostgresProvider::fieldCount(){
  return attributeFields.size();
}
/**
* Fetch attributes for a selected feature
*/
/* void QgsPostgresProvider::getFeatureAttributes(OGRFeature *ogrFet, QgsFeature *f){
  for (int i = 0; i < ogrFet->GetFieldCount(); i++) {

					// add the feature attributes to the tree
					OGRFieldDefn *fldDef = ogrFet->GetFieldDefnRef(i);
					QString fld = fldDef->GetNameRef();
			//		OGRFieldType fldType = fldDef->GetType();
					QString val;

					val = ogrFet->GetFieldAsString(i);
          f->addAttribute(fld, val);
}
} */

std::vector<QgsField> QgsPostgresProvider::fields(){
  return attributeFields;
}

void QgsPostgresProvider::reset(){
 /*  ogrLayer->SetSpatialFilter(0);
  ogrLayer->ResetReading(); */
}

QString QgsPostgresProvider::getFieldTypeName(PGconn *pd, int oid){
  QString typOid = QString().setNum(oid);
  QString sql = "select typname from pg_type where typelem = " + typOid +
    " and typstorage = 'x'";
  
  PGresult * result = PQexec(pd, (const char *)sql);
  QString typeName = PQgetvalue(result,0,0);
  PQclear(result);
  return typeName;
}
/**
* Class factory to return a pointer to a newly created 
* QgsPostgresProvider object
*/
extern "C" QgsPostgresProvider * classFactory(const char *uri)
{
	return new QgsPostgresProvider(uri);
}


