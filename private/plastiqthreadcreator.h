#ifndef PLASTIQTHREADCREATOR_H
#define PLASTIQTHREADCREATOR_H

#include <QObject>
#include <QThread>
#include <QHash>

struct PQObjectWrapper;
struct _zval_struct;


class PlastiQThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit PlastiQThreadWorker(QObject *parent = 0);
    int setReady(PQObjectWrapper *sender, const char *signal,
                 PQObjectWrapper *receiver, const char *slot,
                 int argc, _zval_struct *params, bool dtor_params);

    struct ActivationData {
        PQObjectWrapper *sender;
        QByteArray signal;
        PQObjectWrapper *receiver;
        QByteArray slot;
        int argc;
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

class PlastiQThreadCreator : public QObject
{
    Q_OBJECT
public:
    explicit PlastiQThreadCreator(QThread *thread, void *tsrmls_cache, QObject *parent = 0);
    static void *get_tsrmls_cache(QThread *thread);
    QThread *getThread(void *tsrmls_cache);

public slots:
    void free_tsrmls_cache(QObject *threadObject);

private:
    QHash<QThread*, void*> threads;
};

#endif // PLASTIQTHREADCREATOR_H
