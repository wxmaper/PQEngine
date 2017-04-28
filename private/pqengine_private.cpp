/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PQEngine.
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

#include "phpqt5.h"
#include "pqengine.h"
#include "pqengine_private.h"
#include "zend_exceptions.h"
#include <QCryptographicHash>

/* Static variables */
PQExtensionList PQEnginePrivate::pqExtensions;
qlonglong PQEnginePrivate::pqHashKey;
QString PQEnginePrivate::pqCoreName;

#define INI_DEFAULT(name, value) {\
    zval ztmp;\
    /*Z_SET_REFCOUNT(ztmp, 0);*/\
    /*if(Z_ISREF(ztmp)) { ZVAL_UNREF(&ztmp);*/	\
    ZVAL_STRINGL(&ztmp, zend_strndup(value, strlen(value)), strlen(value));\
    zend_hash_update(configuration_hash, zend_string_init(name, sizeof(name)-1, 0), &ztmp); }

#ifdef Q_OS_WIN
QString getShortPathName(const QString &name)
{
    if (name.isEmpty())
        return name;

    // Determine length, then convert.
    const LPCTSTR nameC = reinterpret_cast<LPCTSTR>(name.utf16()); // MinGW
    const DWORD length = GetShortPathNameW(nameC, NULL, 0);
    if (length == 0)
        return name;
    QScopedArrayPointer<TCHAR> buffer(new TCHAR[length]);
    GetShortPathNameW(nameC, buffer.data(), length);
    const QString rc = QString::fromUtf16(reinterpret_cast<const ushort *>(buffer.data()), length - 1);
    return rc;
}

QString getLongPathName(const QString &name)
{
    if (name.isEmpty())
        return name;

    // Determine length, then convert.
    const LPCTSTR nameC = reinterpret_cast<LPCTSTR>(name.utf16()); // MinGW
    const DWORD length = GetLongPathNameW(nameC, NULL, 0);
    if (length == 0)
        return name;
    QScopedArrayPointer<TCHAR> buffer(new TCHAR[length]);
    GetLongPathNameW(nameC, buffer.data(), length);
    const QString rc = QString::fromUtf16(reinterpret_cast<const ushort *>(buffer.data()), length - 1);
    return rc;
}

// makes sure that capitalization of directories is canonical.
// This mimics the logic in QDeclarative_isFileCaseCorrect
QString normalizePathName(const QString &name)
{
    QString canonicalName = getShortPathName(name);
    if (canonicalName.isEmpty())
        return name;

    canonicalName = getLongPathName(canonicalName);
    if (canonicalName.isEmpty())
        return name;

    // Upper case drive letter
    if (canonicalName.size() > 2 && canonicalName.at(1) == QLatin1Char(':'))
        canonicalName[0] = canonicalName.at(0).toUpper();
    return canonicalName;
}
#else
QString normalizePathName(const QString &name) {
    return name;
}
#endif

static void pqengine_ini_defaults(HashTable *configuration_hash)
{
    INI_DEFAULT("max_execution_time", "0");
    INI_DEFAULT("memory_limit", "3096M");
    INI_DEFAULT("max_input_time", "0");
    INI_DEFAULT("register_argc_argv", "1");
    INI_DEFAULT("implicit_flush", "0");
    //INI_DEFAULT("register_argc_argv", "1");

    QString includePath = normalizePathName(qApp->applicationDirPath()).append("/");
    QString extPath = normalizePathName(qApp->applicationDirPath()).append("/ext");

#ifdef PHP_WIN32
    INI_DEFAULT("extension_dir", extPath.replace("/","\\").toUtf8().constData());
    INI_DEFAULT("include_path", includePath.replace("/","\\").toUtf8().constData());
    INI_DEFAULT("windows_show_crt_warnings", "1");
    INI_DEFAULT("windows.show_crt_warnings", "1");
#else
    INI_DEFAULT("extension_dir", extPath.toUtf8().constData());
    INI_DEFAULT("include_path", includePath.toUtf8().constData());
#endif

    Q_UNUSED(configuration_hash);
}

