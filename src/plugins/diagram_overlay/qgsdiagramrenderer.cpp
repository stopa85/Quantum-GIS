#include "qgsdiagramrenderer.h"
#include "qgsdiagramfactory.h"
#include "qgsfeature.h"

QgsDiagramRenderer::QgsDiagramRenderer(const QList<int>& classificationAttributes): mClassificationAttributes(classificationAttributes)
{
}

QgsDiagramRenderer::~QgsDiagramRenderer()
{
  delete mFactory;
}

QgsDiagramRenderer::QgsDiagramRenderer()
{
}


int QgsDiagramRenderer::classificationValue(const QgsFeature& f, QVariant& value) const
{
  //find out attribute value of the feature
  QgsAttributeMap featureAttributes = f.attributeMap();
  
  QgsAttributeMap::const_iterator iter;
  
  if(value.type() == QVariant::String) //string type
    {
      //we can only handle one classification field for strings
      if(mClassificationAttributes.size() > 1)
	{
	  return 1;
	}
      
      iter = featureAttributes.find(mClassificationAttributes.first());
      if(iter == featureAttributes.constEnd())
	{
	  return 2;
	}
      value = iter.value();
    }
  else //numeric type
    {
      double currentValue;
      double totalValue = 0;

      QList<int>::const_iterator list_it = mClassificationAttributes.constBegin();
      for(; list_it != mClassificationAttributes.end(); ++list_it)
	{
	  QgsAttributeMap::const_iterator iter = featureAttributes.find(*list_it);
	  if(iter == featureAttributes.constEnd())
	    {
	      continue;
	    }
	  currentValue = iter.value().toDouble();
	  totalValue += currentValue;
	}
      value = QVariant(totalValue);
    }
  return 0;
}

void QgsDiagramRenderer::addClassificationAttribute(int attrNr)
{
  mClassificationAttributes.push_back(attrNr);
}
