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

#include <typeinfo>
#include <QMetaObject>
#include <QReturnArgument>

#include "pqclasses.h"
#include "phpqt5.h"
#include "pqengine_private.h"

#include <QBuffer>

QHash<QObject*, int> PHPQt5::acceptedPHPSlots_indexes;
QList< QHash<QString, pq_access_function_entry> > PHPQt5::acceptedPHPSlots_list;
QHash<int, pq_event_wrapper> PHPQt5::pq_eventHash;
QStringList PHPQt5::mArguments;

#define PQ_TEST_CLASS(classname) qo_sender->metaObject()->className() == QString(classname)
#define PQ_CREATE_PHP_CONN(classname) phpqt5Connections->createPHPConnection<classname>(zo_sender, qo_sender, signal, z_receiver, slot)

/*
bool pq_PHP_emit(const QString &qosignal, QObject *sender, QEvent *event)
{
    int index = PHPQt5::acceptedPHPSlots_indexes.value(sender, -1);

    if(index >= 0) {
        if(PHPQt5::acceptedPHPSlots_list.at(index).contains(qosignal)) {
            pq_access_function_entry afe = PHPQt5::acceptedPHPSlots_list.at(index).value(qosignal);
            zend_function *copy_fptr = zend_get_closure_invoke_method(Z_OBJ_P(afe.zo));
            zval zevent = PHPQt5::pq_qevent_to_zobject(event);

            PHPQt5::pq_eventHash.insert(Z_OBJ(zevent)->handle, pq_event_wrapper{ &zevent, event } );
            zval zobject = PHPQt5::objectFactory()->getZObject(sender);

            zend_call_method_with_2_params(afe.zo,
                                           Z_OBJCE_P(afe.zo),
                                           &copy_fptr,
                                           ZEND_INVOKE_FUNC_NAME,
                                           NULL,
                                           &zobject,
                                           &zevent);


            zval_ptr_dtor(&zevent);

            return true;
        }
    }

    return false;
}
*/

void pq_invoke_closure(PQClosure closure, const QVariantList &args)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    int argc = args.size();

    zval function_name, retval, z_receiver;
    zval *params = new zval[argc];

#if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("alloc params");
#endif

    QByteArray slotName("__invoke");
    ZVAL_STRING(&function_name, slotName.constData());
    ZVAL_OBJ(&z_receiver, reinterpret_cast<zend_object*>(closure.closure_zo));

    for(int i = 0; i < argc; i++) {
        QVariant arg = args.at(i);
        params[i] = PHPQt5::pq_cast_to_zval(arg, true PQDBG_LVL_CC);
    }

#ifdef PQDEBUG
    PQDBGLPUP(QString("call: %1->%2").arg(Z_OBJCE_NAME(z_receiver)).arg(slotName.constData()));
#endif

    if(call_user_function(NULL, &z_receiver, &function_name, &retval, argc, params) == FAILURE) {
        QString s = QString("PHPQt5 could not call Closure")
                .arg(Z_STRVAL(function_name))
                .arg(Z_OBJCE(z_receiver)->name->val);

        php_error(E_ERROR, s.toUtf8().constData());
    }

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("free params");
    #endif

    for(int i = 0; i < argc; i++) {
        zval_ptr_dtor(&params[i]);
    }

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("free temp vars");
    #endif

    delete params;
    params = nullptr;

    zval_dtor(&retval);
    zval_dtor(&function_name);

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("done");
    #endif

    PQDBG_LVL_DONE();
}

bool pq_send_event(QObject *sender, QEvent *event)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    bool prevent = false;

    if(PHPQt5::eventListeners.contains(sender)) {
        zend_fcall_info fci = PHPQt5::eventListeners.value(sender).fci;
        zend_fcall_info_cache fci_cache = PHPQt5::eventListeners.value(sender).fci_cache;
        zval zobject = PHPQt5::objectFactory()->getZObject(sender PQDBG_LVL_CC);

        zval argv[2];
        zval retval;

        //zval zevent = PHPQt5::pq_qevent_to_zobject(event PQDBG_LVL_CC);
        zval zevent = PHPQt5::pq_qevent_to_zobject_ex(event PQDBG_LVL_CC);


        PHPQt5::pq_eventHash.insert(Z_OBJ_HANDLE(zevent), pq_event_wrapper{ &zevent, event } );

        ZVAL_ZVAL(&argv[0], &zobject, 0, 0);
        ZVAL_ZVAL(&argv[1], &zevent, 0, 0);

        fci.retval = &retval;
        fci.params = argv;

        if(zend_call_function(&fci, &fci_cache) == SUCCESS) {
            if(Z_TYPE(retval) == IS_TRUE) {
                prevent = true;
            }
            else if(Z_TYPE(retval) == IS_LONG
                    && Z_LVAL(retval) != 0) {
                prevent = true;
            }
            else if(Z_TYPE(retval) == IS_STRING
                    && Z_STRLEN(retval) != 0) {
                prevent = true;
            }
        }

        PHPQt5::pq_eventHash.remove(Z_OBJ_HANDLE(zevent));

        zval_ptr_dtor(&argv[1]);
        zval_ptr_dtor(&retval);
    }

    PQDBG_LVL_DONE();
    return prevent;
}

/*
bool pq_callPHPFunction(const QString &fn_name, QObject *sender, QEvent *event)
{
#ifdef PQDEBUG
    PQDBG2("pq_callPHPFunction()", fn_name);
#endif

    if(!PHPQt5::acceptedPHPFunctions.contains(fn_name)) {
        //QMessageBox::warning(0, "callPHPFunction", QString("callPHPFunction: cannot call function %1($event)").arg(fn_name));
        return false;
    }

    pq_access_function_entry afe = PHPQt5::acceptedPHPFunctions.value(fn_name);

    zval args[2];
    zval retzval;
    zval pqzobj = PHPQt5::objectFactory()->getZObject(sender);
    zval zevent = PHPQt5::pq_qevent_to_zobject(event);

    int handle = Z_OBJ(zevent)->handle;
    bool retval = false;

    ZVAL_ZVAL(&args[0], &pqzobj, 1, 0);
    ZVAL_ZVAL(&args[1], &zevent, 1, 0);

    PHPQt5::pq_eventHash.insert(handle, pq_event_wrapper{&zevent, event});
    call_user_function(EG(function_table), afe.zo, afe.zfn_name, &retzval, 2, args);
    PHPQt5::pq_eventHash.remove(handle);

    if(Z_TYPE(retzval) == IS_TRUE || Z_TYPE(retzval) == IS_FALSE) {
        retval = Z_TYPE(retzval) == IS_TRUE ? true : false;
    }

    zval_ptr_dtor(&args[0]);
    zval_ptr_dtor(&args[1]);
    zval_ptr_dtor(&zevent);
    zval_ptr_dtor(&retzval);

    return retval;
}
*/

