#include "phpqt5objectfactory.h"
#include "phpqt5.h"

#include "plastiqobject.h"

bool PHPQt5ObjectFactory::havePlastiQMetaObject(const QByteArray &className)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("className: %1").arg(className.constData()));
#endif

    PQDBG_LVL_DONE();
    return m_plastiqClasses.contains(className);
}

PlastiQMetaObject PHPQt5ObjectFactory::getMetaObject(const QByteArray &className)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("className: %1").arg(className.constData()));
#endif

    PQDBG_LVL_DONE();
    return m_plastiqClasses.value(className);
}

bool PHPQt5ObjectFactory::createPlastiQObject(const QByteArray &className,
                                              const QByteArray &signature,
                                              zval *pzval,
                                              const PMOGStack &stack)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("className: %1").arg(className.constData()));

    pqdbg_send_message({
                           { "command", "createPlastiQObject" },
                           { "thread", QString::number(reinterpret_cast<quint64>(QThread::currentThread())) },
                           { "class", className },
                           { "zclass", Z_OBJ_P(pzval)->ce->name->val },
                           { "signature", signature },
                           { "zobject", QString::number(reinterpret_cast<quint64>(Z_OBJ_P(pzval))) },
                           { "zhandle", QString::number(Z_OBJ_HANDLE_P(pzval)) },
                       });
#endif

    if(!m_plastiqClasses.contains(className)) {
        PQDBG_LVL_DONE();
        return false;
    }

    PlastiQMetaObject metaObject = m_plastiqClasses.value(className);
    PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

    PlastiQObject *object = metaObject.newInstance(signature, stack);
    PQDBGLPUP(QString("created object: %1").arg(object->plastiq_metaObject()->className()));

#ifdef PQDEBUG
    pqdbg_send_message({
                           { "command", "createdPlastiQObject" },
                           { "thread", QString::number(reinterpret_cast<quint64>(QThread::currentThread())) },
                           { "class", className },
                           { "zclass", Z_OBJ_P(pzval)->ce->name->val },
                           { "signature", signature },
                           { "zobject", QString::number(reinterpret_cast<quint64>(Z_OBJ_P(pzval))) },
                           { "zhandle", QString::number(Z_OBJ_HANDLE_P(pzval)) },
                           { "object", QString::number(reinterpret_cast<quint64>(object)) },
                           { "data", QString::number(reinterpret_cast<quint64>(object->plastiq_data())) },
                       });
#endif

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(pzval));
    pqobject->object = object;
    pqobject->connections = new QHash<QByteArray,ConnectionHash*>();
    pqobject->userProperties = new QHash<QByteArray,zval>();

    PlastiQ::ObjectType objectType = *(object->plastiq_metaObject()->d.objectType);

    switch(objectType) {
    case PlastiQ::IsQtObject: {
        PQDBGLPUP("object type: IsQtObject");

        if(object->plastiq_haveParent()) {
            Z_ADDREF_P(pzval);
        }

        //pqobject->ctx = PHPQt5::threadCreator()->get_tsrmls_cache(QThread::currentThread());
        pqobject->thread = QThread::currentThread();
        PQDBGLPUP(QString("thread: %1; TSRMLS_CACHE: %2")
                  .arg(reinterpret_cast<quint64>(QThread::currentThread()))
                  .arg(reinterpret_cast<quint64>(pqobject->ctx)));
    } break;

    case PlastiQ::IsQObject:
    case PlastiQ::IsQWidget:
    case PlastiQ::IsQWindow: {
        PQDBGLPUP("object type: IsQObject");

        if(object->plastiq_haveParent()) {
            Z_ADDREF_P(pzval);
        }

        extractSignals(pqobject, pzval);

        PMOGStack stack = new PMOGStackItem[1];
        metaObject.invokeMethod(object, "thread()", stack);
        QThread *thread = reinterpret_cast<QThread*>(stack[0].s_voidp);

        //pqobject->ctx = PHPQt5::threadCreator()->get_tsrmls_cache(thread);
        pqobject->thread = thread;
        pqobject->object->moveToThread(thread);
        PQDBGLPUP(QString("thread: %1; TSRMLS_CACHE: %2")
                  .arg(reinterpret_cast<quint64>(thread))
                  .arg(reinterpret_cast<quint64>(pqobject->ctx)));

        delete [] stack;
    } break;

    default:
        PQDBGLPUP("object type: IsObject");
        //pqobject->ctx = PHPQt5::threadCreator()->get_tsrmls_cache(QThread::currentThread());
        pqobject->thread = QThread::currentThread();
        PQDBGLPUP(QString("thread: %1; TSRMLS_CACHE: %2")
                  .arg(reinterpret_cast<quint64>(QThread::currentThread()))
                  .arg(reinterpret_cast<quint64>(pqobject->ctx)));
    }


    zend_class_entry *ce = Z_OBJCE_P(pzval);

    quint64 objectId = reinterpret_cast<quint64>(object->plastiq_data());
    quint32 zhandle = Z_OBJ_HANDLE_P(pzval);

    do {
        zend_update_property_long(ce, pzval, "__pq_uid", sizeof("__pq_uid")-1, objectId);
        zend_update_property_long(ce, pzval, "__pq_zhandle", sizeof("__pq_zhandle")-1, zhandle);
    } while (ce = ce->parent);


    addObject(pqobject, objectId);

    pqobject->isValid = true;
    pqobject->isExtra = false;

    PQDBG_LVL_DONE();
    return false;
}

