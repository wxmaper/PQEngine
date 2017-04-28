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

int PlastiQMetaObject::methodId(const QByteArray &signature, PlastiQMethod::Access filter) const
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("PlastiQMetaObject::d.className = %1").arg(d.className));
    PQDBGLPUP(QString("signature: %1").arg(signature.constData()));
#endif

    const PlastiQMetaObject *pqmo = this;
    int mid = -1;
    int offset = 0;

    while(mid < 0 && pqmo) {
        PlastiQMethod method = pqmo->d.pq_methods->value(signature, PlastiQMethod());
        if(filter == PlastiQMethod::None || filter == method.access) {
            mid = method.index;
        }
        else {
            mid = -1;
        }

        if(mid < 0) {
            offset += pqmo->d.pq_methods->size();
        }

        pqmo = pqmo->d.superdata;
    }

    PQDBG_LVL_DONE();
    return mid >= 0 ? mid + offset : -1;
}

int PlastiQMetaObject::signalId(const QByteArray &signature) const
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("PlastiQMetaObject::d.className = %1").arg(d.className));
    PQDBGLPUP(QString("signature: %1").arg(signature.constData()));
#endif

    const PlastiQMetaObject *pqmo = this;
    int sid = -1;
    int offset = 0;

    while(sid < 0 && pqmo) {
        sid = pqmo->d.pq_signals->value(signature, PlastiQMethod()).index;

        if(sid < 0) {
            offset += pqmo->d.pq_signals->size();
        }

        pqmo = pqmo->d.superdata;
    }

    PQDBG_LVL_DONE();
    return sid >= 0 ? sid + offset : -1;
}

int PlastiQMetaObject::constructorId(const QByteArray &signature) const
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("PlastiQMetaObject::d.className = %1").arg(d.className));
    PQDBGLPUP(QString("signature: %1").arg(signature.constData()));
#endif

    const PlastiQMetaObject *pqmo = this;
    int cid = -1;
    int offset = 0;

    // FIXME нужно ли????????
    while(cid < 0 && pqmo) {
        cid = pqmo->d.pq_constructors->value(signature, PlastiQMethod()).index;

        if(cid < -1) {
            offset += pqmo->d.pq_constructors->size();
        }

        pqmo = pqmo->d.superdata;
    }

    cid = cid >= 0 ? cid + offset : -1;

    PQDBGLPUP(QString("constructorId: %1").arg(cid));

    PQDBG_LVL_DONE();
    return cid;
}

bool PlastiQMetaObject::haveVirtualMethod(const QByteArray &methodName,
                                          int argc,
                                          QByteArray &signature) const
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    static const char _comma = ',';
    static const char _space = ' ';

    signature.clear();

    const PlastiQMetaObject *pqmo = this;
    while (pqmo) {
        PQDBGLPUP(QString("test in: %1").arg(pqmo->className()));
        QHashIterator<QByteArray, PlastiQMethod> i(*pqmo->d.pq_methods);

        while (i.hasNext()) {
            i.next();
            const PlastiQMethod &m = i.value();

            if (m.name == methodName) {
                int _argc = m.argTypes.isEmpty()
                        ? 0
                        : m.argTypes.split(_comma).size();

                if (_argc != argc) {
                    PQDBG_LVL_DONE();
                    return false;
                }

                switch (m.access) {
                case PlastiQMethod::Virtual:
                case PlastiQMethod::VirtualProtected:
                case PlastiQMethod::VirtualPublic:
                    signature = m.returnType;
                    signature.append(_space);
                    signature.append(i.key());

                    PQDBG_LVL_DONE();
                    return true;

                default:
                    continue;
                }
            }
        }

        pqmo = pqmo->d.superdata;
    }

    PQDBG_LVL_DONE();
    return false;
}

QObject* PlastiQMetaObject::toQObject(PlastiQObject *object)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    QObject *qobject = Q_NULLPTR;

    if(!object || !object->plastiq_data()) {
        PQDBG_LVL_DONE();
        return Q_NULLPTR;
    }

    PQDBGLPUP(QString("objectType: %1").arg(object->plastiq_objectType()));

    switch(object->plastiq_objectType()) {
    case PlastiQ::IsQObject:
        qobject = (QObject*) object->plastiq_data();
        break;

    case PlastiQ::IsQWidget:
    case PlastiQ::IsQWindow: {
        PMOGStack stack = new PMOGStackItem[1];
        stack[0].s_voidpp = reinterpret_cast<void**>(&qobject);
        object->plastiq_metaObject()->d.static_metacall(object, PlastiQMetaObject::ToQObject, 0, stack);

        PQDBGLPUP(QString("casted ptr: %1").arg(reinterpret_cast<quint64>(qobject)));

        delete [] stack;
        stack = Q_NULLPTR;
    } break;

    default: qobject = Q_NULLPTR;
    }

    PQDBG_LVL_DONE();
    return qobject;
}

