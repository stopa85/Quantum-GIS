#include "qgsdiagramrenderer.h"
#include "qgsdiagramfactory.h"
#include "qgsfeature.h"

QgsDiagramRenderer::QgsDiagramRenderer(int classificationAttribute): mClassificationAttribute(classificationAttribute)
{
}

QgsDiagramRenderer::~QgsDiagramRenderer()
{
  delete mFactory;
}

QgsDiagramRenderer::QgsDiagramRenderer()
{
}


int QgsDiagramRenderer::classificationValue(const QgsFeature& f, double& value) const
{
  //find out attribute value of the feature
  QgsAttributeMap featureAttributes = f.attributeMap();
  QgsAttributeMap::const_iterator iter = featureAttributes.find(mClassificationAttribute);
  if(iter == featureAttributes.constEnd())
    {
      return 1;
    }

  value = iter.value().toDouble();
  return 0;
}