QObject *pq_createObject(const QString &className, const QVariantList &args)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    QObject *qo = Q_NULLPTR;
    zend_class_entry *ce = PHPQt5::objectFactory()->getClassEntry(QString(className).prepend("P") PQDBG_LVL_CC);

    if(ce != Q_NULLPTR) {
        zval zobject;
        object_init_ex(&zobject, ce);

        qo = PHPQt5::objectFactory()->createObject(className, &zobject, args PQDBG_LVL_CC);
        PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ(zobject));
        pqobject->isinit = false;

        if(qo == Q_NULLPTR) {
            QString constructors;

            QMetaObject metaObject
                    = PHPQt5::objectFactory()->getRegisteredMetaObjects(PQDBG_LVL_C).value(className).metaObject;

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

            QString argsTypes;
            foreach(QVariant arg, args) {
                argsTypes += argsTypes.length()
                        ? QString(", %1").arg(arg.typeName())
                        : QString(arg.typeName());
            }

            php_error(E_ERROR,
                      QString("could not create object %1 with params: (%2)<br><br>"
                              "Available constructors:<br>%3")
                      .arg(className)
                      .arg(argsTypes)
                      .arg(constructors).toUtf8().constData());

            php_request_shutdown((void *) 0);
            php_module_shutdown();
            sapi_deactivate();
            sapi_shutdown();
            tsrm_shutdown();
        }
    }
    else {
        php_error(E_ERROR,
                  QString("class '%1' not found")
                  .arg(className).toUtf8().constData());
    }

    return qo;
}

bool pq_registerQObject(QObject *qo)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    QString className = qo->metaObject()->className();

    zend_class_entry *ce_ptr = PHPQt5::objectFactory()->getClassEntry(className PQDBG_LVL_CC);

    if(ce_ptr) {
        zval zobj_ptr;
        object_init_ex(&zobj_ptr, ce_ptr);

        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return PHPQt5::objectFactory()->registerObject(&zobj_ptr, qo PQDBG_LVL_CC);
    }

    PQDBG_LVL_DONE();
    return false;
}

size_t pq_stream_reader(void *dataStreamPtr, char *buffer, size_t wantlen)
{
    QDataStream *dataStream = (QDataStream*) dataStreamPtr;
    return dataStream->device()->read(buffer, wantlen);
}

void pq_stream_closer(void *dataStreamPtr)
{
    QDataStream *dataStream = (QDataStream*) dataStreamPtr;
    dataStream->device()->close();
}

size_t pq_stream_fsizer(void *dataStreamPtr)
{
    QDataStream *dataStream = (QDataStream*) dataStreamPtr;
    return dataStream->device()->size();
}


void (*ub_write_fn_ptr)(const QString &msg) = 0;
void pq_ub_write(const QString &msg)
{
    if(!ub_write_fn_ptr) {
        foreach (IPQExtension *pqext, PQEnginePrivate::m_extensions) {
            if(pqext->entry().use_ub_write) {
                ub_write_fn_ptr = pqext->entry().ub_write;
                break;
            }
        }
    }

    if(ub_write_fn_ptr) {
        ub_write_fn_ptr( msg );
    }
}

void (*pre_fn_ptr)(const QString &msg, const QString &title) = 0;
void pq_pre(const QString &msg, const QString &title)
{
    if(!pre_fn_ptr) {
        foreach(IPQExtension *pqext, PQEnginePrivate::m_extensions) {
            if(pqext->entry().use_pre) {
                pre_fn_ptr = pqext->entry().pre;
                break;
            }
        }
    }

    if(pre_fn_ptr) {
        pre_fn_ptr(msg, title);
    }
}

bool do_register_long_constant(const QString &className, const QString &constName, int value, QString ceName)
{
#ifdef PQDEBUG
    //PQDBG_LVL_START(__FUNCTION__);
    //PQDBGLPUP(QString("%1::%2 (%3)").arg(className).arg(constName).arg(value));
    if(ceName == "") ceName = QString("P%1").arg(className);

    if(zend_class_entry *ce_ptr = PHPQt5::objectFactory()->getClassEntry(ceName, 0)) {
        if(zend_declare_class_constant_long(ce_ptr,
                                            constName.toUtf8().constData(),
                                            constName.length(),
                                            value) == SUCCESS)
        {
            return true;
        }
    }
#else
    if(ceName == "") ceName = QString("P%1").arg(className);

    if(zend_class_entry *ce_ptr = PHPQt5::objectFactory()->getClassEntry(ceName)) {
        if(zend_declare_class_constant_long(ce_ptr,
                                            constName.toUtf8().constData(),
                                            constName.length(),
                                            value) == SUCCESS)
        {
            return true;
        }
    }
#endif

    return true;

    //PQDBG_LVL_DONE();
}

void do_register_string_constant(const QString &className, const QString &constName, const QString &value, QString ceName)
{
#ifdef PQDEBUG
    //PQDBG_LVL_START(__FUNCTION__);
    //PQDBGLPUP(QString("%1::%2 (%3)").arg(className).arg(constName).arg(value));
    if(ceName == "") ceName = QString("P%1").arg(className);

    if(zend_class_entry *ce_ptr = PHPQt5::objectFactory()->getClassEntry(ceName, 0)) {
        zend_declare_class_constant_stringl(ce_ptr,
                                            constName.toUtf8().constData(),
                                            constName.length(),
                                            value.toUtf8().constData(),
                                            value.length());
    }
#else
    if(ceName == "") ceName = QString("P%1").arg(className);

    if(zend_class_entry *ce_ptr = PHPQt5::objectFactory()->getClassEntry(ceName)) {
        zend_declare_class_constant_stringl(ce_ptr,
                                            constName.toUtf8().constData(),
                                            constName.length(),
                                            value.toUtf8().constData(),
                                            value.length());
    }
#endif

    //PQDBG_LVL_DONE();
}

