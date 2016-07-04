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

#include "classes/pqthread.h"
#include "pqobject_private.h"

PQOBJECT_STANDARD_METHODS(QThread,QThread)

PQThread::PQThread(QObject *parent) :
    QThread(parent)
{
    m_running = false;
    declareOnSignals();
}

//PQThread::~PQThread() {}

bool PQThread::running() {
    return m_running;
}

void PQThread::start() {
    m_running = true;

    QThread::start();
}

void PQThread::stop() {
    m_running = false;

    QThread::quit();
}

void PQThread::setRunning(bool running)
{
    if (m_running == running)
        return;

    if(running) this->start();
    else this->stop();

    emit runningChanged(running);
}

void PQThread::quit()
{
    this->stop();
}

void PQThread::terminate()
{
    QThread::terminate();
}

void PQThread::running_pslot(bool running)
{
    QVariantList args;
    args << running;
    PHPQt5Connection_invoke(this, "running(bool)", args);
}

void PQThread::running_pslot()
{
    PHPQt5Connection_invoke(this, "running()", QVariantList());
}

void PQThread::started_pslot()
{
    PHPQt5Connection_invoke(this, "started()", QVariantList());
}

void PQThread::finished_pslot()
{
    PHPQt5Connection_invoke(this, "finished()", QVariantList());
}
