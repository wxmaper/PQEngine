/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PHPQt5.
**
** BEGIN LICENSE: MPL 2.0
**
** This Source Code Form is subject to the terms of the Mozilla Public
** License, v. 2.0. If a copy of the MPL was not distributed with this
** file, You can obtain one at http://mozilla.org/MPL/2.0/.
**
** END LICENSE
**
****************************************************************************/

#include "pqclasses.h"
#include "phpqt5.h"

QHash<QObject*,EventListenerEntry> PHPQt5::eventListeners;

void PHPQt5::zim_pqobject___construct(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg("0")
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    return_value = getThis();
    zval *this_ptr = return_value;
    zend_class_entry *ce = Z_OBJCE_P(this_ptr);

    const int argc = ZEND_NUM_ARGS();
    zval *args = (zval*) safe_emalloc(sizeof(zval), argc, 0); //= (zval *) safe_emalloc(argc, sizeof(zval), 0);

    if(zend_get_parameters_array_ex(argc, args) == FAILURE)
    {
        efree(args);
        WRONG_PARAM_COUNT;
    }

    QVariantList vargs;
    QString argsTypes;

    for(int i = 0; i < argc; i++) {
        switch(Z_TYPE(args[i])) {
        case IS_TRUE:
            vargs << QVariant(true);
            argsTypes += argsTypes.length()
                    ? ", bool" : "bool";
            break;

        case IS_FALSE:
            vargs << QVariant(false);
            argsTypes += argsTypes.length()
                    ? ", bool" : "bool";
            break;

        case IS_STRING:
            //vargs << QVariant( QByteArray::fromRawData(Z_STRVAL(args[i]), Z_STR(args[i])->len) );
            vargs << QVariant( QString(Z_STRVAL(args[i])) );

            argsTypes += argsTypes.length()
                    ? ", string" : "string";
            break;

        case IS_LONG:
            vargs << QVariant((int) Z_LVAL(args[i]));
            argsTypes += argsTypes.length()
                    ? ", int" : "int";
            break;

        case IS_DOUBLE:
            vargs << QVariant(Z_DVAL(args[i]));
            argsTypes += argsTypes.length()
                    ? ", double" : "double";
            break;

        case IS_OBJECT: {
            if(pq_test_ce(&args[i] PQDBG_LVL_CC)) {
                QObject *arg_qo = objectFactory()->getQObject(&args[i] PQDBG_LVL_CC);
                vargs << QVariant::fromValue<QObject*>(arg_qo);
                argsTypes += argsTypes.length()
                        ? ", " + QString(arg_qo->metaObject()->className()).mid(1)
                        : QString(arg_qo->metaObject()->className()).mid(1);
            }
            else {
                php_error(E_ERROR, QString("Unknown type of argument %1").arg(i).toUtf8().constData());
            }
            break;
        }

        case IS_NULL: {
            pq_nullptr ptr;
            vargs << QVariant::fromValue<pq_nullptr>(ptr);
            argsTypes += argsTypes.length() ? ", null" : "null";
            break;
        }

        default:
            php_error(E_ERROR, QString("Unknown type of argument %1").arg(i).toUtf8().constData());
        }
    }

    while(ce->parent != nullptr
          && !objectFactory()->getRegisteredMetaObjects(PQDBG_LVL_C).contains(ce->name->val))
    {
        ce = ce->parent;
    }

    if(objectFactory()->getRegisteredMetaObjects(PQDBG_LVL_C).contains(ce->name->val)) {
        QObject *qo = objectFactory()->createObject(ce->name->val, this_ptr, vargs PQDBG_LVL_CC);
        PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(this_ptr));
        pqobject->isinit = false;

        if(!qo) {
            QString constructors;

            QMetaObject metaObject
                    = objectFactory()->getRegisteredMetaObjects(PQDBG_LVL_C).value(ce->name->val).metaObject;

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
                        ? QString("<br>%1(%2)").arg(ce->name->val).arg(parameterTypes)
                        : QString("%1(%2)").arg(ce->name->val).arg(parameterTypes);
            }

            pq_pre(QString("<b>Fatal error</b>: could not create object %1 with params: (%2)<br><br>"
                           "Available constructors:<br>%3")
                   .arg(ce->name->val)
                   .arg(argsTypes)
                   .arg(constructors), "Warning");

            php_request_shutdown((void *) 0);
           // SG(server_context) = NULL;
            php_module_shutdown();
            sapi_deactivate();
            sapi_shutdown();
            tsrm_shutdown();
        }
    }
    else {
        php_error(E_ERROR, "Can't create object");
    }

    PQDBGLPUP("free args");
    efree(args);
    PQDBG_LVL_DONE_LPUP();
}

