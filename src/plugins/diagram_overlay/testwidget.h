#include <QWidget>

class QImage;

/**This class is mainly to test the diagram creation. It takes a QImage and displays it*/
class TestWidget: public QWidget
{
 public:
  TestWidget();
  ~TestWidget();
  void setImage(QImage* img){mImage = img;}

 protected:
  void paintEvent(QPaintEvent* e);

 private:
  QImage* mImage;

};
