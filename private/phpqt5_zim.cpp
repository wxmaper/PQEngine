#include "pqclasses.h"
#include "phpqt5.h"

void PHPQt5::zim_pqobject___construct(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2(QString("PHPQt5::zim_pqobject___construct() z: %1")
           .arg(Z_OBJVAL_P(getThis()).handle),
           Z_OBJCE_P(getThis())->name);
#endif

    zend_class_entry *ce = Z_OBJCE_P(getThis());

    const int argc = ZEND_NUM_ARGS();
    zval ***args = (zval ***) safe_emalloc(argc, sizeof(zval **), 0);

    if(zend_get_parameters_array_ex(argc, args) == FAILURE)
    {
        efree(args);
        WRONG_PARAM_COUNT;
    }

    QVariantList vargs;
    QString argsTypes;

    for(int i = 0; i < argc; i++) {
        switch(Z_TYPE_PP(args[i])) {
        case IS_BOOL:
            vargs << QVariant(Z_BVAL_PP(args[i]));
            argsTypes += argsTypes.length()
                    ? ", bool" : "bool";
            break;

        case IS_STRING:
            vargs << QVariant(Z_STRVAL_PP(args[i]));
            argsTypes += argsTypes.length()
                    ? ", string" : "string";
            break;

        case IS_LONG:
            vargs << QVariant((int) Z_LVAL_PP(args[i]));
            argsTypes += argsTypes.length()
                    ? ", int" : "int";
            break;

        case IS_DOUBLE:
            vargs << QVariant(Z_DVAL_PP(args[i]));
            argsTypes += argsTypes.length()
                    ? ", double" : "double";
            break;

        case IS_OBJECT: {
            if(pq_test_ce(*args[i] TSRMLS_CC)) {
                QObject *arg_qo = objectFactory()->getObject(*args[i] TSRMLS_CC);
                vargs << QVariant::fromValue<QObject*>( arg_qo );
                argsTypes += argsTypes.length()
                        ? ", " + QString(arg_qo->metaObject()->className()).mid(1)
                        : QString(arg_qo->metaObject()->className()).mid(1);
            }
            else {
                php_error(E_ERROR, QString("Unknown type of argument %1").arg(i).toUtf8().constData());
            }
            break;
        }

        case IS_NULL:
            vargs << NULL;
            argsTypes += argsTypes.length() ? ", null" : "null";
            break;

        default:
            php_error(E_ERROR, QString("Unknown type of argument %1").arg(i).toUtf8().constData());
        }
    }

    while(!objectFactory()->getRegisteredMetaObjects().contains(ce->name)
          && ce->parent != NULL)
    {
        ce = ce->parent;
    }

    if(objectFactory()->getRegisteredMetaObjects().contains(ce->name)) {
        QObject *qo = objectFactory()->createObject(ce->name, getThis(), vargs TSRMLS_CC);

        if(qo) {
            int is_tmp;
            HashTable *objht = Z_OBJDEBUG_P(getThis(), is_tmp);

            zval *uid;
            MAKE_STD_ZVAL(uid);
            ZVAL_LONG(uid, reinterpret_cast<quint32>(qo));

            zval *zhandle;
            MAKE_STD_ZVAL(zhandle);
            ZVAL_LONG(zhandle, Z_OBJVAL_P(getThis()).handle);

            zend_hash_add(objht, "uid", sizeof("uid"), &uid, sizeof(zval *), NULL);
            zend_hash_add(objht, "zhandle", sizeof("zhandle"), &zhandle, sizeof(zval *), NULL);
        }
        else {
            QString constructors;

            QMetaObject metaObject
                    = objectFactory()->getRegisteredMetaObjects().value(Z_OBJCE_P(getThis())->name).metaObject;

            for(int index = 0;
                index < metaObject.constructorCount();
                index++) {

                QList<QByteArray> parameterTypesList = metaObject.constructor(index).parameterTypes();
                QString parameterTypes;

                foreach(QByteArray parameterType, parameterTypesList) {
                    parameterTypes += parameterTypes.length()
                            ? ", " + parameterType
                            : parameterType;
                }

                constructors += constructors.length()
                        ? QString("<br>%1(%2)").arg(Z_OBJCE_P(getThis())->name).arg(parameterTypes)
                        : QString("%1(%2)").arg(Z_OBJCE_P(getThis())->name).arg(parameterTypes);
            }

            pq_pre(QString("<b>Fatal error</b>: could not create object %1 with params: (%2)<br><br>"
                           "Available constructors:<br>%3")
                   .arg(Z_OBJCE_P(getThis())->name)
                   .arg(argsTypes)
                   .arg(constructors), "Information");

            php_request_shutdown((void *) 0);
            SG(server_context) = NULL;
            php_module_shutdown(TSRMLS_C);
            sapi_deactivate(TSRMLS_C);
            sapi_shutdown();
            tsrm_shutdown();
        }
    }
    else {
        php_error(E_ERROR, "Can't create object");
    }

    efree(args);
}

