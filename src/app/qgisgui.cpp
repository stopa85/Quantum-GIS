
#include "qgisgui.h"

#include <QSettings>

#include "qgsspatialrefsys.h"
#include "qgsproject.h"
#include "qgslayerprojectionselector.h"


namespace QgisGui
{


void customSrsValidation(QgsSpatialRefSys* srs)
{
  QString proj4String;
  QSettings mySettings;
  QString myDefaultProjectionOption =
      mySettings.readEntry("/Projections/defaultBehaviour");
  if (myDefaultProjectionOption=="prompt")
  {
    //@note this class is not a descendent of QWidget so we cant pass
    //it in the ctor of the layer projection selector

    QgsLayerProjectionSelector * mySelector = new QgsLayerProjectionSelector();
    long myDefaultSRS =
        QgsProject::instance()->readNumEntry("SpatialRefSys","/ProjectSRSID",GEOSRS_ID);
    mySelector->setSelectedSRSID(myDefaultSRS);
    if(mySelector->exec())
    {
      srs->createFromSrsId(mySelector->getCurrentSRSID());
    }
    else
    {
      QApplication::restoreOverrideCursor();
    }
    delete mySelector;
  }
  else if (myDefaultProjectionOption=="useProject")
  {
    // XXX TODO: Change project to store selected CS as 'projectSRS' not 'selectedWKT'
    proj4String = QgsProject::instance()->readEntry("SpatialRefSys","//ProjectSRSProj4String",GEOPROJ4);
    srs->createFromProj4(proj4String);  
  }
  else ///Projections/defaultBehaviour==useDefault
  {
    // XXX TODO: Change global settings to store default CS as 'defaultSRS' not 'defaultProjectionWKT'
    proj4String = mySettings.readEntry("/Projections/defaultSRS",GEOPROJ4);
    srs->createFromProj4(proj4String);  
  }

}



}
