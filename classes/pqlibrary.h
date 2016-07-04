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

#ifndef PQLIBRARY_H
#define PQLIBRARY_H

#include <QLibrary>
#include "pqobject.h"

class PQLibrary : public QLibrary
{
    Q_OBJECT
    PQ_OBJECT_EX(QLibrary)

public:
    Q_INVOKABLE explicit PQLibrary(QObject *parent = Q_NULLPTR);
    Q_INVOKABLE explicit PQLibrary(const QString& fileName, QObject *parent = Q_NULLPTR);
    Q_INVOKABLE explicit PQLibrary(const QString& fileName, int verNum, QObject *parent = Q_NULLPTR);
    Q_INVOKABLE explicit PQLibrary(const QString& fileName, const QString &version, QObject *parent = Q_NULLPTR);
    //~PQLibrary();

   // Q_INVOKABLE QFunctionPointer resolve(const QString &symbol);
   // static QFunctionPointer resolve(const QString &fileName, const char *symbol);
   // static QFunctionPointer resolve(const QString &fileName, int verNum, const char *symbol);
   // static QFunctionPointer resolve(const QString &fileName, const QString &version, const char *symbol);

    Q_INVOKABLE bool load();
    Q_INVOKABLE bool unload();
    Q_INVOKABLE bool isLoaded() const;

    Q_INVOKABLE bool isLibrary(const QString &fileName);

    Q_INVOKABLE void setFileName(const QString &fileName);
    Q_INVOKABLE QString fileName() const;

    Q_INVOKABLE void setFileNameAndVersion(const QString &fileName, int verNum);
    Q_INVOKABLE void setFileNameAndVersion(const QString &fileName, const QString &version);
    Q_INVOKABLE QString errorString() const;

    Q_INVOKABLE void setLoadHints(int hints);
    Q_INVOKABLE int loadHints() const;

    Q_INVOKABLE void call(const QString &functionSignature, int i);

public Q_SLOTS:
    PQ_ON_SIGNAL_DECL_START {
        PQOBJECT_ON_SIGNALS();
    } PQ_ON_SIGNAL_DECL_END

private:
    QMap<QString, void*> m_resolvedFunctions;
    QMap<QString, int> m_types;
};

#endif // PQLIBRARY_H