/* Methods */
PQEnginePrivate::PQEnginePrivate(PQExtensionList extensions, QObject *parent)
    : QObject(parent)
{
    pqExtensions = extensions;

    php_pqengine_module = {
        (char *)"cli", // для поддержки pthreads
        (char *)"PQEngine",

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
    //php_pqengine_module.php_ini_path_override = (char*)"pqengine.ini";
    //php_pqengine_module.ini_entries = (char *)"extension_dir=\"ext\";";
    php_pqengine_module.ini_defaults = pqengine_ini_defaults;
    php_pqengine_module.phpinfo_as_text = 1;
}


bool test_pmd5(const QString &pmd5) {
    bool php7ts_file_ok = false;
    bool ini_file_ok = false;

    QStringList pmd5_list = pmd5.split(";");

    if (pmd5_list.size() != 2) { // неизвестное состояние
        pmd5_list.clear();
        pq_ub_write("<b>Fatal error</b>: cannot test files <b>php7ts.dll</b> and <b>pqengine.ini</b>.");
        return false;
    }

    /* проверка хеша php7ts.dll */
    QByteArray php7ts_file_md5 = pmd5_list.at(0).toLatin1();
    if (php7ts_file_md5.size() == 32) {
#ifdef WIN32
        QFile php7ts_file(normalizePathName(qApp->applicationDirPath()).append("/php7ts.dll"));
#else
        QFile php7ts_file(qApp->applicationDirPath().append("/libphp7.so"));
#endif

        if (php7ts_file.exists() && php7ts_file.open(QIODevice::ReadOnly)) {
            QCryptographicHash hash(QCryptographicHash::Md5);
            if (hash.addData(&php7ts_file)) {
                if(php7ts_file_md5 == hash.result().toHex()) {
                    php7ts_file_ok = true;
                }
                else {
                    php7ts_file_md5.fill(0);
                    pq_ub_write(QString("<b>Fatal error</b>: php7ts.dll: "
                                        "incorrect file cannot be loaded."));
                    return false;
                }
            }
        }
        else {
            php7ts_file_md5.fill(0);
            pq_ub_write("<b>Fatal error</b>: cannot read file <b>php7ts.dll</b>.");
            return false;
        }

        php7ts_file_md5.fill(0);
    }
    else if (php7ts_file_md5 == "0x0") { // проверка отключена
        php7ts_file_ok = true;
    }
    else { // неизвестное состояние
        php7ts_file_md5.fill(0);
        pq_ub_write("<b>Fatal error</b>: cannot test file <b>php7ts.dll</b>.");
        return false;
    }

    /* проверка хеша pqengine.ini */
    QByteArray ini_file_md5 = pmd5_list.at(1).toLatin1();
    QFile ini_file(normalizePathName(qApp->applicationDirPath()).append("/pqengine.ini"));

    if (ini_file_md5.size() == 32) { // проверка включена
        if (ini_file.exists() && ini_file.open(QIODevice::ReadOnly)) {
            QCryptographicHash hash(QCryptographicHash::Md5);
            if (hash.addData(&ini_file)) {
                if (ini_file_md5 == hash.result().toHex()) {
                    ini_file_ok = true;
                }
                else {
                    ini_file_md5.fill(0);
                    pq_ub_write(QString("<b>Fatal error</b>: pqengine.ini: "
                                        "incorrect file cannot be loaded."));
                    return false;
                }
            }
        }
        else {
            ini_file_md5.fill(0);
            pq_ub_write("<b>Fatal error</b>: cannot read file <b>pqengine.ini</b>.");
            return false;
        }
    }
    else if(ini_file_md5 == "0x1") { // проверка включена, ini файл запрещён
        if (ini_file.exists()) {
            ini_file_md5.fill(0);
            pq_ub_write(QString("<b>Fatal error</b>: cannot use forbidden file <b>pqengine.ini</b>."));
            return false;
        }
        else ini_file_ok = true;
    }
    else if (ini_file_md5 == "0x0") { // проверка отключена
        ini_file_ok = true;
    }
    else { // неизвестное состояние
        ini_file_md5.fill(0);
        pq_ub_write("<b>Fatal error</b>: cannot test file <b>pqengine.ini</b>.");
        return false;
    }

    ini_file_md5.fill(0);
    pmd5_list.clear();

    return php7ts_file_ok && ini_file_ok;
}

bool PQEnginePrivate::init(int argc, char **argv, const QString &coreName, const PQEngineInitConf &ic)
{
    new QCoreApplication(argc, argv);

#ifdef PQDEBUG
    QLocalSocket *debugSocket = PHPQt5::debugSocket();
    connect(debugSocket, SIGNAL(connected()), this, SLOT(debugConnected()));
    debugSocket->connectToServer(ic.debugSocketName.trimmed().isEmpty()
                                 ? "PQEngine Debug Server"
                                 : ic.debugSocketName);

    PQDBG_LVL_D = 0;
    PQDBGL(QString("Logging started: QLocalSocket(%1)").arg(reinterpret_cast<quint64>(debugSocket)));
    PQDBGLPUP(__FUNCTION__);
#endif

    if (!test_pmd5(ic.pmd5)) {
        return false;
    }

    QByteArray hashKey_ba(ic.hashKey.toUtf8());
    hashKey_ba.append(PQ_APPEND_KEY);
    pqHashKey = strtoll(hashKey_ba.constData(), 0, 16);
    pqCoreName = coreName;

    php_pqengine_module.executable_location = QByteArray(normalizePathName(qApp->applicationDirPath()).toUtf8()).data();

    QByteArray iniPath = normalizePathName(qApp->applicationDirPath()).append("/pqengine.ini").toUtf8();
#ifdef WIN32
    php_pqengine_module.php_ini_path_override = iniPath.replace("/","\\").data();
#else
    php_pqengine_module.php_ini_path_override = iniPath.data();
#endif

    if (sapi_init()) {
        PHPQt5::pq_prepare_args(argc, argv);

        if (ic.checkName) {
            QString appFileName_t = QFileInfo(normalizePathName(QCoreApplication::applicationFilePath())).fileName();
            QString appFileName = appFileName_t.toLower();

#ifdef PQDEBUG
            PQDBGLPUP("Check the application filename...");
            PQDBGLPUP(normalizePathName(QCoreApplication::applicationFilePath()));
#endif

#ifdef WIN32
            QString extname = QString(coreName).append(".exe");
#ifdef PQDEBUG
            PQDBGLPUP(QString("%1 : %2").arg(appFileName).arg(extname));
#endif
            if (appFileName != QString(coreName).append(".exe")) {
#else
            PQDBGLPUP(QString("%1 : %2").arg(appFileName).arg(coreName));
            if (appFileName != coreName) {
#endif
                pq_ub_write("Please don't change the filename of this application.");
                delete qApp; // Will cause a segmentation fault

                // if not :)
                php_error(E_ERROR, "Application terminated...");
                return false;
            }
        }

        PQDBG_LVL_DONE();
        return true;
    }

    PQDBG_LVL_DONE();
    return false;
}

bool PQEnginePrivate::sapi_init()
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__)
#endif

    tsrm_startup(128, 1, 0, NULL);
    ts_resource(0);

    sapi_startup(&php_pqengine_module);

    PHPQt5::threadCreator();

    if(php_pqengine_module.startup(&php_pqengine_module) == FAILURE) {
        PQDBG_LVL_DONE_LPUP();
        return false;
    }

    if(php_module_startup(&php_pqengine_module, PHPQt5::phpqt5_module_entry(), 1) == FAILURE) {
        PQDBG_LVL_DONE_LPUP();
        return false;
    }

    if(php_request_startup() == FAILURE) {
        PQDBG_LVL_DONE_LPUP();
        return false;
    }

    PQDBG_LVL_DONE_LPUP();
    return true;
}

