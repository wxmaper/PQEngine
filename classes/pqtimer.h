#ifndef PQTIMER
#define PQTIMER

#include "pqobject.h"
#include <QTimer>

class PQTimer;
class PQTimer : public QTimer
{
    Q_OBJECT
    PQ_OBJECT

    Q_PROPERTY( int interval READ interval WRITE setInterval )
    Q_PROPERTY( bool running READ isRunning WRITE setRunning )
    Q_PROPERTY( bool active READ isActive WRITE setActive )

public:
    Q_INVOKABLE explicit PQTimer(QObject *parent = 0);
    virtual ~PQTimer();

    Q_INVOKABLE int interval();
    Q_INVOKABLE bool isActive();
    Q_INVOKABLE bool isRunning();

public Q_SLOTS:
    Q_INVOKABLE void setInterval(int interval);
    Q_INVOKABLE void start();
    Q_INVOKABLE void start(int interval);
    Q_INVOKABLE void stop();
    Q_INVOKABLE bool setRunning(bool running);
    Q_INVOKABLE bool setActive(bool active);

    void timeout_pslot();
};

#endif // PQTIMER