void pq_register_long_constant(const QString &className, const QString &constName, int value)
{
    do_register_long_constant(className, constName, value, "");
}

bool pq_register_long_constant_ex(const QString &className, const QString &constName, int value, QString ceName)
{
    return do_register_long_constant(className, constName, value, ceName);
}

void pq_register_string_constant(const QString &className, const QString &constName, const QString &value)
{
    do_register_string_constant(className, constName, value, "");
}

void pq_register_string_constant_ex(const QString &className, const QString &constName, const QString &value, QString ceName)
{
    do_register_string_constant(className, constName, value, ceName);
}

void pq_register_extra_zend_ce(const QString &className)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("className: %1").arg(className));
#endif

    zend_class_entry ce, *ce_ptr;

    INIT_CLASS_ENTRY_EX(ce, className.toUtf8().constData(), className.length(), PHPQt5::phpqt5_no_methods());
    ce_ptr = zend_register_internal_class(&ce);

    PHPQt5::objectFactory()->registerZendClassEntry(className, ce_ptr PQDBG_LVL_CC);

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif
}

void pq_register_qevent_zend_ce(const QString &className)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("className: %1").arg(className));
#endif

    zend_class_entry ce, *ce_ptr;

    INIT_CLASS_ENTRY_EX(ce, className.toUtf8().constData(), className.length(), PHPQt5::phpqt5_qevent_methods());
    ce_ptr = zend_register_internal_class(&ce);

    PHPQt5::objectFactory()->registerZendClassEntry(className, ce_ptr PQDBG_LVL_CC);

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif
}

#define PQ_REGISTER_ZEND_CE(zcname) \
    zend_class_entry zcname##_ce;\
    zend_class_entry *zcname##_ce_ptr;\
    QString zcname##_str = #zcname;\
    INIT_CLASS_ENTRY_EX(zcname##_ce, zcname##_str.toUtf8().constData(), strlen(#zcname), phpqt5_qevent_methods());\
    zcname##_ce_ptr = zend_register_internal_class(&zcname##_ce);\
    objectFactory()->registerZendClassEntry(zcname##_str, zcname##_ce_ptr);

void PHPQt5::pq_prepare_args(int argc,
                             char** argv
                             PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("argc: %1").arg(argc));
#endif

    void *TSRMLS_CACHE = NULL;
    ZEND_TSRMLS_CACHE_UPDATE();

    zval zarr, zargc, ztmp;
    array_init(&zarr);

    for(int i = 0; i < argc; i++) {
        mArguments.append(QString(argv[i]));

        ZVAL_STRING(&ztmp, QByteArray(argv[i]).constData());
        zend_hash_next_index_insert(Z_ARRVAL(zarr), &ztmp);
        zend_string_free(Z_STR(ztmp));
    }

    ZVAL_LONG(&zargc, argc);
    Z_ADDREF(zarr);

    zend_hash_str_update(&EG(symbol_table), "argv", 4, &zarr);
    zend_hash_str_add(&EG(symbol_table), "argc", 4, &zargc);

    PQDBG_LVL_DONE();
}

QStringList PHPQt5::pq_get_arguments()
{
    return mArguments;
}

PQAPI void PHPQt5::pq_core_init(PQDBG_LVL_D)
{   
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    {
        QByteArray className = "qApp";

        zend_class_entry qApp_ce;
        INIT_CLASS_ENTRY_EX(qApp_ce, className.constData(), className.size(), phpqt5_qApp_methods());
        zend_register_internal_class(&qApp_ce);
    }

    {
        //QByteArray className = "QObjectList";

       // zend_class_entry qobjectlist_ce;
       // INIT_CLASS_ENTRY_EX(qobjectlist_ce, className.constData(), className.size(), phpqt5_qobjectlist_methods());
       // zend_class_entry *qobjectlist_ce_ptr = zend_register_internal_class(&qobjectlist_ce);

        //objectFactory()->registerZendClassEntry("PQObjectList", qobjectlist_ce_ptr);
    }

    /*
    PQ_REGISTER_ZEND_CE(QEvent);
    PQ_REGISTER_ZEND_CE(QMouseEvent);
    PQ_REGISTER_ZEND_CE(QKeyEvent);
    PQ_REGISTER_ZEND_CE(QResizeEvent);
    PQ_REGISTER_ZEND_CE(QMoveEvent);
    PQ_REGISTER_ZEND_CE(QHoverEvent);
    PQ_REGISTER_ZEND_CE(QFocusEvent);
    PQ_REGISTER_ZEND_CE(QEnterEvent);
    PQ_REGISTER_ZEND_CE(QContextMenuEvent);
    PQ_REGISTER_ZEND_CE(QTimerEvent);
    */

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif
}



PQAPI void PHPQt5::pq_register_class(const QMetaObject &metaObject
                                     PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(metaObject.className());
#endif

    QString className = objectFactory()->registerMetaObject(metaObject PQDBG_LVL_CC);

    QByteArray ba = className.toUtf8();
    const char* class_name = ba.constData();
    int class_name_len = className.length();

    zend_class_entry ce;

    INIT_CLASS_ENTRY_EX(ce, class_name, class_name_len, phpqt5_generic_methods());

    ce.create_object = pqobject_create;
    zend_class_entry *ce_ptr = zend_register_internal_class(&ce);

    zend_declare_property_long(ce_ptr, "uid", sizeof("uid")-1, 0, ZEND_ACC_PPP_MASK | ZEND_ACC_FINAL);
    zend_declare_property_long(ce_ptr, "zhandle", sizeof("zhandle")-1, 0, ZEND_ACC_PPP_MASK | ZEND_ACC_FINAL);

    objectFactory()->registerZendClassEntry(metaObject.className(), ce_ptr PQDBG_LVL_CC);

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif
}

zend_object *PHPQt5::pq_register_extra_qobject(QObject *qo
                                               PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(qo->metaObject()->className());
#endif

    zval zobject;
    zend_class_entry *ce = objectFactory()->getClassEntry(QString(qo->metaObject()->className()).mid(1) PQDBG_LVL_CC);

    if(!ce) {
        ce = objectFactory()->getClassEntry(qo->metaObject()->className() PQDBG_LVL_CC);
    }

    if(ce) {
        object_init_ex(&zobject, ce);

        objectFactory()->registerObject(&zobject, qo PQDBG_LVL_CC);

        zval uid;
        ZVAL_LONG(&uid, reinterpret_cast<quint64>(qo));

        zval zhandle;
        ZVAL_LONG(&zhandle, Z_OBJ_HANDLE(zobject));

        zend_update_property_long(ce, &zobject, "uid", sizeof("uid")-1, reinterpret_cast<quint64>(qo));
        zend_update_property_long(ce, &zobject, "zhandle", sizeof("zhandle")-1, Z_OBJ_HANDLE(zobject));

        Z_DELREF(zobject);

        PQDBGLPUP(QString("refcount: %1").arg(Z_REFCOUNT(zobject)));

        PQDBG_LVL_DONE();
        return Z_OBJ(zobject);
    }

    PQDBG_LVL_DONE();
    return nullptr;
}

PQAPI bool PHPQt5::pq_test_ce(zval *pzval PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(Z_OBJCE_NAME_P(pzval));
#endif

    if(Z_TYPE_P(pzval) == IS_OBJECT) {
        zend_class_entry *ce = Z_OBJCE_P(pzval);

        while (!objectFactory()->getRegisteredMetaObjects(PQDBG_LVL_C).contains(ce->name->val)
               && ce->parent != NULL)
        {
            ce = ce->parent;
        }

        PQDBG_LVL_DONE();
        return objectFactory()->getRegisteredMetaObjects(PQDBG_LVL_C).contains(ce->name->val);
    }

    PQDBG_LVL_DONE();
    return false;
}

int PHPQt5::pq_call_with_return(QObject *qo,
                                 const char *method,
                                 zval *pzval,
                                 INTERNAL_FUNCTION_PARAMETERS
                                 PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("%1->%2").arg(qo->metaObject()->className()).arg(method));
#endif

    QVariant retVal = pq_call(qo, method, pzval PQDBG_LVL_CC);
    pq_return_qvariant(retVal, INTERNAL_FUNCTION_PARAM_PASSTHRU PQDBG_LVL_CC);

    PQDBG_LVL_DONE();

    return SUCCESS;
}