void PHPQt5::zim_pqobject___call(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::zim_pqobject___call()", Z_OBJCE_P(getThis())->name);
#endif

    char* method;
    int method_len;
    zval *pzval;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &method, &method_len, &pzval) == FAILURE) {
        return;
    }

#ifdef PQDEBUG
    PQDBG(QString("%1->%2").arg(Z_OBJCE_P(getThis())->name).arg(method));
#endif

    QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);

    if(qo != NULL) {
        /*
         * Вызов метода setParent( ... )
         */
        if(method == QString("setParent")) {
            RETURN_BOOL( pq_set_parent(qo, pzval TSRMLS_CC) );
        }

        /*
         * Вызов метода connect( ... )
         */
        if(method == QString("connect")) {
            RETURN_BOOL( pq_connect_ex(getThis(), pzval TSRMLS_CC) )
        }

        /*
         * Вызов метода moveToThread( ... )
         */
        else if(method == QString("moveToThread")) {
            RETURN_BOOL( pq_move_to_thread(qo, pzval TSRMLS_CC) )
        }

        /*
         * Вызов метода getChildObjects()
         */
        else if(method == QString("getChildObjects")) {
            zval *z_sender = pq_get_child_objects(qo, pzval TSRMLS_CC);
            ZVAL_ZVAL(return_value, z_sender, 1, 0);
            return;
        }

        /*
         * Вызов иного метода....
         */
        else {
            pq_call_with_return(qo, method, pzval, INTERNAL_FUNCTION_PARAM_PASSTHRU);
        }
    }
    else {
#ifdef PQDEBUG
        PQDBG("ERROR: NULL POINT OF QOBJECT");
#endif
    }
}

void PHPQt5::zim_pqobject___callStatic(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG("PHPQt5::zim_pqobject___callStatic()");
#endif

    char* method;
    int method_len;
    zval *pzval;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &method, &method_len, &pzval) == FAILURE) {
        return;
    }

    zend_class_entry* ce = NULL;
    if (EG(called_scope)) {
        ce = EG(called_scope);
    } else if (!EG(scope))  {
        ce = EG(scope);
    }

#ifdef PQDEBUG
    PQDBG(QString("%1::%2").arg(ce->name).arg(method));
#endif

    QMetaObject mo = objectFactory()->getRegisteredMetaObjects().value(QString("%1").arg(ce->name)).metaObject;
    QObject *qo = (QObject*)(mo.newInstance());

    if(qo) {
        pq_call_with_return(qo, method, pzval, INTERNAL_FUNCTION_PARAM_PASSTHRU);
        delete qo;
    }
}

void PHPQt5::zim_pqobject___set(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::zim_pqobject___set()", Z_OBJCE_P(getThis())->name);
#endif

    char *property;
    int property_len;
    zval *pzval;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &property, &property_len, &pzval) == FAILURE) {
        return;
    }

#ifdef PQDEBUG
    PQDBG(QString("%1->%2").arg(Z_OBJCE_P(getThis())->name).arg(property));
