/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PQEngine.
**
** BEGIN LICENSE: MPL 2.0
**
** This Source Code Form is subject to the terms of the Mozilla Public
** License, v. 2.0. If a copy of the MPL was not distributed with this
** file, You can obtain one at http://mozilla.org/MPL/2.0/.
**
** END LICENSE
**
****************************************************************************/

#ifndef PQTHREAD_H
#define PQTHREAD_H

#include "pqobject.h"
#include <QThread>

class PQThread : public QThread
{
    Q_OBJECT
    PQ_OBJECT_EX(QThread)

    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

public:
    Q_INVOKABLE explicit PQThread(QObject * parent=0);
    virtual ~PQThread();

    Q_INVOKABLE bool running();

public Q_SLOTS:
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setRunning(bool running);
    Q_INVOKABLE void quit();
    Q_INVOKABLE void terminate();

    void running_pslot(bool running);
    void running_pslot();
    void started_pslot();
    void finished_pslot();

    PQ_ON_SIGNAL_DECL_START {
        PQOBJECT_ON_SIGNALS();
        PQ_ON_SIGNAL(running_pslot(bool));
        PQ_ON_SIGNAL(started_pslot());
        PQ_ON_SIGNAL(finished_pslot());
    } PQ_ON_SIGNAL_DECL_END

signals:
    void runningChanged(bool running);

private:
    bool m_running;
};

Q_DECLARE_METATYPE(QThread*)
Q_DECLARE_METATYPE(PQThread*)

#endif // PQTHREAD_H