QVariant PHPQt5::pq_call(QObject *qo,
                         const char *method,
                         zval *pzval
                         PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("%1->%2").arg(qo->metaObject()->className()).arg(method));
#endif

    /*
     * Парсим параметры и записываем в QVariantList values
     */
    zval *entry;
    ulong num_key;
    zend_string *key;

    QVariantList args;

    QList<pq_call_qo_entry> arg_qos;

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(pzval), num_key, key, entry) {
        switch(Z_TYPE_P(entry)) {
        case IS_TRUE:
            args << QVariant( true );
            break;

        case IS_FALSE:
            args << QVariant( false );
            break;

        case IS_STRING: {
            // определение типа данных
            QString str = QString::fromLatin1(Z_STRVAL_P(entry));

            if(str.length() == Z_STR_P(entry)->len)
                args << QString::fromUtf8(Z_STRVAL_P(entry)); // возврат к utf8
            else
                args << QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
            break;
        }

        case IS_LONG:
            args << QVariant( (int) Z_LVAL_P(entry) );
            break;

        case IS_DOUBLE:
            args << QVariant( Z_DVAL_P(entry) );
            break;

        case IS_OBJECT: {
            if(pq_test_ce(entry PQDBG_LVL_CC)) {
                QObject *arg_qo = objectFactory()->getQObject(entry PQDBG_LVL_CC);
                args << QVariant::fromValue<QObject*>( arg_qo );

                bool before_have_parent = arg_qo->parent() ? true : false;
                arg_qos << pq_call_qo_entry { arg_qo, entry, before_have_parent };
            }
            else {
                zend_function *closure = zend_get_closure_invoke_method(Z_OBJ_P(entry));

                Z_ADDREF_P(entry);
                if(closure) {
                    PQClosure pqc = { reinterpret_cast<void*>( Z_OBJ_P(entry) ) };
                    args << QVariant::fromValue<PQClosure>( pqc );
                }
                else php_error(E_ERROR, QString("Unknown type of argument %1").arg(num_key).toUtf8().constData());
            }
            //else php_error(E_ERROR, QString("Unknown type of argument %1").arg(num_key).toUtf8().constData());

            break;
        }

        case IS_ARRAY:
            args << pq_ht_to_qstringlist(entry PQDBG_LVL_CC);
            break;

        case IS_NULL: {
            pq_nullptr nptr;
            args << QVariant::fromValue<pq_nullptr>(nptr);
            break;
        }

        default:
            php_error(E_ERROR, QString("Unknown type of argument %1").arg(num_key).toUtf8().constData());
        }

    } ZEND_HASH_FOREACH_END();

    QVariant retVal = pq_call(qo, method, args PQDBG_LVL_CC);

    foreach(pq_call_qo_entry cqe, arg_qos) {
        bool after_have_parent = cqe.qo->parent();
        bool before_have_parent = cqe.before_have_parent;

        // не было родителя и появился
        if(!before_have_parent && after_have_parent) {
            Z_ADDREF_P(pzval);
        }
        // был родитель и не стало
        else if(before_have_parent && !after_have_parent) {
            Z_DELREF_P(pzval);
        }
    }

    Q_UNUSED(key);

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif

    return retVal;
}

QVariant PHPQt5::pq_call(QObject *qo,
                         const char *method,
                         QVariantList args
                         PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("%1->%2").arg(qo->metaObject()->className()).arg(method));
