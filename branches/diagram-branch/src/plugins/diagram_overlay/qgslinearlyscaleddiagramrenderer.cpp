#include "qgslinearlyscalingddiagramrenderer.h"
#include "qgsfeature.h"
#include "qgsfeatureattribute.h"

QgsLinearlyScalingdDiagramRenderer::QgsLinearlyScaledDiagramRenderer(const QString& name, const std::list<int>& att, const std::list<QColor>& c): QgsDiagramRenderer(name, att, c)
{

}

QgsLinearlyScalingdDiagramRenderer::~QgsLinearlyScaledDiagramRenderer()
{

}

QImage* QgsLinearlyScaledDiagramRenderer::renderDiagram(const QgsFeature& f) const
{
  //find out attribute value of the feature
  QgsAttributeMap featureAttributes = f.attributeMap();
  QgsAttributeMap::const_iterator iter = featureAttributes.find(mClassificationField);
  if(iter == featureAttributes.constEnd())
    {
      return 0;
    }
  
  double theValue = iter.value().fieldValue().toDouble();
  double scalefactor = (theValue - mLowerItem.lowerBound()) / (mUpperItem.lowerBound() - mLowerItem.lowerBound());

  //linearly interpolate height and width according to min/max value
  int newHeight = (int)(mLowerItem.height() + (mUpperItem.height() - mLowerItem.height())*scalefactor);
  int newWidth = (int)(mLowerItem.width() + (mUpperItem.width() - mLowerItem.height())*scalefactor);
  
  QgsDiagramItem newItem(theValue, theValue, newHeight, newWidth);
  
  return mFactory.createDiagram(newItem, f);
}
