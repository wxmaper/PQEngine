#include "plastiqthreadcreator.h"

#include "phpqt5.h"
#include "pqengine_private.h"
#include <SAPI.h>

PlastiQThreadCreator::PlastiQThreadCreator(QThread *thread, void *tsrmls_cache, QObject *parent)
    : QObject(parent)
{
    threads.insert(thread, tsrmls_cache);
}

/*
void *PlastiQThreadCreator::get_tsrmls_cache(QThread *thread)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    static QHash<QThread*, void*> tsrmls_c;

    void *TSRMLS_CACHE = tsrmls_c.value(thread, Q_NULLPTR);

    if(!TSRMLS_CACHE) {
        THREAD_T thread_id;
        thread_id = tsrm_thread_id();

        //PQDBGLPUP(QString("tsrm_startup"));
        tsrm_startup(128, 1, TSRM_ERROR_LEVEL_ERROR,
                     (QCoreApplication::applicationDirPath() + "tsrm_debug.log").toUtf8().data());
        ts_resource(0);

        PQDBGLPUP(QString("tsrm_new_interpreter_context in thread: %1 (thread_id: %2)")
                  .arg(reinterpret_cast<quint64>(QThread::currentThread()))
                  .arg(thread_id));
        //TSRMLS_CACHE = tsrm_new_interpreter_context();

        PQDBGLPUP(QString("tsrm_set_interpreter_context: %1").arg(reinterpret_cast<quint64>(TSRMLS_CACHE)));
        //tsrm_set_interpreter_context(TSRMLS_CACHE);
        //PQDBGLPUP(QString("QThread: %1").arg(reinterpret_cast<quint64>(QThread::currentThread())));

        //tsrm_set_interpreter_context(TSRMLS_CACHE);

        //PQDBGLPUP(QString("tsrm_set_interpreter_context from `%1` to `%2`")
        //          .arg(reinterpret_cast<quint64>(old_tsrmls_cache))
        //          .arg(reinterpret_cast<quint64>(TSRMLS_CACHE)));

        //PQDBGLPUP("tsrm_set_interpreter_context");
        //tsrm_set_interpreter_context(tsrmls_cache);

        PQDBGLPUP("php_request_startup");
        php_request_startup();
        //PG(during_request_startup) = 0;


        // connect(thread, SIGNAL(destroyed(QObject*)), this, SLOT(free_tsrmls_cache(QObject*)));
        tsrmls_c.insert(thread, TSRMLS_CACHE);

        // this->moveToThread(th);

        //PQDBGLPUP(QString("tsrm_set_interpreter_context from `%1` to `%2`")
        //          .arg(reinterpret_cast<quint64>(TSRMLS_CACHE))
        //          .arg(reinterpret_cast<quint64>(old_tsrmls_cache)));

        //tsrm_set_interpreter_context(old_tsrmls_cache)
    }

    PQDBG_LVL_DONE();
    return TSRMLS_CACHE;
}
*/


void *PlastiQThreadCreator::setContextThread(QThread *thread)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    static QHash<QThread*, void*> tsrmls_c;

    void *context = tsrmls_c.value(thread, Q_NULLPTR);
    void *parentContext = Q_NULLPTR;

    if(!context) {
        void *ls = ts_resource(0);
        PQDBGLPUP(QString("ts_resource: %1").arg(reinterpret_cast<quint64>(ls)));

        parentContext = tsrm_get_ls_cache();
        PQDBGLPUP(QString("current context: %1").arg(reinterpret_cast<quint64>(parentContext)));
        PQDBGLPUP(QString("current thread: %1").arg(reinterpret_cast<quint64>(QThread::currentThread())));
        PQDBGLPUP(QString("tsrm_new_interpreter_context for thread: %1").arg(reinterpret_cast<quint64>(thread)));
        context = tsrm_new_interpreter_context();

        PQDBGLPUP(QString("tsrm_set_interpreter_context: %1").arg(reinterpret_cast<quint64>(context)));
        tsrm_set_interpreter_context(context);

        ZEND_TSRMLS_CACHE_UPDATE();

        PG(expose_php) = 0;
        PG(auto_globals_jit) = 0;

        PQDBGLPUP("php_request_startup");
        php_request_startup();

        //PG(during_request_startup) = 0;

        // connect(thread, SIGNAL(destroyed(QObject*)), this, SLOT(free_tsrmls_cache(QObject*)));
        tsrmls_c.insert(thread, context);
    }
    else {
        PQDBGLPUP(QString("tsrm_set_interpreter_context: %1").arg(reinterpret_cast<quint64>(context)));
        parentContext = tsrm_set_interpreter_context(context);
    }

    PQDBG_LVL_DONE_LPUP();
    return parentContext;
}

