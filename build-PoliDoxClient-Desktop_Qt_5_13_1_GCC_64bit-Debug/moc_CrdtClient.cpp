/****************************************************************************
** Meta object code from reading C++ file 'CrdtClient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../client/CrdtClient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CrdtClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CrdtClient_t {
    QByteArrayData data[10];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CrdtClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CrdtClient_t qt_meta_stringdata_CrdtClient = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CrdtClient"
QT_MOC_LITERAL(1, 11, 13), // "onLocalInsert"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 4), // "Char"
QT_MOC_LITERAL(4, 31, 6), // "symbol"
QT_MOC_LITERAL(5, 38, 13), // "onLocalDelete"
QT_MOC_LITERAL(6, 52, 14), // "onRemoteInsert"
QT_MOC_LITERAL(7, 67, 8), // "position"
QT_MOC_LITERAL(8, 76, 2), // "ch"
QT_MOC_LITERAL(9, 79, 14) // "onRemoteDelete"

    },
    "CrdtClient\0onLocalInsert\0\0Char\0symbol\0"
    "onLocalDelete\0onRemoteInsert\0position\0"
    "ch\0onRemoteDelete"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CrdtClient[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,
       6,    2,   40,    2, 0x06 /* Public */,
       9,    1,   45,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Char,    7,    8,
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void CrdtClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CrdtClient *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onLocalInsert((*reinterpret_cast< Char(*)>(_a[1]))); break;
        case 1: _t->onLocalDelete((*reinterpret_cast< Char(*)>(_a[1]))); break;
        case 2: _t->onRemoteInsert((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< char(*)>(_a[2]))); break;
        case 3: _t->onRemoteDelete((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CrdtClient::*)(Char );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CrdtClient::onLocalInsert)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CrdtClient::*)(Char );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CrdtClient::onLocalDelete)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CrdtClient::*)(int , char );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CrdtClient::onRemoteInsert)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CrdtClient::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CrdtClient::onRemoteDelete)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CrdtClient::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_CrdtClient.data,
    qt_meta_data_CrdtClient,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CrdtClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CrdtClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CrdtClient.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CrdtClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void CrdtClient::onLocalInsert(Char _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CrdtClient::onLocalDelete(Char _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CrdtClient::onRemoteInsert(int _t1, char _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CrdtClient::onRemoteDelete(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