#endif

    QVariant retVal;

    /*
     * Пытаемся найти метод
     */
    QMetaMethod qmm;
    QByteArray static_method = QByteArray(method).prepend(PQ_STATIC_PREFIX);

    int parameterCount = args.size();

    bool ok = false;

    // перебираем все доступные методы
    for(int i = qo->metaObject()->methodOffset(); i < qo->metaObject()->methodCount(); ++i) {
        qmm = qo->metaObject()->method(i);

        // ищем совпадение названия метода
        if(qmm.name() == method || qmm.name() == static_method) {

            // ищем совпадение количества параметров
            if(qmm.parameterCount() == parameterCount)
            {
                // останавливаем цикл только в случае успеха
                // обеспечивает возможность перебора перегруженных методов
                if(objectFactory()->call(qo, qmm, args, &retVal, nullptr PQDBG_LVL_CC)) {
                    ok = true;
                    break;
                }
            }
        }
    }

    if(!ok) {
        QString qt_class_name = qo->metaObject()->className();
        qt_class_name = qt_class_name.mid(1);

        QString types = "";
        foreach(QVariant arg, args) {
            if(types.length())
                types.append(",");

            types.append(arg.typeName());
        }

        php_error(E_ERROR,
                  QString("Call to undefined method %1::%2(%3)")
                  .arg(qt_class_name)
                  .arg(method)
                  .arg(types)
                  .toUtf8().constData());
    }

    PQDBG_LVL_DONE();
    return retVal;
}

zval PHPQt5::pq_cast_to_zval(const QVariant &value,
                             bool addref
                             PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("QVariant(%1) [Type::%2]")
              .arg(value.typeName())
              .arg(value.type()));
#endif

    zval ret_zval;

    if(value.isValid()) {
        QVariant r = value; // make copy

        while(r.type() == QMetaType::QVariant) {
            PQDBGLPUP("extract QVariant");
            r = r.value<QVariant>(); // re-qvariant :-)

            PQDBGLPUP(QString("new QVariant(%1) [Type::%2]")
                      .arg(r.typeName())
                      .arg(r.type()));
        }

        switch (r.type()) {
        case QMetaType::QByteArray: {
            QByteArray ba = r.toByteArray();
            zend_string *str = zend_string_init(ba.constData(), ba.length(), 0);
            //ZVAL_STRINGL(&ret_zval, ba.constData(), ba.length());
            ZVAL_STR(&ret_zval, str);
            break;
        }

        case QMetaType::QString: {
            QByteArray ba = r.toString().toUtf8();
            ZVAL_STRINGL(&ret_zval, ba.constData(), ba.length());
            break;
        }

        case QMetaType::Int:
            ZVAL_LONG(&ret_zval, r.toInt() );
            break;

        case QMetaType::LongLong:
            ZVAL_DOUBLE(&ret_zval, r.toLongLong() );
            break;

        case QMetaType::Double:
            ZVAL_DOUBLE(&ret_zval, r.toDouble() );
            break;

        case QMetaType::Bool:
            ZVAL_BOOL(&ret_zval, r.toBool() );
            break;

        case QMetaType::QStringList:
            ret_zval = pq_qstringlist_to_ht(r.toStringList() PQDBG_LVL_CC);
            Z_DELREF(ret_zval);
            break;

        case QMetaType::QPoint:
            ret_zval = pq_qpoint_to_ht(r.toPoint() PQDBG_LVL_CC);
            Z_DELREF(ret_zval);
            break;

        case QMetaType::QPointF:
            ret_zval = pq_qpointf_to_ht(r.toPointF() PQDBG_LVL_CC);
            Z_DELREF(ret_zval);
            break;

        case QMetaType::QRect:
            ret_zval = pq_qrect_to_ht(r.toRect() PQDBG_LVL_CC);
            Z_DELREF(ret_zval);
            break;

        case QMetaType::QObjectStar: {
            QObject *qo = qvariant_cast<QObject*>(r);
            zval test_zval = objectFactory()->getZObject(qo PQDBG_LVL_CC);

            if(Z_TYPE(test_zval) == IS_OBJECT) {
                ZVAL_ZVAL(&ret_zval, &test_zval, 1, 0);
            }
            else {
                zend_object *zobject = pq_register_extra_qobject(qo PQDBG_LVL_CC);

                if(zobject) {
                    ZVAL_OBJ(&ret_zval, zobject);
                }
                else {
                    php_error(E_WARNING, "Object could not be found or was destroyed!\n");
                    ZVAL_NULL(&ret_zval);
                }
            }

            break;
        }

        case QMetaType::User: {
            PQDBGLPUP( QString("QMetaType::User: %1").arg(r.typeName()) );

            if(r.typeName() == QString("zval")) {
                ZVAL_ZVAL(&ret_zval, &(r.value<zval>()), 1, 0);
            }
            else if(r.typeName() == QString("zval*")) {
                ZVAL_ZVAL(&ret_zval, r.value<zval*>(), 1, 0);
            }
            else if(r.typeName() == QString("QObjectList")) {
                zval zobjectList = pq_qobjectlist_to_ht(r.value<QObjectList>() PQDBG_LVL_CC);
                ZVAL_ZVAL(&ret_zval, &zobjectList, 1, 0);
            }
            else if(r.canConvert(QMetaType::QObjectStar)) {
                QObject *qo = qvariant_cast<QObject*>(r);
                zval test_zval = objectFactory()->getZObject(qo PQDBG_LVL_CC);

                if(Z_TYPE(test_zval) == IS_OBJECT) {
                    ZVAL_ZVAL(&ret_zval, &test_zval, 1, 0);
                }
                else {
                    zend_object *zobject = pq_register_extra_qobject(qo PQDBG_LVL_CC);

                    if(zobject) {
                        ZVAL_OBJ(&ret_zval, zobject);
                        if(addref) Z_ADDREF(ret_zval);
                    }
                    else {
                        php_error(E_WARNING, "Object could not be found or was destroyed!\n");
                        ZVAL_NULL(&ret_zval);
                    }
                }
            }
            else {
                #ifdef PQDEBUG
                    PQDBGLPUP("INVALID QVARIANT VALUE");
                #endif
                    ZVAL_UNDEF(&ret_zval);
            }

            break;
        }

        default:
            ZVAL_NULL(&ret_zval);
        }
    }
    else {
        if(!value.isNull()) {
            #ifdef PQDEBUG
                PQDBGLPUP("INVALID QVARIANT VALUE");
            #endif
                ZVAL_UNDEF(&ret_zval);
        }
        else {
            ZVAL_NULL(&ret_zval);
        }
    }

    PQDBG_LVL_DONE();
    return ret_zval;
}

