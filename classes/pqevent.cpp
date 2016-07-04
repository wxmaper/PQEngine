/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PQEngineCore extension of the PQEngine.
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

//PQEvent::~PQEvent(){}

