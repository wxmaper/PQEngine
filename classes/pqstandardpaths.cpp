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

#include "pqstandardpaths.h"
#include "pqobject_private.h"

PQOBJECT_STANDARD_METHODS(QStandardPaths,PQObject)

PQStandardPaths::PQStandardPaths()
    : PQObject()
{
    declareOnSignals();
}

PQStandardPaths::~PQStandardPaths()
{

}

QString PQStandardPaths::writableLocation(int type)
{
    return QStandardPaths::writableLocation(QStandardPaths::StandardLocation(type));
}

QString PQStandardPaths::static_writableLocation(int type)
{
    return QStandardPaths::QStandardPaths::writableLocation(QStandardPaths::StandardLocation(type));
}

QStringList PQStandardPaths::standardLocations(int type)
{
    return QStandardPaths::standardLocations(QStandardPaths::StandardLocation(type));
}

QStringList PQStandardPaths::static_standardLocations(int type)
{
    return QStandardPaths::QStandardPaths::standardLocations(QStandardPaths::StandardLocation(type));
}

QString PQStandardPaths::locate(int type, const QString &fileName, int options)
{
    return QStandardPaths::locate(QStandardPaths::StandardLocation(type),
                                  fileName,
                                  QStandardPaths::LocateOptions(options));
}

QString PQStandardPaths::static_locate(int type, const QString &fileName, int options)
{
    return QStandardPaths::QStandardPaths::locate(QStandardPaths::StandardLocation(type),
                                  fileName,
                                  QStandardPaths::LocateOptions(options));
}

QStringList PQStandardPaths::locateAll(int type, const QString &fileName, int options)
{
    return QStandardPaths::locateAll(QStandardPaths::StandardLocation(type),
                                     fileName,
                                     QStandardPaths::LocateOptions(options));
}

QStringList PQStandardPaths::static_locateAll(int type, const QString &fileName, int options)
{
    return QStandardPaths::QStandardPaths::locateAll(QStandardPaths::StandardLocation(type),
                                     fileName,
                                     QStandardPaths::LocateOptions(options));
}

QString PQStandardPaths::displayName(int type)
{
    return QStandardPaths::displayName(QStandardPaths::StandardLocation(type));
}

QString PQStandardPaths::static_displayName(int type)
{
    return QStandardPaths::QStandardPaths::displayName(QStandardPaths::StandardLocation(type));
}

QString PQStandardPaths::findExecutable(const QString &executableName, const QStringList &paths)
{
    return QStandardPaths::findExecutable(executableName, paths);
}

QString PQStandardPaths::static_findExecutable(const QString &executableName, const QStringList &paths)
{
    return QStandardPaths::QStandardPaths::findExecutable(executableName, paths);
}

void PQStandardPaths::setTestModeEnabled(bool testMode)
{
    QStandardPaths::setTestModeEnabled(testMode);
}

void PQStandardPaths::static_setTestModeEnabled(bool testMode)
{
    QStandardPaths::QStandardPaths::setTestModeEnabled(testMode);
}

bool PQStandardPaths::isTestModeEnabled()
{
    return QStandardPaths::isTestModeEnabled();
}

bool PQStandardPaths::static_isTestModeEnabled()
{
    return QStandardPaths::QStandardPaths::isTestModeEnabled();
}