bool PHPQt5ObjectFactory::havePlastiQObject(quint64 &objectId)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBG_LVL_DONE();
#endif

    Q_UNUSED(objectId);

    PQDBG_LVL_DONE();
    return false;
}

void PHPQt5ObjectFactory::addObject(PQObjectWrapper *pqobject, quint64 objectId)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("className: %1, objectId: %2").arg(pqobject->zo.ce->name->val).arg(objectId));

    pqdbg_send_message({
                           { "command", "addObjectToStorage" },
                           { "thread", QString::number(reinterpret_cast<quint64>(QThread::currentThread())) },
                           { "class", pqobject->object->plastiq_metaObject()->className() },
                           { "zclass", pqobject->zo.ce->name->val },
                           { "zhandle", QString::number(pqobject->zo.handle) },
                           { "zobject", QString::number(reinterpret_cast<quint64>(&pqobject->zo)) },
                           { "object", QString::number(reinterpret_cast<quint64>(pqobject->object)) },
                           { "data", QString::number(reinterpret_cast<quint64>(pqobject->object->plastiq_data())) },
                       });

#endif

    quint64 addId = objectId == 0
            ? reinterpret_cast<quint64>(pqobject->object->plastiq_data())
            : objectId;

    PQDBGLPUP(QString("objectId: %1").arg(addId));
    m_plastiqObjects.insert(addId, pqobject);

    QObject *qobject;
    if(qobject = pqobject->object->plastiq_toQObject()) {
        connect(qobject, SIGNAL(destroyed(QObject*)), this, SLOT(freeObject_slot(QObject*)));
    }

    PQDBG_LVL_DONE();
}

void PHPQt5ObjectFactory::removeObject(PQObjectWrapper *pqobject, quint64 objectId)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);

    pqdbg_send_message({
                           { "command", "removeObjectFromStorage" },
                           { "thread", QString::number(reinterpret_cast<quint64>(QThread::currentThread())) },
                           { "class", pqobject->object->plastiq_metaObject()->className() },
                           { "zclass", pqobject->zo.ce->name->val },
                           { "zhandle", QString::number(pqobject->zo.handle) },
                           { "zobject", QString::number(reinterpret_cast<quint64>(&pqobject->zo)) },
                           { "object", QString::number(reinterpret_cast<quint64>(pqobject->object)) },
                           { "data", QString::number(reinterpret_cast<quint64>(pqobject->object->plastiq_data())) },
                       });
#endif

    quint64 removeId = objectId == 0
            ? reinterpret_cast<quint64>(pqobject->object->plastiq_data())
            : objectId;

    PQDBGLPUP(QString("objectId: %1").arg(removeId));
    m_plastiqObjects.remove(removeId);

    PQDBG_LVL_DONE();
}

