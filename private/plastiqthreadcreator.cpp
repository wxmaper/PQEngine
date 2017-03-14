#include "plastiqthreadcreator.h"

#include "phpqt5.h"

PlastiQThreadCreator::PlastiQThreadCreator(QThread *thread, void *tsrmls_cache, QObject *parent)
    : QObject(parent)
{
    threads.insert(thread, tsrmls_cache);
}

void *PlastiQThreadCreator::get_tsrmls_cache(QThread *thread)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    static QHash<QThread*, void*> tsrmls_c;

    void *TSRMLS_CACHE = tsrmls_c.value(thread, Q_NULLPTR);

    if(!TSRMLS_CACHE) {

        // this->moveToThread(thread);

        PQDBGLPUP(QString("tsrm_new_interpreter_context in thread: %1")
                  .arg(reinterpret_cast<quint64>(QThread::currentThread())));
        TSRMLS_CACHE = tsrm_new_interpreter_context();

        tsrm_set_interpreter_context(TSRMLS_CACHE);
        PQDBGLPUP(QString("TSRMLS_CACHE: %1").arg(reinterpret_cast<quint64>(TSRMLS_CACHE)));
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

QThread *PlastiQThreadCreator::getThread(void *tsrmls_cache)
{
    QThread *thread = threads.key(tsrmls_cache);
    return thread;
}

void PlastiQThreadCreator::free_tsrmls_cache(QObject *threadObject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

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

PlastiQThreadWorker::PlastiQThreadWorker(QObject *parent)
    : QObject(parent)
{
}

int PlastiQThreadWorker::setReady(PQObjectWrapper *sender, const char *signal,
                                  PQObjectWrapper *receiver, const char *slot,
                                  int argc, _zval_struct *params, bool dtor_params)
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


    //void *old_tsrmls_cache = d->sender->ctx;
    //PQDBGLPUP(QString("old_tsrmls_cache: %1").arg(reinterpret_cast<quint64>(old_tsrmls_cache)));
    //void *tsrmls_cache = PHPQt5::threadCreator()->get_tsrmls_cache(old_tsrmls_cache, QThread::currentThread());

    //PQDBGLPUP(QString("tsrm_set_interpreter_context from `%1` to `%2`")
    //          .arg(reinterpret_cast<quint64>(old_tsrmls_cache))
    //          .arg(reinterpret_cast<quint64>(tsrmls_cache)));
    //PQDBGLPUP(QString("QThread: %1").arg(reinterpret_cast<quint64>(QThread::currentThread())));

    // tsrm_set_interpreter_context(tsrmls_cache);

    bool ok = PHPQt5::doActivateConnection(d->sender, d->signal,
                                           d->receiver, d->slot,
                                           d->argc, d->params, d->dtor_params);

    //PQDBGLPUP(QString("tsrm_set_interpreter_context from `%1` to `%2`")
    //          .arg(reinterpret_cast<quint64>(tsrmls_cache))
    //          .arg(reinterpret_cast<quint64>(old_tsrmls_cache)));

    // tsrm_set_interpreter_context(old_tsrmls_cache);

    activateHash.remove(idx);
    delete d;

    PQDBG_LVL_DONE();
    return ok;
}
