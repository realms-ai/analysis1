/****************************************************************************
** Meta object code from reading C++ file 'financedemo.h'
**
** Created: Thu May 8 19:37:28 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "financedemo.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'financedemo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FinanceDemo[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      36,   12,   12,   12, 0x08,
      56,   12,   12,   12, 0x08,
      87,   76,   12,   12, 0x08,
     112,   12,   12,   12, 0x08,
     138,  132,   12,   12, 0x08,
     178,  172,   12,   12, 0x08,
     203,  132,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FinanceDemo[] = {
    "FinanceDemo\0\0onComboBoxChanged(int)\0"
    "onCheckBoxChanged()\0onLineEditChanged()\0"
    "mouseUsage\0onMouseUsageChanged(int)\0"
    "onViewPortChanged()\0event\0"
    "onMouseMovePlotArea(QMouseEvent*)\0"
    "value\0onHScrollBarChanged(int)\0"
    "onMouseWheelChart(QWheelEvent*)\0"
};

void FinanceDemo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FinanceDemo *_t = static_cast<FinanceDemo *>(_o);
        switch (_id) {
        case 0: _t->onComboBoxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->onCheckBoxChanged(); break;
        case 2: _t->onLineEditChanged(); break;
        case 3: _t->onMouseUsageChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onViewPortChanged(); break;
        case 5: _t->onMouseMovePlotArea((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 6: _t->onHScrollBarChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->onMouseWheelChart((*reinterpret_cast< QWheelEvent*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FinanceDemo::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FinanceDemo::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FinanceDemo,
      qt_meta_data_FinanceDemo, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FinanceDemo::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FinanceDemo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FinanceDemo::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FinanceDemo))
        return static_cast<void*>(const_cast< FinanceDemo*>(this));
    return QDialog::qt_metacast(_clname);
}

int FinanceDemo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
