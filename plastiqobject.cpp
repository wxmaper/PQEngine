#include "plastiqobject.h"
#include "plastiqclasses.h"

#include "pqengine_global.h"

QHash<QByteArray, PlastiQMethod> PlastiQObject::platiqMethods;
QHash<QByteArray, PlastiQMethod> PlastiQObject::platiqConstructors;
PlastiQMetaObject PlastiQObject::plastiq_metaObject;
const uint PlastiQObject::plastiq_objectType = PlastiQObject::IsObject;

int PlastiQObject::plastiq_metacall(PlastiQMetaObject::Call call, int id, const PMOGStack &stack)
{
    PQDBG_LVL_START(__FUNCTION__);
    return -1;
}

void PlastiQObject::plastiq_static_metacall(PlastiQObject *object, PlastiQMetaObject::Call call, int id, const PMOGStack &stack)
{
    PQDBG_LVL_START(__FUNCTION__);
}


PlastiQObject::PlastiQObject()
    : m_typeName(""), m_typeId(-1), m_isNull(true)
{
}

PlastiQObject::PlastiQObject(const QByteArray &typeName, int typeId, void *data)
    : m_typeName(typeName), m_typeId(typeId)
{
    m_typeId = typeId;

    dptr = data;

    if(dptr != Q_NULLPTR) {
        m_isNull = false;
    }
    else {
        m_isNull = true;
    }
}

PlastiQObject::PlastiQObject(const QByteArray &typeName, void *data)
    : m_typeName(typeName)
{
    m_typeId = PlastiQClasses::typeId(m_typeName);
    dptr = data;

    if(dptr != Q_NULLPTR) {
        m_isNull = false;
    }
    else {
        m_isNull = true;
    }
}

PlastiQObject::PlastiQObject(const QByteArray &typeName, const QByteArray &constructorSignature, const PMOGStack &stack)
    : m_typeName(typeName)
{
    m_typeId = PlastiQClasses::typeId(m_typeName);
    // invoke(PlastiQMetaObject::Call::CreateInstance, constructorSignature, stack);

    if(dptr != Q_NULLPTR) {
        m_isNull = false;
    }
    else {
        m_isNull = true;
    }
}

void PlastiQObject::setData(void *data, PlastiQObject *ddata) {
    dptr = data;
    ddptr = ddata;

    if(dptr != Q_NULLPTR) {
        m_isNull = false;
    }
    else {
        m_isNull = true;
    }
}

void *PlastiQObject::data() {
    return dptr;
}

PlastiQMetaObject *PlastiQObject::dynamicMetaObject()
{
    return &ddptr->plastiq_metaObject;
}

PlastiQObject::ObjectType PlastiQObject::objectType()
{
    return PlastiQObject::ObjectType(*(metaObject()->d.objectType));
}

const PlastiQMetaObject *PlastiQObject::metaObject() const
{

    qDebug() << __FUNCTION__;
   // QObject::d_ptr->dynamicMetaObject();
    // QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject

    if(ddptr) {
        qDebug() << "ddptr" << reinterpret_cast<quint64>(ddptr);
        qDebug() << ddptr->plastiq_metaObject.d.className;
    }

    return ddptr ? &ddptr->plastiq_metaObject : &plastiq_metaObject;
}

//bool PlastiQObject::invoke(PlastiQMetaObject::Call call, const QByteArray &signature, const PMOGStack &stack)
//{
//    qDebug() << __FUNCTION__;

//    int typeId = PlastiQClasses::typeId(m_typeName);
//    bool ok = false;

//    switch(typeId) {
//    case 0:
//        ok = PlastiQQObject::invoke(call , &dptr, signature, stack);

//        qDebug() << reinterpret_cast<quint64>(dptr);
//        break;

//    case 1:
//        // ok = PlastiQQTimer::invoke(call, dptr, signature, stack);
//        break;

//    default:
//        break;
//    }

//    if(call == PlastiQMetaObject::Call::CreateInstance && ok) {
//        m_isNull = false;
//    }

//    return ok;
//}

//bool PlastiQObject::invokeMethod(const QByteArray &signature, const PMOGStack &stack)
//{
//    return invoke(PlastiQMetaObject::Call::InvokeMethod, signature, stack);
//}