void PHPQt5::zim_pqobject___call(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char* method;
    int method_len;
    zval *args;
    int argc = ZEND_NUM_ARGS();

    if (zend_parse_parameters(argc, "sz", &method, &method_len, &args) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

#ifdef PQDEBUG
    PQDBGLPUP(QString("%1->%2").arg(Z_OBJCE_P(getThis())->name->val).arg(method));
#endif

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);

    if(qo != nullptr) {
        /*
         * Вызов метода connect( ... )
         */
        if(method == QString("connect")) {
            PQDBG_LVL_DONE();
            RETURN_BOOL( pq_connect_ex(getThis(), args PQDBG_LVL_CC) )
        }

        /*
         * Вызов метода moveToThread( ... )
         */
        else if(method == QString("moveToThread")) {
            PQDBG_LVL_DONE();
            RETURN_BOOL( pq_move_to_thread(qo, args PQDBG_LVL_CC) )
        }

        /*
         * Вызов иного метода....
         */
        else {
            pq_call_with_return(qo, method, args, INTERNAL_FUNCTION_PARAM_PASSTHRU PQDBG_LVL_CC);
        }
    }
    else {
        PQDBGLPUP("ERROR: NULL POINT OF QOBJECT");
        ZVAL_NULL(return_value);
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_pqobject___callStatic(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    void *TSRMLS_CACHE = tsrm_get_ls_cache();

    char* method;
    int method_len;
    zval *pzval;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &method, &method_len, &pzval) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    zend_class_entry* ce = nullptr;

    if (execute_data->called_scope) {
        ce = execute_data->called_scope;
    } else if (!EG(scope)) {
        ce = EG(scope);
    }

#ifdef PQDEBUG
    PQDBGLPUP(QString("%1::%2").arg(ce->name->val).arg(method));
#endif

    QString className = QString(ce->name->val);

    if(className == "QApplication"
            || className == "QCoreApplication"
            || className == "QGuiApplication") {
        pq_call_with_return(qApp, method, pzval, INTERNAL_FUNCTION_PARAM_PASSTHRU PQDBG_LVL_CC);
    }
    else {
        QMetaObject mo = objectFactory()->getRegisteredMetaObjects(PQDBG_LVL_C).value(QString("%1").arg(ce->name->val)).metaObject;
        QObject *qo = mo.newInstance();

        if(qo) {
            pq_call_with_return(qo,
                                QByteArray(method).prepend(PQ_STATIC_PREFIX).constData(),
                                pzval,
                                INTERNAL_FUNCTION_PARAM_PASSTHRU
                                PQDBG_LVL_CC);
            delete qo;
        }
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_pqobject___set(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char *property;
    int property_len;
    zval *pzval;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &property, &property_len, &pzval) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

#ifdef PQDEBUG
    PQDBGLPUP(QString("%1->%2").arg(Z_OBJCE_P(getThis())->name->val).arg(property));
#endif

    QObject *qo = (QObject*) objectFactory()->getQObject(getThis() PQDBG_LVL_CC);

    if(qo != nullptr) {
        switch(Z_TYPE_P(pzval)) {
        case IS_TRUE: {
            qo->setProperty(property, true);
            //if(!qo->setProperty(property, true)) {
            //    pq_set_user_property(qo, QString(property), true PQDBG_LVL_CC);
            //}
            break;
        }

        case IS_FALSE: {
            qo->setProperty(property, false);
            //if(!qo->setProperty(property, false)) {
            //    pq_set_user_property(qo, QString(property), false PQDBG_LVL_CC);
            //}
            break;
        }

        case IS_STRING: {
            //QByteArray strval = QByteArray::fromRawData(Z_STRVAL_P(pzval), Z_STR_P(pzval)->len);
            QByteArray strval = QByteArray(Z_STRVAL_P(pzval), Z_STR_P(pzval)->len);
            //if(!qo->setProperty(property, QString(toUTF8(strval)))) {
            //    pq_set_user_property(qo, QString(property), strval PQDBG_LVL_CC);
            //}
            qo->setProperty(property, strval);
            //if(!qo->setProperty(property, strval)) {
            //    pq_set_user_property(qo, QString(property), strval PQDBG_LVL_CC);
            //}
            break;
        }

        case IS_LONG: {
            int lval = Z_LVAL_P(pzval);
            qo->setProperty(property, lval);
            //if(!qo->setProperty(property, lval)) {
            //    pq_set_user_property(qo, QString(property), lval PQDBG_LVL_CC);
            //}
            break;
        }

        case IS_DOUBLE: {
            double dval = Z_DVAL_P(pzval);
            qo->setProperty(property, dval);
            //if(!qo->setProperty(property, dval)) {
            //    pq_set_user_property(qo, QString(property), dval PQDBG_LVL_CC);
            //}
            break;
        }

        case IS_NULL:
            qo->setProperty(property, 0);
            //if(!qo->setProperty(property, 0)) {
            //    pq_set_user_property(qo, QString(property), 0 PQDBG_LVL_CC);
            //}
            break;

        case IS_OBJECT: {
            QString qosignal = property;

            if(qosignal.mid(0,2) == "on"
                && pq_create_php_slot(qo, qosignal, pzval PQDBG_LVL_CC)) {
                PQDBG_LVL_DONE();
                RETURN_TRUE;
            }

            if(pq_test_ce(pzval PQDBG_LVL_CC)) {
                QObject *arg_qo = objectFactory()->getQObject(pzval PQDBG_LVL_CC);
                if(arg_qo) {

                    bool before_have_parent = arg_qo->parent() ? true : false;
                    bool this_before_have_parent = qo->parent() ? true : false;

                    //if(qo->setProperty(property, QVariant::fromValue<QObject*>(arg_qo))) {
                    qo->setProperty(property, QVariant::fromValue<QObject*>(arg_qo));
                    bool after_have_parent = arg_qo->parent() ? true : false;
                    bool this_after_have_parent = qo->parent() ? true : false;

                    // не было родителя и появился
                    if(!before_have_parent
                            && after_have_parent) {
                        Z_ADDREF_P(pzval);
                    }
                    // был родитель и не стало
                    else if(before_have_parent
                            && !after_have_parent) {
                        Z_DELREF_P(pzval);
                    }

                    // не было родителя и появился
                    if(!this_before_have_parent
                            && this_after_have_parent) {
                        Z_ADDREF_P(getThis());
                    }
                    // был родитель и не стало
                    else if(this_before_have_parent
                            && !this_after_have_parent) {
                        Z_DELREF_P(getThis());
                    }

                    break;
                    //}

                    /*
                     else {
                        php_error(E_WARNING,
                                  QString("PQEngine: %1::%2: Cannot set property.")
                                  .arg(Z_OBJCE_P(getThis())->name->val)
                                  .arg(property).toUtf8().constData());

                        PQDBG_LVL_DONE();
                        RETURN_FALSE;
                    }
                    */
                }
            }
            else {
                php_error(E_WARNING,
                          QString("PQEngine: %1::%2: Bad type of argument!")
                          .arg(Z_OBJCE_P(getThis())->name->val)
                          .arg(property).toUtf8().constData());

                PQDBG_LVL_DONE();
                RETURN_FALSE;
            }

            break;
        }

        default:
            php_error(E_WARNING,
                      QString("PQEngine: %1::%2: Bad type of argument!")
                      .arg(Z_OBJCE_P(getThis())->name->val)
                      .arg(property).toUtf8().constData());

            PQDBG_LVL_DONE();
            RETURN_FALSE;
        }

        PQDBG_LVL_DONE();
        RETURN_TRUE;
    }
    else {
        php_error(E_WARNING, "PQEngine: __set(): Object not found!");

        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_pqobject___get(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char* property;
    int property_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &property, &property_len) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

#ifdef PQDEBUG
    PQDBGLPUP(QString("%1->%2").arg(Z_OBJCE_P(getThis())->name->val).arg(property));
#endif

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);

    zval ret_pzval;
    ZVAL_NULL(&ret_pzval);

    if(qo != NULL) {
        QVariant retVal;

        int indexOfProperty = qo->metaObject()->indexOfProperty(property);
        if(indexOfProperty >= 0
                || qo->dynamicPropertyNames().contains(property)) {
            retVal = qo->property(property);
        }

        if(retVal.isValid()) {
            ret_pzval = pq_cast_to_zval(retVal, false PQDBG_LVL_CC);
        }
        else {
            PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(getThis()));
            if(pqobject->isinit) {
                php_error(E_NOTICE,
                          QString("Undefined property: %1::%2")
                          .arg(Z_OBJCE_P(getThis())->name->val)
                          .arg(property).toUtf8().constData());
            }
        }
    }

    RETVAL_ZVAL(&ret_pzval, 1, 0);
    PQDBG_LVL_DONE();
}

void PHPQt5::zim_qApp___callStatic(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char* method;
    int method_len;
    zval *pzval;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &method, &method_len, &pzval) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    if(QString(method) == "arguments") {
        pq_return_qvariant(pq_get_arguments(), INTERNAL_FUNCTION_PARAM_PASSTHRU PQDBG_LVL_CC);
    }
    else {
        pq_call_with_return(qApp, method, pzval, INTERNAL_FUNCTION_PARAM_PASSTHRU PQDBG_LVL_CC);
    }

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif
}