//void PlastiQMetaObject::selfDtor(PlastiQObject *object)
//{
//#ifdef PQDEBUG
//    PQDBG_LVL_START(__FUNCTION__);
//#endif

//    if(!object || !object->data()) {
//        PQDBG_LVL_DONE();
//        return;
//    }

//    object->metaObject()->d.static_metacall(object, PlastiQMetaObject::Dtor, 0, Q_NULLPTR);
//    PQDBG_LVL_DONE();
//}

bool PlastiQMetaObject::haveParent(PlastiQObject *object)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    if(!object || !object->plastiq_data()) {
        PQDBG_LVL_DONE();
        return false;
    }

    /*
    bool haveParent = false;
    PlastiQ::ObjectType objectType = *(object->metaObject()->d.objectType);

    switch(objectType) {
    case PlastiQ::IsQObject:
    case PlastiQ::IsQWidget:
    case PlastiQ::IsQWindow: {
        PMOGStack stack = new PMOGStackItem[1];

        invokeMethod(object, "parent()", stack);
        QObject *qobject = reinterpret_cast<QObject*>(stack[0].s_voidp);

        if(qobject != Q_NULLPTR) {
            haveParent = true;
        }

        delete [] stack;
    } break;

    case PlastiQ::IsQtObject: {
        PMOGStack stack = new PMOGStackItem[1];
        object->metaObject()->d.static_metacall(object, PlastiQMetaObject::HaveParent, -1, stack);

        haveParent = stack->s_bool;

        delete [] stack;
    }

    default: ;
    }
    */

    PMOGStack stack = new PMOGStackItem[1];
    object->plastiq_metaObject()->d.static_metacall(object, PlastiQMetaObject::HaveParent, -1, stack);

    bool haveParent = stack[0].s_bool;

    delete [] stack;

    PQDBGLPUP(QStringLiteral("%1::haveParent: %2").arg(object->plastiq_metaObject()->className()).arg(haveParent));

    PQDBG_LVL_DONE();
    return haveParent;
}

bool PlastiQMetaObject::invokeMethod(PlastiQObject *object, const QByteArray &signature, const PMOGStack &stack)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("object->metaObject()->d.className = %1").arg(object->plastiq_metaObject()->d.className));
#endif

    if(!object || !object->plastiq_data()) {
        PQDBG_LVL_DONE();
        return false;
    }

    int mid = object->plastiq_metaObject()->methodId(signature);
    PQDBGLPUP(QString("signature: %1, methodId: %2").arg(signature.constData()).arg(mid));

    object->plastiq_metaObject()->d.static_metacall(object, PlastiQMetaObject::InvokeMethod, mid, stack);

    PQDBG_LVL_DONE();
    return true;
}

bool PlastiQMetaObject::connect(PQObjectWrapper *sender, const QByteArray &signalSignature, PQObjectWrapper *receiver, const QByteArray &slot)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    int signalId = sender->object->plastiq_metaObject()->signalId(signalSignature);
    if(signalId >= 0) {
        PQDBGLPUP(QString("signalId: %1").arg(signalId));

        PMOGStack stack = new PMOGStackItem[10];
        stack[1].s_voidp = reinterpret_cast<void*>(sender);
        stack[2].s_voidp = reinterpret_cast<void*>(receiver);
        stack[3].s_bytearray = slot;

        sender->object->plastiq_metaObject()->d.static_metacall(sender->object, PlastiQMetaObject::CreateConnection, signalId, stack);

        delete [] stack;
    }

#ifdef PQDEBUG
    else {
        PQDBGLPUP(QString("signalId: %1").arg(signalId));
    }
#endif

    PQDBG_LVL_DONE();
    return signalId >= 0;
}

