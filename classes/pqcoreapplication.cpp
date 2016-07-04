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

#include "pqcoreapplication.h"
#include "pqobject_private.h"

PQOBJECT_STANDARD_METHODS(QCoreApplication,QCoreApplication)

PQCoreApplication::PQCoreApplication(int argc, char **argv) :
    QCoreApplication(argc, argv)
{
}

//PQCoreApplication::~PQCoreApplication(){}

int PQCoreApplication::exec()
{
    return QCoreApplication::exec();
}

void PQCoreApplication::quit()
{
    QCoreApplication::quit();
}

int PQCoreApplication::applicationPid()
{
    return QCoreApplication::applicationPid();
}

QString PQCoreApplication::applicationVersion() const
{
    return QCoreApplication::applicationVersion();
}

void PQCoreApplication::setApplicationVersion(const QString &version)
{
    QCoreApplication::setApplicationVersion(version);
}

bool PQCoreApplication::isQuitLockEnabled()
{
    return QCoreApplication::isQuitLockEnabled();
}

void PQCoreApplication::setQuitLockEnabled(bool enabled)
{
    QCoreApplication::setQuitLockEnabled(enabled);
}

QString PQCoreApplication::applicationName() const
{
    return QCoreApplication::applicationName();
}

void PQCoreApplication::setApplicationName(const QString &applicationName)
{
    QCoreApplication::setApplicationName(applicationName);
}

QString PQCoreApplication::organizationName() const
{
    return QCoreApplication::organizationName();
}

void PQCoreApplication::setOrganizationName(const QString &orgName)
{
    QCoreApplication::setOrganizationName(orgName);
}

QString PQCoreApplication::organizationDomain() const
{
    return QCoreApplication::organizationDomain();
}

void PQCoreApplication::setOrganizationDomain(const QString &orgDomain)
{
    QCoreApplication::setOrganizationDomain(orgDomain);
}

QString PQCoreApplication::applicationDirPath() const
{
    return QCoreApplication::applicationDirPath();
}

QString PQCoreApplication::applicationFilePath() const
{
    return QCoreApplication::applicationFilePath();
}
