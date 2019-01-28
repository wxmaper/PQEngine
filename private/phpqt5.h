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

#ifndef PHPQT5_H
#define PHPQT5_H

#include "phpqt5objectfactory.h"
#include "plastiqthreadcreator.h"

#include "pqengine.h"
#include "plastiqobject.h"

ZEND_BEGIN_ARG_INFO_EX(phpqt5__call, 0, 0, 2)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpqt5__set, 0, 0, 2)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpqt5_setupUi, 0, 0, 1)
ZEND_ARG_INFO(0, parent)
ZEND_ARG_INFO(0, path)
ZEND_ARG_INFO(1, retWidget)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpqt5__get, 0, 0, 1)
ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpqt5__add, 0, 0, 1)
ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(phpqt5__sub, 0, 0, 1)
ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(phpqt5__mul, 0, 0, 1)
ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(phpqt5__div, 0, 0, 1)
ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

#if (PHP_VERSION_ID >= 70300)
#define ZEND_FN_CALL_SPEC ZEND_FASTCALL
#else
#define ZEND_FN_CALL_SPEC
#endif

#define PQ_ZEND_NUM_ARGS() int(ZEND_NUM_ARGS())

struct ConnectionData
{
    PQObjectWrapper *sender;
    QByteArray signal;
    PQObjectWrapper *receiver;
    QByteArray slot;
};

typedef QHash<QByteArray, ConnectionData> ConnectionHash;

class VirtualMethodList;
struct PQObjectWrapper
{
    PlastiQObject *object = Q_NULLPTR;
    bool isExtra = false;       // true if object created from return value
    bool isCopy = false;        // true if is copy object created from return value
    bool isValid = false;       // false if object not created or has destroyed
    bool selfDestroy = false;   // true if object removed by parent

    zend_object *zoptr = Q_NULLPTR; // closure object
    bool isClosure = false; // true if this is a Closure object, then zoptr is a pointer to Closure object

    qint64 enumVal = 0;
    bool isEnum = false;

    QHash<QByteArray,ConnectionHash*> *connections = Q_NULLPTR;
    QHash<QByteArray,zval> *userProperties = Q_NULLPTR;
    VirtualMethodList *virtualMethods = Q_NULLPTR;

    void *ctx = Q_NULLPTR;
    QThread *thread = Q_NULLPTR;

    zend_object zo; // valid object if this is not Closure object
};

struct PQEnumWrapper
{
    qint64 enum_val = 0;
    zend_object zo;
};

struct qrc_stream_data
{
    QDataStream *qrc_file;
    php_stream *stream;
};

typedef struct _pq_tmp_call_info
{
    PQObjectWrapper *pqo;
    zval *zv;
    bool haveParent;
} pq_tmp_call_info;

extern void pq_ub_write(const QString &msg);
extern void pq_pre(const QString &msg, const QString &title);
extern void pq_php_error(const QString &error);
extern void pq_php_warning(const QString &warning);
extern void pq_php_notice(const QString &notice);

extern size_t pq_stream_reader(void *dataStreamPtr, char *buffer, size_t wantlen);
extern void pq_stream_closer(void *dataStreamPtr);
extern size_t pq_stream_fsizer(void *dataStreamPtr);

class PQDLAPI PHPQt5
{
public:
    static void             pq_prepare_args(int argc, char** argv);

    /* PHPQt5 Conversions */
    static zval             plastiq_cast_to_zval(const PMOGStackItem &stackItem);
    static zval             plastiq_cast_to_zval(const QVariant &value, const QByteArray &typeName = QByteArray());
    static PMOGStackItem    plastiq_cast_to_stackItem(zval *entry);
    static zval             plastiq_stringlist_to_array(const QStringList &list);

    static void             pq_register_basic_classes();
    static void             pq_register_plastiq_class(const PlastiQMetaObject &metaObject);
    static void             pq_qdbg_message(zval *value, zval *return_value, const QString &ftype);
    static zval             pq_create_extra_object(const QByteArray &className,
                                                   void *obj,
                                                   bool addToFactoryHash,
                                                   bool isExtra,
                                                   bool isCopy = false);

    /* HANDLERS */
    static zend_object *    pqobject_create(zend_class_entry *class_type);
    static void             pqobject_object_free(zend_object *zobject);
    static void             pqobject_object_dtor(zend_object *zobject);
    static int              pqobject_compare_objects(zval *op1, zval *op2);
    static int              pqobject_compare(zval *result, zval *op1, zval *op2);

    static zend_object *    pqenum_create(zend_class_entry *ce);
    static void             pqenum_object_free(zend_object *zenum);
    static void             pqenum_object_dtor(zend_object *zenum);

    static QHash<QString,zval> loadChilds(QObject *object);

    static bool             pq_test_ce(zval *pzval);
    static bool             pq_test_ce(zend_object *pzval);
    static bool             pq_declareSignal(QObject *qo, const QByteArray signalSignature); // FIXME: to new API


    static bool             downCastTest(const PlastiQMetaObject *metaObject, const QString &className);

    static int zm_startup_phpqt5(INIT_FUNC_ARGS);
    static int zm_activate_phpqt5(INIT_FUNC_ARGS);

