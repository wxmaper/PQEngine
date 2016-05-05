#include "pqengine_private.h"
#include "phpqt5.h"

int PQEnginePrivate::php_pqengine_startup(sapi_module_struct *sapi_module)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_startup()");
#endif

    /*
    if (php_module_startup(sapi_module, PHPQt5::phpqt5_module_entry(), 1) == FAILURE) {
        return FAILURE;
    }
    */

    return SUCCESS;
}

int PQEnginePrivate::php_pqengine_deactivate()
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_deactivate()");
#endif

    return SUCCESS;
}

size_t PQEnginePrivate::php_pqengine_ub_write(const char *str, size_t str_length)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("[%1]: %2").arg(str_length).arg(str));
#endif

    pq_ub_write(PHPQt5::toUTF8(str));

    PQDBG_LVL_DONE();
    return str_length;
}

void PQEnginePrivate::php_pqengine_error(int type, const char *format, ...)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_error()");
#endif

    Q_UNUSED(type)
    Q_UNUSED(format)
}

void PQEnginePrivate::php_pqengine_flush(void *server_context)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_flush()");
#endif

    Q_UNUSED(server_context)

  //  php_handle_aborted_connection();
}

void PQEnginePrivate::php_pqengine_send_header(sapi_header_struct *sapi_header, void *server_context)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_send_header()");
#endif

    Q_UNUSED(sapi_header)
    Q_UNUSED(server_context)
}

void PQEnginePrivate::php_pqengine_register_variables(zval *track_vars_array)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_register_variables()");
#endif

    php_register_variable((char*)"PHP_SELF", (char*)"-", NULL);

    char buf[128];
    char **env, *p, *t = buf;
    size_t alloc_size = sizeof(buf);
    unsigned long nlen; /* ptrdiff_t is not portable */

    for (env = environ; env != NULL && *env != NULL; env++) {
        p = strchr(*env, '=');
        if (!p) {				/* malformed entry? */
            continue;
        }
        nlen = p - *env;
        if (nlen >= alloc_size) {
            alloc_size = nlen + 64;
            t = (char*) (t == buf ? emalloc(alloc_size): erealloc(t, alloc_size));
        }
        memcpy(t, *env, nlen);
        t[nlen] = '\0';
        php_register_variable(t, p + 1, track_vars_array);
    }
    if (t != buf && t != NULL) {
        efree(t);
    }
}
#include <QTextCodec>
void PQEnginePrivate::php_pqengine_log_message(char *message)
{
#ifdef PQDEBUG
    PQDBG2("PQEnginePrivate::php_pqengine_log_message()", message);
#endif

    pq_ub_write(PHPQt5::toUTF8(message));
}
