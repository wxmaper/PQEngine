#ifndef PHPQT5_H
#define PHPQT5_H

#include "phpqt5objectfactory.h"
#include "phpqt5connection.h"

#include "pqengine_global.h"

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

extern bool pq_PHP_emit(const QString &qosignal, QObject *sender, QEvent *event);
extern bool pq_callPHPFunction(const QString &fn_name, QObject *sender, QEvent *event);
extern bool pq_registerQObject(QObject *qo);
extern void pq_ub_write(const QString &msg);
extern void pq_pre(const QString &msg, const QString &title);
extern void pq_register_extra_zend_ce(const QString &className);
extern void pq_register_long_constant(const QString &className, const QString &constName, int value);
extern void pq_register_long_constant_ex(const QString &className, const QString &constName, int value, QString ceName);
extern void pq_register_string_constant(const QString &className, const QString &constName, const QString &value);
extern void pq_register_string_constant_ex(const QString &className, const QString &constName, const QString &value, QString ceName);

extern size_t pq_stream_reader(void *dataStreamPtr, char *buffer, size_t wantlen TSRMLS_DC);
extern void pq_stream_closer(void *dataStreamPtr TSRMLS_DC);
extern size_t pq_stream_fsizer(void *dataStreamPtr TSRMLS_DC);

class PQDLAPI PHPQt5
{
public:
    static void             setMemoryManager(MemoryManager mmng);
    static QByteArray       toW(const QByteArray &ba);
    static QByteArray       toUTF8(const QByteArray &ba);

    static void             pq_prepare_args(int argc, char** argv);
    static QStringList      pq_get_arguments();

    /* PHPQt5 Conversions */
    static zval *           pq_qstringlist_to_ht(QStringList stringList TSRMLS_DC);
    static zval *           pq_qpoint_to_ht(const QPoint &point TSRMLS_DC);
    static zval *           pq_qrect_to_ht(const QRect &rect TSRMLS_DC);
    static zval *           pq_qsize_to_ht(const QSize &size TSRMLS_DC);
    static zval *           pq_qevent_to_zobject(QEvent *event TSRMLS_DC);
    static QStringList      pq_ht_to_qstringlist(zval *pzval TSRMLS_DC);

    PQAPI static void       pq_core_init(TSRMLS_D) ;
    PQAPI static void       pq_register_class(const QMetaObject &metaObject TSRMLS_DC);

    PQAPI static bool       pq_test_ce(zval *pzval TSRMLS_DC);
    static void             pq_remove_connections(QObject *qo);

    static int              zm_startup_phpqt5(INIT_FUNC_ARGS);
    static void             zif_SIGNAL(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_SLOT(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_connect(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_disconnect(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_c(INTERNAL_FUNCTION_PARAMETERS);
    //static void             zif_widgetAt(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_tr(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_set_tr_lang(INTERNAL_FUNCTION_PARAMETERS);
   // static void             zif_mousePos(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_aboutPQ(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_setQStringCodePage(INTERNAL_FUNCTION_PARAMETERS);

    static void             zif_pre(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_pqinclude(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_pqread(INTERNAL_FUNCTION_PARAMETERS);
    static void             zif_pqpack(INTERNAL_FUNCTION_PARAMETERS);

    /* ... */
    static QHash<QObject*, int> acceptedPHPSlots_indexes;
    static QList<QHash<QString, pq_access_function_entry> > acceptedPHPSlots_list;
    static QHash<QString, pq_access_function_entry> acceptedPHPFunctions;
    static QHash<int, pq_event_wrapper> pq_eventHash;

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
            //PHP_FE(mousePos, NULL)
            PHP_FE(setQStringCodePage, NULL)
            PHP_FE(aboutPQ, NULL)
            PHP_FE(pqread, NULL)
            PHP_FE(pre, NULL)
            PHP_FE(pqinclude, NULL)
            PHP_FE(pqpack, NULL)
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
            ZEND_ME(pqobject, __call, phpqt5__call, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, __set, phpqt5__set, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, __get, phpqt5__get, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, __toString, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, __callStatic, phpqt5__call, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
            ZEND_ME(pqobject, qobjInfo, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, qobjProperties, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, qobjMethods, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, free, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, setPHPEventListener, NULL, ZEND_ACC_PUBLIC)
            ZEND_ME(pqobject, children, NULL, ZEND_ACC_PUBLIC)
            ZEND_FE_END
        };

        return instance;
    }

/*
    static zend_function_entry *phpqt5_extra_methods() {
        static zend_function_entry instance[] = {
            ZEND_ME(extra_pqobject, __callStatic, phpqt5__call, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
            ZEND_FE_END
        };

        return instance;
    }
*/

private:
    static QVariant         pq_call(QObject *qo, const char *method, zval *pzval TSRMLS_DC);
    static QVariant         pq_call(QObject *qo, const char *method, QVariantList args TSRMLS_DC);
    static void             pq_call_with_return(QObject *qo, const char *method, zval *pzval, INTERNAL_FUNCTION_PARAMETERS);

    static void             pq_return_qvariant(const QVariant &retVal, INTERNAL_FUNCTION_PARAMETERS);
    static void             pq_set_user_property(QObject *qo, const QString &property, const QVariant &value TSRMLS_DC);

    static bool             pq_set_parent(QObject *qo, zval *pzval TSRMLS_DC);
    static bool             pq_connect_ex(zval *zobj_ptr, zval *pzval TSRMLS_DC);
    static bool             pq_connect(zval *z_sender, zval *z_signal, zval *z_receiver, zval *z_slot, bool cDisconnect TSRMLS_DC);
    static bool             pq_move_to_thread(QObject *qo, zval *pzval TSRMLS_DC);
    static zval*            pq_get_child_objects(QObject *qo, zval *pzval TSRMLS_DC);

    template <typename T>
    static bool             pq_test_enabledPHPSlots(QObject *qo, QString slotName);
    static bool             pq_create_php_slot(QObject *qo, const QString &qosignal, zval *pzval TSRMLS_DC);
    static bool             pq_register_closure(QObject *qo, const QString &name, zval *pzval TSRMLS_DC);

    static void             zim_pqobject___construct(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___destruct(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___call(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___callStatic(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___set(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___get(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject___toString(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_qobjInfo(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_qobjProperties(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_qobjMethods(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_free(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_setPHPEventListener(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_pqobject_children(INTERNAL_FUNCTION_PARAMETERS);

    static void             zim_qApp___callStatic(INTERNAL_FUNCTION_PARAMETERS);

    static void             zim_qevent_ignore(INTERNAL_FUNCTION_PARAMETERS);
    static void             zim_qevent_accept(INTERNAL_FUNCTION_PARAMETERS);

    /* PRIVATE VARIABLES */
    static QByteArray W_CP;
    static QStringList mArguments;
    static QByteArray mTrLang;
    static QHash<QByteArray, QByteArray> mTrData;
    static MemoryManager m_mmng;
};


#endif // PHPQT5_H

