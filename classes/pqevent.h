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
