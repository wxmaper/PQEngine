#include "plastiqmetaobject.h"
#include "plastiqobject.h"
#include "plastiqmethod.h"

#include "pqengine_global.h"
#include "phpqt5.h"

#include <QHashIterator>

const char *PlastiQMetaObject::className() const
{
    return d.className;
}

int PlastiQMetaObject::classId() const
{
    return *d.classId;
}

int PlastiQMetaObject::methodId(const QByteArray &signature) const
{
    const PlastiQMetaObject *data = this;
    int mid = -1;
    int offset = 0;

    while(mid < 0 && data) {
        mid = data->d.pq_methods->value(signature, PlastiQMethod()).index;

        if(mid < 0) {
            offset += data->d.pq_methods->size();
        }

        data = data->d.superdata;
    }

    return mid >= 0 ? mid + offset : -1;
}

int PlastiQMetaObject::constructorId(const QByteArray &signature) const
{
    qDebug() << __FUNCTION__;
    qDebug() << d.className;

    const PlastiQMetaObject *data = this;
    int mid = -1;
    int offset = 0;

    while(mid < 0 && data) {
        qDebug() << __FUNCTION__ << "while" << signature;
        mid = data->d.pq_constructors->value(signature, PlastiQMethod()).index;

        if(mid < -1) {
            offset += data->d.pq_constructors->size();
        }

        data = data->d.superdata;
    }

    return mid >= 0 ? mid + offset : -1;
}

bool PlastiQMetaObject::invokeMethod(PlastiQObject *object, const QByteArray &signature, const PMOGStack &stack)
{
    qDebug() << __FUNCTION__;
    if(!object || !object->data()) {
        return false;
    }

    int mid = object->metaObject()->methodId(signature);
    qDebug() << "signature" << signature;
    object->metaObject()->d.static_metacall(object, PlastiQMetaObject::InvokeMethod, mid, stack);
}

bool PlastiQMetaObject::connect(PQObjectWrapper *sender, const QByteArray &signal, PQObjectWrapper *receiver, const QByteArray &slot)
{
    PMOGStack stack = new PMOGStackItem[10];
    stack[1].s_voidp = reinterpret_cast<void*>(sender);
    stack[2].s_voidp = reinterpret_cast<void*>(receiver);
    stack[3].s_bytearray = slot;

    int signalId = sender->object->metaObject()->d.pq_signals->value(signal).index;
    sender->object->metaObject()->d.static_metacall(sender->object, PlastiQMetaObject::CreateConnection, signalId, stack);

    delete [] stack;
    return true;
}

PlastiQObject *PlastiQMetaObject::newInstance(const QByteArray &signature, const PMOGStack &stack)
{
    qDebug() << __FUNCTION__;

    int cid = constructorId(signature);
    PlastiQObject *object = Q_NULLPTR;

    qDebug() << reinterpret_cast<quint64>(object);

    stack[0].s_voidpp = reinterpret_cast<void**>(&object);

    qDebug() << reinterpret_cast<quint64>(stack[0].s_voidpp);
    qDebug() << "cid" << cid;

    if(cid < 0) {
        return object;
    }

    static_metacall(PlastiQMetaObject::CreateInstance, cid, stack);
    qDebug() << "instance" << reinterpret_cast<quint64>(stack[0].s_voidpp) << reinterpret_cast<quint64>(object);

    qDebug() << "object->metaObject()->className(): " << object->metaObject()->className();
    //qDebug() << "object->metaObject()->classId()" << object->metaObject()->classId();

    return object;
}

bool PlastiQMetaObject::static_metacall(PlastiQMetaObject::Call call, int id, const PMOGStack &stack)
{
    if(!d.static_metacall) {
        return false;
    }

    d.static_metacall(Q_NULLPTR, call, id, stack);
    return true;
}

const QList<PlastiQCandidateMethod> PlastiQMetaObject::candidates(const QByteArray &name, int argc, PlastiQMethod::Type type) const
{
    PQDBG_LVL_START(__FUNCTION__);

    QList<PlastiQCandidateMethod> candidateList;

    const PlastiQMetaObject *m = this;
    int offset = 0;

    do {
        QHashIterator<QByteArray, PlastiQMethod> *i;
        switch(type) {
        case PlastiQMethod::Method:
            i = new QHashIterator<QByteArray, PlastiQMethod>(*(m->d.pq_methods));
            break;

        case PlastiQMethod::Constructor:
            i = new QHashIterator<QByteArray, PlastiQMethod>(*(m->d.pq_constructors));
            break;

        default:
            return candidateList;
        }


        while(i->hasNext()) {
            i->next();

            if(i->value().name != name) continue;
            int idx = i->key().indexOf("(");
            QString methodSignature = i->key().mid(idx + 1, i->key().size() - idx - 2);

            QStringList argTypes = methodSignature.split(",");

            PQDBGLPUP(QString("candidate: %1 x %2").arg(argTypes.size()).arg(argc));

            if(argTypes.size() != argc) continue;

            qDebug() << "append candidate:" << name << methodSignature << i->key();
            candidateList.append({ name, argc, argTypes, i->value().index + offset });
        }

        if(type == PlastiQMethod::Constructor) {
            delete i;
            break;
        }
        else {
            offset += m->d.pq_methods->size();
            delete i;
        }
    } while(m = m->d.superdata);

    return candidateList;
}
