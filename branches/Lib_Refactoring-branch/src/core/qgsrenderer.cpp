
#include "qgsrenderer.h"

#include <QColor>
#include <QString>


QColor QgsRenderer::mSelectionColor=QColor(0,0,0);

QgsRenderer::QgsRenderer()
{

}

QgsRenderer::~QgsRenderer()
{
}

void QgsRenderer::setSelectionColor(QColor color)
{
  mSelectionColor = color;
}
