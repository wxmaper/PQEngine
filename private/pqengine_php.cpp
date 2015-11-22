#include "pqengine_private.h"
#include "phpqt5.h"

int PQEnginePrivate::php_pqengine_startup(sapi_module_struct *sapi_module)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_startup()");
#endif

    return php_module_startup(sapi_module, PHPQt5::phpqt5_module_entry(), 1);
}

int PQEnginePrivate::php_pqengine_deactivate(TSRMLS_D)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_deactivate()");
#endif

    return SUCCESS;
}

int PQEnginePrivate::php_pqengine_ub_write(const char *str, uint str_length TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG2("PQEnginePrivate::php_pqengine_ub_write()", str);
#endif

    pq_ub_write(PHPQt5::toUTF8(str));

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

    php_handle_aborted_connection();
}

void PQEnginePrivate::php_pqengine_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_send_header()");
#endif

    Q_UNUSED(sapi_header)
    Q_UNUSED(server_context)
}

void PQEnginePrivate::php_pqengine_register_variables(zval *track_vars_array TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::php_pqengine_register_variables()");
#endif

    php_register_variable("PHP_SELF", "-", NULL TSRMLS_CC);

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
        php_register_variable(t, p + 1, track_vars_array TSRMLS_CC);
    }
    if (t != buf && t != NULL) {
        efree(t);
    }
}

void PQEnginePrivate::php_pqengine_log_message(char *message TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG2("PQEnginePrivate::php_pqengine_log_message()", message);
#endif

    pq_ub_write(message);
}
