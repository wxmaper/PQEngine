#include "pqevent.h"
#include "pqobject_private.h"

PQOBJECT_STANDARD_METHODS(QEvent,PQObject)

PQEvent::PQEvent(int type)
    : PQObject()
{

}

PQEvent::PQEvent(QEvent *other)
    : PQObject(), m_event(other)
{
}

PQEvent::~PQEvent()
{

}

