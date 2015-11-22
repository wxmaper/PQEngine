#include "phpqt5.h"
#include "pqengine_private.h"

PHPQt5Connection *PHPQt5::phpqt5Connections;

int PHPQt5::zm_startup_phpqt5(INIT_FUNC_ARGS)
{
#ifdef PQDEBUG
    PQDBG("PHPQt5::zm_startup_phpqt5()");
#endif

    PQEnginePrivate::pq_register_extensions(TSRMLS_C);
    phpqt5Connections = new PHPQt5Connection(TSRMLS_C);

    return SUCCESS;
}

