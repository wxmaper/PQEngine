#ifndef PQENGINE_PRIVATE_H
#define PQENGINE_PRIVATE_H

#ifdef mkdir
#undef mkdir
#endif

#include "simplecrypt.h"
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <qglobal.h>

#include "pqengine.h"
#include "ipqengineext.h"

/* REMOVE MULTIPLE DEFINITION ERRORS :-[ */
#ifdef PHP_WIN32
#include <time.h>
#include <ws2tcpip.h>
#endif

#include <math.h>
#include <io.h>
#include <inttypes.h>
#include <sys/stat.h>
/* end */

extern "C" {
#include <php.h>
#include <php_main.h>
#include <TSRM.h>
#include <SAPI.h>
}

class PQEnginePrivate
{
public:
    PQEnginePrivate(PQExtensionHash pqExtensions = PQExtensionHash(),
                    MemoryManager mmng = MemoryManager::Hybrid);

    bool                    init(int argc,
                                 char **argv,
                                 const char *coreName,
                                 const char *hashKey,
                                 const char *appName,
                                 const char *appVersion,
                                 const char *orgName,
                                 const char *orgDomain);

    bool                    sapi_init();

    int                     exec(const char *script);
    int                     execpq();

    static void             pq_register_extensions(TSRMLS_D);

    static QByteArray       readMainFile();
    static QByteArray       pqe_unpack(const QByteArray &pqeData, qlonglong key);

    void***                 getTSRMLS();

private:
    //static void             pq_register_extension(IPQEngineExt* pqExtension TSRMLS_DC);
    static void             pq_register_extension(IPQExtension *pqExtension TSRMLS_DC);
    static void             pq_register_classes(QMetaObjectList classes TSRMLS_DC);

    static int              php_pqengine_startup(sapi_module_struct *sapi_module);
    static int              php_pqengine_deactivate(TSRMLS_D);
    static int              php_pqengine_ub_write(const char *str, uint str_length TSRMLS_DC);
    static void             php_pqengine_error(int type, const char *format, ...);
    static void             php_pqengine_flush(void *server_context);
    static void             php_pqengine_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC);
    static void             php_pqengine_register_variables(zval *track_vars_array TSRMLS_DC);
    static void             php_pqengine_log_message(char *message TSRMLS_DC);

    static size_t           pqengine_stream_reader(void *dataStreamPtr, char *buffer, size_t wantlen TSRMLS_DC);
    static void             pqengine_stream_closer(void *dataStreamPtr TSRMLS_DC);
    static size_t           pqengine_stream_fsizer(void *dataStreamPtr TSRMLS_DC);

    /* VARIABLES */
    sapi_module_struct php_pqengine_module;
    //static QList<IPQEngineExt*> pqExtensions;
    static PQExtensionHash m_PQExtensions;
    static qlonglong pqHashKey;
    //static int indexOfAppInstanceExt;
    static QString pqCoreName;
    static MemoryManager m_mmng;
};

#endif // PQENGINE_PRIVATE_H

