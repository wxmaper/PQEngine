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

#ifndef PQENGINE_H
#define PQENGINE_H

#include "ipqengineext.h"
#include "pqengine_global.h"

typedef struct _PQEngineModule {
    void (*ub_write)(const QString &msg,
                     const QString &title);
    void (*pre)(const QString &msg,
                     const QString &title);
} PQEngineModule;

class PQEnginePrivate;
class PQEngine
{
public:
    PQEngine(PQExtensionList extensions = PQExtensionList());

    bool                    init(int argc,
                                 char **argv,
                                 QString pmd5,
                                 const QString &coreName = "",
                                 bool checkName = false,
                                 const QString &hashKey = "0x0",
                                 const QString &appName = "PQEngine application",
                                 const QString &appVersion = "0.1",
                                 const QString &orgName = "PHPQt5",
                                 const QString &orgDomain = "phpqt.ru");

    int                     exec(const char *script = "-");

    static PQExtensionList  pqeExtensions;
    static QString          pqeCoreName;
    static PQEnginePrivate *pqeEngine;

private:
    static bool pqeInitialized;
};

#endif // PQENGINE_H
