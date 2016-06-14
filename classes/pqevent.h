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

#ifndef PQEVENT_H
#define PQEVENT_H

#include <QEvent>
#include "pqobject.h"

class PQEvent : public PQObject
{
    Q_OBJECT
    PQ_OBJECT_EX(QEvent)

    Q_PROPERTY(int type READ type)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    Q_INVOKABLE explicit PQEvent(int type);
    explicit PQEvent(QEvent *other);
    ~PQEvent();

    Q_INVOKABLE int type() const { return m_event->type(); }
    Q_INVOKABLE bool spontaneous() const { return m_event->spontaneous(); }

    Q_INVOKABLE void setAccepted(bool accepted) { m_event->setAccepted(accepted); }
    Q_INVOKABLE bool isAccepted() const { return m_event->isAccepted(); }

    Q_INVOKABLE void accept() { m_event->accept(); }
    Q_INVOKABLE void ignore() { m_event->ignore(); }

    QEvent *getEvent() { return m_event; }
    void setEvent(QEvent *event) { m_event = event; }

public slots:
    PQ_ON_SIGNAL_DECL_START {
        PQOBJECT_ON_SIGNALS();
    } PQ_ON_SIGNAL_DECL_END

private:
    QEvent *m_event = Q_NULLPTR;
};

Q_DECLARE_METATYPE(PQEvent*)

#endif // PQEVENT_H
