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
#include "phpqt5connection.h"

#include "pqengine.h"

ZEND_BEGIN_ARG_INFO_EX(phpqt5__call, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpqt5__set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpqt5__get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

#define ZHANDLE(zobject) (zobject)->handle
#define PQZHANDLE_PROP_NAME "__pq_zhandle_"

#define PQTHREAD __pq_th
#define FETCH_PQTHREAD() QThread *PQTHREAD = QThread::currentThread();
#define PQZHANDLE "__pq_zhandle_"

typedef struct pqobject_wrapper {
    QPointer<QObject> qo_sptr;
    bool isinit;
    zend_object zo;
} PQObjectWrapper;

typedef struct _pq_call_qo_entry {
    QObject *qo;
    zval *zv;
    bool before_have_parent;
} pq_call_qo_entry;

typedef struct _pq_access_function_entry {
    QString fn_name;
    zval *zfn_name;
    zval *zfn;
    zval *zo;
    QObject *qo;
} pq_access_function_entry;

typedef struct _pq_event_wrapper {
    zval *z;
    QEvent *e;
} pq_event_wrapper;

typedef struct _phpqt5_wrap_t{
    zend_object zo;
    zval *zv;
} phpqt5_wrap_t;

typedef struct pq_event_listener_entry {
    QObject *qo;
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;
} EventListenerEntry;

struct qrc_stream_data {
    QDataStream *qrc_file;
    php_stream *stream;
};

static inline PQObjectWrapper *fetch_pqobject(zend_object *zobject) {
    return (PQObjectWrapper *)((char*)(zobject) - XtOffsetOf(PQObjectWrapper, zo));
}

extern QObject* pq_createObject(const QString &className, const QVariantList &args);

extern bool pq_registerQObject(QObject *qo);
extern void pq_ub_write(const QString &msg);
extern void pq_pre(const QString &msg, const QString &title);
extern void pq_register_extra_zend_ce(const QString &className);
extern void pq_register_qevent_zend_ce(const QString &className);
extern void pq_register_long_constant(const QString &className, const QString &constName, int value);
extern bool pq_register_long_constant_ex(const QString &className, const QString &constName, int value, QString ceName);
extern void pq_register_string_constant(const QString &className, const QString &constName, const QString &value);
extern void pq_register_string_constant_ex(const QString &className, const QString &constName, const QString &value, QString ceName);
extern bool pq_send_event(QObject *sender, QEvent *event);
extern void pq_php_error(const QString &error);
extern void pq_php_warning(const QString &warning);
extern void pq_php_notice(const QString &notice);

extern void pq_invoke_closure(PQClosure closure, const QVariantList &args);

extern size_t pq_stream_reader(void *dataStreamPtr, char *buffer, size_t wantlen);
extern void pq_stream_closer(void *dataStreamPtr);
extern size_t pq_stream_fsizer(void *dataStreamPtr);

class PQDLAPI PHPQt5
{
public:
    static QByteArray       toW(const QByteArray &ba);
    static QByteArray       toUTF8(const QByteArray &ba);

    static void             pq_prepare_args(int argc, char** argv PQDBG_LVL_DC);
    static QStringList      pq_get_arguments();

    /* PHPQt5 Conversions */
    static zval             pq_cast_to_zval(const QVariant &value, bool addref PQDBG_LVL_DC);
    static zval             pq_qstringlist_to_ht(QStringList stringList PQDBG_LVL_DC);
    static zval             pq_qpoint_to_ht(const QPoint &point PQDBG_LVL_DC);
    static zval             pq_qpointf_to_ht(const QPointF &point PQDBG_LVL_DC);
    static zval             pq_qrect_to_ht(const QRect &rect PQDBG_LVL_DC);
    static zval             pq_qsize_to_ht(const QSize &size PQDBG_LVL_DC);
    static zval             pq_qevent_to_zobject(QEvent *event PQDBG_LVL_DC);
    static zval             pq_qevent_to_zobject_ex(QEvent *event PQDBG_LVL_DC);
    static zval             pq_qobjectlist_to_ht(const QObjectList &objectList PQDBG_LVL_DC);
    static QStringList      pq_ht_to_qstringlist(zval *pzval PQDBG_LVL_DC);

    PQAPI static void       pq_core_init(PQDBG_LVL_D) ;
    PQAPI static void       pq_register_class(const QMetaObject &metaObject PQDBG_LVL_DC);

    /* HANDLERS */
    static zend_object *    pqobject_create(zend_class_entry *class_type);
    static void             pqobject_free_storage(zend_object *object);
    static int              pqobject_call_method(zend_string *method, zend_object *object, INTERNAL_FUNCTION_PARAMETERS);
    static zend_function *  pqobject_get_method(zend_object **zobject, zend_string *method, const zval *key);

    static zval *           pqobject_get_property_ptr_ptr(zval *object,
                                                          zval *member,
                                                          int type,
                                                          void **cache_slot);
    static HashTable *      pqobject_get_properties(zval *object);
    static zval *           pqobject_read_property(zval *object,
                                                  zval *member,
                                                  int type,
                                                  void **cache_slot,
                                                  zval *rv);
    static int              pqobject_has_property(zval *object,
                                                 zval *member,
                                                 int type,
                                                 void **cache_slot);

    static zend_object *    pq_register_extra_qobject(QObject *qo PQDBG_LVL_DC);
    PQAPI static bool       pq_test_ce(zval *pzval PQDBG_LVL_DC);
    static void             pq_remove_connections(QObject *qo PQDBG_LVL_DC);
    static bool             pq_declareSignal(QObject *qo, const QByteArray signalSignature);

    static int              zm_startup_phpqt5(INIT_FUNC_ARGS);
    static void             zif_SIGNAL(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_SLOT(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_connect(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_disconnect(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_c(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_tr(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_set_tr_lang(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_aboutPQ(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_setQStringCodePage(INTERNAL_FUNCTION_PARAMETERS);

    static void             zif_pre(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_pqinclude(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_pqread(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_pqpack(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_pqGetStarCoords(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_R(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_qApp(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_emit(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_test_one(INTERNAL_FUNCTION_PARAMETERS);

    /* ... */
    static QHash<QObject*, int> acceptedPHPSlots_indexes;
    static QList<QHash<QString, pq_access_function_entry> > acceptedPHPSlots_list;
    static QHash<int, pq_event_wrapper> pq_eventHash;
    static QHash<QObject*,EventListenerEntry> eventListeners;

    static PHPQt5Connection *phpqt5Connections;

    static PHPQt5ObjectFactory *objectFactory() {
        static PHPQt5ObjectFactory *objectFactory_instance = new PHPQt5ObjectFactory;
        return objectFactory_instance;
    }

    static zend_function_entry *phpqt5_functions() {
        static zend_function_entry instance[] = {
            PHP_FE(SIGNAL, NULL)
            PHP_FE(SLOT, NULL)
            PHP_FE(connect, NULL)
            PHP_FE(disconnect, NULL)
            PHP_FE(c, NULL)
            PHP_FE(tr, NULL)
            PHP_FE(set_tr_lang, NULL)
            PHP_FE(setQStringCodePage, NULL)
            PHP_FE(aboutPQ, NULL)
            PHP_FE(pqread, NULL)
            PHP_FE(pre, NULL)
            PHP_FE(pqinclude, NULL)
            PHP_FE(pqpack, NULL)
            PHP_FE(pqGetStarCoords, NULL)
            PHP_FE(R, NULL)
            PHP_FE(test_one, NULL)
            { "qApp", zif_qApp, NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), 0 },
            { "emit", zif_emit, NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), 0 },
            ZEND_FE_END
        };

        return instance;
    }

    PQAPI static zend_module_entry *phpqt5_module_entry() {
        static zend_module_entry instance = {
            STANDARD_MODULE_HEADER,
            "PHPQt5",
            phpqt5_functions(),
            zm_startup_phpqt5,
            NULL,
            NULL,
            NULL,
            NULL,
            PQENGINE_VERSION,
            STANDARD_MODULE_PROPERTIES
        };

        return &instance;
    }

    static zend_function_entry *phpqt5_no_methods() {
        static zend_function_entry instance[] = {
            ZEND_FE_END
        };
        return instance;
    }

    static zend_function_entry *phpqt5_qevent_methods() {
        static zend_function_entry instance[] = {
            ZEND_ME(qevent, ignore, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(qevent, accept, NULL, ZEND_ACC_PUBLIC)
            ZEND_FE_END
        };
        return instance;
    }

    static zend_function_entry *phpqt5_qApp_methods() {
        static zend_function_entry instance[] = {
            ZEND_ME(qApp, __callStatic, phpqt5__call, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
            ZEND_FE_END
        };

        return instance;
    }

    static zend_function_entry *phpqt5_generic_methods() {
        static zend_function_entry instance[] = {
            ZEND_ME(pqobject, __construct, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, __destruct, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, __set, phpqt5__set, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, __get, phpqt5__get, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, __toString, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, __callStatic, phpqt5__call, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
            ZEND_ME(pqobject, qobjInfo, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, qobjProperties, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, qobjMethods, NULL, ZEND_ACC_PUBLIC)
            // ZEND_ME(pqobject, qobjSignals, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, qobjOnSignals, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, free, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, setEventListener, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, children, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, declareSignal, NULL, ZEND_ACC_PUBLIC)
            { "emit", ZEND_MN(pqobject_emit), NULL, (uint32_t) (sizeof(NULL)/sizeof(struct _zend_internal_arg_info)-1), ZEND_ACC_PUBLIC },
            ZEND_FE_END
        };

        return instance;
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

private:
    static QVariant         pq_call(QObject *qo, const char *method, zval *pzval PQDBG_LVL_DC);
    static QVariant         pq_call(QObject *qo, const char *method, QVariantList args PQDBG_LVL_DC);
    static int              pq_call_with_return(QObject *qo, const char *method, zval *pzval, INTERNAL_FUNCTION_PARAMETERS PQDBG_LVL_DC);

    static void             pq_return_qvariant(const QVariant &retVal, INTERNAL_FUNCTION_PARAMETERS PQDBG_LVL_DC);
    static void             pq_set_user_property(QObject *qo, const QString &property, const QVariant &value PQDBG_LVL_DC);

    static bool             pq_set_parent(QObject *qo, zval *zobject_parent PQDBG_LVL_DC);
    static bool             pq_connect_ex(zval *zobj_ptr, zval *pzval PQDBG_LVL_DC);
    static bool             pq_connect(zval *z_sender, zval *z_signal, zval *z_receiver, zval *z_slot, bool cDisconnect PQDBG_LVL_DC);
    static bool             pq_move_to_thread(QObject *qo, zval *zobject_thread PQDBG_LVL_DC);
    static zval             pq_get_child_objects(QObject *qo, zval *pzval PQDBG_LVL_DC);
    static void             pq_emit(QObject *qo, const QByteArray signalSignature, zval *args);

    template <typename T>
    static bool             pq_test_enabledPHPSlots(QObject *qo, QString slotName PQDBG_LVL_DC);
    static bool             pq_create_php_slot(QObject *qo, const QString &qosignal, zval *pzval PQDBG_LVL_DC);
    static bool             pq_register_closure(QObject *qo, const QString &name, zval *pzval PQDBG_LVL_DC);

    static void             zim_pqobject___construct(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___destruct(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___callStatic(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___set(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___get(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___toString(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_qobjInfo(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_qobjProperties(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_qobjMethods(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_qobjOnSignals(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_free(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_setEventListener(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_children(INTERNAL_FUNCTION_PARAMETERS);

    static void             zim_pqobject_emit(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_declareSignal(INTERNAL_FUNCTION_PARAMETERS);

    static void             zim_qApp___callStatic(INTERNAL_FUNCTION_PARAMETERS);

    static void             zim_qevent_ignore(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_qevent_accept(INTERNAL_FUNCTION_PARAMETERS);

    /* PRIVATE VARIABLES */
    static QByteArray W_CP;
    static QStringList mArguments;
    static QByteArray mTrLang;
    static QHash<QByteArray, QByteArray> mTrData;

};

Q_DECLARE_METATYPE(zval)
Q_DECLARE_METATYPE(zval*)

#endif // PHPQT5_H

