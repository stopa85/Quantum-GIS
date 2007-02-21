#include "qgsdiagramrenderer.h"
#include "qgsfield.h"
#include "qgsvectordataprovider.h"
#include <QImage>

QgsDiagramRenderer::QgsDiagramRenderer(const QString& name, const QgsAttributeList& att, const std::list<QColor>& c): mWellKnownName(name), mAttributes(att), mColors(c)
{
  mFactory.setDiagramType(name);
  mFactory.setAttributes(att);
  mFactory.setColorSeries(c);
}

QgsDiagramRenderer::~QgsDiagramRenderer()
{
  
}


