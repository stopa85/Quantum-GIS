<!DOCTYPE qgis PUBLIC 'http://mrcc.com/qgis.dtd' 'SYSTEM'>
<qgis version="0.9.2-Ganymede" >
  <maplayer minScale="1" maxScale="1e+08" scaleBasedVisibilityFlag="0" geometry="Point" type="vector" >
    <id>points20080109122431720</id>
    <datasource>/tmp/points.shp</datasource>
    <layername>points</layername>
    <srs>
      <spatialrefsys>
        <proj4>+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs</proj4>
        <srsid>2585</srsid>
        <srid>4326</srid>
        <epsg>4326</epsg>
        <description>WGS 84</description>
        <projectionacronym>longlat</projectionacronym>
        <ellipsoidacronym>WGS84</ellipsoidacronym>
        <geographicflag>true</geographicflag>
      </spatialrefsys>
    </srs>
    <transparencyLevelInt>255</transparencyLevelInt>
    <provider>ogr</provider>
    <encoding>System</encoding>
    <classificationattribute>Importance</classificationattribute>
    <classificationattribute>Heading</classificationattribute>
    <displayfield>Class</displayfield>
    <label>0</label>
    <attributeactions/>
    <graduatedsymbol>
      <classificationfield>2</classificationfield>
      <symbol>
        <lowervalue>0.999</lowervalue>
        <uppervalue>7.333</uppervalue>
        <label></label>
        <pointsymbol>hard:circle</pointsymbol>
        <pointsize>11</pointsize>
        <rotationclassificationfield>-1</rotationclassificationfield>
        <scaleclassificationfield>2</scaleclassificationfield>
        <outlinecolor red="0" blue="0" green="0" />
        <outlinestyle>SolidLine</outlinestyle>
        <outlinewidth>1</outlinewidth>
        <fillcolor red="0" blue="0" green="255" />
        <fillpattern>SolidPattern</fillpattern>
        <texturepath></texturepath>
      </symbol>
      <symbol>
        <lowervalue>7.333</lowervalue>
        <uppervalue>13.667</uppervalue>
        <label></label>
        <pointsymbol>hard:rectangle</pointsymbol>
        <pointsize>11</pointsize>
        <rotationclassificationfield>1</rotationclassificationfield>
        <scaleclassificationfield>2</scaleclassificationfield>
        <outlinecolor red="0" blue="0" green="0" />
        <outlinestyle>SolidLine</outlinestyle>
        <outlinewidth>1</outlinewidth>
        <fillcolor red="0" blue="86" green="169" />
        <fillpattern>SolidPattern</fillpattern>
        <texturepath></texturepath>
      </symbol>
      <symbol>
        <lowervalue>13.667</lowervalue>
        <uppervalue>20.001</uppervalue>
        <label></label>
        <pointsymbol>hard:triangle</pointsymbol>
        <pointsize>11</pointsize>
        <rotationclassificationfield>1</rotationclassificationfield>
        <scaleclassificationfield>2</scaleclassificationfield>
        <outlinecolor red="0" blue="0" green="0" />
        <outlinestyle>SolidLine</outlinestyle>
        <outlinewidth>1</outlinewidth>
        <fillcolor red="0" blue="171" green="84" />
        <fillpattern>SolidPattern</fillpattern>
        <texturepath></texturepath>
      </symbol>
    </graduatedsymbol>
    <labelattributes>
      <label field="" text="Label" />
      <family field="" name="Lucida Grande" />
      <size field="" units="pt" value="12" />
      <bold field="" on="0" />
      <italic field="" on="0" />
      <underline field="" on="0" />
      <color field="" red="0" blue="0" green="0" />
      <x field="" />
      <y field="" />
      <offset x="0" y="0" yfield="-1" xfield="-1" units="pt" />
      <angle field="" value="0" />
      <alignment field="-1" value="center" />
      <buffercolor field="" red="255" blue="255" green="255" />
      <buffersize field="" units="pt" value="1" />
      <bufferenabled field="" on="" />
    </labelattributes>
  </maplayer>
</qgis>
