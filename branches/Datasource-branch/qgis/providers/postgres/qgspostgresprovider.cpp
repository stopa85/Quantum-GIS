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
    tableName = uri.mid(uri.find("table=") + 6);
    QString connInfo = uri.left(uri.find("table="));
    std::cout << "Table name is " << tableName << std::endl;
    std::cout << "Connection info is " << connInfo << std::endl;
    // calculate the schema if specified
    QString schema = "";
    if (tableName.find(".") > -1) {
        schema = tableName.left(tableName.find("."));
    }
    geometryColumn = tableName.mid(tableName.find(" (") + 2);
    geometryColumn.truncate(geometryColumn.length() - 1);
    tableName = tableName.mid(tableName.find(".") + 1, tableName.find(" (") - (tableName.find(".") + 1));

    qWarning("Geometry column is: " + geometryColumn);
    qWarning("Schema is: " + schema);
    qWarning("Table name is: " + tableName);

    PGconn *pd = PQconnectdb((const char *) connInfo);
    // check the connection status
    if (PQstatus(pd) == CONNECTION_OK) {
        std::cout << "Connection to the database was successful\n";

        // check the geometry column
        QString sql = "select f_geometry_column,type,srid from geometry_columns where f_table_name='"
          + tableName + "' and f_geometry_column = '" + geometryColumn + "' and f_table_schema = '" + schema + "'";
        qWarning("Getting geometry column: " + sql);
        PGresult *result = PQexec(pd, (const char *) sql);
        if (PQresultStatus(result) == PGRES_TUPLES_OK) {
            // this is a valid layer
            valid = true;
            std::cout << "geometry column query returned " << PQntuples(result) << std::endl;
            // store the srid 
            std::cout << "column number of srid is " << PQfnumber(result, "srid") << std::endl;
            srid = PQgetvalue(result, 0, PQfnumber(result, "srid"));
            std::cout << "SRID is " << srid << std::endl;
            //std::cout << "Format is " << PQfformat(result, PQfnumber(result,"srid")) << std::endl;
            // set the type
            // set the simple type for use with symbology operations
            QString fType = PQgetvalue(result, 0, PQfnumber(result, "type"));
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
              "ymax(extent(" + geometryColumn + ")) as ymax," "ymin(extent(" + geometryColumn + ")) as ymin" " from " + tableName;
            result = PQexec(pd, (const char *) sql);
            layerExtent.setXmax(QString(PQgetvalue(result, 0, PQfnumber(result, "xmax"))).toDouble());
            layerExtent.setXmin(QString(PQgetvalue(result, 0, PQfnumber(result, "xmin"))).toDouble());
            layerExtent.setYmax(QString(PQgetvalue(result, 0, PQfnumber(result, "ymax"))).toDouble());
            layerExtent.setYmin(QString(PQgetvalue(result, 0, PQfnumber(result, "ymin"))).toDouble());
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
            result = PQexec(pd, (const char *) sql);
            numberFeatures = QString(PQgetvalue(result, 0, 0)).toLong();
            std::cout << "Feature count is " << numberFeatures << std::endl;
            PQclear(result);
             // selectSQL stores the select sql statement. This has to include each attribute
             // plus the geometry column in binary form
            selectSQL = "select ";
            // Populate the field vector for this layer. The field vector contains
            // field name, type, length, and precision (if numeric)
            sql = "select * from " + tableName + " limit 1";
            result = PQexec(pd, (const char *) sql);
            std::cout << "Field: Name, Type, Size, Modifier:" << std::endl;
            for (int i = 0; i < PQnfields(result); i++) {

                QString fieldName = PQfname(result, i);
                int fldtyp = PQftype(result, i);
                QString typOid = QString().setNum(fldtyp);
                int fieldModifier = PQfmod(result, i);
                QString sql = "select typelem from pg_type where typelem = " + typOid + " and typlen = -1";
                //  std::cout << sql << std::endl;
                PGresult *oidResult = PQexec(pd, (const char *) sql);
                // get the oid of the "real" type
                QString poid = PQgetvalue(oidResult, 0, PQfnumber(oidResult, "typelem"));
                PQclear(oidResult);
                sql = "select typname, typlen from pg_type where oid = " + poid;
                // std::cout << sql << std::endl;
                oidResult = PQexec(pd, (const char *) sql);

                QString fieldType = PQgetvalue(oidResult, 0, 0);
                QString fieldSize = PQgetvalue(oidResult, 0, 1);
                PQclear(oidResult);
                std::cout << "Field: " << fieldName << ", " << fieldType << " (" << fldtyp << "),  " << fieldSize << ", " <<
                  fieldModifier << std::endl;
                attributeFields.push_back(QgsField(fieldName, fieldType, fieldSize.toInt(), fieldModifier));
                // add to the select sql statement
                if(i > 0){
                  selectSQL += ", ";
                }
                if(fieldType == "geometry"){
                  selectSQL += "asbinary(" + geometryColumn + ",'" + endianString() + "') as qgs_feature_geometry";
                }else{
                  selectSQL += fieldName;
                }
            }
            selectSQL += " from " + tableName;
            std::cout << "selectSQL: " << (const char *)selectSQL << std::endl;
            PQclear(result);
            // get the total number of features in the layer
            sql = "select count(*) from " + tableName;
            result = PQexec(pd, (const char *) sql);
            numberFeatures = QString(PQgetvalue(result, 0, 0)).toLong();
            std::cout << "Number of features in " << (const char *) tableName << ": " << numberFeatures << std::endl;
        } else {
            // the table is not a geometry table
            valid = false;
        }
//      reset tableName to include schema
        schemaTableName += schema + "." + tableName;
    
        // store the connection for future use
        connection = pd;
        ready = false; // not ready to read yet cuz the cursor hasn't been created
    } else {
        valid = false;
        std::cout << "Connection to database failed\n";
    }
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