#endif

    QObject *qo = (QObject*) objectFactory()->getObject(getThis() TSRMLS_CC);

    if(qo != NULL) {
        switch(Z_TYPE_P(pzval)) {
        case IS_BOOL: {
            bool bval = Z_BVAL_P(pzval);
            if(!qo->setProperty(property, bval)) {
                pq_set_user_property(qo, QString(property), bval TSRMLS_CC);
            }
            break;
        }

        case IS_STRING: {
            QByteArray strval = QByteArray(Z_STRVAL_P(pzval));

            if(!qo->setProperty(property, QString(toUTF8(strval)))) {
                pq_set_user_property(qo, QString(property), strval TSRMLS_CC);
            }
            break;
        }

        case IS_LONG: {
            int lval = Z_LVAL_P(pzval);
            if(!qo->setProperty(property, lval)) {
                pq_set_user_property(qo, QString(property), lval TSRMLS_CC);
            }
            break;
        }

        case IS_DOUBLE: {
            double dval = Z_DVAL_P(pzval);
            if(!qo->setProperty(property, dval)) {
                pq_set_user_property(qo, QString(property), dval TSRMLS_CC);
            }
            break;
        }

        case IS_NULL:
            if(!qo->setProperty(property, NULL)) {
                pq_set_user_property(qo, QString(property), NULL TSRMLS_CC);
            }
            break;

        case IS_OBJECT: {
            QString qosignal = property;

            if(pq_create_php_slot(qo, qosignal, pzval TSRMLS_CC)) {
                RETURN_TRUE;
            }

            if(pq_test_ce(pzval TSRMLS_CC)) {
                QObject *arg_qo = objectFactory()->getObject(pzval TSRMLS_CC);
                if(arg_qo) {

                    bool before_have_parent = arg_qo->parent() ? true : false;
                    if(qo->setProperty(property, QVariant::fromValue<QObject*>(arg_qo))) {
                        bool after_have_parent = arg_qo->parent() ? true : false;

                        switch (m_mmng) {
                        case MemoryManager::PHPQt5:

                            break;

                        case MemoryManager::Zend:
                            if(!before_have_parent
                                    && after_have_parent) {
                                Z_ADDREF_P(pzval);
                            }
                            break;

                        case MemoryManager::Hybrid:
                        default:
                            if(!arg_qo->isWidgetType()
                                    && !before_have_parent
                                    && after_have_parent) {
                                Z_ADDREF_P(pzval);
                            }
                        }

                    }
                    else {
                        php_error(E_WARNING,
                                  QString("PQEngine: %1::%2: Cannot set property.")
                                  .arg(Z_OBJCE_P(getThis())->name)
                                  .arg(property).toUtf8().constData());

                        RETURN_FALSE;
                    }
                }
            }
            else {
                php_error(E_WARNING,
                          QString("PQEngine: %1::%2: Bad type of argument!")
                          .arg(Z_OBJCE_P(getThis())->name)
                          .arg(property).toUtf8().constData());

                RETURN_FALSE;
            }
            break;
        }

        default:
            php_error(E_WARNING,
                      QString("PQEngine: %1::%2: Bad type of argument!")
                      .arg(Z_OBJCE_P(getThis())->name)
                      .arg(property).toUtf8().constData());

            RETURN_FALSE;
        }

        RETURN_TRUE;
    }
    else {
        php_error(E_WARNING, "PQEngine: __set(): Object not found!");
        RETURN_FALSE;
    }
}

void PHPQt5::zim_pqobject___get(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::zim_pqobject___get()", Z_OBJCE_P(getThis())->name);
#endif

    char* property;
    int property_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &property, &property_len) == FAILURE) {
        return;
    }

#ifdef PQDEBUG
    PQDBG(QString("%1->%2").arg(Z_OBJCE_P(getThis())->name).arg(property));