PlastiQObject *PlastiQMetaObject::newInstance(const QByteArray &signature, const PMOGStack &stack)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("PlastiQMetaObject::d.className = %1").arg(d.className));
#endif

    int cid = constructorId(signature);

    PlastiQObject *object = Q_NULLPTR;

    stack[0].s_voidpp = reinterpret_cast<void**>(&object);

    if(cid < 0) {
        PQDBG_LVL_DONE();
        return object;
    }

    static_metacall(PlastiQMetaObject::CreateInstance, cid, stack);

    PQDBG_LVL_DONE();
    return object;
}

PlastiQObject *PlastiQMetaObject::createInstanceFromData(void *data)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("objectId: %1").arg(reinterpret_cast<quint64>(data)));
#endif

    PMOGStack stack = new PMOGStackItem[2];

    PlastiQObject *object = Q_NULLPTR;
    stack[0].s_voidpp = reinterpret_cast<void**>(&object);
    stack[1].s_voidp = data;

    static_metacall(PlastiQMetaObject::CreateDataInstance, 0, stack);

    delete [] stack;

    PQDBG_LVL_DONE();
    return object;
}

bool PlastiQMetaObject::static_metacall(PlastiQMetaObject::Call call, int id, const PMOGStack &stack)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    if(!d.static_metacall) {
        PQDBG_LVL_DONE();
        return false;
    }

    PQDBGLPUP(QString("d.static_metacall id %1").arg(id));
    d.static_metacall(Q_NULLPTR, call, id, stack);

    PQDBG_LVL_DONE();
    return true;
}

const QList<PlastiQCandidateMethod> PlastiQMetaObject::candidates(const QByteArray &name, int argc, PlastiQMethod::Type type, PlastiQMethod::Access filter, bool ignoreCase) const
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("PlastiQMetaObject::d.className = %1").arg(d.className));
    PQDBGLPUP(QString("find candidates for `%1`").arg(name.constData()));
#endif

    QList<PlastiQCandidateMethod> candidateList;

    const PlastiQMetaObject *m = this;
    int offset = 0;
    bool maxArgs = argc == -1;
    QByteArray lowerName = name.toLower();

    do {
        QHashIterator<QByteArray, PlastiQMethod> *i;
        const QHash<QByteArray, PlastiQMethod> *hash;

        switch(type) {
        case PlastiQMethod::Method:
            i = new QHashIterator<QByteArray, PlastiQMethod>(*(m->d.pq_methods));
            hash = m->d.pq_methods;
            break;

        case PlastiQMethod::Constructor:
            i = new QHashIterator<QByteArray, PlastiQMethod>(*(m->d.pq_constructors));
            hash = m->d.pq_constructors;
            break;

        case PlastiQMethod::Signal:
            i = new QHashIterator<QByteArray, PlastiQMethod>(*(m->d.pq_signals));
            hash = m->d.pq_signals;
            break;

        default:
            PQDBG_LVL_DONE();
            return candidateList;
        }

        while(i->hasNext()) {
            i->next();


            if(ignoreCase) {
                if(i->value().name.toLower() != lowerName) continue;
            }
            else {
                if(i->value().name != name) continue;
            }

            int idx = i->key().indexOf("(");
            QString methodSignature = i->key().mid(idx + 1, i->key().size() - idx - 2);

            QStringList argTypes = methodSignature.split(",");
            int candidateArgc = methodSignature.length() ? argTypes.size() : 0;

            if(type == PlastiQMethod::Signal && maxArgs) {
                if(argc < candidateArgc) {
                    argc = candidateArgc;
                    candidateList.clear();
                    candidateList.append({ i->value().name, argc, argTypes, i->value().index + offset });
                    continue;
                }
            }
            else {
                if(!maxArgs && candidateArgc != argc) {
                    continue;
                }
            }

            if(filter == PlastiQMethod::None || filter == i->value().access) {
                if(maxArgs) {
                    if(argc < candidateArgc) {
                        PQDBGLPUP(QString("reappend candidate: %1(%2)").arg(name.constData()).arg(methodSignature));
                        argc = candidateArgc;
                        candidateList.clear();
                        candidateList.append({ i->value().name, argc, argTypes, i->value().index + offset });
                        continue;
                    }
                }
                else {
                    PQDBGLPUP(QString("append candidate: %1(%2)").arg(name.constData()).arg(methodSignature));
                    candidateList.append({ i->value().name, argc, argTypes, i->value().index + offset });
                }
            }
        }

        if(type == PlastiQMethod::Constructor) {
            delete i;
            break;
        }
        else {
            offset += hash->size();
            delete i;
        }
    } while(m = m->d.superdata);

    PQDBG_LVL_DONE();
    return candidateList;
}
