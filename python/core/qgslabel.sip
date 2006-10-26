
/** Render class to display labels */
class QgsLabel
{
%TypeHeaderCode
#include <qgslabel.h>
%End

  private:
    QgsLabel (); // pretend that constructor is private for now

public:
    // TODO:
    //QgsLabel ( std::vector<QgsField> const & fields  );

    ~QgsLabel();

    /* Fields */
    enum LabelField {
	Text = 0,
	Family,
	Size,
	Bold,
	Italic,
	Underline,
	Color,
	XCoordinate,
	YCoordinate,
	XOffset,
	YOffset,
	Angle,
	Alignment,
        BufferEnabled,
	BufferSize,
	BufferColor,
	BufferBrush,
	BorderWidth,
	BorderColor,
	BorderStyle,
	LabelFieldCount
    };

    /** \brief render label
     *  \param sizeScale global scale factor for size in pixels, labels in map units are not scaled
     */
    void renderLabel ( QPainter* painter, QgsRect* viewExtent, 
                       QgsCoordinateTransform* coordTransform,
                       QgsMapToPixel *transform,
		       QgsFeature *feature, bool selected, QgsLabelAttributes *classAttributes=0, double sizeScale = 1);

    /** Reads the renderer configuration from an XML file
     @param rnode the DOM node to read 
    */
    void readXML(const QDomNode& node);

    /** Writes the contents of the renderer to a configuration file */
    // TODO: wrap
    //void writeXML(std::ostream& xml);

    //! add vector of required fields to existing list of fields
    // TODO: wrap
    //void addRequiredFields ( std::list<int> *fields );

    //! Set available fields
    // TODO: wrap
    //void setFields( std::vector<QgsField> const & fields  );

    //! Available vector fields
    // TODO: wrap
    //std::vector<QgsField> & fields ( void );

    //! Pointer to default attributes
    QgsLabelAttributes *layerAttributes ( );

    //! Set label field
    void setLabelField ( int attr, const QString str );

    //! label field
    QString labelField ( int attr );

    /** Get field value if : 1) field name is not empty
     *                       2) field exists
     *                       3) value is defined
     *  otherwise returns empty string
    */
    QString fieldValue ( int attr, QgsFeature *feature );

};