void PQEnginePrivate::shutdown(PQDBG_LVL_D)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    php_request_shutdown((void *) 0);
    php_module_shutdown();
    sapi_shutdown();
    tsrm_shutdown();

    PQDBG_LVL_DONE_LPUP();
}

int PQEnginePrivate::exec(const char *script PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    QDir::setCurrent(normalizePathName(qApp->applicationDirPath()));

    if(QString(script) == "-") {
        return execpq(PQDBG_LVL_C);
    }

    QFile file(QString(script).replace(QRegExp("^qrc://"), ":/"));
    if(file.exists()) {
        if(qApp != Q_NULLPTR) delete qApp;

        zend_file_handle file_handle;
        file_handle.type = ZEND_HANDLE_FILENAME;
        file_handle.filename = script;
        file_handle.free_filename = 0;
        file_handle.opened_path = NULL;

        //        old old implementation
        //        if(php_execute_script(&file_handle) == SUCCESS)
        //            return 0;
        //        else
        //            return 1;

        //        old implementation
        //        zend_try {
        //            if (php_execute_simple_script(&file_handle, &ret) == FAILURE) {
        //                ZVAL_LONG(&ret, 1);
        //            }
        //        } zend_end_try();

        //      new implementation
        zval ret;

        void *TSRMLS_CACHE = 0;
        TSRMLS_CACHE_UPDATE();
        zend_try {
            zend_op_array *op_array = compile_file(&file_handle, ZEND_INCLUDE); // dont use zend_compile_file
            if (op_array) {
                zend_execute(op_array, &ret);
                zend_exception_restore();
                zend_try_exception_handler();

                if (EG(exception)) {
                    zend_exception_error(EG(exception), E_ERROR);
                }

                destroy_op_array(op_array);
                efree_size(op_array, sizeof(zend_op_array));
            }
            else {
                // error on compile file
                php_execute_script(&file_handle);
                ZVAL_LONG(&ret, 1);
            }
        } zend_end_try();

        int scret;

        switch(Z_TYPE(ret)) {
        case IS_LONG:
            scret = Z_LVAL(ret);
            break;

        case IS_TRUE:
            scret = 0; // all right, it's inversion!
            break;

        case IS_FALSE:
            scret = 1; // all right, it's inversion!
            break;

        default: scret = 1;
        }

        shutdown(PQDBG_LVL_C);

        PQDBGLPUP(QString("application return: %1").arg(scret));
        PQDBG_LVL_DONE();
        return scret;
    }
    else {
        pq_ub_write(QString("<b>%1</b> (%2): <font color='red'>file not found</font> \n<br>in %3")
                    .arg(script)
                    .arg(file.fileName())
                    .arg(normalizePathName(qApp->applicationDirPath())));

        shutdown(PQDBG_LVL_C);

        PQDBGLPUP(QString("application return: 1"));
        PQDBG_LVL_DONE();
        return 1;
    }
}

