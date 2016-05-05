#include "pqobject.h"
#include "pqobject_private.h"

PQOBJECT_STANDARD_METHODS(QObject,QObject)

PQObject::PQObject(QObject *parent)
    : QObject(parent)
{
}

PQObject::~PQObject()
{
}

