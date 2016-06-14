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

#ifndef PQCOREAPPLICATION_H
#define PQCOREAPPLICATION_H

#include "pqobject.h"
#include <QCoreApplication>

class PQCoreApplication;
class PQCoreApplication : public QCoreApplication {
    Q_OBJECT
    PQ_OBJECT_EX(QCoreApplication)

public:
    Q_INVOKABLE explicit PQCoreApplication(int argc, char **argv);
    virtual ~PQCoreApplication();

    Q_INVOKABLE int exec();
    Q_INVOKABLE void quit();
    Q_INVOKABLE int applicationPid();

    Q_INVOKABLE bool isQuitLockEnabled();
    Q_INVOKABLE void setQuitLockEnabled(bool enabled);

    Q_INVOKABLE QString applicationName() const;
    Q_INVOKABLE void setApplicationName(const QString &applicationName);

    Q_INVOKABLE QString applicationVersion() const;
    Q_INVOKABLE void setApplicationVersion(const QString &version);

    Q_INVOKABLE QString organizationName() const;
    Q_INVOKABLE void setOrganizationName(const QString &orgName);

    Q_INVOKABLE QString organizationDomain() const;
    Q_INVOKABLE void setOrganizationDomain(const QString &orgDomain);

    Q_INVOKABLE QString applicationDirPath() const;
    Q_INVOKABLE QString applicationFilePath() const;
};

#endif // PQCOREAPPLICATION_H

