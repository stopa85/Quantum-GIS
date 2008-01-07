#include <iostream>
#include "qgssymbolrenderer.h"

QgsSymbolRenderer::QgsSymbolRenderer()
{
//  mSize = 6;//really ought to be set up by the symbol which owns this renderer
  //maybe there doesn't need to be a no-param constructor...
}

QgsSymbolRenderer::~QgsSymbolRenderer()
{}

void QgsSymbolRenderer::render(QPainter* p)
{
  //Don't have to set up the pen and brush, because that has been done
  //before we were called.
  QRectF symbolBoundingRect(-mSize / 2, -mSize / 2, mSize, mSize);
  p->drawEllipse(symbolBoundingRect);
}

//inline?
void QgsSymbolRenderer::setSize(float size)
{
  mSize = size; 
}

//inline?
float QgsSymbolRenderer::size(void)
{
  return(mSize);  
}
