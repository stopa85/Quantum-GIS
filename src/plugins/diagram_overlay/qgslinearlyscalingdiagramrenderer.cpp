#include "qgslinearlyscalingdiagramrenderer.h"
#include "qgsdiagramfactory.h"
#include "qgsfeature.h"
#include <limits>
#include <QDomElement>
#include <QImage>
#include <cmath>

QgsLinearlyScalingDiagramRenderer::QgsLinearlyScalingDiagramRenderer(int classificationAttribute): QgsDiagramRenderer(classificationAttribute), mProportion(QgsLinearlyScalingDiagramRenderer::AREA)
{
  
}

QgsLinearlyScalingDiagramRenderer::~QgsLinearlyScalingDiagramRenderer()
{

}

QImage* QgsLinearlyScalingDiagramRenderer::renderDiagram(const QgsFeature& f) const
{
  if(!mFactory || mItems.size() < 1)
    {
      return 0;
    }

  int size;
  if(calculateDiagramSize(f, size) != 0)
    {
      return 0;
    }
 
  return mFactory->createDiagram(size, f);
}

int QgsLinearlyScalingDiagramRenderer::getDiagramSize(int& width, int& height, const QgsFeature& f) const
{
  //first find out classification value
  if(!mFactory || mItems.size() < 1)
    {
      return 1;
    }

  int size;
  if(calculateDiagramSize(f, size) != 0)
    {
      return 2;
    }
 
  if(mFactory->getDiagramDimensions(size, f, width, height) != 0)
    {
      return 3;
    }
  return 0;
}

int QgsLinearlyScalingDiagramRenderer::createLegendContent(QMap<QString, QImage*> items) const
{
  if(!mFactory || mItems.size() < 1)
    {
      return 1;
    }

  //determine a size and value for the legend, use the middle item for this
  int element = (int)(mItems.size()/2);
  double value = mItems.at(element).value;
  int size = mItems.at(element).size;

  if(mFactory->createLegendContent(size, value, items) != 0)
    {
      return 2;
    }
  return 0;
}

bool QgsLinearlyScalingDiagramRenderer::readXML(const QDomNode& rendererNode)
{
#if 0
  QDomElement rendererElem = rendererNode.toElement();

  double lowerBound, upperBound;
  int width, height;
  bool conversionOk;

  //loweritem
  QDomNodeList lowerItemList = rendererElem.elementsByTagName("loweritem"); 
  if(lowerItemList.size() < 1)
    {
      return false;
    }

  QDomElement lowerItemElem = lowerItemList.at(0).toElement();
  lowerBound = lowerItemElem.attribute("lower_bound").toDouble(&conversionOk);
  if(!conversionOk)
    {
      return false;
    }
  upperBound = lowerItemElem.attribute("upper_bound").toDouble(&conversionOk);
  if(!conversionOk)
    {
      return false;
    }
  width = lowerItemElem.attribute("width").toInt(&conversionOk);
  if(!conversionOk)
    {
      return false;
    }
  height = lowerItemElem.attribute("height").toInt(&conversionOk);
  if(!conversionOk)
    {
      return false;
    }
  setLowerItem(QgsDiagramItem(lowerBound, upperBound, height, width));

  //upperitem
  QDomNodeList upperItemList = rendererElem.elementsByTagName("upperitem");
  if(upperItemList.size() < 1)
    {
      return false;
    }
  
  QDomElement upperItemElem = upperItemList.at(0).toElement();
  lowerBound = upperItemElem.attribute("lower_bound").toDouble(&conversionOk);
  if(!conversionOk)
    {
      return false;
    }
  upperBound = upperItemElem.attribute("upper_bound").toDouble(&conversionOk);
  if(!conversionOk)
    {
      return false;
    }
  width = upperItemElem.attribute("width").toInt(&conversionOk);
  if(!conversionOk)
    {
      return false;
    }
  height = upperItemElem.attribute("height").toInt(&conversionOk);
  if(!conversionOk)
    {
      return false;
    }
  setUpperItem(QgsDiagramItem(lowerBound, upperBound, height, width));

  return true;
#endif //0
  return false;
}

bool QgsLinearlyScalingDiagramRenderer::writeXML(QDomNode& overlay_node, QDomDocument& doc) const
{
  QDomElement rendererElement = doc.createElement("renderer");
  rendererElement.setAttribute("type", "linearly_scaling");
  overlay_node.appendChild(rendererElement);

  QList<QgsDiagramItem>::const_iterator item_it = mItems.constBegin();
  for(; item_it != mItems.constEnd(); ++item_it)
    {
      QDomElement itemElement = doc.createElement("diagramitem");
      itemElement.setAttribute("size", item_it->size);
      itemElement.setAttribute("value", item_it->value);
    }
  return true;
}

int QgsLinearlyScalingDiagramRenderer::calculateDiagramSize(const QgsFeature& f, int& size) const
{
  //find out value for classificationAttribute
  double value;
  if(classificationValue(f, value) != 0)
    {
      return 1;
    }

  //find out size
  bool sizeAssigned = false;

  QList<QgsDiagramItem>::const_iterator current_it = mItems.constBegin();
  QList<QgsDiagramItem>::const_iterator last_it = mItems.constEnd();
  
  for(; current_it != mItems.constEnd(); ++current_it)
    {
      if(value < current_it->value)
	{
	  if(last_it == mItems.constEnd()) //values below classifications receive first items size
	    {
	      size = current_it->size;
	    }
	  else
	    {
	      size = interpolateSize(value, last_it->value, current_it->value, \
				     last_it->size, current_it->size);
	    }
	  sizeAssigned = true;
	  break;
	}
      last_it = current_it;
    }

  if(!sizeAssigned)//values above classification receive last items size
    {
      size = last_it->size;
    }

  return 0;
}

int QgsLinearlyScalingDiagramRenderer::interpolateSize(double value, double lowerValue, double upperValue, \
						       int lowerSize, int upperSize) const
{
  if(value <= lowerValue)
    {
      return lowerSize;
    }
  else if(value >= upperValue)
    {
      return upperSize;
    }

  QgsDiagramFactory::SizeType t;
  if(mFactory)
    {
      t = mFactory->sizeType();
    }

  if(!mFactory || t == QgsDiagramFactory::HEIGHT)
    {
      //do one dimensional linear interpolation
      return (int)(((value - lowerValue) * upperSize + (upperValue - value) * lowerSize) / (upperValue - lowerValue));
    }
  else if(t == QgsDiagramFactory::DIAMETER)
    {
      double lowerArea = (lowerSize/2)*(lowerSize/2)*M_PI;
      double upperArea = (upperSize/2)*(upperSize/2)*M_PI;
      double valueArea = ((value - lowerValue) * upperArea + (upperValue - value) * lowerArea)/(upperValue - lowerValue);
      return (int)(2*sqrt(valueArea/M_PI));
    }
  else
    {
      return 0; //unknown type...
    }
}
