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

#ifndef PQSETTINGS_H
#define PQSETTINGS_H

#include "pqobject.h"
#include <QSettings>

class PQSettings;
class PQSettings : public QSettings {
    Q_OBJECT
    PQ_OBJECT_EX(QSettings)

public:
    Q_INVOKABLE explicit PQSettings(QObject *parent = 0);
    Q_INVOKABLE explicit PQSettings(const QString &organization,
                                    const QString &application = QString(),
                                    QObject *parent = 0);
    Q_INVOKABLE explicit PQSettings(int scope, const QString &organization,
                           const QString &application = QString(),
                           QObject *parent = 0);
    Q_INVOKABLE explicit PQSettings(int format, int scope, const QString &organization,
                           const QString &application = QString(),
                           QObject *parent = 0);
    Q_INVOKABLE explicit PQSettings(const QString &fileName, int format,
                           QObject *parent = 0);
    virtual ~PQSettings();

    Q_INVOKABLE QStringList allKeys() const;
    Q_INVOKABLE QString applicationName() const;
    Q_INVOKABLE void clear();
    Q_INVOKABLE bool contains(const QString &key) const;
    Q_INVOKABLE QString	fileName() const;
    Q_INVOKABLE int format();
    Q_INVOKABLE bool isWritable() const;
    Q_INVOKABLE QString	organizationName() const;
    Q_INVOKABLE void remove(const QString &key);
    Q_INVOKABLE int scope();
    /*
    Q_INVOKABLE void setValue(const QString &key, const QByteArray &value);
    Q_INVOKABLE void setValue(const QString &key, const QString &value);
    Q_INVOKABLE void setValue(const QString &key, bool &value);
    Q_INVOKABLE void setValue(const QString &key, int &value);
    Q_INVOKABLE void setValue(const QString &key, double &value);
    Q_INVOKABLE QVariant value(const QString &key) const;
    Q_INVOKABLE QVariant value(const QString &key, const QString &defaultValue) const;
    Q_INVOKABLE QVariant value(const QString &key, bool defaultValue) const;
    Q_INVOKABLE QVariant value(const QString &key, int defaultValue) const;
    Q_INVOKABLE QVariant value(const QString &key, double defaultValue) const;
    */

    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = QVariant());

    Q_INVOKABLE int status();
    Q_INVOKABLE void sync();

    Q_INVOKABLE QStringList	childGroups() const;
    Q_INVOKABLE QStringList	childKeys() const;

    Q_INVOKABLE void beginGroup(const QString &prefix);
    Q_INVOKABLE QString group() const;
    Q_INVOKABLE void endGroup();

    Q_INVOKABLE void setPath(int format, int scope, const QString &path);
};

#endif // PQSETTINGS_H
