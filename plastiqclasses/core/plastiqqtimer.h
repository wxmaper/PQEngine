#ifndef PLASTIQQTIMER_H
#define PLASTIQQTIMER_H

#include "plastiqobject.h"

class PlastiQQTimer : public PlastiQObject
{
    PLASTIQ_OBJECT(IsQObject,QTimer,QObject)

    PQ_CONSTRUCTOR(QTimer(QObject*=))

    PQ_PUBLIC_METHOD(bool isActive())
    PQ_PUBLIC_METHOD(int timerId())
    PQ_PUBLIC_METHOD(void setInterval(int))
    PQ_PUBLIC_METHOD(int interval())
    PQ_PUBLIC_METHOD(int remainingTime())
    //PQ _PUBLIC_METHOD(void setTimerType(Qt::TimerType))
    //PQ _PUBLIC_METHOD(Qt::TimerType timerType())
    PQ_PUBLIC_METHOD(void setSingleShot(bool))
    PQ_PUBLIC_METHOD(bool isSingleShot())
    PQ_PUBLIC_STATIC_METHOD(void singleShot(int, const QObject *, const char *))
    //PQ _PUBLIC_STATIC_METHOD(void singleShot(int, Qt::TimerType, const QObject *, const char *))
    PQ_PUBLIC_SLOT(void start(int))
    PQ_PUBLIC_SLOT(void start())
    PQ_PUBLIC_SLOT(void stop())

    PQ_PUBLIC_SIGNAL(void timeout())
};

#endif // PLASTIQQTIMER_H
