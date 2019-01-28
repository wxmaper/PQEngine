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

#define PQ_TEST_CLASS(classname) qo_sender->metaObject()->className() == QString(classname)
#define PQ_CREATE_PHP_CONN(classname) phpqt5Connections->createPHPConnection<classname>(zo_sender, qo_sender, signal, z_receiver, slot)

size_t pq_stream_reader(void *dataStreamPtr, char *buffer, size_t wantlen)
{
    QDataStream *dataStream = reinterpret_cast<QDataStream*>(dataStreamPtr);
    return size_t(dataStream->device()->read(buffer, wantlen));
}

void pq_stream_closer(void *dataStreamPtr)
{
    QDataStream *dataStream = reinterpret_cast<QDataStream*>(dataStreamPtr);
    dataStream->device()->close();
}

size_t pq_stream_fsizer(void *dataStreamPtr)
{
    QDataStream *dataStream = reinterpret_cast<QDataStream*>(dataStreamPtr);
    return size_t(dataStream->device()->size());
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

    void *TSRMLS_CACHE = nullptr;
    ZEND_TSRMLS_CACHE_UPDATE();

    zval z_argv, zargc;
    array_init(&z_argv);
    Z_ADDREF(z_argv);

    for(int i = 0; i < argc; i++) {
        add_next_index_string(&z_argv, argv[i]);
    }

    ZVAL_LONG(&zargc, argc);

    //SG(request_info).argc=argc;
    //memcpy(SG(request_info).argv,argv,sizeof(argv));

    zend_hash_str_update(&EG(symbol_table), "argv", 4, &z_argv);
    zend_hash_str_add(&EG(symbol_table), "argc", 4, &zargc);

    PQDBG_LVL_DONE();
}

