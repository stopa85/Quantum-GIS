#include "qgsspatialrefsys.h"
QgsSpatialRefSys::QgsSpatialRefSys(){}
QgsSpatialRefSys::QgsSpatialRefSys(QString srid, QString authName, QString authSrid, 
    QString srtext, QString proj4Text):mSrid(srid), mAuthName(authName),
                                       mAuthSrid(authSrid), mSrtext(srtext),
                                       mProj4text(proj4Text)
{
}
QString QgsSpatialRefSys::srid() const
{
  return mSrid;
}

QString QgsSpatialRefSys::authName() const
{
  return mAuthName;
}
QString QgsSpatialRefSys::authSrid() const
{
  return mAuthSrid;
}
QString QgsSpatialRefSys::srText() const
{
  return mSrtext;
}
QString QgsSpatialRefSys::proj4Text() const
{
  return mProj4text;
}

void QgsSpatialRefSys::setSrid(QString& srid)
{
  mSrid = srid;
}
void QgsSpatialRefSys::setAuthName(QString &authname)
{
  mAuthName = authname;
}
void QgsSpatialRefSys::setAuthSrid(QString &authsrid)
{
  mAuthSrid = authsrid;
}
void QgsSpatialRefSys::setSrText(QString &srtext)
{
  mSrtext = srtext;
}
void QgsSpatialRefSys::setProjText(QString &projtext)
{
  mProj4text = projtext;
}
