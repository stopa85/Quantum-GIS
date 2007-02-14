#include "testwidget.h"
#include <QImage>
#include <QPainter>

TestWidget::TestWidget(): QWidget(), mImage(0)
{

}
  
TestWidget::~TestWidget()
{

}

void TestWidget::paintEvent(QPaintEvent* e)
{
  if(mImage)
    {
      QPainter p(this);
      p.drawImage(0, 0, *mImage);
    }
}
