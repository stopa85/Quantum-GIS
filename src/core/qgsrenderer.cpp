#include "qgslegendvectorsymbologyitem.h"
#include "qgsrenderer.h"
#include "qgssymbol.h"

#include <QColor>
#include <QPixmap>
#include <QString>

// TODO: remove! [MD]
#include "qgsproject.h"
#include <QDomNode>
#include <QDomDocument>


QColor QgsRenderer::mSelectionColor=QColor(0,0,0);

QgsRenderer::QgsRenderer()
{

}

void QgsRenderer::refreshLegend(std::list< std::pair<QString, QPixmap> >* symbologyList) const
{
    if(symbologyList)
    {
      //add the new items
      QString lw, uv, label;
      const std::list<QgsSymbol*> sym = symbols();
	
      for(std::list<QgsSymbol*>::const_iterator it=sym.begin(); it!=sym.end(); ++it)
	{
	  QPixmap pix;
	  if((*it)->type() == QGis::Point)
	    {
	      pix = (*it)->getPointSymbolAsPixmap();
	    }
	  else if((*it)->type() == QGis::Line)
	    {
	      pix = (*it)->getLineSymbolAsPixmap();
	    }
	  else //polygon
	    {
	      pix = (*it)->getPolygonSymbolAsPixmap();
	    }
 
	  QString values;
	  lw = (*it)->lowerValue();
	  if(!lw.isEmpty())
	    {
	      values += lw;
	    }
	  uv = (*it)->upperValue();
	  if(!uv.isEmpty())
	    {
	      values += " - ";
	      values += uv;
	    }
	  label = (*it)->label();
	  if(!label.isEmpty())
	    {
	      values += " ";
	      values += label;
	    }
	  symbologyList->push_back(std::make_pair(values, pix));
	}
    }
}

void QgsRenderer::initialiseSelectionColor()
{
  int myRedInt = QgsProject::instance()->readNumEntry("Gui","/SelectionColorRedPart",255);
  int myGreenInt = QgsProject::instance()->readNumEntry("Gui","/SelectionColorGreenPart",255);
  int myBlueInt = QgsProject::instance()->readNumEntry("Gui","/SelectionColorBluePart",0);
  mSelectionColor = QColor(myRedInt,myGreenInt,myBlueInt);
}
