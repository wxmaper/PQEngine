#ifndef PLASTIQQOBJECT_H
#define PLASTIQQOBJECT_H

#include "plastiqobject.h"

class PlastiQQObject : public PlastiQObject
{
    PLASTIQ_OBJECT(IsQObject,QObject,Q_NULLPTR)

    PQ_CONSTRUCTOR(QObject(QObject*=))

    PQ_PUBLIC_METHOD(QString objectName())
    PQ_PUBLIC_METHOD(void setObjectName(const QString &))
    PQ_PUBLIC_METHOD(bool isWidgetType())
    PQ_PUBLIC_METHOD(bool isWindowType())
    PQ_PUBLIC_METHOD(bool signalsBlocked())
    PQ_PUBLIC_METHOD(bool blockSignals(bool))
    PQ_PUBLIC_METHOD(QThread* thread())
    PQ_PUBLIC_METHOD(void moveToThread(QThread*))
    PQ_PUBLIC_METHOD(void killTimer(int))
    PQ_PUBLIC_METHOD(void setParent(QObject*))
    PQ_PUBLIC_METHOD(void installEventFilter(QObject*))
    PQ_PUBLIC_METHOD(void removeEventFilter(QObject*))
    PQ_PUBLIC_METHOD(bool setProperty(char*, QVariant))
    PQ_PUBLIC_METHOD(QVariant property(char*))
    PQ_PUBLIC_METHOD(void deleteLater())
};

#endif // PLASTIQQOBJECT_H
