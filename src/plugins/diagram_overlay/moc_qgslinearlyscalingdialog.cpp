/****************************************************************************
** Meta object code from reading C++ file 'qgslinearlyscalingdialog.h'
**
** Created: Mon Feb 12 09:25:39 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qgslinearlyscalingdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qgslinearlyscalingdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QgsLinearlyScalingDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QgsLinearlyScalingDialog[] = {
    "QgsLinearlyScalingDialog\0\0"
    "insertMaximumAttributeValue()\0"
};

const QMetaObject QgsLinearlyScalingDialog::staticMetaObject = {
    { &QgsDiagramRendererWidget::staticMetaObject, qt_meta_stringdata_QgsLinearlyScalingDialog,
      qt_meta_data_QgsLinearlyScalingDialog, 0 }
};

const QMetaObject *QgsLinearlyScalingDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QgsLinearlyScalingDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QgsLinearlyScalingDialog))
	return static_cast<void*>(const_cast<QgsLinearlyScalingDialog*>(this));
    return QgsDiagramRendererWidget::qt_metacast(_clname);
}

int QgsLinearlyScalingDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QgsDiagramRendererWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: insertMaximumAttributeValue(); break;
        }
        _id -= 1;
    }
    return _id;
}
