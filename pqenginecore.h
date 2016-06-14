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

#ifndef PQENGINECORE_H
#define PQENGINECORE_H

#include "pqengine.h"

class PQDLAPI PQEngineCore : public IPQExtension {
public:
    QMetaObjectList classes();
    bool start();
    bool finalize();

    static QCoreApplication * instance(int argc, char** argv);
    static void ub_write(const QString &msg);
    static void pre(const QString &msg, const QString &title);

    PQEXT_USE(instance)
    PQEXT_USE(ub_write)
    PQEXT_USE(pre)

    PQEXT_ENTRY_START(PQEngineCore)
        PQEXT_INSTANCE(PQEngineCore)
        PQEXT_UB_WRITE(PQEngineCore)
        PQEXT_PRE(PQEngineCore)
    PQEXT_ENTRY_END
};

#endif // PQENGINECORE_H
