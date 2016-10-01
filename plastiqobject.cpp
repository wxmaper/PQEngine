#include "plastiqobject.h"
#include "plastiqclasses.h"

#include "pqengine_global.h"

QVector<PlastiQMetaObject*> PlastiQObject::plastiqInherits;
QHash<QByteArray, PlastiQMethod> PlastiQObject::plastiqMethods;
QHash<QByteArray, PlastiQMethod> PlastiQObject::plastiqConstructors;
PlastiQMetaObject PlastiQObject::plastiq_static_metaObject;
const PlastiQ::ObjectType PlastiQObject::plastiq_static_objectType = PlastiQ::IsObject;

int PlastiQObject::plastiq_metacall(PlastiQMetaObject::Call call, int id, const PMOGStack &stack)
{
    return -1;
}

void PlastiQObject::plastiq_static_metacall(PlastiQObject *object, PlastiQMetaObject::Call call, int id, const PMOGStack &stack)
{
}


PlastiQObject::PlastiQObject()
    : m_typeName(""), m_typeId(-1), m_isNull(true), QObject()
{
}

PlastiQObject::PlastiQObject(const QByteArray &typeName, int typeId, void *data)
    : m_typeName(typeName), m_typeId(typeId), QObject()
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
    : m_typeName(typeName), QObject()
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
    : m_typeName(typeName), QObject()
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

PlastiQObject::~PlastiQObject()
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    if(dptr) {
        PQDBGLPUP("delete dptr");
        delete dptr;
    }

    if(ddptr) {
        PQDBGLPUP("delete ddptr");
       // delete ddptr;
    }

    PQDBG_LVL_DONE();
}

void PlastiQObject::plastiq_setData(void *data, PlastiQObject *ddata) {
    dptr = data;
    ddptr = ddata;

    if(dptr != Q_NULLPTR) {
        m_isNull = false;
    }
    else {
        m_isNull = true;
    }
}

void *PlastiQObject::plastiq_data() {
    return dptr;
}

PlastiQMetaObject *PlastiQObject::plastiq_dynamicMetaObject()
{
    return &ddptr->plastiq_static_metaObject;
}

//PlastiQObject::ObjectType PlastiQObject::objectType()
//{
//    return PlastiQObject::ObjectType(*(metaObject()->d.objectType));
//}

PlastiQ::ObjectType PlastiQObject::plastiq_objectType()
{
    return *plastiq_metaObject()->d.objectType;
}

QObject *PlastiQObject::plastiq_toQObject()
{
    QObject *qobject = plastiq_metaObject()->toQObject(this);
    return qobject;
}

bool PlastiQObject::plastiq_haveParent()
{
    return plastiq_metaObject()->haveParent(this);
}


const PlastiQMetaObject *PlastiQObject::plastiq_metaObject() const
{
    return ddptr ? &ddptr->plastiq_static_metaObject : &plastiq_static_metaObject;
}