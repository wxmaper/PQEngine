#ifndef PQTIMER
#define PQTIMER

#include "pqobject.h"
#include <QTimer>

class PQTimer;
class PQTimer : public QTimer {
    Q_OBJECT
    PQ_OBJECT_EX(QTimer)

    Q_PROPERTY( int interval READ interval WRITE setInterval )
    Q_PROPERTY( bool running READ isRunning WRITE setRunning )
    Q_PROPERTY( bool active READ isActive WRITE setActive )
    Q_PROPERTY( bool singleShot READ isSingleShot WRITE setSingleShot )

public:
    Q_INVOKABLE explicit PQTimer(QObject *parent = 0);
    virtual ~PQTimer();

    Q_INVOKABLE int interval();
    Q_INVOKABLE bool isActive();
    Q_INVOKABLE bool isRunning();
    Q_INVOKABLE bool isSingleShot();

public Q_SLOTS:
    Q_INVOKABLE void setInterval(int interval);
    Q_INVOKABLE void start();
    Q_INVOKABLE void start(int interval);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setRunning(bool running);
    Q_INVOKABLE void setActive(bool active);
    Q_INVOKABLE void setSingleShot(bool on);

    void timeout_pslot();

    PQ_ON_SIGNAL_DECL_START {
        PQOBJECT_ON_SIGNALS();
        PQ_ON_SIGNAL(timeout_pslot());
    } PQ_ON_SIGNAL_DECL_END
};

#endif // PQTIMER

