#include "phpqt5.h"
#include "pqengine_private.h"

/* Static variables */
//QList<IPQEngineExt*> PQEnginePrivate::pqExtensions;
PQExtensionHash PQEnginePrivate::m_PQExtensions;
qlonglong PQEnginePrivate::pqHashKey;
//int PQEnginePrivate::indexOfAppInstanceExt;
QString PQEnginePrivate::pqCoreName;

/*
class PQEngineCoreExt : public IPQEngineExt {
    QByteArray extensionName() {
        return "PQEngine Core";
    }

    QCoreApplication *appInstance(int argc, char** argv) {
        return QCoreApplication::instance()
                ? QCoreApplication::instance()
                : new PQCoreApplication(argc, argv);
    }

    QList<QMetaObject> classes()  {
        QList<QMetaObject> classes;
        classes.append(PQCoreApplication::staticMetaObject);
        //classes.append(PQDir::staticMetaObject);
        classes.append(PQObject::staticMetaObject);
        classes.append(PQProcess::staticMetaObject);
        classes.append(PQRegExp::staticMetaObject);
        classes.append(PQSettings::staticMetaObject);
        classes.append(PQThread::staticMetaObject);
        classes.append(PQTimer::staticMetaObject);

        return classes;
    }

    void init() {}

    void finalize() {
        TSRMLS_FETCH_FROM_CTX(PQEngine::getTSRMLS());
        PHPQt5::pq_core_init(TSRMLS_C);
    }
};
*/

#define INI_DEFAULT(name, value) {\
    zval tmp;\
    Z_SET_REFCOUNT(tmp, 0);\
    Z_UNSET_ISREF(tmp);	\
    ZVAL_STRINGL(&tmp, zend_strndup(value, strlen(value)), strlen(value), 0);\
    zend_hash_update(configuration_hash, name, sizeof(name), &tmp, sizeof(zval), NULL); }

static void pqengine_ini_defaults(HashTable *configuration_hash)
{
    INI_DEFAULT("extension_dir", "ext");
    INI_DEFAULT("max_execution_time", "0");
    INI_DEFAULT("memory_limit", "-1");
    INI_DEFAULT("max_input_time", "0");
    INI_DEFAULT("register_argc_argv", "0");
    INI_DEFAULT("implicit_flush", "0");

    QString includePath = qApp->applicationDirPath().append("/");

#ifdef PHP_WIN32
    INI_DEFAULT("include_path", includePath.replace("/","\\").toUtf8().constData());
    INI_DEFAULT("windows_show_crt_warnings", "1");
    INI_DEFAULT("windows.show_crt_warnings", "1");
#else
    INI_DEFAULT("include_path", includePath.toUtf8().constData());
#endif

    Q_UNUSED(configuration_hash);
}

/* Methods */
PQEnginePrivate::PQEnginePrivate(PQExtensionHash pqExtensions, MemoryManager mmng)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::PQEnginePrivate");
#endif

    m_PQExtensions = pqExtensions;
    m_mmng = mmng;
   // m_PQExtensions.prepend(PQEXT(PQEngineCore));

    // +1 because PQEngineCoreExt is a first in list
    // PQEnginePrivate::indexOfAppInstanceExt = indexOfAppInstanceExt + 1;

    php_pqengine_module = {
        "embed",
        "PQEngine",

        php_pqengine_startup,               /* startup */
        php_module_shutdown_wrapper,        /* shutdown */

        NULL,                               /* activate */
        php_pqengine_deactivate,            /* deactivate */

        php_pqengine_ub_write,              /* unbuffered write */
        php_pqengine_flush,                 /* flush */
        NULL,                               /* get uid */
        NULL,                               /* getenv */

        php_pqengine_error,                 /* error handler */

        NULL,                               /* header handler */
        NULL,                               /* send headers handler */
        php_pqengine_send_header,           /* send header handler */

        NULL,                               /* read POST data */
        NULL,                               /* read Cookies */

        php_pqengine_register_variables,    /* register server variables */
        php_pqengine_log_message,           /* Log message */
        NULL,                               /* Get request time */
        NULL,                               /* Child terminate */

        STANDARD_SAPI_MODULE_PROPERTIES
    };

    php_pqengine_module.php_ini_ignore = 0;
    php_pqengine_module.php_ini_ignore_cwd = 1;
    php_pqengine_module.php_ini_path_override = "pqengine.ini";
    php_pqengine_module.ini_entries = "extension_dir = \"ext\";";
    php_pqengine_module.ini_defaults = pqengine_ini_defaults;
    php_pqengine_module.phpinfo_as_text = 1;
}

bool PQEnginePrivate::init(int argc,
                           char **argv,
                           const char *coreName,
                           const char *hashKey,
                           const char *appName,
                           const char *appVersion,
                           const char *orgName,
                           const char *orgDomain)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::init()");
