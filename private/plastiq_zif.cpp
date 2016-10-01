#include "phpqt5.h"

#include "plastiqobject.h"
#include "plastiqmethod.h"

void PHPQt5::zif_connect(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *sender;
    char *signal;
    int signal_len;

    zval *receiver;
    char *slot;
    int slot_len;

    int argc = ZEND_NUM_ARGS();

    bool ok = false;
    switch(argc) {
    case 3:
        if(zend_parse_parameters(argc, "oso", &sender, &signal, &signal_len, &receiver) == FAILURE) {
            PQDBG_LVL_DONE();
            RETURN_FALSE;
        }

        ok = plastiqConnect(sender, QString(signal), receiver, QString(ZEND_INVOKE_FUNC_NAME), false);
        break;

    case 4:
        if(zend_parse_parameters(argc, "osos", &sender, &signal, &signal_len, &receiver, &slot, &slot_len) == FAILURE) {
            PQDBG_LVL_DONE();
            RETURN_FALSE;
        }

        ok = plastiqConnect(sender, QString(signal), receiver, QString(slot), false);
        break;

    default:
        zend_wrong_paramers_count_error(argc, 3, 4);
    }

    PQDBG_LVL_DONE();
    RETURN_BOOL(ok);
}

void PHPQt5::zif_setEngineErrorHandler(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *eh;
    zend_string *errorHandlerName = Q_NULLPTR;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "z", &eh) == FAILURE) {
        return;
    }

    PQDBGLPUP(QString("engineErrorHandler type: %1").arg(Z_TYPE_P(eh)));
    if(Z_TYPE_P(eh) != IS_NULL) { /* NULL == unset */
        if(!zend_is_callable(eh, 0, &errorHandlerName)) {
            PQDBGLPUP(QString("%1() expects the argument (%2) to be a valid callback").arg(get_active_function_name()).arg(errorHandlerName ? ZSTR_VAL(errorHandlerName) : "unknown"));
            zend_error(E_WARNING, "%s() expects the argument (%s) to be a valid callback",
                       get_active_function_name(), errorHandlerName ? ZSTR_VAL(errorHandlerName) : "unknown");
            zend_string_release(errorHandlerName);
            return;
        }
        else {
            PQDBGLPUP(QString("ok: %1").arg(errorHandlerName->val));
        }

        zend_string_release(errorHandlerName);
    }

    Z_ADDREF_P(eh);
    ZVAL_COPY(&engineErrorHandler, eh);
    Z_ADDREF(engineErrorHandler);

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_qApp(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    if(ZEND_NUM_ARGS() > 0) {
        zend_wrong_paramers_count_error(ZEND_NUM_ARGS(), 0 ,0);
    }


    if(qApp == Q_NULLPTR) {
        PQDBG_LVL_DONE();
        RETURN_NULL();
    }

    zval zobject;
    quint64 objectId = reinterpret_cast<quint64>(qApp);
    PQObjectWrapper *pqobject;

    if(pqobject = objectFactory()->getObject(objectId)) {
        ZVAL_OBJ(&zobject, &pqobject->zo);
        PQDBG_LVL_DONE();
        RETURN_ZVAL(&zobject, 1, 0);
    }
    else {
        QByteArray className(qApp->metaObject()->className());

        if(zend_class_entry *ce = objectFactory()->getClassEntry(className)) {
            PlastiQMetaObject metaObject = objectFactory()->getMetaObject(className);
            PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

            PlastiQObject *object = metaObject.createInstanceFromData(reinterpret_cast<void*>(qApp));
            PQDBGLPUP(QString("created object: %1").arg(object->plastiq_metaObject()->className()));

            PQDBGLPUP("object_init_ex");
            object_init_ex(&zobject, ce);

            PQDBGLPUP("fetch_pqobject");
            pqobject = fetch_pqobject(Z_OBJ(zobject));
            pqobject->object = object;
            pqobject->isExtra = true;
            pqobject->isValid = true;

            quint32 zhandle = Z_OBJ_HANDLE(zobject);

            zend_update_property_long(ce, &zobject, "__pq_uid", sizeof("__pq_uid")-1, objectId);
            zend_update_property_long(ce, &zobject, "__pq_zhandle", sizeof("__pq_zhandle")-1, zhandle);

            objectFactory()->addObject(pqobject, objectId);

            PQDBG_LVL_DONE();
            RETURN_ZVAL(&zobject, 1, 0);
        }
        else {
            PQDBG_LVL_DONE();
            RETURN_NULL();
        }
    }

    PQDBG_LVL_DONE(); // this never execute
}