void PHPQt5::pq_return_qvariant(const QVariant &retVal,
                                INTERNAL_FUNCTION_PARAMETERS
                                PQDBG_LVL_DC)
{
    #ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("QVariant(%1)").arg(retVal.typeName()).toUtf8().constData());
    #endif

    zval ret_pzval = pq_cast_to_zval(retVal, false PQDBG_LVL_CC);

    PQDBG_LVL_DONE();
    RETVAL_ZVAL(&ret_pzval, 1, 0);
}

bool PHPQt5::pq_set_parent(QObject *qo, zval *zobject_parent PQDBG_LVL_DC)
{
    #ifdef PQDEBUG
        PQDBG_LVL_PROCEED(__FUNCTION__);
    #endif

   // HashPosition pos;
   // zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(pzval), &pos);

   // zval *ppzval_object = zend_hash_get_current_data_ex(Z_ARRVAL_P(pzval), &pos);

    if(Z_TYPE_P(zobject_parent) == IS_OBJECT) {
        zend_class_entry *parent_ce = Z_OBJCE_P(zobject_parent);

        while(!objectFactory()->getRegisteredMetaObjects(PQDBG_LVL_C).contains(parent_ce->name->val)
              && parent_ce->parent != NULL)
        {
            parent_ce = parent_ce->parent;
        }

        if(objectFactory()->getRegisteredMetaObjects(PQDBG_LVL_C).contains(parent_ce->name->val)) {
            QObject *parent_qo = objectFactory()->getQObject(zobject_parent PQDBG_LVL_CC);

            if(parent_qo != nullptr) {
                qo->setParent(parent_qo);
                PQDBGLPUP("Set new parent");
                PQDBG_LVL_DONE();
                return true;
            }
            else {
                PQDBGLPUP("Unset parent");
                qo->setParent(Q_NULLPTR);
            }
        }
    }
    else if((Z_TYPE_P(zobject_parent) == IS_LONG && Z_LVAL_P(zobject_parent) == 0)
            || Z_TYPE_P(zobject_parent) == IS_NULL) {
        qo->setParent(Q_NULLPTR);
        PQDBGLPUP("Unset parent");
        PQDBG_LVL_DONE();
        return true;
    }

    php_error(E_WARNING, "Unknown argument 1, cannot prepare method call setParent(QObject *parent)\n");

    PQDBG_LVL_DONE();
    return false;
}

bool PHPQt5::pq_connect_ex(zval *zobj_ptr,
                           zval *pzval
                           PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    if(Z_TYPE_P(pzval) != IS_ARRAY) {
        php_error(E_WARNING, "Wrong zval type for method `pq_connect_ex(zval *, zval *)`. Expected type: ARRAY\n");

        PQDBG_LVL_DONE();
        return false;
    }

    zval *entry = nullptr,
            *z_sender = nullptr,
            *z_receiver = nullptr,
            *z_signal = nullptr,
            *z_slot = nullptr;

    HashTable *ht = Z_ARRVAL_P(pzval);
    zend_string *key;
    ulong num_key;

    int argc = ht->nNumOfElements;

    if(argc == 3) {
        z_sender = zobj_ptr;
    }

    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, key, entry) {
        int index = 4 - argc + num_key;

        switch(index) {
        case 0:
            z_sender = entry;
            break;

        case 1:
            z_signal = entry;
            break;

        case 2:
            z_receiver = entry;
            break;

        case 3:
            z_slot = entry;
            break;
        }

    } ZEND_HASH_FOREACH_END();

    Q_UNUSED(key);

    PQDBG_LVL_DONE();
    return pq_connect(z_sender, z_signal, z_receiver, z_slot, false PQDBG_LVL_CC);
}

#define QO_SIGNAL(className, signal) className::signal

bool PHPQt5::pq_connect(zval *z_sender,
                        zval *z_signal,
                        zval *z_receiver,
                        zval *z_slot,
                        bool cDisconnect
                        PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    const QObject *qo_sender;
    const QObject *qo_receiver;
    const char *signal;
    const char *slot;

    /*
     * Первый параметр должен быть объектом
     */
    if(Z_TYPE_P(z_sender) != IS_OBJECT
            || !pq_test_ce(z_sender PQDBG_LVL_CC)) {
        php_error(E_WARNING, "Unknown argument 1, cannot prepare method call connect(QObject *sender, SIGNAL(), QObject *receiver, SLOT())\n");

        PQDBG_LVL_DONE();
        return false;
    }
    qo_sender = objectFactory()->getQObject(z_sender PQDBG_LVL_CC);


    /*
     * Второй параметр должен быть строкой
     */
    if(Z_TYPE_P(z_signal) != IS_STRING) {
        php_error(E_WARNING, "Unknown argument 2, cannot prepare method call connect(QObject *sender, SIGNAL(), QObject *receiver, SLOT())\n");

        PQDBG_LVL_DONE();
        return false;
    }

    QByteArray signal_ba = QMetaObject::normalizedSignature( Z_STRVAL_P(z_signal) );
    signal = signal_ba.constData();

    /*
     * Третий параметр должен быть объектом...
     */
    if(Z_TYPE_P(z_receiver) == IS_OBJECT
            && pq_test_ce(z_receiver PQDBG_LVL_CC)) {

        qo_receiver = objectFactory()->getQObject(z_receiver PQDBG_LVL_CC);
    }
    /*
     * или строкой qApp
     */
    else if(Z_TYPE_P(z_receiver) == IS_STRING
            && Z_STRVAL_P(z_receiver) == QString("qApp")) {
        qo_receiver = qApp;
        zval z_receiver_tmp = objectFactory()->getZObject(qApp PQDBG_LVL_CC);
        z_receiver = &z_receiver_tmp;
    }
    else {
        php_error(E_WARNING, "Unknown argument 3, cannot prepare method call connect(QObject *sender, SIGNAL(), QObject *receiver, SLOT())\n");

        PQDBG_LVL_DONE();
        return false;
    }

    /*
     * Четвёртый параметр должен быть строкой
     */
    if(Z_TYPE_P(z_slot) != IS_STRING) {
        php_error(E_WARNING, "Unknown argument 4, cannot prepare method call connect(QObject *sender, SIGNAL(), QObject *receiver, SLOT())\n");

        PQDBG_LVL_DONE();
        return false;
    }
    QByteArray slot_ba = QMetaObject::normalizedSignature( Z_STRVAL_P(z_slot) );
    slot = slot_ba.constData();

    if(qo_sender != nullptr && qo_receiver != nullptr) {
        // баг из-за которого объект, полученный функцией c() "теряется":
        // берем ссылку на объект из основного хранилища.
        // zval zo_sender = objectFactory()->getZObject(z_sender PQDBG_LVL_CC);

        if(cDisconnect) {
            qo_sender->disconnect(qo_sender, signal, qo_receiver, slot);
            phpqt5Connections->removeConnection(qo_sender,
                                                signal,
                                                slot
                                                PQDBG_LVL_CC);

            PQDBG_LVL_DONE();
            return true;
        }

        int indexOfSlot = qo_sender->metaObject()->indexOfMethod(slot_ba.mid(1));

        if(indexOfSlot < 0 || !qo_sender->connect(qo_sender, signal, qo_receiver, slot)) {
            bool ok = phpqt5Connections->createConnection(z_sender,
                                                          qo_sender,
                                                          signal_ba,
                                                          z_receiver,
                                                          qo_receiver,
                                                          slot_ba
                                                          PQDBG_LVL_CC);

            if(!ok) {
                php_error(E_WARNING, QString("Cannot connect SIGNAL `%1` with SLOT `%2`\n").arg(signal).arg(slot).toUtf8().constData());
            }

            #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
            PQDBGLPUP("done");
            #endif

            PQDBG_LVL_DONE();
            return ok;
        }
        else {
            PQDBG_LVL_DONE();
            return true;
        }
    }
    else {
        PQDBG_LVL_DONE();
        return false;
    }
}