#endif

    QCoreApplication *app = 0;

   // if(m_PQExtensions.size() > indexOfAppInstanceExt) {
    //    app = pqExtensions.at(indexOfAppInstanceExt)->appInstance(argc, argv);
    //}
    bool use_instance = false;

    foreach(IPQExtension *pqExtension, m_PQExtensions.values()) {
        if(!use_instance
                && pqExtension->entry().use_instance) {
            use_instance = true;
            app = pqExtension->entry().instance(argc, argv);
        }
    }

    if(!qApp) {
        app = new PQCoreApplication(argc, argv);
    }

    qApp->setApplicationName(appName);
    qApp->setApplicationVersion(appVersion);
    qApp->setOrganizationName(orgName);
    qApp->setOrganizationDomain(orgDomain);

    PHPQt5::pq_prepare_args(argc, argv);
    PHPQt5::setMemoryManager(m_mmng);

    QByteArray hashKey_ba(hashKey);
    hashKey_ba.append(PQ_APPEND_KEY);
    pqHashKey = strtoll(hashKey_ba.constData(), 0, 16);
    pqCoreName = QString(coreName);

    if(sapi_init()) {
        TSRMLS_FETCH_FROM_CTX(PQEngine::getTSRMLS());

        zend_class_entry *qApp_ce = PHPQt5::objectFactory()->getClassEntry(QString(qApp->metaObject()->className()));

        zval *pzval = NULL;
        MAKE_STD_ZVAL(pzval);
        object_init_ex(pzval, qApp_ce);

        Z_ADDREF_P(pzval);
        PHPQt5::objectFactory()->registerObject(pzval, qApp);

        return true;
    }

    return false;
}

bool PQEnginePrivate::sapi_init()
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::sapi_init()");
#endif

    tsrm_startup(128, 1, 0, NULL);
    TSRMLS_D = (void***) ts_resource(0);

    sapi_startup(&php_pqengine_module);

    if(php_pqengine_module.startup(&php_pqengine_module) == FAILURE) {
        return false;
    }

    if(php_request_startup(TSRMLS_C) == FAILURE) {
        return false;
    }

    return true;
}

int PQEnginePrivate::exec(const char *script)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::exec()");
#endif

    QDir::setCurrent(qApp->applicationDirPath());

    if(QString(script) == "-") {
        return execpq();
    }

    QFile file(script);
    if(file.exists()) {
        TSRMLS_FETCH();

        zend_file_handle file_handle;
        file_handle.type = ZEND_HANDLE_FILENAME;
        file_handle.filename = script;
        file_handle.free_filename = 0;
        file_handle.opened_path = NULL;

        return php_execute_script(&file_handle TSRMLS_CC);
    }
    else {
        pq_ub_write("php script not exists");
        return 1;
    }
}

QByteArray PQEnginePrivate::pqe_unpack(const QByteArray &pqeData, qlonglong key)
{
    SimpleCrypt crypto(key);

    QByteArray base64_ba = qUncompress(pqeData);
    QByteArray encrypted_ba = QByteArray::fromBase64(base64_ba);
    QByteArray decrypted_ba = crypto.decryptToByteArray(encrypted_ba);

    return decrypted_ba;
}

void ***PQEnginePrivate::getTSRMLS()
{
    TSRMLS_FETCH();
    return TSRMLS_C;
}

QByteArray PQEnginePrivate::readMainFile()
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::readMainFile()");
#endif

    QByteArray data_ba;

    qlonglong nullKey = strtoll(PQ_APPEND_KEY, 0, 16);

    if(pqHashKey == nullKey) {
        #ifdef PQDEBUG
            PQDBG("PQEnginePrivate::readMainFile() : no HK");
        #endif

        QFile file(QString(":/%1/main.php").arg(pqCoreName));

        if(file.open(QIODevice::ReadOnly)) {
            data_ba = file.readAll();
        }
        else {
            php_error(E_ERROR, QString("failed to open stream: %1").arg(file.errorString()).toUtf8().constData());
        }
    }
    else {
        #ifdef PQDEBUG
            PQDBG("PQEnginePrivate::readMainFile() : with HK");
        #endif

        QFile file(QString(":/%1/main.pqe").arg(pqCoreName));

        if(file.open(QIODevice::ReadOnly)) {
            data_ba = pqe_unpack(file.readAll(), pqHashKey);
        }
        else {
            php_error(E_ERROR, QString("failed to open stream: %1").arg(file.errorString()).toUtf8().constData());
        }
    }

    return data_ba;
}

int PQEnginePrivate::execpq()
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::execpq()");
#endif

    TSRMLS_FETCH();

    Q_INIT_RESOURCE(res);

    QDataStream dataStream( readMainFile() );

    zend_stream zs;
    zs.reader = pq_stream_reader;
    zs.closer = pq_stream_closer;
    zs.fsizer = pq_stream_fsizer;
    zs.handle = &dataStream;

    zend_file_handle file_handle;
    file_handle.type = ZEND_HANDLE_STREAM;
    file_handle.filename = "-";
    file_handle.free_filename = 0;
    file_handle.opened_path = NULL;
    file_handle.handle.stream = zs;

    return php_execute_script(&file_handle TSRMLS_CC);
}
