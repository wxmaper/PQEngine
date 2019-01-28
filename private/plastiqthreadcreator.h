#ifndef PLASTIQTHREADCREATOR_H
#define PLASTIQTHREADCREATOR_H

#include <QObject>
#include <QThread>
#include <QHash>

struct PQObjectWrapper;
struct _zval_struct;
struct _sapi_module_struct;

class PlastiQThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit PlastiQThreadWorker(QThread *thread, QObject *parent = Q_NULLPTR);
    int setReady(PQObjectWrapper *sender, const char *signal,
                 PQObjectWrapper *receiver, const char *slot,
                 uint32_t argc, _zval_struct *params, bool dtor_params);

    struct ActivationData {
        PQObjectWrapper *sender;
        QByteArray signal;
        PQObjectWrapper *receiver;
        QByteArray slot;
        uint32_t argc;
        _zval_struct *params;
        bool dtor_params;
    };

signals:
    void ready(int idx);

public slots:
    bool activate(int idx);

private:
    QHash<int,ActivationData*> activateHash;
};

class PlastiQThreadCtxCreator : public QObject
{
    Q_OBJECT
public:
    void *contextThread();
    void *ctx = Q_NULLPTR;

public slots:
    void shutdown();
};

class PlastiQThreadCreator : public QObject
{
    Q_OBJECT
public:
    explicit PlastiQThreadCreator(QThread *thread, void *tsrmls_cache, QObject *parent = Q_NULLPTR);
    //static void *get_tsrmls_cache(QThread *thread);
    void *setContextThread(QThread *thread);
    QThread *getThread(void *tsrmls_cache);

    void *contextThread(QThread *thread);

public slots:
    void free_tsrmls_cache(QObject *threadObject);

private:
    QHash<QThread*, void*> threads;
};

#endif // PLASTIQTHREADCREATOR_H