void PHPQt5::zim_qevent_ignore(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    int handle = Z_OBJ_HANDLE_P(getThis());
    pq_eventHash.value(handle).e->ignore();

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif
}

void PHPQt5::zim_qevent_accept(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    int handle = Z_OBJ_HANDLE_P(getThis());
    pq_eventHash.value(handle).e->accept();

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif
}

void PHPQt5::zim_pqobject___destruct(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(fetch_pqobject(Z_OBJ_P(getThis()))->qo_sptr.data()))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "")) {
        PQDBG_LVL_DONE();
        return;
    }

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);

    if(qo) {
        objectFactory()->freeObject(qo PQDBG_LVL_CC);
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_pqobject_qobjInfo(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(), PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    zend_bool do_return = 0;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &do_return) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);
    QString out;

    if(qo != nullptr) {
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

            //case QMetaType::Long:
            case QMetaType::Int:
            case QMetaType::LongLong:
                out += QString(" = %1").arg(qo->property(property.name()).toInt());
                break;

            case QMetaType::Double:
            //case QMetaType::Float:
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

    zval retval = pq_cast_to_zval(out, false PQDBG_LVL_CC);

    if(do_return) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        RETURN_ZVAL(&retval, 1, 0);
    }

    php_output_start_default();
    zend_print_zval_r(&retval, 0);
    php_output_get_contents(return_value);

    pq_pre(toUTF8( Z_STRVAL_P(return_value) ), qApp->applicationName());

    php_output_discard();

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif

    RETURN_NULL();
}

