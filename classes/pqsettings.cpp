#include "pqsettings.h"

PQSettings::PQSettings(QObject *parent)
    : QSettings(parent)
{
}

PQSettings::PQSettings(const QString &organization,
                       const QString &application,
                       QObject *parent)
    : QSettings(organization, application, parent)
{
}

PQSettings::PQSettings(int scope,
                       const QString &organization,
                       const QString &application,
                       QObject *parent)
    : QSettings(Scope(scope), organization, application, parent)
{
}

PQSettings::PQSettings(int format,
                       int scope,
                       const QString &organization,
                       const QString &application,
                       QObject *parent)
    : QSettings(Format(format), Scope(scope), organization, application, parent)
{
}

PQSettings::PQSettings(const QString &fileName,
                       int format,
                       QObject *parent)
    : QSettings(fileName, Format(format), parent)
{
}

PQSettings::~PQSettings(){}

QStringList PQSettings::allKeys() const
{
    return QSettings::allKeys();
}

QString PQSettings::applicationName() const
{
    return QSettings::applicationName();
}

void PQSettings::clear()
{
    QSettings::clear();
}

bool PQSettings::contains(const QString &key) const
{
    return QSettings::contains(key);
}

QString PQSettings::fileName() const
{
    return QSettings::fileName();
}

int PQSettings::format()
{
    return QSettings::format();
}

bool PQSettings::isWritable() const
{
    return QSettings::isWritable();
}

QString PQSettings::organizationName() const
{
    return QSettings::organizationName();
}

void PQSettings::remove(const QString &key)
{
    QSettings::remove(key);
}

int PQSettings::scope()
{
    return QSettings::scope();
}

void PQSettings::setValue(const QString &key, const QString &value)
{
    QSettings::setValue(key, value);
}

void PQSettings::setValue(const QString &key, bool &value)
{
    QSettings::setValue(key, value);
}

void PQSettings::setValue(const QString &key, int &value)
{
    QSettings::setValue(key, value);
}

void PQSettings::setValue(const QString &key, double &value)
{
    QSettings::setValue(key, value);
}

QVariant PQSettings::value(const QString &key) const
{
    return QSettings::value(key);
}

QVariant PQSettings::value(const QString &key, const QString &defaultValue) const
{
    return QSettings::value(key, defaultValue);
}

QVariant PQSettings::value(const QString &key, int defaultValue) const
{
    return QSettings::value(key, defaultValue);
}

QVariant PQSettings::value(const QString &key, bool defaultValue) const
{
    return QSettings::value(key, defaultValue);
}

QVariant PQSettings::value(const QString &key, double defaultValue) const
{
    return QSettings::value(key, defaultValue);
}

int PQSettings::status()
{
    return QSettings::status();
}

void PQSettings::sync()
{
    QSettings::sync();
}

QStringList PQSettings::childGroups() const
{
    return QSettings::childGroups();
}

QStringList PQSettings::childKeys() const
{
    return QSettings::childKeys();
}

void PQSettings::beginGroup(const QString &prefix)
{
    QSettings::beginGroup(prefix);
}

QString PQSettings::group() const
{
    return QSettings::group();
}

void PQSettings::endGroup()
{
    QSettings::endGroup();
}

void PQSettings::setPath(int format, int scope, const QString &path)
{
    QSettings::setPath(Format(format), Scope(scope), path);
}