bool PHPQt5::pq_move_to_thread(QObject *qo,
                               zval *zobject_thread
                               PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    if(Z_TYPE_P(zobject_thread) == IS_OBJECT) {
        zend_class_entry *ce_thread = Z_OBJCE_P(zobject_thread);
        QString qt_class_name = QString(ce_thread->name->val);

        if(qt_class_name != "QThread") {
            php_error(E_WARNING, "Unknown argument 1, cannot prepare method call moveToThread(QThread *thread)\n");

            #ifdef PQDEBUG
                PQDBG_LVL_DONE();
            #endif

            return false;
        }
    }
    else {
        php_error(E_WARNING, "Unknown argument 1, cannot prepare method call moveToThread(QThread *thread)\n");

        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return false;
    }

    QObject *thread_qo = objectFactory()->getQObject(zobject_thread PQDBG_LVL_CC);
    if(thread_qo != nullptr)
    {
        FETCH_PQTHREAD();

        QThread *new_th = qobject_cast<QThread*>(thread_qo);
        QVariant vpqzhandle = qo->property(PQZHANDLE);

        qo->moveToThread(new_th);
        objectFactory()->moveObjectToThread(vpqzhandle.toInt(),
                                            PQTHREAD,
                                            new_th
                                            PQDBG_LVL_CC);

        PQDBG_LVL_DONE();
        return true;
    }

    PQDBG_LVL_DONE();
    return false;
}

zval PHPQt5::pq_get_child_objects(QObject *qo,
                                  bool subchilds
                                  PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    zval array;
    array_init(&array);

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
            add_index_zval(&array, index, &pzval);
            index++;
        }
    }

    PQDBG_LVL_DONE();
    return array;
}


void PHPQt5::pq_set_user_property(QObject *qo,
                                  const QString &property,
                                  const QVariant &value
                                  PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("%1->%2")
              .arg(qo->metaObject()->className())
              .arg(property));
#endif

    if(!qo->metaObject()->invokeMethod(qo, "setUserProperty",
                                       Q_ARG(QString, property),
                                       Q_ARG(QVariant, value))) {
        php_error(E_WARNING,
                  QString("Can't set object property `%1`")
                  .arg(property).toUtf8().constData());
    }
}

bool PHPQt5::pq_create_php_slot(QObject *qo_sender,
                                const QString &qosignal,
                                zval *z_receiver
                                PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("slot name: %1").arg(qosignal));
#endif

    QByteArray qSignalSignature_ba;

    qo_sender->metaObject()->invokeMethod(qo_sender, "getOnSignalSignature",
                                   Q_RETURN_ARG(QByteArray, qSignalSignature_ba),
                                   Q_ARG(QByteArray, qosignal.toUtf8()));

    if(qSignalSignature_ba.length() > 2 && Z_OBJ_HANDLER_P(z_receiver, get_closure)) {
        zend_function *closure = zend_get_closure_invoke_method(Z_OBJ_P(z_receiver));

        if(closure) {
            qSignalSignature_ba = QMetaObject::normalizedSignature(qSignalSignature_ba.constData());
            qSignalSignature_ba.prepend("2");

            zval z_sender = objectFactory()->getZObject(qo_sender PQDBG_LVL_CC);

            Z_ADDREF_P(z_receiver);

            #ifdef PQDEBUG
                PQDBG_LVL_DONE();
            #endif

            return phpqt5Connections->createConnection(&z_sender,
                                                       qo_sender,
                                                       qSignalSignature_ba,
                                                       z_receiver,
                                                       nullptr,
                                                       "1" ZEND_INVOKE_FUNC_NAME "()"
                                                       PQDBG_LVL_CC);
        }
    }

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif

    return false;
}

void PHPQt5::pq_remove_connections(QObject *qo
                                   PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString(qo->metaObject()->className()).mid(1));
