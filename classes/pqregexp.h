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

#ifndef PQREGEXP_H
#define PQREGEXP_H

#include "pqobject.h"
#include <QRegExp>

class PQRegExp;
class PQRegExp : public QObject, public QRegExp {
    Q_OBJECT
    PQ_OBJECT_EX(QObject)

    Q_PROPERTY(int matchedLength READ matchedLength)
    Q_PROPERTY(int captureCount READ captureCount)
    Q_PROPERTY(const QString errorString READ errorString)
    Q_PROPERTY(bool isMinimal READ isMinimal)
    Q_PROPERTY(bool isEmpty READ isEmpty)
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(const QString pattern READ pattern WRITE setPattern)

public:
    Q_INVOKABLE explicit PQRegExp(const QString &pattern = "", QObject *parent = 0);
    Q_INVOKABLE int indexIn(const QString &str, int offset = 0, int caretMode = CaretAtZero);
    Q_INVOKABLE int lastIndexIn(const QString &str, int offset = -1, int caretMode = CaretAtZero);

    Q_INVOKABLE bool isEmpty();
    Q_INVOKABLE bool isValid();
    Q_INVOKABLE QString pattern();
    Q_INVOKABLE void setPattern(const QString &pattern);

    Q_INVOKABLE bool isMinimal();
    Q_INVOKABLE void setMinimal(bool minimal);

    Q_INVOKABLE bool exactMatch(const QString &str);

    Q_INVOKABLE int matchedLength();

    Q_INVOKABLE int captureCount();
    Q_INVOKABLE QStringList capturedTexts();

    Q_INVOKABLE QString errorString();
};


#endif // PQREGEXP_H