void PHPQt5::zif_pqProperties(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *zobject;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "o", &zobject) == FAILURE) {
        return;
    }

    QByteArray className = "";
    zend_class_entry *ce = Z_OBJCE_P(zobject);

    do {
        if(objectFactory()->havePlastiQMetaObject(ce->name->val)) {
            className = QByteArray(ce->name->val);
            break;
        }
    } while(ce = ce->parent);

    zval zProps;
    array_init(&zProps);

    if(className.length()) {
        PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(zobject));

        if(pqobject->isValid) {
            const QHash<QByteArray, PlastiQProperty> *properties = pqobject->object->plastiq_metaObject()->d.pq_properties;
            foreach(const PlastiQProperty &property, *properties) {
                QByteArray typeBa = QByteArray(property.type).replace("*", "").replace("&", "").replace("const ", "");

                add_assoc_stringl_ex(&zProps,
                                     property.name.constData(), property.name.length(),
                                     (char*) typeBa.data(), typeBa.length());
            }
        }
        else {
            php_error(E_ERROR, "Object have been destroyed");
        }
    }
    else {
        php_error(E_ERROR, "Unknown class type `<b>%s</b>`", Z_OBJCE_NAME_P(zobject));
    }

    Z_DELREF(zProps);
    RETVAL_ZVAL(&zProps, 1, 0);

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_pqMethods(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *zobject;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "o", &zobject) == FAILURE) {
        return;
    }

    QByteArray className = "";
    zend_class_entry *ce = Z_OBJCE_P(zobject);

    do {
        if(objectFactory()->havePlastiQMetaObject(ce->name->val)) {
            className = QByteArray(ce->name->val);
            break;
        }
    } while(ce = ce->parent);

    zval zMethods;
    array_init(&zMethods);

    if(className.length()) {
        PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(zobject));

        if(pqobject->isValid) {
            const QHash<QByteArray, PlastiQMethod> *methods = pqobject->object->plastiq_metaObject()->d.pq_methods;
            foreach(const PlastiQMethod &method, *methods) {
                zval zMethodInfo;
                array_init(&zMethodInfo);

                QByteArray returnTypeBa = QByteArray(method.returnType).replace("*", "").replace("&", "").replace("const ", "");
                add_assoc_stringl_ex(&zMethodInfo,
                                     "returnType", strlen("returnType"),
                                     (char*) returnTypeBa.data(), returnTypeBa.length());

                QStringList argTypes = QString(method.argTypes).split(",");
                int argc = method.argTypes.length() ? argTypes.size() : 0;
                add_assoc_long_ex(&zMethodInfo,
                                  "argc", strlen("argc"),
                                  argc);

                QByteArray argTypesBa = QByteArray(method.argTypes).replace("*", "").replace("&", "").replace("const ", "");
                add_assoc_stringl_ex(&zMethodInfo,
                                     "argTypes", strlen("argTypes"),
                                     (char*) argTypesBa.data(), argTypesBa.length());

                add_assoc_long_ex(&zMethodInfo,
                                  "type", strlen("type"),
                                  (zend_long) method.type);

                add_assoc_long_ex(&zMethodInfo,
                                  "access", strlen("access"),
                                  (zend_long) method.access);

                add_assoc_zval_ex(&zMethods,
                                  method.name.constData(), method.name.length(),
                                  &zMethodInfo);
            }
        }
        else {
            php_error(E_ERROR, "Object have been destroyed");
        }
    }
    else {
        php_error(E_ERROR, "Unknown class type `<b>%s</b>`", Z_OBJCE_NAME_P(zobject));
    }

    Z_DELREF(zMethods);
    RETVAL_ZVAL(&zMethods, 1, 0);

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_pqStaticFunctions(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *zobject;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "o", &zobject) == FAILURE) {
        return;
    }

    QByteArray className = "";
    zend_class_entry *ce = Z_OBJCE_P(zobject);

    do {
        if(objectFactory()->havePlastiQMetaObject(ce->name->val)) {
            className = QByteArray(ce->name->val);
            break;
        }
    } while(ce = ce->parent);

    zval zMethods;
    array_init(&zMethods);

    if(className.length()) {
        PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(zobject));

        if(pqobject->isValid) {
            const QHash<QByteArray, PlastiQMethod> *methods = pqobject->object->plastiq_metaObject()->d.pq_methods;
            foreach(const PlastiQMethod &method, *methods) {
                if(method.access != PlastiQMethod::StaticPublic) {
                    continue;
                }

                zval zMethodInfo;
                array_init(&zMethodInfo);

                QByteArray returnTypeBa = QByteArray(method.returnType).replace("*", "").replace("&", "").replace("const ", "");
                add_assoc_stringl_ex(&zMethodInfo,
                                     "returnType", strlen("returnType"),
                                     (char*) returnTypeBa.data(), returnTypeBa.length());

                QStringList argTypes = QString(method.argTypes).split(",");
                int argc = method.argTypes.length() ? argTypes.size() : 0;
                add_assoc_long_ex(&zMethodInfo,
                                  "argc", strlen("argc"),
                                  argc);

                QByteArray argTypesBa = QByteArray(method.argTypes).replace("*", "").replace("&", "").replace("const ", "");
                add_assoc_stringl_ex(&zMethodInfo,
                                     "argTypes", strlen("argTypes"),
                                     (char*) argTypesBa.data(), argTypesBa.length());

                add_assoc_long_ex(&zMethodInfo,
                                  "type", strlen("type"),
                                  (zend_long) method.type);

                add_assoc_long_ex(&zMethodInfo,
                                  "access", strlen("access"),
                                  (zend_long) method.access);

                add_assoc_zval_ex(&zMethods,
                                  method.name.constData(), method.name.length(),
                                  &zMethodInfo);
            }
        }
        else {
            php_error(E_ERROR, "Object have been destroyed");
        }
    }
    else {
        php_error(E_ERROR, "Unknown class type `<b>%s</b>`", Z_OBJCE_NAME_P(zobject));
    }

    Z_DELREF(zMethods);
    RETVAL_ZVAL(&zMethods, 1, 0);

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_test_ref(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval val;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "z", &val) == FAILURE) {
        return;
    }

    PQDBGLPUP("---------------------------------");
    PQDBGLPUP(Z_STRVAL(val));

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_pqSignals(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *zobject;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "o", &zobject) == FAILURE) {
        return;
    }

    QByteArray className = "";
    zend_class_entry *ce = Z_OBJCE_P(zobject);

    do {
        if(objectFactory()->havePlastiQMetaObject(ce->name->val)) {
            className = QByteArray(ce->name->val);
            break;
        }
    } while(ce = ce->parent);

    zval zSignals;
    array_init(&zSignals);

    if(className.length()) {
        PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(zobject));

        if(pqobject->isValid) {
            const QHash<QByteArray, PlastiQMethod> *pqSignals = pqobject->object->plastiq_metaObject()->d.pq_signals;
            foreach(const PlastiQMethod &signal, *pqSignals) {
                zval zSignalInfo;
                array_init(&zSignalInfo);

                QByteArray returnTypeBa = QByteArray(signal.returnType).replace("*", "").replace("&", "").replace("const ", "");
                add_assoc_stringl_ex(&zSignalInfo,
                                     "returnType", strlen("returnType"),
                                     (char*) returnTypeBa.data(), returnTypeBa.length());

                QStringList argTypes = QString(signal.argTypes).split(",");
                int argc = signal.argTypes.length() ? argTypes.size() : 0;
                add_assoc_long_ex(&zSignalInfo,
                                  "argc", strlen("argc"),
                                  argc);

                QByteArray argTypesBa = QByteArray(signal.argTypes).replace("*", "").replace("&", "").replace("const ", "");
                add_assoc_stringl_ex(&zSignalInfo,
                                     "argTypes", strlen("argTypes"),
                                     (char*) argTypesBa.data(), argTypesBa.length());

                add_assoc_long_ex(&zSignalInfo,
                                  "type", strlen("type"),
                                  (zend_long) signal.type);

                add_assoc_long_ex(&zSignalInfo,
                                  "access", strlen("access"),
                                  (zend_long) signal.access);

                add_assoc_zval_ex(&zSignals,
                                  signal.name.constData(), signal.name.length(),
                                  &zSignalInfo);
            }
        }
        else {
            php_error(E_ERROR, "Object have been destroyed");
        }
    }
    else {
        php_error(E_ERROR, "Unknown class type `<b>%s</b>`", Z_OBJCE_NAME_P(zobject));
    }

    Z_DELREF(zSignals);
    RETVAL_ZVAL(&zSignals, 1, 0);

    PQDBG_LVL_DONE();
}
//static void             zif_pqMethods(INTERNAL_FUNCTION_PARAMETERS);
//static void             zif_pqStaticFunctions(INTERNAL_FUNCTION_PARAMETERS);
//static void             zif_pqSignals(INTERNAL_FUNCTION_PARAMETERS);
