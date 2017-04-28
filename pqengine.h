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

struct PQEngineInitConf {
    QString appName = "PQEngine application";
    QString appVersion = "1.0";
    QString orgName = "PHPQt5";
    QString orgDomain = "phpqt.ru";
    QString hashKey = "0x0";
    QString pmd5;
    QString debugSocketName = "PQEngine Debug Server";
    bool checkName = false;
};

class PQEnginePrivate;
class PQEngine
{
public:
    PQEngine(PQExtensionList extensions = PQExtensionList());

    bool                    init(int argc,
                                 char **argv,
                                 const QString &coreName,
                                 const PQEngineInitConf &ic);

    int                     exec(const char *script = "-");

    static PQExtensionList  pqeExtensions;
    static QString          pqeCoreName;
    static PQEnginePrivate *pqeEngine;

private:
    static bool pqeInitialized;
};

#endif // PQENGINE_H