//TODO - we may not need this function - consider removing it from
//       the dataprovider.h interface
/**
	* Get the first feature resutling from a select operation
	* @return QgsFeature
	*/
  //TODO - this function is a stub and always returns 0
QgsFeature *QgsPostgresProvider::getFirstFeature(bool fetchAttributes)
{
    QgsFeature *f = 0;
    if (valid) {
        std::cout << "getting first feature\n";

        f = new QgsFeature();
        /*  f->setGeometry(getGeometryPointer(feat));
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
    
    if (valid){
      // If the query result is not valid, then select all features
      // before proceeding
   /*    if(!ready) {
        // set up the 
        QString declare = QString("declare qgisf binary cursor for select oid," 
          "asbinary(%1,'%2') as qgs_feature_geometry from %3").arg(geometryColumn).arg(endianString()).arg(tableName);
        std::cout << "Selecting features using: " << declare << std::endl;
        // set up the cursor
        PQexec(connection,"begin work");
        PQexec(connection, (const char *)declare);
         std::cout << "Error: " << PQerrorMessage(connection) << std::endl;
           ready = true;
      } */
        QString fetch = "fetch forward 1 from qgisf";
        queryResult = PQexec(connection, (const char *)fetch);
        //std::cout << "Error: " << PQerrorMessage(connection) << std::endl;
        std::cout << "Fetched " << PQntuples(queryResult) << "rows" << std::endl;
        if(PQntuples(queryResult) == 0){
          PQexec(connection, "end work");
          return 0;
        } 
     //  std::cout <<"Raw value of the geometry field: " << PQgetvalue(queryResult,0,PQfnumber(queryResult,"qgs_feature_geometry")) << std::endl;
       std::cout << "Length of oid is " << PQgetlength(queryResult,0, PQfnumber(queryResult,"oid")) << std::endl;
      int oid = *(int *)PQgetvalue(queryResult,0,PQfnumber(queryResult,"oid"));
      // oid is in big endian
      int *noid;
      if(endian() == NDR){
        std::cout << "converting oid to little endian" << std::endl;
        // convert oid to little endian
        char *temp  = new char[sizeof(oid)];
        char *ptr = (char *)&oid + sizeof(oid) -1;
        int cnt = 0;
        while(cnt < sizeof(oid)){
          temp[cnt] = *ptr--;
          cnt++;
        }
        noid = (int *)temp;
        
      }
      // noid contains the oid to be used in fetching attributes if 
      // fetchAttributes = true
      std::cout << "OID: " << *noid << std::endl;
      int returnedLength = PQgetlength(queryResult,0, PQfnumber(queryResult,"qgs_feature_geometry"));
      std::cout << "Returned length is " << returnedLength << std::endl;
      if(returnedLength > 0){
      unsigned char *feature = new unsigned char[returnedLength + 1];
      memset(feature, '\0', returnedLength + 1);
      memcpy(feature, PQgetvalue(queryResult,0,PQfnumber(queryResult,"qgs_feature_geometry")), returnedLength);
      int wkbType = *((int *) (feature + 1));
      std::cout << "WKBtype is: " << wkbType << std::endl;
      f = new QgsFeature(*noid);
      f->setGeometry(feature);
        if (fetchAttributes) {
          getFeatureAttributes(*noid, f);
        }
      }else{
        std::cout <<"Couldn't get the feature geometry in binary form" << std::endl;
      }
         //std::cout << "getting next feature\n";
        // Get the geometry first
        
     /*    OGRFeature *fet = ogrLayer->GetNextFeature();
        if (fet) {
            OGRGeometry *geom = fet->GetGeometryRef();

            // get the wkb representation
            unsigned char *feature = new unsigned char[geom->WkbSize()];
            geom->exportToWkb((OGRwkbByteOrder) endian(), feature);
            f = new QgsFeature();
            f->setGeometry(feature);
            if (fetchAttributes) {
                getFeatureAttributes(fet, f);
            }

            delete fet;
        } else {
            std::cout << "Feature is null\n";
            // probably should reset reading here
            ogrLayer->ResetReading();
        }

 */
    } else {
        std::cout << "Read attempt on an invalid postgresql data source\n";
    }
    return f;
}

    /**
	* Select features based on a bounding rectangle. Features can be retrieved
	* with calls to getFirstFeature and getNextFeature.
	* @param mbr QgsRect containing the extent to use in selecting features
	*/
