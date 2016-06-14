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

#include "pqregexp.h"
#include "pqobject_private.h"

PQOBJECT_STANDARD_METHODS(QRegExp,QObject)

PQRegExp::PQRegExp(const QString &pattern, QObject *parent)
    : QRegExp(pattern), QObject(parent)
{
}

int PQRegExp::indexIn(const QString &str, int offset, int caretMode)
{
    return QRegExp::indexIn(str, offset, CaretMode(caretMode));
}

int PQRegExp::lastIndexIn(const QString &str, int offset, int caretMode)
{
    return QRegExp::lastIndexIn(str, offset, CaretMode(caretMode));
}

bool PQRegExp::isEmpty()
{
    return QRegExp::isEmpty();
}

bool PQRegExp::isValid()
{
    return QRegExp::isValid();
}

QString PQRegExp::pattern()
{
    return QRegExp::pattern();
}

void PQRegExp::setPattern(const QString &pattern)
{
    QRegExp::setPattern(pattern);
}

bool PQRegExp::isMinimal()
{
    return QRegExp::isMinimal();
}

void PQRegExp::setMinimal(bool minimal)
{
    QRegExp::setMinimal(minimal);
}

bool PQRegExp::exactMatch(const QString &str)
{
    return QRegExp::exactMatch(str);
}

int PQRegExp::matchedLength()
{
    return QRegExp::matchedLength();
}

int PQRegExp::captureCount()
{
    return QRegExp::captureCount();
}

QStringList PQRegExp::capturedTexts()
{
    return QRegExp::capturedTexts();
}

QString PQRegExp::errorString()
{
    return QRegExp::errorString();
}
