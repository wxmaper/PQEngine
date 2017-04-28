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

#include "phpqt5.h"
#include "pqengine_private.h"
#include "phpqt5constants.h"

QStringList PHPQt5::mArguments;

#define PQ_TEST_CLASS(classname) qo_sender->metaObject()->className() == QString(classname)
#define PQ_CREATE_PHP_CONN(classname) phpqt5Connections->createPHPConnection<classname>(zo_sender, qo_sender, signal, z_receiver, slot)

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


void pq_ub_write(const QString &msg)
{
    default_ub_write(msg, "");
}

void pq_pre(const QString &msg, const QString &title)
{
    default_ub_write(msg, title);
}

void PHPQt5::pq_prepare_args(int argc,
                             char** argv)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("argc: %1").arg(argc));
#endif

    void *TSRMLS_CACHE = NULL;
    ZEND_TSRMLS_CACHE_UPDATE();

    zval z_argv, zargc;
    array_init(&z_argv);
    Z_ADDREF(z_argv);

    for(int i = 0; i < argc; i++) {
        add_next_index_string(&z_argv, argv[i]);
    }

    ZVAL_LONG(&zargc, argc);

    zend_hash_str_update(&EG(symbol_table), "argv", 4, &z_argv);
    zend_hash_str_add(&EG(symbol_table), "argc", 4, &zargc);

    PQDBG_LVL_DONE();
}

QStringList PHPQt5::pq_get_arguments()
{
    return mArguments;
}

void PHPQt5::pq_register_basic_classes()
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    using namespace PHPQt5NS;

    // PlastiQDestroyedObject
    QByteArray className = "PlastiQDestroyedObject";

    zend_class_entry ce;
    INIT_CLASS_ENTRY_EX(ce, className.constData(), className.length(), NULL);
    zend_class_entry *ce_ptr = zend_register_internal_class(&ce);


    objectFactory()->registerZendClassEntry(className, ce_ptr);

    // QEnum
    // objectFactory()->registerPlastiQMetaObject(metaObject);

    zend_class_entry enum_ce;
    INIT_CLASS_ENTRY_EX(enum_ce, "QEnum", 5, phpqt5_qenum_methods());
    enum_ce.create_object = pqobject_create;
    zend_class_entry *enum_ce_ptr = zend_register_internal_class(&enum_ce);
    objectFactory()->registerZendClassEntry("QEnum", enum_ce_ptr);

    PQDBG_LVL_DONE();
}

void PHPQt5::pq_register_plastiq_class(const PlastiQMetaObject &metaObject)
{
#ifdef PQDEBUG
    //PQDBG_LVL_PROCEED(__FUNCTION__);
    //PQDBGLPUP(metaObject.className());
#endif

    QByteArray className = objectFactory()->registerPlastiQMetaObject(metaObject);

    zend_class_entry ce, *ce_ptr;

    if(*(metaObject.d.objectType) == PlastiQ::IsNamespace) {
        INIT_CLASS_ENTRY_EX(ce,
                            className.constData(),
                            className.length(),
                            phpqt5_no_methods());

        ce_ptr = zend_register_internal_class(&ce);
    }
    else {
        INIT_CLASS_ENTRY_EX(ce,
                            className.constData(),
                            className.length(),
                            phpqt5_plastiq_methods());

        ce.create_object = pqobject_create;
        ce_ptr = zend_register_internal_class(&ce);
    }

    objectFactory()->registerZendClassEntry(className, ce_ptr);

    QHashIterator<QByteArray,long> iter(*metaObject.d.pq_constants);
    while(iter.hasNext()) {
        iter.next();

        zend_declare_class_constant_long(ce_ptr,
                                         iter.key().constData(),
                                         iter.key().length(),
                                         iter.value());
    }
}

PQAPI bool PHPQt5::pq_test_ce(zval *pzval)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    bool isPQObject = false;

    if (Z_TYPE_P(pzval) == IS_OBJECT) {
        isPQObject = pq_test_ce(Z_OBJ_P(pzval));
    }

    PQDBG_LVL_DONE();
    return isPQObject;
}

bool PHPQt5::pq_test_ce(zend_object *zo)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    bool isPQObject = false;
    zend_class_entry *ce = zo->ce;

    do {
        if (objectFactory()->havePlastiQMetaObject(ce->name->val)) {
            isPQObject = true;
            break;
        }
    } while(ce = ce->parent);

    PQDBG_LVL_DONE();
    return isPQObject;
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
        //PHPQt5Connection_invoke(qo, t1SignalSignature, vargs);
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

void PHPQt5::pq_qdbg_message(zval *value, zval *return_value, const QString &ftype) {
    php_output_start_default();
    zend_print_zval_r(value, 0);
    php_output_get_contents(return_value);

#ifdef PQDEBUG
    QString msg(Z_STRVAL_P(return_value));
    default_ub_write(msg, QStringLiteral("qCritical"));

    pqdbg_send_message({
                           { "command", ftype },
                           { "thread", QString::number(reinterpret_cast<quint64>(QThread::currentThread())) },
                           { "message", msg }
                       });
#else
    Q_UNUSED(ftype);
#endif

    php_output_discard();
}

void PHPQt5::pq_declare_wrapper_props(zend_class_entry *ce_ptr)
{
    using namespace PHPQt5NS;
    static const int PQ_UID_LEN = strlen(PQ_UID);
    static const int PQ_ZHANDLE_LEN = strlen(PQ_ZHANDLE);
    zend_declare_property_long(ce_ptr, PQ_UID, PQ_UID_LEN, 0, ZEND_ACC_PROTECTED);
    zend_declare_property_long(ce_ptr, PQ_ZHANDLE, PQ_ZHANDLE_LEN, 0, ZEND_ACC_PROTECTED);
}

void PHPQt5::pq_update_wrapper_props(zval *zv, qint64 uid)
{
    using namespace PHPQt5NS;
    static const int PQ_UID_LEN = strlen(PQ_UID);
    static const int PQ_ZHANDLE_LEN = strlen(PQ_ZHANDLE);
    zend_update_property_long(Z_OBJCE_P(zv), zv, PQ_UID, PQ_UID_LEN, uid);
    zend_update_property_long(Z_OBJCE_P(zv), zv, PQ_ZHANDLE, PQ_ZHANDLE_LEN, Z_OBJ_HANDLE_P(zv));
}

zval PHPQt5::pq_create_extra_object(const QByteArray &className,
                                    void *obj,
                                    bool addToFactoryHash,
                                    bool isExtra)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zend_class_entry *ce = objectFactory()->getClassEntry(className);
    PlastiQMetaObject metaObject = objectFactory()->getMetaObject(className);
    PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

    PlastiQObject *senderObject = metaObject.createInstanceFromData(reinterpret_cast<void*>(obj));
    PQDBGLPUP(QString("created object: %1").arg(senderObject->plastiq_metaObject()->className()));

    PQDBGLPUP("object_init_ex");
    zval zobject;
    object_init_ex(&zobject, ce);

    PQDBGLPUP("fetch_pqobject");
    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ(zobject));
    pqobject->object = senderObject;
    pqobject->isExtra = isExtra;
    pqobject->isValid = true;

    quint64 objectId = reinterpret_cast<quint64>(obj);
    pq_update_wrapper_props(&zobject, objectId);

    if (addToFactoryHash) {
        objectFactory()->addObject(pqobject, objectId);
    }

    PQDBG_LVL_DONE();
    return zobject;
}
