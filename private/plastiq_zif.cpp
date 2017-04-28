#include "phpqt5.h"

#include "plastiqobject.h"
#include "plastiqmethod.h"
#include "phpqt5constants.h"
#include "pqengine_private.h"

#include <pqenginecore.h>

QHash<QString,zval> PHPQt5::loadChilds(QObject *object) {
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif
    QHash<QString,zval> objectList; // objectName -> zval

    foreach (QObject *child, object->children()) {
        const QString objectName = child->objectName();
        PQDBGLPUP(QString("objectName: %1").arg(objectName));

        if (objectName == "centralwidget") {
            QHash<QString,zval> objectList2 = loadChilds(child);
            QMutableHashIterator<QString,zval> objectListIt(objectList2);

            while (objectListIt.hasNext()) {
                objectListIt.next();
                objectList.insert(objectListIt.key(), objectListIt.value());
            }
        }

        if (!objectName.isEmpty() && objectName.at(0) != '_'
                && (child->parent() == object
                    || (child->parent() != Q_NULLPTR && child->parent()->objectName() == "centralwidget")))  {
            if (objectFactory()->havePlastiQMetaObject(child->metaObject()->className())) {
                zval zChild = pq_create_extra_object(child->metaObject()->className(), child, true, true);
                objectList.insert(objectName, zChild);
                Z_ADDREF(zChild);
            }
        }
    }

    PQDBG_LVL_DONE();
    return objectList;
}

void PHPQt5::zif_setupUi(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    RETVAL_NULL();
    zval *zobject;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "o", &zobject) == FAILURE) {
        return;
    }

    const char* fileName = zend_get_executed_filename();
    QFileInfo fi(fileName);

    IPQExtension *coreExt = PQEnginePrivate::pqExtensions.at(0);
    PlastiQ::IPlastiQUi *ui = coreExt->plastiqForms().value(fi.fileName().toUtf8(), Q_NULLPTR);

    if (!ui) {
        php_error(E_ERROR, QString("setupUi: not found a UI interface for `%1'.")
                  .arg(fi.fileName()).toUtf8().constData());
    }

    if (!pq_test_ce(zobject)) {
        php_error(E_ERROR, "setupUi: parent class is not QWidget.");
    }

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(zobject));
    switch (pqobject->object->plastiq_objectType()) {
    case PlastiQ::IsQWidget:
    case PlastiQ::IsQWindow: {
        QObject *obj = pqobject->object->plastiq_toQObject();
        ui->setupUi(pqobject, obj);

        zval retUi;
        object_init(&retUi);
        QHash<QString,zval> objectList = loadChilds(obj);

        QMutableHashIterator<QString,zval> it(objectList);
        while (it.hasNext()) {
            it.next();
            zend_update_property(Z_OBJCE(retUi), &retUi,
                                 it.key().toUtf8().constData(), it.key().length(), &it.value());
        }

        RETVAL_ZVAL(&retUi, 1, 0);
    } break;

    default:
        php_error(E_ERROR, "setupUi: parent class is not QWidget.");
    }

    PQDBG_LVL_DONE();
}

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

#if (PHP_VERSION_ID < 70101)
        zend_wrong_paramers_count_error(argc, 3, 4);
#else
        zend_wrong_parameters_count_error(argc, 3, 4);
#endif
    }

    PQDBG_LVL_DONE();
    RETURN_BOOL(ok);
}

void PHPQt5::zif_setEngineErrorHandler(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    RETVAL_NULL();

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
#if (PHP_VERSION_ID < 70101)
        zend_wrong_paramers_count_error(ZEND_NUM_ARGS(), 0 ,0);
#else
        zend_wrong_parameters_count_error(ZEND_NUM_ARGS(), 0 ,0);
#endif
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

        if (objectFactory()->havePlastiQMetaObject(className)) {
            zval zobject = PHPQt5::pq_create_extra_object(className, qApp, true, true);
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