void PHPQt5::zim_pqobject_qobjProperties(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(), PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    zend_bool do_return = 0;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &do_return) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);
    QMap<QString,QString> properties;

    if(qo != nullptr) {
        const QMetaObject* metaObject = qo->metaObject();

        for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
            properties.insert(QString(metaObject->property(i).name()),
                           QString(metaObject->property(i).typeName()));
        }
    }

    zval retval;
    array_init(&retval);

    QMapIterator<QString,QString> iter(properties);
    while(iter.hasNext()) {
        iter.next();
        add_assoc_string(&retval,
                         iter.key().toUtf8().constData(),
                         iter.value().toUtf8().data());
    }

    if(do_return) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        RETURN_ZVAL(&retval, 1, 0);
    }

    php_output_start_default();
    zend_print_zval_r(&retval, 0);
    php_output_get_contents(return_value);

    pq_pre(toUTF8( Z_STRVAL_P(return_value) ), qApp->applicationName());

    php_output_discard();

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif

    RETURN_NULL();
}

void PHPQt5::zim_pqobject_qobjMethods(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(), PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    zend_bool do_return = 0;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &do_return) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);
    QMap<QString,QString> methods;

    if(qo != nullptr) {
        const QMetaObject* metaObject = qo->metaObject();

        for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i) {
            QMetaMethod metaMethod = metaObject->method(i);

            if(metaMethod.methodSignature().left(7) == PQ_STATIC_PREFIX // skip static methods
                    || metaMethod.methodSignature().left(4) == "__pq_" // skip PQObject methods
                    || metaMethod.methodSignature().right(6) == "_pslot") // skip *_pslot methods
                continue;

            methods.insert(QString(metaMethod.methodSignature().constData()),
                           QString(metaMethod.typeName()));
        }
    }

    zval retval;
    array_init(&retval);

    QMapIterator<QString,QString> iter(methods);
    while(iter.hasNext()) {
        iter.next();
        add_assoc_string(&retval,
                         iter.key().toUtf8().constData(),
                         iter.value().toUtf8().data());
    }

    if(do_return) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        RETURN_ZVAL(&retval, 1, 0);
    }

    php_output_start_default();
    zend_print_zval_r(&retval, 0);
    php_output_get_contents(return_value);

    pq_pre(toUTF8( Z_STRVAL_P(return_value) ), qApp->applicationName());

    php_output_discard();

    #ifdef PQDEBUG
        PQDBG_LVL_DONE();
    #endif

    RETURN_NULL();
}

