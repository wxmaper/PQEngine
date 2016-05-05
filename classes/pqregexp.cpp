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
