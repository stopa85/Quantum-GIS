#include "qgsfeature.h"
#include "qgsfeatureattribute.h"
#include "qgsdiagramfactory.h"
#include "qgsdiagramitem.h"
#include "qgslinearlyscalingdiagramrenderer.h"
#include "testwidget.h"
#include <QApplication>
#include <QPainter>
#include <QWidget>

//testing for QgsDiagramItem and QgsDiagramFactory
int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  TestWidget appWidget;

  QgsFeature f;
  f.addAttribute(0, QgsFeatureAttribute("first", "40"));
  f.addAttribute(1, QgsFeatureAttribute("second", "20"));
  f.addAttribute(2, QgsFeatureAttribute("third", "70"));
  f.addAttribute(3, QgsFeatureAttribute("fourth", "50"));

  QgsAttributeList attributes;
  attributes.push_back(0);
  attributes.push_back(1);
  attributes.push_back(2);
  attributes.push_back(3);
  
  QgsDiagramItem item;
  item.setWidth(100);
  item.setHeight(100);

  //todo: create lower item, upper item and use QgsLinearlyScaledDiagramRenderer
  QgsDiagramItem lowerItem(0, 0, 50, 50);
  QgsDiagramItem upperItem(100, 100, 300, 300);

  std::list<QColor> colors;
  colors.push_back(QColor(255, 0, 0));
  colors.push_back(QColor(0, 255, 0));
  colors.push_back(QColor(0, 0, 255));
  colors.push_back(QColor(125, 125, 125));

  QgsLinearlyScalingDiagramRenderer renderer("Bar", attributes, colors);
  renderer.setLowerItem(lowerItem);
  renderer.setUpperItem(upperItem);
  renderer.setClassificationField(2);
  QImage* testImage = renderer.renderDiagram(f);
    
  //display testImage
  appWidget.resize(testImage->width(), testImage->height());
  appWidget.setImage(testImage);
  appWidget.show();
  app.exec();
}