#endif

    phpqt5Connections->removeConnections(qo
                                         PQDBG_LVL_CC);

    int index = PHPQt5::acceptedPHPSlots_indexes.value(qo, -1);
    if(index != -1) {
        QMutableHashIterator<QString, pq_access_function_entry> i(acceptedPHPSlots_list[index]);
        while(i.hasNext()) {
            i.next();
            if(i.value().zfn != NULL) {
                zval_ptr_dtor(i.value().zfn);
            }

            if(i.value().zfn_name != NULL) {
                zval_ptr_dtor(i.value().zfn_name);
            }

            if(i.value().zo != NULL) {
                Z_DELREF_P(i.value().zo);
            }

            i.value().fn_name.clear();
        }
    }
}

bool PHPQt5::pq_register_closure(QObject *qo,
                                 const QString &name,
                                 zval *pzval
                                 PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("name: %1").arg(name));
#endif

    zend_function *closure = zend_get_closure_invoke_method(Z_OBJ_P(pzval));

    if(closure) {
        pq_access_function_entry afe {
            name,
                    NULL,
                    NULL,
                    pzval,
                    qo
        };

        int index = acceptedPHPSlots_indexes.value(qo, -1);

        if(index >= 0) {
            if(acceptedPHPSlots_list.at(index).contains(name)) {
                pq_access_function_entry oldAfe = acceptedPHPSlots_list.at(index).value(name);
                Z_DELREF_P(oldAfe.zo);
                acceptedPHPSlots_list.value(index).remove(name);
            }
        }
        else {
            QHash<QString, pq_access_function_entry> hash;
            index = acceptedPHPSlots_list.count();
            acceptedPHPSlots_indexes.insert(qo, index);
            acceptedPHPSlots_list.append(hash);
        }

        QHash<QString, pq_access_function_entry> hash =
                acceptedPHPSlots_list.value(index);

        hash.insert(name, afe);

        acceptedPHPSlots_list.replace(index, hash);

        Z_ADDREF_P(pzval);

        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return true;
    }

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif

    return false;
}

bool PHPQt5::pq_declareSignal(QObject *qo, const QByteArray signalSignature)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:?")
              .arg(QString(qo->metaObject()->className()).mid(1))
              .arg(reinterpret_cast<quint64>(qo))
             );
#endif

    if(qo != nullptr) {
        bool ok = false;

        /* light validator */
        QByteArray nSignalSignature = QMetaObject::normalizedSignature(signalSignature);
        if(!nSignalSignature.length()
                || !QMetaObject::checkConnectArgs(nSignalSignature.constData(), nSignalSignature.constData())) {
            php_error(E_ERROR,
                      QString("Invalid signature of signal `<b>%1</b>`")
                      .arg(signalSignature.constData()).toUtf8().constData());
        }

        nSignalSignature.replace(",string",",QString")
                .replace("string,","QString,")
                .replace("(string)","(QString)");

        if(!QMetaObject::invokeMethod(qo, "declareSignal",
                                          Q_RETURN_ARG(bool, ok),
                                          Q_ARG(QByteArray, nSignalSignature))) {
            php_error(E_ERROR,
                      QString("Can't declare signal `<b>%1</b>`")
                      .arg(signalSignature.constData()).toUtf8().constData());
        }

        if(!ok) {
            php_error(E_WARNING,
                      QString("Multiple declaration for signal `<b>%1</b>`")
                      .arg(signalSignature.constData()).toUtf8().constData());
        }

        #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
        PQDBGLPUP(QString("declared: %1").arg(nSignalSignature.constData()));
        #endif

        PQDBG_LVL_DONE();
        return true;
    }
    else {
        PQDBG_LVL_DONE();
        return false;
    }
}

void PHPQt5::pq_emit(QObject *qo, const QByteArray signalSignature, zval *args)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:?")
              .arg(QString(qo->metaObject()->className()).mid(1))
              .arg(reinterpret_cast<quint64>(qo))
             );
#endif

    if(qo != nullptr) {
        zval *arg;
        zend_ulong index;
        QByteArray argsTypes;
        QVariantList vargs;

        ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(args), index, arg) {

            switch(Z_TYPE_P(arg)) {
            case IS_TRUE:
            case IS_FALSE:
                argsTypes += argsTypes.length()
                        ? ",bool" : "bool";
                break;

            case IS_STRING:
                argsTypes += argsTypes.length()
                        ? ",string" : "string";
                break;

            case IS_DOUBLE:
                argsTypes += argsTypes.length()
                        ? ",double" : "double";
                break;

            case IS_LONG:
            case IS_NULL:
                argsTypes += argsTypes.length()
                        ? ",int" : "int";
                break;

            case IS_ARRAY:
                argsTypes += argsTypes.length()
                        ? ",array" : "array";
                break;

            case IS_OBJECT:
                argsTypes += argsTypes.length()
                        ? ",object" : "object";
                break;

            case IS_RESOURCE:
                argsTypes += argsTypes.length()
                        ? ",resource" : "resource";
                break;

            default:
                php_error(E_ERROR, "Passed unsupported argument type to signal");
            }

            zval copy;
            ZVAL_ZVAL(&copy, arg, 1, 0);

            vargs << QVariant::fromValue<zval>(copy);

        } ZEND_HASH_FOREACH_END();

        QByteArray signalName = signalSignature.mid(0, signalSignature.indexOf("("));

        QByteArray t1SignalSignature = QByteArray(signalName)
                .append("(").append(argsTypes).append(")");

        QByteArray t2SignalSignature =
                QByteArray(QMetaObject::normalizedSignature(signalSignature).constData());

        t1SignalSignature.replace(",string",",QString")
                .replace("string,","QString,")
                .replace("(string)","(QString)");

        if(t1SignalSignature != t2SignalSignature) {
            php_error(E_WARNING, QString("Signal-Slot type mismatch %1 %2")
                      .arg(t1SignalSignature.constData())
                      .arg(t2SignalSignature.constData()).toUtf8().constData());
        }

        //zval_ptr_dtor(args);
        PHPQt5Connection_invoke(qo, t1SignalSignature, vargs);
    }

    PQDBG_LVL_DONE();
}

void pq_php_error(const QString &error) {
    php_error(E_ERROR, error.toUtf8().constData());
}

void pq_php_warning(const QString &warning) {
    php_error(E_WARNING, warning.toUtf8().constData());
}

void pq_php_notice(const QString &notice) {
    php_error(E_NOTICE, notice.toUtf8().constData());
}
