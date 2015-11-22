#ifndef PQTHREAD
#define PQTHREAD

#include "pqobject.h"
#include <QThread>

class PQThread : public QThread {
    Q_OBJECT
    PQ_OBJECT

public:
    Q_INVOKABLE explicit PQThread(QObject * parent=0);
    virtual ~PQThread();

    Q_INVOKABLE bool isRunning();

public Q_SLOTS:
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

private:
    bool mRunning;
};

#endif // PQTHREAD

