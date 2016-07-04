#include "phpqt5objectfactory.h"
#include "phpqt5.h"

#include "plastiqobject.h"

bool PHPQt5ObjectFactory::havePlastiQMetaObject(const QByteArray &className PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("className: %1").arg(className.constData()));
#endif

    return m_plastiqClasses.contains(className);
}

PlastiQMetaObject PHPQt5ObjectFactory::getMetaObject(const QByteArray &className PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("className: %1").arg(className.constData()));
#endif

    return m_plastiqClasses.value(className);
}

bool PHPQt5ObjectFactory::createPlastiQObject(const QByteArray &className,
                                              const QByteArray &signature,
                                              zval *pzval,
                                              const PMOGStack &stack
                                              PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("className: %1").arg(className.constData()));
#endif

    if(!m_plastiqClasses.contains(className)) {
        return false;
    }

    PlastiQMetaObject metaObject = m_plastiqClasses.value(className);
    PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

    PlastiQObject *object = metaObject.newInstance(signature, stack);
    PQDBGLPUP(QString("created object: %1").arg(object->metaObject()->className()));

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(pzval));
    pqobject->object = object;

    zend_class_entry *ce = Z_OBJCE_P(pzval);

    quint64 uid = reinterpret_cast<quint64>(object);
    quint32 zhandle = Z_OBJ_HANDLE_P(pzval);

    do {
        zend_update_property_long(ce, pzval, "__pq_uid", sizeof("__pq_uid")-1, uid);
        zend_update_property_long(ce, pzval, "__pq_zhandle", sizeof("__pq_zhandle")-1, zhandle);
    } while (ce = ce->parent);

    return false;
}
