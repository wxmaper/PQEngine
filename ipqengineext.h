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
class PlastiQ::IPlastiQUi;
class QMetaObjectList : public QList<QMetaObject> {};
class PlastiQMetaObjectList : public QList<PlastiQMetaObject> {};
typedef QHash<QByteArray,PlastiQ::IPlastiQUi*> PlastiQUiHash;

class IPQExtension {
public:
    virtual PlastiQMetaObjectList plastiqClasses() { return PlastiQMetaObjectList(); }
    virtual PlastiQUiHash plastiqForms() = 0;
    virtual QMetaObjectList classes() = 0;
    virtual bool start() = 0;
    virtual bool finalize() = 0;
};

class PQExtensionList : public QList<IPQExtension*> {};

#endif // IPQENGINEEXT_H
