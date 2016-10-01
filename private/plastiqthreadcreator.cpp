#include "plastiqthreadcreator.h"

#include "phpqt5.h"

PlastiQThreadCreator::PlastiQThreadCreator(QThread *thread, void *tsrmls_cache, QObject *parent)
    : QObject(parent)
{
    threads.insert(thread, tsrmls_cache);
}

void *PlastiQThreadCreator::get_tsrmls_cache(QThread *thread)
{
    void *tsrmls_cache = threads.value(thread, Q_NULLPTR);

    //QThread *old_thread = this->thread();

    if(!tsrmls_cache) {
#ifdef PQDEBUG
        PQDBG_LVL_START(__FUNCTION__);
#endif
        //void *old_tsrmls_cache = tsrm_get_ls_cache();
        //this->moveToThread(thread);

        PQDBGLPUP("tsrm_new_interpreter_context");
        tsrmls_cache = tsrm_new_interpreter_context();

        //PQDBGLPUP("tsrm_set_interpreter_context");
        //tsrm_set_interpreter_context(tsrmls_cache);

        PQDBGLPUP("php_request_startup");
        php_request_startup();

        connect(thread, SIGNAL(destroyed(QObject*)), this, SLOT(free_tsrmls_cache(QObject*)));
        threads.insert(thread, tsrmls_cache);

        //this->moveToThread(old_thread);

        //PQDBGLPUP("tsrm_set_interpreter_context");
        //tsrm_set_interpreter_context(old_tsrmls_cache);

        PQDBG_LVL_DONE();
    }

    return tsrmls_cache;
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

    emit ready(idx);
    return idx;
}

bool PlastiQThreadWorker::activate(int idx)
{
    ActivationData *d = activateHash.value(idx);

    PHPQt5::threadCreator()->PlastiQThreadCreator::get_tsrmls_cache(QThread::currentThread());

    return PHPQt5::doActivateConnection(d->sender, d->signal,
                                        d->receiver, d->slot,
                                        d->argc, d->params, d->dtor_params);

    activateHash.remove(idx);
    delete d;
}
