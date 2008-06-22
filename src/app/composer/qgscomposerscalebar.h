/***************************************************************************
                            qgscomposerscalebar.h
                             -------------------
    begin                : March 2005
    copyright            : (C) 2005 by Radim Blazek
    email                : blazek@itc.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSCOMPOSERSCALEBAR_H
#define QGSCOMPOSERSCALEBAR_H

#include "qgscomposeritem.h"
#include <QPen>

class QgsComposerMap;

class QgsComposerScaleBar: public QgsComposerItem
{
 public:

  enum Style
    {
      Bar_Ticks_Down,
      Bar_Ticks_Middle,
      Bar_Ticks_Up,
      Single_Box,
      Double_Box,
    };

  QgsComposerScaleBar(QgsComposition* composition);
  ~QgsComposerScaleBar();

  /** \brief Reimplementation of QCanvasItem::paint*/
  void paint (QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget);

  //getters and setters
  int numSegments() const {return mNumSegments;}
  void setNumSegments(int nSegments){mNumSegments = nSegments;}
  
  int numSegmentsLeft() const {return mNumSegmentsLeft;}
  void setNumSegmentsLeft(int nSegmentsLeft) {mNumSegmentsLeft = nSegmentsLeft;}

  double numUnitsPerSegment() const {return mNumUnitsPerSegment;}
  void setNumUnitsPerSegment(double units);

  QString unitLabeling() const {return mUnitLabeling;}
  void setUnitLabeling(const QString& label){mUnitLabeling = label;}

  QFont font() const {return mFont;}
  void setFont(const QFont& font){mFont = font;}

  QPen pen() const {return mPen;}
  void setPen(const QPen& pen){mPen = pen;}

  QBrush brush() const {return mBrush;}
  void setBrush(const QBrush& brush){mBrush = brush;}

  double height() const {return mHeight;}
  void setHeight(double h) {mHeight = h;}

  void setComposerMap(const QgsComposerMap* map);
  const QgsComposerMap* composerMap() const {return mComposerMap;}

  /**Apply default settings (scale bar 1/5 of map item width)*/
  void applyDefaultSettings();

  void setStyle(QgsComposerScaleBar::Style style){mStyle = style;}

  
 protected:

  /**Reference to composer map object*/
  const QgsComposerMap* mComposerMap;
  /**Number of segments on right side*/
  int mNumSegments;
  /**Number of segments on left side*/
  int mNumSegmentsLeft;
  /**Size of a segment (in map units)*/
  double mNumUnitsPerSegment;
  /**Labeling of map units*/
  QString mUnitLabeling;
  /**Font*/
  QFont mFont;
  /**Outline*/
  QPen mPen;
  /**Fill*/
  QBrush mBrush;
  /**Height of bars/lines*/
  double mHeight;
  /**Scalebar style*/
  QgsComposerScaleBar::Style mStyle;
  /**Space between bar and Text labels*/
  double mLabelBarSpace;

  /**Width of a segment (in mm)*/
  double mSegmentMM;

  /**Calculates with of a segment in mm and stores it in mSegmentMM*/
  void refreshSegmentMM();
  
  /**Draw text labels using the current font*/
  void drawLabels(QPainter* p);

  /**Draws this bar using single box style
   @param barTopPosition Item coordinates of the bar top. Necessary because of Labels*/
  void drawScaleBarSingleBox(QPainter* p, double barTopPosition) const;
};

#endif //QGSCOMPOSERSCALEBAR_H

#if 0

#include "ui_qgscomposerscalebarbase.h"
#include "qgscomposeritem.h"
#include <QAbstractGraphicsShapeItem>
#include <QRect>
#include <QPen>

class QgsMapCanvas;
class QgsComposition;
class QBrush;
class QDomNode;
class QDomDocument;
class QFont;
class QPainter;
class QPen;

/** \class QgsComposerScalebar
 *  \brief Object representing map window. 
 */
// NOTE: QgsComposerScalebarBase must be first, otherwise does not compile
class QgsComposerScalebar : public QWidget, private Ui::QgsComposerScalebarBase, public QgsComposerItem
{
    Q_OBJECT

public:
    /** \brief Constructor  
     *  \param id object id
     *  \param fontSize font size in typographic points!
     */
    QgsComposerScalebar( QgsComposition *composition, int id, int x, int y );

    /** \brief Constructor. Settings are read from project. 
     *  \param id object id
     */
    QgsComposerScalebar( QgsComposition *composition, int id );
    ~QgsComposerScalebar();

    /** \brief Initialise GUI etc., shared by constructors. */
    void init(void);

    // Reimplement QgsComposerItem:
    void setSelected( bool s );
    bool selected( void );
    QWidget *options ( void );
    bool writeSettings ( void );
    bool readSettings ( void );
    bool removeSettings ( void );
    bool writeXML( QDomNode & node, QDomDocument & document, bool temp = false );
    bool readXML( QDomNode & node );
     
    /** \brief Draw to paint device, internal use 
     *  \param painter painter or 0
     *  \return bounding box 
     */
    QRectF render (QPainter *painter);

    /** \brief Reimplementation of QGraphicsItem::paint - draw on canvas */
    void paint ( QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget);

    //void drawShape(QPainter&);
    QPolygonF areaPoints() const;
    
    /** \brief Calculate size according to current settings */
    void recalculate ( void );
    
    /** \brief Set values in GUI to current values */
    void setOptions ( void );

    // Move to position
//    void moveBy ( double x, double y );

public slots:
    // Open font dialog
    void on_mFontButton_clicked ( void );

    // Title changed
    void on_mUnitLabelLineEdit_editingFinished ( void );

    // Size changed
    void on_mLineWidthSpinBox_valueChanged ( void );
    void on_mMapUnitsPerUnitLineEdit_editingFinished ( void );
    void on_mNumSegmentsLineEdit_editingFinished ( void );
    void on_mSegmentLengthLineEdit_editingFinished ( void );
    
    // Called by GUI when map selection changed
    void on_mMapComboBox_activated ( int i );

    // Called when map was changed
    void mapChanged ( int id );

private:
    // Pointer to composition
    QgsComposition *mComposition;
    
    // Pointer to map canvas
    QgsMapCanvas *mMapCanvas;
    
    // Composer map id or 0
    int mMap;

    // Vector of map id for maps in combobox
    std::vector<int> mMaps;

    // Number of map units in scalebar unit
    double mMapUnitsPerUnit;

    // Unit label
    QString mUnitLabel;

    // Font. Font size in typographic points!
    QFont mFont;

    // Pen
    QPen mPen;

    // Brush
    QBrush mBrush;

    // Number of parts 
    int mNumSegments;

    // Segment size in map units
    double mSegmentLength;

    // Height of scalebar box in canvas units (box style only)
    double mHeight;

    // Margin
    int mMargin;

    // Size changed
    void sizeChanged ( void );
};

#endif //0
