#include "../qgsdataprovider.h"
class QgsFeature;

class QgsShapeFileProvider : public QgsDataProvider {
public:
	QgsShapeFileProvider();
	virtual ~QgsShapeFileProvider();
/** 
	* Get the first feature resutling from a select operation
	* @return QgsFeature
	*/
	QgsFeature * getFirstFeature();
	/** 
	* Get the next feature resutling from a select operation
	* @return QgsFeature
	*/
	QgsFeature * getNextFeature();
	/**
	* Select features based on a bounding rectangle. Features can be retrieved 
	* with calls to getFirstFeature and getNextFeature.
	* @param extent QgsRect containing the extent to use in selecting features
	*/
	void select();
	/** 
		* Set the data source specification. This may be a path or database
	* connection string
	* @uri data source specification
	*/
	void setDataSourceUri(QString uri);
	
		/** 
	* Get the data source specification. This may be a path or database
	* connection string
	* @return data source specification
	*/
	QString getDataSourceUri();
	
	/**
	* Identify features within the search radius specified by rect
	* @param rect Bounding rectangle of search radius
	* @return std::vector containing QgsFeature objects that intersect rect
	*/
	virtual std::vector<QgsFeature> identify(QgsRect *rect);
private:
	std::vector<QgsFeature> features;
};