    static void ZEND_FN_CALL_SPEC zif_SIGNAL(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_SLOT(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_setEngineErrorHandler(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_connect(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_disconnect(INTERNAL_FUNCTION_PARAMETERS); // FIXME: to new API
    static void ZEND_FN_CALL_SPEC zif_tr(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_set_tr_lang(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_aboutPQ(INTERNAL_FUNCTION_PARAMETERS);

    static void ZEND_FN_CALL_SPEC zif_pqpack(INTERNAL_FUNCTION_PARAMETERS); // FIXME: move include lines before call
    static void ZEND_FN_CALL_SPEC zif_R(INTERNAL_FUNCTION_PARAMETERS);

    static void ZEND_FN_CALL_SPEC zif_qenum(INTERNAL_FUNCTION_PARAMETERS); // FIXME: to new API

    static void ZEND_FN_CALL_SPEC zif_qApp(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_qvariant_cast(INTERNAL_FUNCTION_PARAMETERS);

    static void ZEND_FN_CALL_SPEC zif_qDebug(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_qWarning(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_qCritical(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_qInfo(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_qFatal(INTERNAL_FUNCTION_PARAMETERS);

    static void ZEND_FN_CALL_SPEC zif_pqProperties(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_pqMethods(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_pqStaticFunctions(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_pqSignals(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zif_setupUi(INTERNAL_FUNCTION_PARAMETERS);

    /* ... */

    static PHPQt5ObjectFactory *objectFactory()
    {
        static PHPQt5ObjectFactory *objectFactory_instance = new PHPQt5ObjectFactory;
        return objectFactory_instance;
    }

    static PlastiQThreadCreator *threadCreator()
    {
        static PlastiQThreadCreator *threadCreatorInstance
                = new PlastiQThreadCreator(QThread::currentThread(),
                                           tsrm_get_ls_cache());

        return threadCreatorInstance;
    }

    static zend_function_entry *phpqt5_functions()
    {
        static zend_function_entry instance[] = {
            PHP_FE(SIGNAL, NULL)
            PHP_FE(SLOT, NULL)
            PHP_FE(connect, NULL)
            //PHP_FE(disconnect, NULL)
            PHP_FE(tr, NULL)
            PHP_FE(set_tr_lang, NULL)
            PHP_FE(aboutPQ, NULL)
            PHP_FE(pqpack, NULL)
            PHP_FE(R, NULL)
            PHP_FE(setEngineErrorHandler, NULL)
            PHP_FE(qenum, NULL)

            PHP_FE(pqProperties, NULL)
            PHP_FE(pqMethods, NULL)
            PHP_FE(pqStaticFunctions, NULL)
            PHP_FE(pqSignals, NULL)

            PHP_FE(qvariant_cast, NULL)
            PHP_FE(setupUi, phpqt5_setupUi)

            { "qDebug", zif_qDebug, NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), 0 },
            { "qInfo", zif_qInfo, NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), 0 },
            { "qWarning", zif_qWarning, NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), 0 },
            { "qCritical", zif_qCritical, NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), 0 },
            { "qFatal", zif_qFatal, NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), 0 },
            { "qApp", zif_qApp, NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), 0 },

            ZEND_FE_END
        };

        return instance;
    }

    static zend_module_entry *phpqt5_module_entry()
    {
        static zend_module_entry instance = {
            STANDARD_MODULE_HEADER,
            "PHPQt5",
            phpqt5_functions(),
            zm_startup_phpqt5,
            nullptr,
            zm_activate_phpqt5,
            nullptr,
            nullptr,
            PQENGINE_VERSION,
            STANDARD_MODULE_PROPERTIES
        };

        return &instance;
    }

    static zend_function_entry *phpqt5_plastiq_methods()
    {
        static zend_function_entry plastiq_methods[] = {
            ZEND_ME(plastiq, __construct, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(plastiq, __call, phpqt5__call, ZEND_ACC_PUBLIC)
            ZEND_ME(plastiq, __callStatic, phpqt5__call, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
            ZEND_ME(plastiq, __set, phpqt5__set, ZEND_ACC_PUBLIC)
            ZEND_ME(plastiq, __get, phpqt5__get, ZEND_ACC_PUBLIC)
            ZEND_ME(plastiq, __add, phpqt5__add, ZEND_ACC_PUBLIC) // RFC
            ZEND_ME(plastiq, __mul, phpqt5__mul, ZEND_ACC_PUBLIC) // RFC
            ZEND_ME(plastiq, __sub, phpqt5__sub, ZEND_ACC_PUBLIC) // RFC
            ZEND_ME(plastiq, __div, phpqt5__div, ZEND_ACC_PUBLIC) // RFC
            ZEND_ME(plastiq, __toString, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(plastiq, free, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(plastiq, connect, NULL, ZEND_ACC_PUBLIC)
            { "emit", ZEND_MN(plastiq_emit), NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), ZEND_ACC_PUBLIC },
            ZEND_FE_END
        };

        return plastiq_methods;
    }

    static zend_function_entry *phpqt5_qenum_methods()
    {
        static zend_function_entry qenum_methods[] = {
            ZEND_ME(qenum, __construct, NULL, ZEND_ACC_PUBLIC)
            ZEND_FE_END
        };

        return qenum_methods;
    }

    static zend_function_entry *phpqt5_no_methods()
    {
        // for pq_register_extra_zend_ce
        static zend_function_entry no_methods[] = {
            ZEND_FE_END
        };
        return no_methods;
    }

    static size_t php_qrc_read(php_stream *stream, char *buf, size_t count);
    static int php_qrc_close(php_stream *stream, int close_handle);
    static int php_qrc_flush(php_stream *stream);
    static size_t php_qrc_write(php_stream *stream, const char *buf, size_t count);
    static php_stream *qrc_opener(php_stream_wrapper *wrapper,
                                  const char *path,
                                  const char *mode,
                                  int options,
                                  zend_string **opened_path,
                                  php_stream_context *context STREAMS_DC);

    static zend_object_handlers pqobject_handlers;
    static zend_object_handlers pqenum_handlers;

    static zval engineErrorHandler;

    static void             qevent_cast(PQObjectWrapper *pqobject);
    static QByteArray       plastiqGetQEventClassName(QEvent *event);
    static bool             activateConnection(PQObjectWrapper *sender, const char *signal,
                                               PQObjectWrapper *receiver, const char *slot,
                                               uint32_t argc, zval *params);
    static bool             doActivateConnection(PQObjectWrapper *sender, const char *signal,
                                                 PQObjectWrapper *receiver, const char *slot,
                                                 uint32_t argc, zval *params, bool dtor_params = false);
    static bool             plastiqConnect(zval *z_sender, const QString &signalSignature,
                                           zval *z_receiver, const QString &slotSignature,
                                           bool isOnSignal);

#ifdef PQDEBUG
    static QLocalSocket *debugSocket()
    {
        static QLocalSocket* ds = new QLocalSocket();
        return ds;
    }
#endif

private:
    // static void pq_emit(QObject *qo, const QByteArray signalSignature, zval *args);

    // QEnum
    static void ZEND_FN_CALL_SPEC zim_qenum___construct(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_qenum___call(INTERNAL_FUNCTION_PARAMETERS);

    // PlastiQ
    static void ZEND_FN_CALL_SPEC zim_plastiq___construct(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq___call(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq___callStatic(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq___set(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq___get(INTERNAL_FUNCTION_PARAMETERS);

    static void ZEND_FN_CALL_SPEC zim_plastiq___add(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq___sub(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq___mul(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq___div(INTERNAL_FUNCTION_PARAMETERS);

    static void ZEND_FN_CALL_SPEC zim_plastiq___toString(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq_free(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq_connect(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq_emit(INTERNAL_FUNCTION_PARAMETERS);
    static void ZEND_FN_CALL_SPEC zim_plastiq_testFn(INTERNAL_FUNCTION_PARAMETERS);

    static zval plastiqCall(PQObjectWrapper *pqobject, const QByteArray &methodName, int argc, zval *argv, const PlastiQMetaObject *metaObject = Q_NULLPTR);
    static void plastiqErrorHandler(int error_num, const char *error_filename, const uint error_lineno, const char *format, va_list args);

    /* PRIVATE VARIABLES */
    static QHash<QByteArray, QByteArray> mTrData;
};

static inline QByteArray fetchClassName(zval *zobject)
{
    zend_class_entry *ce = Z_OBJCE_P(zobject);

    do {
        if (PHPQt5::objectFactory()->havePlastiQMetaObject(ce->name->val)) {
            return QByteArray(ce->name->val);
        }
    }
    while ((ce = ce->parent) != nullptr);

    return QByteArray();
}

static inline PQObjectWrapper *fetchPQObjectWrapper(zend_object *zobject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    if (zobject->handlers->compare != PHPQt5::pqobject_compare) {
        PQDBG_LVL_DONE();
        return Q_NULLPTR;
    }


    PQObjectWrapper *pqobject = reinterpret_cast<PQObjectWrapper*>(reinterpret_cast<char*>(zobject) - XtOffsetOf(PQObjectWrapper, zo));

#ifdef PQDEBUG
    PQDBGLPUP("fetch info..."); // displayed if application is crashed
    if (pqobject->object != Q_NULLPTR) {
        PQDBGLPUP(QString("fetch: %1:%2:%3 - %4:%5")
                  .arg(zobject->ce->name->val)
                  .arg(zobject->handle)
                  .arg(reinterpret_cast<quint64>(zobject))
                  .arg(pqobject->object->plastiq_metaObject()->className())
                  .arg(reinterpret_cast<quint64>(pqobject->object->plastiq_data())));
    }
    else {
        PQDBGLPUP(QString("fetch: %1:%2:%3 - Q_NULLPTR:0")
                  .arg(zobject->ce->name->val)
                  .arg(zobject->handle)
                  .arg(reinterpret_cast<quint64>(zobject)));
    }
#endif

    PQDBG_LVL_DONE();

    return pqobject;
}

Q_DECLARE_METATYPE(zval)
Q_DECLARE_METATYPE(zval*)

#endif // PHPQT5_H

