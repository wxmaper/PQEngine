#ifndef PLASTIQ_H
#define PLASTIQ_H

#include <QByteArray>
#include <QString>

namespace PlastiQ {
    class PlastiQObject;

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
        FunctorOrLambda // 27
    };

    enum ObjectType {
        IsObject = 0, // any object
        IsQObject,
        IsQWidget,
        IsQWindow,
        IsQtObject, // any Qt object (without Q_OBJECT macros)
        IsNamespace, // no any functions/methods/constructors
        IsQEvent
    };



    struct StackItem {
        ItemType type;
        QByteArray name;

        void* s_voidp;
        void** s_voidpp;
        PlastiQObject* s_object;

        bool s_bool;

        long s_long;
        int s_int;

        float s_float;
        double s_double;
        char *s_charstar;

        quint16 s_uint16;
        quint32 s_uint32;
        quint64 s_uint64;

        qint16 s_int16;
        qint32 s_int32;
        qint64 s_int64;

        QByteArray s_bytearray;
        QString s_string;
    };
}

#endif // PLASTIQ_H