PQObjectWrapper *PHPQt5ObjectFactory::getObject(quint64 objectId)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("objectId: %1").arg(objectId));
#endif

    PQDBG_LVL_DONE();
    return m_plastiqObjects.value(objectId, Q_NULLPTR);
}

void PHPQt5ObjectFactory::extractSignals(PQObjectWrapper *pqobject, zval *zobject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    //    PQDBGLPUP("TSRMLS_CACHE_UPDATE");
    //    void *TSRMLS_CACHE = Q_NULLPTR;
    //    TSRMLS_CACHE_UPDATE();

    //    PQDBGLPUP("update scope");
    //    zend_class_entry *old_scope = EG(scope);
    //    EG(scope) = Z_OBJCE_P(zobject);

    zval *zsignals, rv;
    PQDBGLPUP("zend_read_property");
    zsignals = zend_read_property(Z_OBJCE_P(zobject), zobject, "signals", 7, 1, &rv);

    PQDBGLPUP("zval_dtor");
    zval_dtor(&rv);

    PQDBGLPUP("Z_TYPE_P(zsignals)");
    switch(Z_TYPE_P(zsignals)) {
    case IS_ARRAY: {
        zval *zsignal;
        zend_ulong index;

        PQDBGLPUP("ZEND_HASH_FOREACH_NUM_KEY_VAL");
        ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(zsignals), index, zsignal) {
            PQDBGLPUP("Z_TYPE_P(zsignal)");
            if(Z_TYPE_P(zsignal) == IS_STRING) {
                PQDBGLPUP(QString("register internal signal: %1").arg(QMetaObject::normalizedSignature(Z_STRVAL_P(zsignal)).constData()));
                pqobject->connections->insert(QMetaObject::normalizedSignature(Z_STRVAL_P(zsignal)), new ConnectionHash);
            }
            else {
                // zend_wrong_paramer_type_error(index, zend_expected_type(IS_STRING), zsignal);
                php_error(E_ERROR, QString("Error loading signals for `<b>%1</b>`<br>"
                                           "%2::signals expects value %3 to be <b>%4</b>, <b>%5</b> given")
                          .arg(Z_OBJCE_P(zobject)->name->val)
                          .arg(Z_OBJCE_P(zobject)->name->val)
                          .arg(index)
                          .arg(zend_expected_type(IS_STRING))
                          .arg(zend_zval_type_name(zsignal))
                          .toUtf8().constData());
            }
        } ZEND_HASH_FOREACH_END();
    } break;

    case IS_NULL:
    case IS_UNDEF:
        break;

    default:
        php_error(E_ERROR, QString("Cannot prepare signals for `<b>%1</b>`<br>"
                                   "%2::signals expects parameter to be <b>%3</b>, <b>%4</b> given")
                  .arg(Z_OBJCE_P(zobject)->name->val)
                  .arg(Z_OBJCE_P(zobject)->name->val)
                  .arg(zend_expected_type(IS_ARRAY))
                  .arg(zend_zval_type_name(zsignals))
                  .toUtf8().constData());
    }

    //    PQDBGLPUP("restore scope");
    //    EG(scope) = old_scope;

    PQDBG_LVL_DONE_LPUP();
}

PHPQt5ObjectFactory::PHPQt5ObjectFactory(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<pq_nullptr>("pq_nullptr");
    qRegisterMetaType<pq_nullptr*>("pq_nullptr*");
    qRegisterMetaType<QObjectList>("QObjectList");
    qRegisterMetaType<QObjectList*>("QObjectList*");
}

void PHPQt5ObjectFactory::freeObject(zend_object *zobject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);

    pqdbg_send_message({
                           { "command", "freeObject" },
                           { "thread", QString::number(reinterpret_cast<quint64>(QThread::currentThread())) },
                           { "zclass", zobject->ce->name->val },
                           { "zobject", QString::number(reinterpret_cast<quint64>(zobject)) },
                           { "zhandle", QString::number(zobject->handle) }
                       });