#endif

    QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);

    if(qo != NULL) {
        QVariant retVal;

        int indexOfProperty = qo->metaObject()->indexOfProperty(property);
        if(indexOfProperty != -1) {
            retVal = qo->property(property);
        }
        else {
            if(!qo->metaObject()->invokeMethod(qo, "getUserProperty",
                                              Q_RETURN_ARG(QVariant, retVal),
                                              Q_ARG(QString, property))) {
                php_error(E_WARNING,
                          QString("Can't get object property `%1`")
                          .arg(property).toUtf8().constData());
            }
        }

        pq_return_qvariant(retVal, INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
}

void PHPQt5::zim_qApp___callStatic(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2("qApp", "__callStatic()");
#endif

    char* method;
    int method_len;
    zval *pzval;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &method, &method_len, &pzval) == FAILURE) {
        return;
    }

    if(QString(method) == "arguments") {
        pq_return_qvariant(pq_get_arguments(), INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
    else {
        pq_call_with_return(qApp, method, pzval, INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
}

void PHPQt5::zim_qevent_ignore(INTERNAL_FUNCTION_PARAMETERS) {
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    int handle = Z_OBJVAL_P(getThis()).handle;
    pq_eventHash.value(handle).e->ignore();
}

void PHPQt5::zim_qevent_accept(INTERNAL_FUNCTION_PARAMETERS) {
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    int handle = Z_OBJVAL_P(getThis()).handle;
    pq_eventHash.value(handle).e->accept();
}

void PHPQt5::zim_pqobject___destruct(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::zim_pqobject___destruct", QString::number( Z_OBJVAL_P(getThis()).handle ));
#endif

    switch (m_mmng) {
    case MemoryManager::PHPQt5:
        // no need to remove
        break;

    case MemoryManager::Zend:
        zim_pqobject_free(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        break;

    case MemoryManager::Hybrid:
    default: {
        QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);

        if(qo
                && !qo->isWidgetType()
                && !qo->isWindowType()
                && !qo->parent()) {
            zim_pqobject_free(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        }
    }
    }
}

void PHPQt5::zim_pqobject_qobjInfo(INTERNAL_FUNCTION_PARAMETERS) {
    zend_bool do_return = 0;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &do_return) == FAILURE) {
        return;
    }

    QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);
    QString out;

    if(qo != NULL) {
        const QMetaObject* metaObject = qo->metaObject();

        out = QString(metaObject->className()).mid(1)
                + " Object"
                + "\n(";

        for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
            QMetaProperty property = metaObject->property(i);
            out += QString("\n    "
                    + QString(property.typeName())
                    + " "
                    + QString(property.name()));

            switch (property.type()) {
            case QMetaType::QString:
                out += QString(" = \"%1\"").arg(qo->property(property.name()).toString());
                break;

            case QMetaType::Long:
            case QMetaType::Int:
            case QMetaType::LongLong:
                out += QString(" = %1").arg(qo->property(property.name()).toInt());
                break;

            case QMetaType::Double:
            case QMetaType::Float:
                out += QString(" = %1").arg(qo->property(property.name()).toDouble());
                break;

            case QMetaType::Bool:
                out += QString(" = %1").arg(
                            qo->property(property.name()).toBool() ? "true" : "false"
                            );
                break;

            default:
                out += QString(" = *unknown*");
                break;
            }
        }

        out += "\n)";
    }

    zval *zout = NULL;
    MAKE_STD_ZVAL(zout);
    ZVAL_STRING(zout, toW(out.toUtf8()).constData(), 1);

    if (do_return) {
        php_output_start_default(TSRMLS_C);
    }

    zend_print_zval_r(zout, 0 TSRMLS_CC);
    efree(zout);

    if (do_return) {
        php_output_get_contents(return_value TSRMLS_CC);
        php_output_discard(TSRMLS_C);
    } else {
        RETURN_TRUE;
    }
}

void PHPQt5::zim_pqobject_qobjProperties(INTERNAL_FUNCTION_PARAMETERS) {
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);
    QMap<QString,QString> properties;

    if(qo != NULL) {
        const QMetaObject* metaObject = qo->metaObject();

        for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
            properties.insert(QString(metaObject->property(i).name()),
                           QString(metaObject->property(i).typeName()));
        }
    }

    array_init(return_value);

    QMapIterator<QString,QString> iter(properties);
    while(iter.hasNext()) {
        iter.next();
        add_assoc_string(return_value,
                         iter.key().toUtf8().constData(),
                         iter.value().toUtf8().data(),
                         1);
    }
}

void PHPQt5::zim_pqobject_qobjMethods(INTERNAL_FUNCTION_PARAMETERS) {
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);
    QMap<QString,QString> methods;

    if(qo != NULL) {
        const QMetaObject* metaObject = qo->metaObject();

        for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i) {
            methods.insert(QString(metaObject->method(i).methodSignature().constData()),
                           QString(metaObject->method(i).typeName()));
        }
    }

    array_init(return_value);

    QMapIterator<QString,QString> iter(methods);
    while(iter.hasNext()) {
        iter.next();
        add_assoc_string(return_value,
                         iter.key().toUtf8().constData(),
                         iter.value().toUtf8().data(),
                         1);
    }
}

