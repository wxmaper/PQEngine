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

#ifndef IPQENGINEEXT_H
#define IPQENGINEEXT_H

#include <QCoreApplication>
#include <QMetaObject>
#include <QMetaMethod>

#include "plastiqmetaobject.h"
#include "plastiqmethod.h"

#define PQENGINEEXT_MAJOR_VERSION 0
#define PQENGINEEXT_MINOR_VERSION 5
#define PQENGINEEXT_RELEASE_VERSION 0
#define PQENGINEEXT_VERSION "0.5"
#define PQENGINEEXT_VERSION_ID 50

class IPQExtension;
class QMetaObjectList : public QList<QMetaObject> {};
class PlastiQMetaObjectList : public QList<PlastiQMetaObject> {};

typedef struct pqext_entry {
    const char *fullName;

    bool                    have_instance;
    bool                    use_instance;
    QCoreApplication *      (*instance)(int argc, char** argv);

    bool                    have_ub_write;
    bool                    use_ub_write;
    void                    (*ub_write)(const QString &msg);

    bool                    have_pre;
    bool                    use_pre;
    void                    (*pre)(const QString &msg, const QString &title);
} PQExtensionEntry;

#define PQEXT_ENTRY_START(extname)\
    PQExtensionEntry entry() { return pqExtEntry; }\
    PQExtensionEntry pqExtEntry = {\
        #extname,

#define PQEXT_NO_INSTANCE false, false, 0,
#define PQEXT_INSTANCE(extname) true, false, extname::instance,

#define PQEXT_NO_UB_WRITE false, false, 0,
#define PQEXT_UB_WRITE(extname) true, false, extname::ub_write,

#define PQEXT_NO_PRE false, false, 0
#define PQEXT_PRE(extname) true, false, extname::pre

#define PQEXT_ENTRY_END\
    };

#define PQEXT_USE(usefn) void use_##usefn() { \
    if(pqExtEntry.have_##usefn) {\
        pqExtEntry.use_##usefn = true; \
    }\
}

class IPQExtension {
public:
    virtual PlastiQMetaObjectList plastiqClasses() { return PlastiQMetaObjectList(); }
    virtual QMetaObjectList classes() = 0;
    virtual bool start() = 0;
    virtual bool finalize() = 0;

    virtual void use_instance() = 0;
    virtual void use_ub_write() = 0;
    virtual void use_pre() = 0;

    virtual PQExtensionEntry entry() = 0;
};

class PQExtensionList : public QList<IPQExtension*> {};

#endif // IPQENGINEEXT_H
