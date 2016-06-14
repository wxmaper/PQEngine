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

#include "pqtimer.h"
#include "pqobject_private.h"

PQOBJECT_STANDARD_METHODS(QTimer,QTimer)

PQTimer::PQTimer(QObject *parent) :
    QTimer(parent)
{
    declareOnSignals();
}

PQTimer::~PQTimer() {}

int PQTimer::interval() {
    return QTimer::interval();
}

bool PQTimer::isActive() {
    return QTimer::isActive();
}

bool PQTimer::isRunning() {
    return QTimer::isActive();
}

bool PQTimer::isSingleShot()
{
    return QTimer::isSingleShot();
}

void PQTimer::setInterval(int interval) {
    QTimer::setInterval(interval);
}

void PQTimer::start() {
    QTimer::start();
}

void PQTimer::start(int interval) {
    QTimer::start(interval);
}

void PQTimer::stop() {
    QTimer::stop();
}

void PQTimer::setRunning(bool running)
{
    running ? this->start() : this->stop();
}

void PQTimer::setActive(bool active)
{
    active ? this->start() : this->stop();
}

void PQTimer::setSingleShot(bool on)
{
    QTimer::setSingleShot(on);
}

void PQTimer::timeout_pslot()
{
    QVariantList args;
    PHPQt5Connection_invoke(this, "timeout()", args);
}