QThread *PlastiQThreadCreator::getThread(void *tsrmls_cache)
{
    QThread *thread = threads.key(tsrmls_cache);
    return thread;
}

void *PlastiQThreadCreator::contextThread(QThread *thread)
{
    PlastiQThreadCtxCreator *ctxCreator = new PlastiQThreadCtxCreator;
    ctxCreator->moveToThread(thread);

    connect(thread, &QThread::finished,
            ctxCreator, &PlastiQThreadCtxCreator::shutdown, Qt::DirectConnection);

    return ctxCreator->contextThread();
}

void PlastiQThreadCreator::free_tsrmls_cache(QObject *threadObject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif
;
    QThread *thread = qobject_cast<QThread*>(threadObject);

    void *tsrmls_cache = threads.value(thread, Q_NULLPTR);

    if(tsrmls_cache) {
        PQDBGLPUP("tsrm_set_interpreter_context");
        tsrm_set_interpreter_context(tsrmls_cache);

        PQDBGLPUP("php_request_shutdown");
        php_request_shutdown(0);

        PQDBGLPUP("tsrm_free_interpreter_context");
        tsrm_free_interpreter_context(tsrmls_cache);
    }

    threads.remove(thread);

    PQDBG_LVL_DONE();
}

PlastiQThreadWorker::PlastiQThreadWorker(QThread *thread, QObject *parent)
    : QObject(parent)
{
    PQDBG_LVL_START(__FUNCTION__);

//    void *parentContext = PHPQt5::threadCreator()->setContextThread(thread);
//    void *context = tsrm_get_ls_cache();
//    PQDBGLPUP(QString("change context from `%1' to `%2'")
//              .arg(reinterpret_cast<quint64>(context))
//              .arg(reinterpret_cast<quint64>(parentContext)));
//    tsrm_set_interpreter_context(parentContext);

    PQDBG_LVL_DONE();
}

int PlastiQThreadWorker::setReady(PQObjectWrapper *sender, const char *signal,
                                  PQObjectWrapper *receiver, const char *slot,
                                  uint32_t argc, _zval_struct *params, bool dtor_params)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    int idx;

    do {
        idx = qrand();
    } while(activateHash.contains(idx));

    ActivationData *d = new ActivationData;
    d->argc = argc;
    d->dtor_params = dtor_params;
    d->params = params;
    d->receiver = receiver;
    d->sender = sender;
    d->signal = QByteArray(signal);
    d->slot = QByteArray(slot);

    activateHash.insert(idx, d);

    PQDBGLPUP(QString("Emit ready(%1)").arg(idx));
    emit ready(idx);

    PQDBG_LVL_DONE();
    return idx;
}

bool PlastiQThreadWorker::activate(int idx)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    ActivationData *d = activateHash.value(idx);

    PQDBGLPUP(QString("ActivationData `%1`::`%2` -> `%3`::`%4`")
              .arg(reinterpret_cast<quint64>(d->receiver))
              .arg(d->signal.constData())
              .arg(reinterpret_cast<quint64>(d->sender))
              .arg(d->slot.constData()));

    bool ok = PHPQt5::doActivateConnection(d->sender, d->signal,
                                           d->receiver, d->slot,
                                           d->argc, d->params, d->dtor_params);

    activateHash.remove(idx);
    delete d;

    PQDBG_LVL_DONE();
    return ok;
}

void *PlastiQThreadCtxCreator::contextThread()
{
    ctx = ts_resource(0);
    TSRMLS_CACHE_UPDATE();

    SG(server_context) = ((sapi_globals_struct*) (*((void ***) ctx))[TSRM_UNSHUFFLE_RSRC_ID(sapi_globals_id)])->server_context;

    PG(expose_php) = 0;
    PG(auto_globals_jit) = 0;

    php_request_startup();
    PG(during_request_startup) = 0;
    PG(report_memleaks) = 0;

    SG(sapi_started) = 1;
    SG(headers_sent) = 1;
    SG(request_info).no_headers = 1;

    return ctx;
}

void PlastiQThreadCtxCreator::shutdown()
{
    void *octx = tsrm_set_interpreter_context(ctx);
    php_request_shutdown(Q_NULLPTR);
    tsrm_free_interpreter_context(ctx);
    ctx = Q_NULLPTR;
    // ts_free_thread();
    tsrm_set_interpreter_context(octx);
    deleteLater();
}
