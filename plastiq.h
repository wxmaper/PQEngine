#ifndef PLASTIQ_H
#define PLASTIQ_H

#include <QByteArray>
#include <QString>

struct PQObjectWrapper;
class PlastiQObject;

namespace PlastiQ {

class IPlastiQUi {
public:
    virtual void setupUi(PQObjectWrapper*, QObject*) = 0;
};

enum ItemType {
    Undef = 0,
    Null, // 1
    False, // 2
    True, // 3
    Long, // 4
    Double, // 5
    String, // 6
    Array, // 7
    Object, // 8
    Resource, // 9
    Refence, // end of ZEND types  // 10

    /* PlastiQ types */
    ByteArray, // 11
    Int, // 12
    Enum, // 13
    Bool, // 14
    Char, // 15
    CharStar, // 16
    Void, // 17
    VoidStar, // 18
    VoidStarStar, // 19
    QObject, // Qt object with Q_OBJECT macros // 20
    QtObject, // any Qt object (without Q_OBJECT macros) // 21
    ObjectStar, // any object (registered in plastiq) // 22
    QObjectStar, // Qt object with Q_OBJECT macros // 23
    QtObjectStar, // any Qt object (without Q_OBJECT macros) // 24
    QEventStar, // QEvent object // 25
    PlastiQObjectStar, // PlastiQObject instance // 26
    FunctorOrLambda, // 27
    UInt, // 28
    ULong, // 29
    Variant, // 30

    Error // 31
};

enum ObjectType {
    IsObject = 0, // any object
    IsQObject,
    IsQWidget,
    IsQWindow,
    IsQtObject, // any Qt object (without Q_OBJECT macros)
    IsQtItem, // any Qt item
    IsNamespace, // no any functions/methods/constructors
    IsQEvent
};

}

#endif // PLASTIQ_H