void PHPQt5::zim_pqobject_free(INTERNAL_FUNCTION_PARAMETERS)
{
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
        return;
    }

    QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);

#ifdef PQDEBUG
    PQDBG2("PHPQt5::zim_pqobject_free()",
           QString("z: %1, qo: %2")
           .arg(Z_OBJVAL_P(getThis()).handle)
           .arg(reinterpret_cast<qint32>(qo)));
#endif

    if(qo) {
        objectFactory()->freeObject(qo);
    }
}


void PHPQt5::zim_pqobject_setPHPEventListener(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::zim_pqobject_setPHPEventListener()", Z_OBJCE_P(getThis())->name);
#endif

    zval *callback;
    zval *listener;
    zval *pzval1;
    zval *pzval2;
    char *callback_name;
    int callback_name_len;
    QString callback_sign = "";
    char *error;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &pzval1, &pzval2) == FAILURE) {
        return;
    }

    QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);

    if(qo != NULL) {
        if(ZEND_NUM_ARGS() == 1) {
            listener = NULL;
            callback = pzval1;
        }
        else if(ZEND_NUM_ARGS() == 2) {
            listener = pzval1;
            callback = pzval2;

            if(Z_TYPE_P(listener) != IS_OBJECT) {
                pq_ub_write("Cannot set eventListener: unknown argument 1, expected object");
                RETURN_FALSE;
            }

            callback_sign = QString(Z_OBJCE_P(listener)->name).append("_%1").arg(Z_OBJVAL_P(listener).handle);
        }

        if(zend_is_callable_ex(callback, listener, 0, &callback_name, &callback_name_len, NULL, &error TSRMLS_CC))
        {
            zval *fn_name;
            MAKE_STD_ZVAL(fn_name);
            ZVAL_STRING(fn_name, QByteArray(callback_name).constData(), 1);

            pq_access_function_entry afe {
                "",
                fn_name,
                callback,
                listener,
                NULL
            };

            callback_sign += QString(callback_name).append("_%1").arg(Z_OBJVAL_P(getThis()).handle);

            QVariantList args;
            args << QString(callback_sign);

            pq_call(qo, "setPHPEventListener", args TSRMLS_CC);
            acceptedPHPFunctions.insert(QString(callback_sign), afe);
            RETURN_TRUE;
        }
        else {
            pq_ub_write(QString("Cannot set eventListener: %1").arg(callback_name));
            RETURN_FALSE;
        }
    }

    RETURN_FALSE;
}

void PHPQt5::zim_pqobject___toString(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::zim_pqobject___toString()", Z_OBJCE_P(getThis())->name);
#endif

    QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);
    QString retStr = "";

    if(qo != NULL) {
        QString pClassName = QString(Z_OBJCE_P(getThis())->name);
        QString qClassName = QString(qo->metaObject()->className()).mid(1);
        QString objectName = qo->objectName();

        retStr = QString("%1[%2:%3]").arg(qClassName).arg(pClassName).arg(objectName);
    }

    RETURN_STRING(toW(retStr.toUtf8()).constData(), 1);
}

void PHPQt5::zim_pqobject_children(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::zim_pqobject_children()", Z_OBJCE_P(getThis())->name);
#endif

    bool subchilds = true;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &subchilds) == FAILURE) {
        return;
    }

    QObject *qo = objectFactory()->getObject(getThis() TSRMLS_CC);

    zval *array = NULL;
    MAKE_STD_ZVAL(array);
    array_init(array);

    if(qo != NULL) {
        QObjectList childs = qo->children();

        int index = 0;
        foreach(QObject *child, childs) {
            if(!subchilds) {
                if(child->parent() != qo) {
                    continue;
                }
            }

            zval* pzval = objectFactory()->getZObject(child);

            if(pzval != NULL) {
                add_index_zval(array, index, pzval);
                index++;
            }
        }
    }

    RETURN_ZVAL(array, 1, 0);
}