QByteArray *PQEnginePrivate::pqe_unpack(const QByteArray &pqeData, qlonglong key)
{
    SimpleCrypt *crypto = new SimpleCrypt(key);
    crypto->setCompressionMode(SimpleCrypt::CompressionNever);

    QByteArray base64_ba = qUncompress(pqeData);

    QByteArray encrypted_ba = QByteArray::fromBase64(base64_ba);
    base64_ba.fill(0);
    base64_ba.clear();

    QByteArray decrypted_ba = crypto->decryptToByteArray(encrypted_ba);
    QByteArray *ret_ba = new QByteArray(decrypted_ba);

    decrypted_ba.fill(0);
    decrypted_ba.clear();
    encrypted_ba.clear();
    delete crypto;
    memset(crypto, 0, sizeof(SimpleCrypt*));

    return ret_ba;
}

#ifdef PQDEBUG
void PQEnginePrivate::debugConnected()
{

}
#endif

QByteArray *PQEnginePrivate::readMainFile(PQDBG_LVL_D)
{
#ifdef PQDEBUG
    PQDBGL(__FUNCTION__);
#endif

    QByteArray *data_ba;

    qlonglong nullKey = strtoll(PQ_APPEND_KEY, 0, 16);

    if(pqHashKey == nullKey) {
        PQDBGLPUP("no HK");
        QFile file(QString(":/%1/main.php").arg(pqCoreName));

        if(file.open(QIODevice::ReadOnly)) {
            data_ba = new QByteArray(file.readAll());
            file.close();
        }
        else {
            php_error(E_ERROR, QString("failed to open stream: %1").arg(file.errorString()).toUtf8().constData());
        }
    }
    else {
        PQDBGLPUP("with HK");
        QFile file(QString(":/main.pqe"));

        if(file.open(QIODevice::ReadOnly)) {
            data_ba = pqe_unpack(file.readAll(), pqHashKey);
            file.close();
        }
        else {
            php_error(E_ERROR, QString("failed to open stream: %1").arg(file.errorString()).toUtf8().constData());
        }
    }

    PQDBG_LVL_DONE();
    return data_ba;
}

int PQEnginePrivate::execpq(PQDBG_LVL_D)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    QByteArray *data = readMainFile(PQDBG_LVL_C);
    QDataStream dataStream(data, QIODevice::ReadOnly);

    zend_op_array *op;
    {
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

        op = compile_file(&file_handle, 0); // dont use zend_compile_file

        dataStream.device()->close();
        dataStream.unsetDevice();
        data->fill(0);
        data->clear();

        memset(file_handle.handle.stream.mmap.buf, 0, file_handle.handle.stream.mmap.len);

        delete data;
        data = nullptr;
    }

    zval retVal;
    ZVAL_UNDEF(&retVal);

    if(qApp != Q_NULLPTR) delete qApp;
    zend_execute(op, &retVal);

    int ret = 1;
    if(Z_TYPE(retVal) == IS_LONG) {
        ret = Z_LVAL(retVal);
    }

    shutdown(PQDBG_LVL_C);

    PQDBGLPUP(QString("application return: %1").arg(ret));
    PQDBG_LVL_DONE();
    return ret;
}
