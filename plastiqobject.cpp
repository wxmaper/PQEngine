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
    Q_UNUSED(call)
    Q_UNUSED(id)
    Q_UNUSED(stack)

    return -1;
}

void PlastiQObject::plastiq_static_metacall(PlastiQObject *object, PlastiQMetaObject::Call call, int id, const PMOGStack &stack)
{
    Q_UNUSED(object)
    Q_UNUSED(call)
    Q_UNUSED(id)
    Q_UNUSED(stack)
}

PlastiQObject::PlastiQObject()
    : m_typeName(""),
      m_typeId(-1),
      m_isNull(true),
      m_isWrapper(false)//, QObject()
{
}

PlastiQObject::PlastiQObject(const QByteArray &typeName, int typeId, void *data)
    : m_typeName(typeName),
      m_typeId(typeId),
      m_isWrapper(false)//, QObject()
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
    : m_typeName(typeName),
      m_isWrapper(false)//, QObject()
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

    PQDBG_LVL_DONE_LPUP();
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

bool PlastiQObject::isWrapper()
{
    return m_isWrapper;
}

void PlastiQObject::setWrapperMark(bool isWrapper)
{
    m_isWrapper = isWrapper;
}

const PlastiQMetaObject *PlastiQObject::plastiq_metaObject() const
{
    return ddptr ? &ddptr->plastiq_static_metaObject : &plastiq_static_metaObject;
}
