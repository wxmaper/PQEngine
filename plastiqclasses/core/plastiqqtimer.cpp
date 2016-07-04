#include "plastiqmethod.h"
#include "plastiqqtimer.h"

#include "plastiqqobject.h"
#include <QTimer> 

QHash<QByteArray, PlastiQMethod> PlastiQQTimer::platiqConstructors = {
    { "QTimer()", { "QTimer", "QTimer()", "", "QTimer*", 0, PlastiQMethod::Public, PlastiQMethod::Constructor } },
    { "QTimer(QObject*)", { "QTimer", "QTimer(QObject*)", "QObject*", "QTimer*", 1, PlastiQMethod::Public, PlastiQMethod::Constructor } },

};

QHash<QByteArray, PlastiQMethod> PlastiQQTimer::platiqMethods = {
    { "isActive()", { "isActive", "isActive()", "", "bool", 0, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "timerId()", { "timerId", "timerId()", "", "int", 1, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "setInterval(int)", { "setInterval", "setInterval(int)", "int", "void", 2, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "interval()", { "interval", "interval()", "", "int", 3, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "remainingTime()", { "remainingTime", "remainingTime()", "", "int", 4, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "setSingleShot(bool)", { "setSingleShot", "setSingleShot(bool)", "bool", "void", 5, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "isSingleShot()", { "isSingleShot", "isSingleShot()", "", "bool", 6, PlastiQMethod::Public, PlastiQMethod::Method } },
    { "start(int)", { "start", "start(int)", "int", "void", 7, PlastiQMethod::Public, PlastiQMethod::Slot } },
    { "start()", { "start", "start()", "", "void", 8, PlastiQMethod::Public, PlastiQMethod::Slot } },
    { "stop()", { "stop", "stop()", "", "void", 9, PlastiQMethod::Public, PlastiQMethod::Slot } },

};

QHash<QByteArray, PlastiQMethod> PlastiQQTimer::platiqSignals = {
    { "timeout()", { "timeout", "timeout()", "", "void", 0, PlastiQMethod::Public, PlastiQMethod::Signal } },

};

const uint PlastiQQTimer::plastiq_objectType = PlastiQObject::IsQObject;
const uint PlastiQQTimer::plastiq_classId = 1;
PlastiQMetaObject PlastiQQTimer::plastiq_metaObject = {
    { &PlastiQQObject::plastiq_metaObject, "QTimer", &plastiq_classId, &plastiq_objectType,
      &platiqConstructors,
      &platiqMethods,
      &platiqSignals,
      plastiq_static_metacall
    }
};

const PlastiQMetaObject *PlastiQQTimer::metaObject() const {
    return &plastiq_metaObject;
}

void PlastiQQTimer::plastiq_static_metacall(PlastiQObject *object, PlastiQMetaObject::Call call, int id, const PMOGStack &stack) {
    if(call == PlastiQMetaObject::CreateInstance) {
        QTimer *o = Q_NULLPTR;

        switch(id) {
        case 0: o = new QTimer(); break;
        case 1: o = new QTimer(reinterpret_cast<QObject*>(stack[1].s_voidp)); break;

        default: ;
        }

        PlastiQQTimer *p = new PlastiQQTimer();
        p->setData(reinterpret_cast<void*>(o), p);
        *reinterpret_cast<PlastiQObject**>(stack[0].s_voidpp) = p;
    }
    else if(call == PlastiQMetaObject::InvokeMethod) {
        if(id >= 10) {
            id -= 10;
            PlastiQQObject::plastiq_metaObject.d.static_metacall(object, call, id, stack);
            return;
        }

        QTimer *o = reinterpret_cast<QTimer*>(object->data());

        switch(id) {
        case 0: { bool _r = o->isActive();
                  stack[0].s_bool = _r; } break;
        case 1: { long _r = o->timerId();
                  stack[0].s_long = _r; } break;
        case 2: o->setInterval(stack[1].s_int); break;
        case 3: { long _r = o->interval();
                  stack[0].s_long = _r; } break;
        case 4: { long _r = o->remainingTime();
                  stack[0].s_long = _r; } break;
        case 5: o->setSingleShot(stack[1].s_bool); break;
        case 6: { bool _r = o->isSingleShot();
                  stack[0].s_bool = _r; } break;
        case 7: o->start(stack[1].s_int); break;
        case 8: o->start(); break;
        case 9: o->stop(); break;

        default: ;
        }
    }
    else if(call == PlastiQMetaObject::CreateConnection) {
        QTimer *o = reinterpret_cast<QTimer*>(object->data());
        PQObjectWrapper *sender = reinterpret_cast<PQObjectWrapper *>(stack[1].s_voidp);
        PQObjectWrapper *receiver = reinterpret_cast<PQObjectWrapper *>(stack[2].s_voidp);
        QByteArray slot = stack[3].s_bytearray;

        switch(id) {
        case 0: QObject::connect(o, &QTimer::timeout, [=]() {
                    PMOGStack closurestack = new PMOGStackItem[10];
                    PHPQt5Connection_invoke_new(sender, receiver, slot, "", 0, closurestack);
                    delete [] closurestack;
                });
            break;


        default: ;
        }
    }
}

