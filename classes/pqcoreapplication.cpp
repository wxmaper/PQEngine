#include "pqcoreapplication.h"
#include "pqobject_private.h"

PQOBJECT_STANDARD_METHODS(QCoreApplication,QCoreApplication)

PQCoreApplication::PQCoreApplication(int argc, char **argv) :
    QCoreApplication(argc, argv)
{
}

PQCoreApplication::~PQCoreApplication(){}

int PQCoreApplication::exec()
{
    return QCoreApplication::exec();
}

void PQCoreApplication::quit()
{
    QCoreApplication::quit();
}

int PQCoreApplication::applicationPid()
{
    return QCoreApplication::applicationPid();
}

QString PQCoreApplication::applicationVersion() const
{
    return QCoreApplication::applicationVersion();
}

void PQCoreApplication::setApplicationVersion(const QString &version)
{
    QCoreApplication::setApplicationVersion(version);
}

bool PQCoreApplication::isQuitLockEnabled()
{
    return QCoreApplication::isQuitLockEnabled();
}

void PQCoreApplication::setQuitLockEnabled(bool enabled)
{
    QCoreApplication::setQuitLockEnabled(enabled);
}

QString PQCoreApplication::applicationName() const
{
    return QCoreApplication::applicationName();
}

void PQCoreApplication::setApplicationName(const QString &applicationName)
{
    QCoreApplication::setApplicationName(applicationName);
}

QString PQCoreApplication::organizationName() const
{
    return QCoreApplication::organizationName();
}

void PQCoreApplication::setOrganizationName(const QString &orgName)
{
    QCoreApplication::setOrganizationName(orgName);
}

QString PQCoreApplication::organizationDomain() const
{
    return QCoreApplication::organizationDomain();
}

void PQCoreApplication::setOrganizationDomain(const QString &orgDomain)
{
    QCoreApplication::setOrganizationDomain(orgDomain);
}

QString PQCoreApplication::applicationDirPath() const
{
    return QCoreApplication::applicationDirPath();
}

QString PQCoreApplication::applicationFilePath() const
{
    return QCoreApplication::applicationFilePath();
}