void QgsPostgresProvider::select(QgsRect * rect)
{
    // spatial query to select features
    std::cout << "Selection rectangle is " << *rect << std::endl;
    QString declare = QString("declare qgisf binary cursor for select oid," 
    "asbinary(%1,'%2') as qgs_feature_geometry from %3").arg(geometryColumn).arg(endianString()).arg(tableName);
    declare += " where " + geometryColumn;
    declare += " && GeometryFromText('BOX3D(" + rect->stringRep();
    declare += ")'::box3d,";
    declare += srid;
    declare += ")";          
    std::cout << "Selecting features using: " << declare << std::endl;
    // set up the cursor
    if(ready){
      PQexec(connection, "end work");
    }
    PQexec(connection,"begin work");
    PQexec(connection, (const char *)declare);
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
    return &layerExtent;      //extent_->MinX, extent_->MinY, extent_->MaxX, extent_->MaxY);
}

/** 
* Return the feature type
*/
int QgsPostgresProvider::geometryType()
{
    return geomType;
}

/** 
* Return the feature type
*/
long QgsPostgresProvider::featureCount()
{
    return numberFeatures;
}

/**
* Return the number of fields
*/
int QgsPostgresProvider::fieldCount()
{
    return attributeFields.size();
}

/**
* Fetch attributes for a selected feature
*/
 void QgsPostgresProvider::getFeatureAttributes(int oid, QgsFeature *f){
  QString sql = QString("select * from %1 where oid = %2").arg(tableName).arg(oid);
  PGresult *attr = PQexec(connection, (const char *)sql);
  
   for (int i = 0; i < fieldCount(); i++) {
     QString fld = PQfname(attr, i);
     // Dont add the WKT representation of the geometry column to the identify
     // results
     if(fld != geometryColumn){
       // Add the attribute to the feature
       QString val = PQgetvalue(attr,0, i);
       f->addAttribute(fld, val);
     }
   }
} 

std::vector < QgsField > QgsPostgresProvider::fields()
{
    return attributeFields;
}

void QgsPostgresProvider::reset()
{
  
    /*  ogrLayer->SetSpatialFilter(0);
       ogrLayer->ResetReading(); */
}
/* QString QgsPostgresProvider::getFieldTypeName(PGconn * pd, int oid)
{
    QString typOid = QString().setNum(oid);
    QString sql = "select typelem from pg_type where typelem = " + typOid + " and typlen = -1";
    //std::cout << sql << std::endl;
    PGresult *result = PQexec(pd, (const char *) sql);
    // get the oid of the "real" type
    QString poid = PQgetvalue(result, 0, PQfnumber(result, "typelem"));
    PQclear(result);
    sql = "select typname, typlen from pg_type where oid = " + poid;
    // std::cout << sql << std::endl;
    result = PQexec(pd, (const char *) sql);

    QString typeName = PQgetvalue(result, 0, 0);
    QString typeLen = PQgetvalue(result, 0, 1);
    PQclear(result);
    typeName += "(" + typeLen + ")";
    return typeName;
} */
QString QgsPostgresProvider::endianString()
{
	char *chkEndian = new char[4];
	memset(chkEndian, '\0', 4);
	chkEndian[0] = 0xE8;
	int *ce = (int *) chkEndian;
	if (232 == *ce)
		return QString("NDR");
	else
		return QString("XDR");
}
QString QgsPostgresProvider::getPrimaryKey(){
  /*
Process to determine the fields used in a primary key:
  test=# select oid from pg_class where relname = 'earthquakes';
    oid
  -------
   24865
  (1 row)
 
  test=# select indkey from pg_index where indrelid = 24865 and indisprimary = 't';
   indkey
  --------
   1 5
  (1 row)

Primary key is composed of fields 1 and 5
*/
}
/**
* Class factory to return a pointer to a newly created 
* QgsPostgresProvider object
*/
extern "C" QgsPostgresProvider * classFactory(const char *uri)
{
    return new QgsPostgresProvider(uri);
}