void PHPQt5::zim_pqobject_qobjOnSignals(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(), PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    zend_bool do_return = 0;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &do_return) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);
    QStringList onsignals;

    if(qo != nullptr) {
        qo->metaObject()->invokeMethod(qo, "getOnSignals", Q_RETURN_ARG(QStringList, onsignals));
    }

    if(do_return) {
        pq_return_qvariant(onsignals, INTERNAL_FUNCTION_PARAM_PASSTHRU PQDBG_LVL_CC);
        PQDBG_LVL_DONE();
        return;
    }

    zval retval = pq_cast_to_zval(onsignals, false PQDBG_LVL_CC);

    php_output_start_default();
    zend_print_zval_r(&retval, 0);
    php_output_get_contents(return_value);

    pq_pre(toUTF8( Z_STRVAL_P(return_value) ), qApp->applicationName());

    php_output_discard();

    PQDBG_LVL_DONE();
    RETURN_NULL();
}

void PHPQt5::zim_pqobject_free(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(), PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    zim_pqobject___destruct(INTERNAL_FUNCTION_PARAM_PASSTHRU);

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_pqobject_setEventListener(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(),  PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "f", &fci, &fci_cache) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);

    if(qo != nullptr) {
        fci.param_count = 2;

        EventListenerEntry ele = {
            qo,
            fci,
            fci_cache
        };

        eventListeners.insert(qo, ele);

        qo->metaObject()->invokeMethod(qo, "enableEventListener");
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_pqobject___toString(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(), PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);
    QString retStr = "";

    if(qo != NULL) {
        QString pClassName = QString(Z_OBJCE_P(getThis())->name->val);
        QString qClassName = QString(qo->metaObject()->className()).mid(1);
        QString objectName = qo->objectName();

        retStr = QString("%1[%2:%3]").arg(qClassName).arg(pClassName).arg(objectName);
    }

    PQDBG_LVL_DONE();
    RETURN_STRING(toW(retStr.toUtf8()).constData());
}

void PHPQt5::zim_pqobject_children(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(), PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    bool subchilds = true;
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &subchilds) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);

    zval array;
    array_init(&array);

    if(qo != NULL) {
        QObjectList childs = qo->children();

        int index = 0;
        foreach(QObject *child, childs) {
            if(!subchilds) {
                if(child->parent() != qo) {
                    continue;
                }
            }

            zval pzval = objectFactory()->getZObject(child PQDBG_LVL_CC);
            if(Z_TYPE(pzval) == IS_OBJECT) {
                add_next_index_zval(&array, &pzval);
                index++;
            }
        }
    }

    PQDBG_LVL_DONE();
    RETURN_ZVAL(&array, 1, 0);
}


void PHPQt5::zim_pqobject_emit(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(), PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    char *signal_signature;
    int signal_signature_len;
    zval *args;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "sa", &signal_signature, &signal_signature_len, &args) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);

    if(qo != nullptr) {
        bool haveSignalConnection = false;
        QByteArray signalSignature(signal_signature);

        signalSignature.replace(",string",",QString")
                .replace("string,","QString,")
                .replace("(string)","(QString)");

        if(QMetaObject::invokeMethod(qo,
                                     "haveSignalConnection",
                                     Qt::DirectConnection,
                                     Q_RETURN_ARG(bool, haveSignalConnection),
                                     Q_ARG(QByteArray, signalSignature))) {
            if(haveSignalConnection) {
                #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
                PQDBGLPUP(QString("emit: %1").arg(signal_signature));
                #endif
                pq_emit(qo, signalSignature, args);
            }
            #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
            else {
                #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
                PQDBGLPUP(QString("no have connections for %1").arg(signal_signature));
                #endif
            }
            #endif
        }
        #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
        else {
            #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
            PQDBGLPUP(QString("ERROR invokeMethod( haveSignalConnection )"));
            #endif
        }
        #endif
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_pqobject_declareSignal(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(reinterpret_cast<quint64>(objectFactory()->getQObject(getThis(), PQDBG_LVL_PUP(1))))
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    char *signal_name;
    int signal_name_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &signal_name, &signal_name_len) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);

    if(qo != nullptr) {
        pq_declareSignal(qo, QByteArray(signal_name));
    }

    PQDBG_LVL_DONE();
}
