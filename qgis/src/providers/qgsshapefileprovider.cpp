#include "../qgsdataprovider.h"
#include "../qgsfeature.h"
#include "../qgsrect.h"
#include "qgsshapefileprovider.h"
QgsShapeFileProvider::QgsShapeFileProvider(){
}
QgsShapeFileProvider::~QgsShapeFileProvider(){
}
/** 
	* Get the first feature resutling from a select operation
	* @return QgsFeature
	*/
	QgsFeature *QgsShapeFileProvider::getFirstFeature(){
		return new QgsFeature();
	}
	/** 
	* Get the next feature resutling from a select operation
	* @return QgsFeature
	*/
	QgsFeature *QgsShapeFileProvider::getNextFeature(){
		return(new QgsFeature());
	}
	/**
	* Select features based on a bounding rectangle. Features can be retrieved 
	* with calls to getFirstFeature and getNextFeature.
	* @param extent QgsRect containing the extent to use in selecting features
	*/
	void QgsShapeFileProvider::select(){
	}
	/** 
		* Set the data source specification. This may be a path or database
	* connection string
	* @uri data source specification
	*/
	void QgsShapeFileProvider::setDataSourceUri(QString uri){
	}
	
		/** 
	* Get the data source specification. This may be a path or database
	* connection string
	* @return data source specification
	*/
	QString QgsShapeFileProvider::getDataSourceUri(){
		return QString("uri");
	}
	/**
	* Identify features within the search radius specified by rect
	* @param rect Bounding rectangle of search radius
	* @return std::vector containing QgsFeature objects that intersect rect
	*/
	std::vector<QgsFeature> QgsShapeFileProvider::identify(QgsRect *rect){
		return features;
	}
extern "C" QgsShapeFileProvider * classFactory(){
	return new QgsShapeFileProvider();
}