#endif

    PQObjectWrapper *pqobject = fetch_pqobject(zobject);

    if(pqobject->object != Q_NULLPTR && pqobject->isValid) {
        quint64 objectId = reinterpret_cast<quint64>(pqobject->object->plastiq_data());
        removeObject(pqobject, objectId);

        if(pqobject->userProperties != Q_NULLPTR) {
            QMutableHashIterator<QByteArray,zval> propertiesIter(*pqobject->userProperties);

            while(propertiesIter.hasNext()) {
                propertiesIter.next();

                if(Z_REFCOUNTED(propertiesIter.value()))
                    Z_DELREF(propertiesIter.value());
            }

            PQDBGLPUP("delete properties");
            delete pqobject->userProperties;
            pqobject->userProperties = Q_NULLPTR;
        }

        if(pqobject->connections != Q_NULLPTR) {
            QMutableHashIterator<QByteArray,ConnectionHash*> connectionsIter(*pqobject->connections);

            while(connectionsIter.hasNext()) {
                PQDBGLPUP("delete connections hash");
                connectionsIter.next();

                delete connectionsIter.value();
                connectionsIter.value() = Q_NULLPTR;
            }

            PQDBGLPUP("delete connections");
            delete pqobject->connections;
            pqobject->connections = Q_NULLPTR;
        }

        if(!pqobject->isExtra) {
            PQDBGLPUP(QString("delete object [%1]").arg(reinterpret_cast<quint64>(pqobject->object)));

#ifdef PQDEBUG
            pqdbg_send_message({
                                   { "command", "deleteObject" },
                                   { "thread", QString::number(reinterpret_cast<quint64>(QThread::currentThread())) },
                                   { "object", QString::number(reinterpret_cast<quint64>(pqobject->object)) },
                                   { "data", QString::number(reinterpret_cast<quint64>(pqobject->object->plastiq_data())) },
                                   { "class", pqobject->object->plastiq_metaObject()->className() }
                               });
#endif

            pqobject->isValid = false;

            delete pqobject->object;
            pqobject->object = Q_NULLPTR;
        }
#ifdef PQDEBUG
        else {
            PQDBGLPUP("extra object has not deleted");
        }
#endif
    }

#ifdef PQDEBUG
    else {
        if(pqobject->isEnum) {
            PQDBGLPUP("enum object doesn't need to delete");
        }
        else if(pqobject->object == Q_NULLPTR) {
            PQDBGLPUP("interrupt double free");
        }
        else if(!pqobject->isValid) {
            PQDBGLPUP("invalid object has not deleted");
        }
    }
#endif

    PQDBG_LVL_DONE();
}

void PHPQt5ObjectFactory::freeObject_slot(QObject *qobject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    quint64 objectId = reinterpret_cast<quint64>(qobject);

    PQObjectWrapper *pqobject = m_plastiqObjects.value(objectId, Q_NULLPTR);
    if(pqobject != Q_NULLPTR) {
        if(qobject->parent() && pqobject->isValid) {
            zval zv;
            ZVAL_OBJ(&zv, &pqobject->zo);
            zend_update_property_long(Z_OBJCE(zv), &zv, "__pq_uid", sizeof("__pq_uid")-1, 0);

            PQDBGLPUP("DELREF");
            Z_DELREF(zv);
        }

#ifdef PQDEBUG
        pqdbg_send_message({
                               { "command", "freeObjectSlot" },
                               { "thread", QString::number(reinterpret_cast<quint64>(QThread::currentThread())) },
                               { "object", QString::number(reinterpret_cast<quint64>(pqobject->object)) },
                               { "data", QString::number(reinterpret_cast<quint64>(pqobject->object->plastiq_data())) },
                               { "class", pqobject->object->plastiq_metaObject()->className() }
                           });
#endif

        pqobject->object = Q_NULLPTR;
        pqobject->isValid = false;
    }

    m_plastiqObjects.remove(objectId);

    PQDBG_LVL_DONE();
}

void PHPQt5ObjectFactory::registerZendClassEntry(QString className, zend_class_entry *ce_ptr)
{
    z_classes.insert(className, ce_ptr);
}

zend_class_entry *PHPQt5ObjectFactory::getClassEntry(const QByteArray &className)
{
    return z_classes.value(className, Q_NULLPTR);
}

QByteArray PHPQt5ObjectFactory::registerPlastiQMetaObject(const PlastiQMetaObject &metaObject)
{
    m_plastiqClasses.insert(metaObject.className(), metaObject);
    return metaObject.className();
}
