#include "plastiqmethod.h"
#include "plastiqqobject.h"

#include <QObject> 
#include <QString>
#include <QThread>
#include <QVariant>

QHash<QByteArray, PlastiQMethod> PlastiQQObject::platiqConstructors = {
    { "QObject()", { "QObject", "QObject()", "", "QObject*", 0, PlastiQMethod::Public, PlastiQMethod::Constructor } },
    { "QObject(QObject*)", { "QObject", "QObject(QObject*)", "QObject*", "QObject*", 1, PlastiQMethod::Public, PlastiQMethod::Constructor } },

};

QHash<QByteArray, PlastiQMethod> PlastiQQObject::platiqMethods = {
    { "objectName()", { "objectName", "objectName()", "", "QString", 0, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "setObjectName(QString)", { "setObjectName", "setObjectName(const QString &)", "QString&", "void", 1, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "isWidgetType()", { "isWidgetType", "isWidgetType()", "", "bool", 2, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "isWindowType()", { "isWindowType", "isWindowType()", "", "bool", 3, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "signalsBlocked()", { "signalsBlocked", "signalsBlocked()", "", "bool", 4, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "blockSignals(bool)", { "blockSignals", "blockSignals(bool)", "bool", "bool", 5, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "thread()", { "thread", "thread()", "", "QThread*", 6, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "moveToThread(QThread*)", { "moveToThread", "moveToThread(QThread*)", "QThread*", "void", 7, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "killTimer(int)", { "killTimer", "killTimer(int)", "int", "void", 8, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "setParent(QObject*)", { "setParent", "setParent(QObject*)", "QObject*", "void", 9, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "installEventFilter(QObject*)", { "installEventFilter", "installEventFilter(QObject*)", "QObject*", "void", 10, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "removeEventFilter(QObject*)", { "removeEventFilter", "removeEventFilter(QObject*)", "QObject*", "void", 11, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "setProperty(char*,QVariant)", { "setProperty", "setProperty(char*,QVariant)", "char*,QVariant", "bool", 12, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "property(char*)", { "property", "property(char*)", "char*", "QVariant", 13, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "deleteLater()", { "deleteLater", "deleteLater()", "", "void", 14, PlastiQMethod::Public, PlastiQMethod::Method } },

};

QHash<QByteArray, PlastiQMethod> PlastiQQObject::platiqSignals = {

};

const uint PlastiQQObject::plastiq_objectType = PlastiQObject::IsQObject;
const uint PlastiQQObject::plastiq_classId = 0;
PlastiQMetaObject PlastiQQObject::plastiq_metaObject = {
    { Q_NULLPTR, "QObject", &plastiq_classId, &plastiq_objectType,
      &platiqConstructors,
      &platiqMethods,
      &platiqSignals,
      plastiq_static_metacall
    }
};

const PlastiQMetaObject *PlastiQQObject::metaObject() const {
    return &plastiq_metaObject;
}

void PlastiQQObject::plastiq_static_metacall(PlastiQObject *object, PlastiQMetaObject::Call call, int id, const PMOGStack &stack) {
    if(call == PlastiQMetaObject::CreateInstance) {
        QObject *o = Q_NULLPTR;

        switch(id) {
        case 0: o = new QObject(); break;
        case 1: o = new QObject(reinterpret_cast<QObject*>(stack[1].s_voidp)); break;

        default: ;
        }

        PlastiQQObject *p = new PlastiQQObject();
        p->setData(reinterpret_cast<void*>(o), p);
        *reinterpret_cast<PlastiQObject**>(stack[0].s_voidpp) = p;
    }
    else if(call == PlastiQMetaObject::InvokeMethod) {
        if(id >= 15) {
            id -= 15;
            return;
        }

        QObject *o = reinterpret_cast<QObject*>(object->data());

        switch(id) {
        case 0: { QString _r = o->objectName();
                  stack[0].s_string = _r; } break;
        case 1: o->setObjectName(stack[1].s_string); break;
        case 2: { bool _r = o->isWidgetType();
                  stack[0].s_bool = _r; } break;
        case 3: { bool _r = o->isWindowType();
                  stack[0].s_bool = _r; } break;
        case 4: { bool _r = o->signalsBlocked();
                  stack[0].s_bool = _r; } break;
        case 5: { bool _r = o->blockSignals(stack[1].s_bool);
                  stack[0].s_bool = _r; } break;
        case 6: { QThread* _r = o->thread();
                  reinterpret_cast<void*>(stack[0].s_voidp) = _r; } break;
        case 7: o->moveToThread(reinterpret_cast<QThread*>(stack[1].s_voidp)); break;
        case 8: o->killTimer(stack[1].s_int); break;
        case 9: o->setParent(reinterpret_cast<QObject*>(stack[1].s_voidp)); break;
        case 10: o->installEventFilter(reinterpret_cast<QObject*>(stack[1].s_voidp)); break;
        case 11: o->removeEventFilter(reinterpret_cast<QObject*>(stack[1].s_voidp)); break;
        case 12: { bool _r = o->setProperty(reinterpret_cast<char*>(stack[1].s_voidp),
                (*reinterpret_cast< QVariant(*) >(stack[2].s_voidp)));
                  stack[0].s_bool = _r; } break;
        case 13: { QVariant _r = o->property(reinterpret_cast<char*>(stack[1].s_voidp));
                  reinterpret_cast<void*>(stack[0].s_voidp) = &_r; } break;
        case 14: o->deleteLater(); break;

        default: ;
        }
    }
    else if(call == PlastiQMetaObject::CreateConnection) {
        QObject *o = reinterpret_cast<QObject*>(object->data());
        PQObjectWrapper *sender = reinterpret_cast<PQObjectWrapper *>(stack[1].s_voidp);
        PQObjectWrapper *receiver = reinterpret_cast<PQObjectWrapper *>(stack[2].s_voidp);
        QByteArray slot = stack[3].s_bytearray;

        switch(id) {

        default: ;
        }
    }
}