void PHPQt5::pq_register_basic_classes()
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    using namespace PHPQt5NS;

    // PlastiQDestroyedObject
    {
        const QByteArray className = "PlastiQDestroyedObject";
        zend_class_entry ce;
        INIT_CLASS_ENTRY_EX(ce, className.constData(), size_t(className.length()), NULL);
        zend_class_entry *ce_ptr = zend_register_internal_class(&ce);
        objectFactory()->registerZendClassEntry(className, ce_ptr);
    }

    // QEnum
    {
        const QByteArray className = "PlastiQDestroyedObject";
        zend_class_entry ce;
        INIT_CLASS_ENTRY_EX(ce, "QEnum", 5, phpqt5_qenum_methods());
        ce.create_object = pqobject_create;
        zend_class_entry *enum_ce_ptr = zend_register_internal_class(&ce);
        objectFactory()->registerZendClassEntry("QEnum", enum_ce_ptr);
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::pq_register_plastiq_class(const PlastiQMetaObject &metaObject)
{
    QByteArray className = objectFactory()->registerPlastiQMetaObject(metaObject);

    zend_class_entry ce, *ce_ptr;

    if (*(metaObject.d.objectType) == PlastiQ::IsNamespace) {
        INIT_CLASS_ENTRY_EX(ce,
                            className.constData(),
                            size_t(className.length()),
                            phpqt5_no_methods());

        ce_ptr = zend_register_internal_class(&ce);
    }
    else {
        INIT_CLASS_ENTRY_EX(ce,
                            className.constData(),
                            size_t(className.length()),
                            phpqt5_plastiq_methods());

        ce.create_object = pqobject_create;
        ce_ptr = zend_register_internal_class(&ce);
    }

    objectFactory()->registerZendClassEntry(className, ce_ptr);

    QHashIterator<QByteArray,long> iter(*metaObject.d.pq_constants);
    while (iter.hasNext()) {
        iter.next();

        zend_declare_class_constant_long(ce_ptr,
                                         iter.key().constData(),
                                         size_t(iter.key().length()),
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
    }
    while ((ce = ce->parent) != nullptr);

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
        if (!nSignalSignature.length()
                || !QMetaObject::checkConnectArgs(nSignalSignature.constData(), nSignalSignature.constData())) {
            php_error(E_ERROR, "Invalid signature of signal `<b>%s</b>`",
                      signalSignature.constData());
        }

        nSignalSignature.replace(",string",",QString")
                .replace("string,","QString,")
                .replace("(string)","(QString)");

        if (!QMetaObject::invokeMethod(qo, "declareSignal",
                                      Q_RETURN_ARG(bool, ok),
                                      Q_ARG(QByteArray, nSignalSignature))) {
            php_error(E_ERROR, "Can't declare signal `<b>%s</b>`",
                      signalSignature.constData());
        }

        if (!ok) {
            php_error(E_WARNING, "Multiple declaration for signal `<b>%s</b>`",
                      signalSignature.constData());
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
    default_ub_write(msg, ftype);

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

zval PHPQt5::pq_create_extra_object(const QByteArray &className,
                                    void *obj,
                                    bool addToFactoryHash,
                                    bool isExtra,
                                    bool isCopy)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zend_class_entry *ce = objectFactory()->getClassEntry(className);
    PlastiQMetaObject metaObject = objectFactory()->getMetaObject(className);
    PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

    PlastiQObject *object = metaObject.createInstanceFromData(reinterpret_cast<void*>(obj));
    PQDBGLPUP(QString("created object: %1").arg(object->plastiq_metaObject()->className()));

    PQDBGLPUP("object_init_ex");
    zval zobject;
    object_init_ex(&zobject, ce);

    PQDBGLPUP("fetch_pqobject");
    PQObjectWrapper *pqobject = fetchPQObjectWrapper(Z_OBJ(zobject));
    pqobject->object = object;
    pqobject->isExtra = isExtra;
    pqobject->isValid = true;
    pqobject->isCopy = isCopy;

    PlastiQ::ObjectType objectType = *(object->plastiq_metaObject()->d.objectType);
    switch (objectType) {
    case PlastiQ::IsQtItem: {
        PQDBGLPUP("object type: IsQtItem");

        pqobject->thread = QThread::currentThread();
        PQDBGLPUP(QString("thread: %1; TSRMLS_CACHE: %2")
                  .arg(reinterpret_cast<quint64>(QThread::currentThread()))
                  .arg(reinterpret_cast<quint64>(pqobject->ctx)));
    } break;

    case PlastiQ::IsQtObject: {
        PQDBGLPUP("object type: IsQtObject");
        pqobject->thread = QThread::currentThread();
        PQDBGLPUP(QString("thread: %1; TSRMLS_CACHE: %2")
                  .arg(reinterpret_cast<quint64>(QThread::currentThread()))
                  .arg(reinterpret_cast<quint64>(pqobject->ctx)));
    } break;

    case PlastiQ::IsQObject:
    case PlastiQ::IsQWidget:
    case PlastiQ::IsQWindow: {
        PQDBGLPUP("object type: IsQObject");
        PMOGStack stack = new PMOGStackItem[1];
        metaObject.invokeMethod(object, "thread()", stack);
        QThread *thread = reinterpret_cast<QThread*>(stack[0].s_voidp);
        pqobject->thread = thread;
        PQDBGLPUP(QString("thread: %1; TSRMLS_CACHE: %2")
                  .arg(reinterpret_cast<quint64>(thread))
                  .arg(reinterpret_cast<quint64>(pqobject->ctx)));

        delete [] stack;
    } break;

    default:
        PQDBGLPUP("object type: IsObject");
        pqobject->thread = QThread::currentThread();
        PQDBGLPUP(QString("thread: %1; TSRMLS_CACHE: %2")
                  .arg(reinterpret_cast<quint64>(QThread::currentThread()))
                  .arg(reinterpret_cast<quint64>(pqobject->ctx)));
    }

    if (addToFactoryHash) {
        objectFactory()->addObject(pqobject);
    }

    PQDBG_LVL_DONE();
    return zobject;
}
