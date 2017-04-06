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

#include "pqenginecore.h"

//#include "plastiqclasses/core/plastiqqobject.h"
//#include "plastiqclasses/core/plastiqqtimer.h"
#include <QSettings>
#include <QStandardPaths>

QMetaObjectList PQEngineCore::classes() {
    QMetaObjectList classes;

    return classes;
}

bool PQEngineCore::start() {
    return true;
}

bool PQEngineCore::finalize() {
    return true;
}

void PQEngineCore::ub_write(const QString &msg)
{
    default_ub_write(msg, "");
}

void PQEngineCore::pre(const QString &msg, const QString &title)
{
    default_ub_write(msg, title);
}
