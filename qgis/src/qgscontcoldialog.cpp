#include "qgscontcoldialog.h"
#include "qgsvectorlayer.h"
#include "qpushbutton.h"
#include "qcolordialog.h"
#include <qcombobox.h>
#include <cfloat>
#include "qgscontinuouscolrenderer.h"
#include <iostream>
#include "qgslayerproperties.h"
#include "qgslegenditem.h"
#include <qlineedit.h>
#include "qgsvectorlayerproperties.h"
#include "qgsdataprovider.h"

QgsContColDialog::QgsContColDialog(QgsVectorLayer* layer): QgsContColDialogBase(), m_vectorlayer(layer)
{
    QObject::connect(closebutton,SIGNAL(clicked()),this,SLOT(hide()));
    QObject::connect(applybutton,SIGNAL(clicked()),this,SLOT(apply()));
    QObject::connect(mincolorbutton,SIGNAL(clicked()),this,SLOT(selectMinimumColor()));
    QObject::connect(maxcolorbutton,SIGNAL(clicked()),this,SLOT(selectMaximumColor()));

    //Set the initial display name
    displaynamefield->setText(m_vectorlayer->name());

    //find out the numerical fields of m_vectorlayer
    QgsDataProvider* provider;
    if(provider=m_vectorlayer->getDataProvider())
    {
	provider->reset();
	QgsFeature* feature; 
	if(feature=provider->getFirstFeature(true))
	{
	    std::vector<QgsFeatureAttribute> vec = feature->attributeMap();
	    int fieldnumber=0; 

	    //iterate through all entries of vec
	    for(std::vector<QgsFeatureAttribute>::iterator it=vec.begin();it!=vec.end();++it)
	    {
		QString teststring=(*it).fieldValue();
		bool containsletter=false;
		for(uint j=0;j<teststring.length();j++)
		{
		    if(teststring.ref(j).isLetter())
		    {
			containsletter=true;
		    }
		}
		if(!containsletter)//add it to the map and to comboBox1 if it seems to be a numeric field
		{
		    QString str=(*it).fieldName();
		    classificationComboBox->insertItem(str);
		    //add teststring and i to the map
		    m_fieldmap.insert(std::make_pair(str,fieldnumber));
		}
		fieldnumber++;
	    } 
	}
	else
	{
	    qWarning("Warning, getNextFeature returned null in QgsGraSyDialog::QgsGraSyDialog(...)");
	}
    }
    else
    {
	qWarning("Warning, data provider is null in QgsGraSyDialog::QgsGraSyDialog(...)");
	return;
    }
}

QgsContColDialog::QgsContColDialog()
{
    
}

QgsContColDialog::~QgsContColDialog()
{

}

void QgsContColDialog::apply()
{
    //minimum and maximum values for equal interval
    double minimum=DBL_MAX;
    double maximum=DBL_MIN;//find out the number of the classification field
    QString fieldstring=classificationComboBox->currentText();
    std::map<QString,int>::iterator iter=m_fieldmap.find(fieldstring);
    int classfield=iter->second;

    QgsDataProvider* provider = m_vectorlayer->getDataProvider();
    if(provider)
    {
	provider->reset();
	QgsFeature* fet=provider->getFirstFeature(true);
	do
	{
	    double value=(fet->attributeMap())[classfield].fieldValue().toDouble();
	    if(value<minimum)
	    {
		minimum=value;
	    }
	    if(value>maximum)
	    {
		maximum=value;
	    }
	}
	while(fet=provider->getNextFeature(true));
    }
    else
    {
	qWarning("Warning, provider is null in QgsGraSyExtensionWidget::QgsGraSyExtensionWidget(...)");
	return;
    }

    //create the render items for minimum and maximum value
    QgsSymbol minsymbol;
    if(m_vectorlayer->vectorType()==QGis::Line)
    {
	minsymbol.setPen(QPen(mincolorbutton->paletteBackgroundColor()));  
    }
    else
    {
	minsymbol.setBrush(QBrush(mincolorbutton->paletteBackgroundColor()));
	minsymbol.setPen(QPen(QColor(0,0,0),1));
    }
    QgsRenderItem* minimumitem=new QgsRenderItem(minsymbol,QString::number(minimum,'f')," ");

    QgsSymbol maxsymbol;
    if(m_vectorlayer->vectorType()==QGis::Line)
    {
	maxsymbol.setPen(QPen(maxcolorbutton->paletteBackgroundColor()));
    }
    else
    {
	maxsymbol.setBrush(QBrush(maxcolorbutton->paletteBackgroundColor()));
	maxsymbol.setPen(QPen(QColor(0,0,0),1));
    }
    QgsRenderItem* maximumitem=new QgsRenderItem(maxsymbol,QString::number(maximum,'f')," ");

    //set the render items to the render of m_vectorlayer
    ((QgsContinuousColRenderer*)(m_vectorlayer->renderer()))->setMinimumItem(minimumitem);
    ((QgsContinuousColRenderer*)(m_vectorlayer->renderer()))->setMaximumItem(maximumitem);
    ((QgsContinuousColRenderer*)(m_vectorlayer->renderer()))->setClassificationField(classfield);
	
    //add a pixmap to the QgsLegendItem
    QPixmap* pix=m_vectorlayer->legendPixmap();
    //use the name and the maximum value to estimate the necessary width of the pixmap (12 pixel width per letter seems to be appropriate)
    QString name=displaynamefield->text();
    int namewidth=45+name.length()*12;
    int numberlength=(int)(60+QString::number(maximum,'f',2).length()*12);
    int pixwidth=(numberlength > namewidth) ? numberlength : namewidth;
    pix->resize(pixwidth,200);
    pix->fill();
    QPainter p(pix);

    p.setPen(QPen(QColor(0,0,0),1));
    //draw the layer name and the name of the classification field into the pixmap
    p.drawText(45,35,name);
    m_vectorlayer->setlayerName(name);
    p.drawText(45,70,classificationComboBox->currentText());
	
    //draw the color range line by line
    for(int i=0;i<100;i++)
    {
	p.setPen(QColor(mincolorbutton->paletteBackgroundColor().red()+(maxcolorbutton->paletteBackgroundColor().red()-mincolorbutton->paletteBackgroundColor().red())/100*i,mincolorbutton->paletteBackgroundColor().green()+(maxcolorbutton->paletteBackgroundColor().green()-mincolorbutton->paletteBackgroundColor().green())/100*i,mincolorbutton->paletteBackgroundColor().blue()+(maxcolorbutton->paletteBackgroundColor().blue()-mincolorbutton->paletteBackgroundColor().blue())/100*i));//use the appropriate color
	p.drawLine(10,90+i,50,90+i);
    }

    //draw the minimum and maximum values beside the color range
    p.setPen(QPen(QColor(0,0,0),1));
    p.drawText(60,105,QString::number(minimum,'f',2));
    p.drawText(60,190,QString::number(maximum,'f',2));

    m_vectorlayer->triggerRepaint();
    m_vectorlayer->legendItem()->setPixmap(0,(*pix));
}

void QgsContColDialog::selectMinimumColor()
{
    mincolorbutton->setPaletteBackgroundColor(QColorDialog::getColor());
    m_vectorlayer->propertiesDialog()->raise();
    raise();
}

void QgsContColDialog::selectMaximumColor()
{
    maxcolorbutton->setPaletteBackgroundColor(QColorDialog::getColor());
    m_vectorlayer->propertiesDialog()->raise();
    raise();
}
