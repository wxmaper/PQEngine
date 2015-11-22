#include "pqdir.h"

PQDir::PQDir(const QString &path)
    : QObject(0)
{
    mdir = QDir(path);
}

PQDir::PQDir(const QString &path, const QString &nameFilter, int sortFlags, int filterFlags)
{
    mdir = QDir(path, nameFilter, QDir::SortFlags(sortFlags), QDir::Filters(filterFlags));
}

PQDir::~PQDir()
{
}

QString PQDir::absoluteFilePath(const QString &fileName) const
{
    return mdir.absoluteFilePath(fileName);
}

QString PQDir::absolutePath() const
{
    return mdir.absolutePath();
}

QString PQDir::canonicalPath() const
{
    return mdir.canonicalPath();
}

bool PQDir::cd(const QString &dirName)
{
    return mdir.cd(dirName);
}

bool PQDir::cdUp()
{
    return mdir.cdUp();
}

uint PQDir::count() const
{
    return mdir.count();
}

QString PQDir::dirName() const
{
    return mdir.dirName();
}

QStringList PQDir::entryList(const QStringList &nameFilters, int filterFlags, int sortFlags) const
{
    mdir.entryList(nameFilters, QDir::Filters(filterFlags), QDir::SortFlags(sortFlags));
}

QStringList PQDir::entryList(int filterFlags, int sortFlags) const
{
    mdir.entryList(QDir::Filters(filterFlags), QDir::SortFlags(sortFlags));
}

bool PQDir::exists(const QString &name) const
{
    return mdir.exists(name);
}

bool PQDir::exists() const
{
    return mdir.exists();
}

QString PQDir::filePath(const QString &fileName) const
{
    return mdir.filePath(fileName);
}

int PQDir::filter() const
{
    return (int) mdir.filter();
}

bool PQDir::isAbsolute() const
{
    return mdir.isAbsolute();
}

bool PQDir::isReadable() const
{
    return mdir.isReadable();
}

bool PQDir::isRelative() const
{
    return mdir.isRelative();
}

bool PQDir::isRoot() const
{
    return mdir.isRoot();
}

bool PQDir::makeAbsolute()
{
    return mdir.makeAbsolute();
}

bool PQDir::mkdir(const QString &dirName) const
{
    return mdir.mkdir(dirName);
}

bool PQDir::mkpath(const QString &dirPath) const
{
    return mdir.mkpath(dirPath);
}

QStringList PQDir::nameFilters() const
{
    return mdir.nameFilters();
}

QString PQDir::path() const
{
    return mdir.path();
}

void PQDir::refresh() const
{
    mdir.refresh();
}

QString PQDir::relativeFilePath(const QString &fileName) const
{
    return mdir.relativeFilePath(fileName);
}

bool PQDir::remove(const QString &fileName)
{
    return mdir.remove(fileName);
}

bool PQDir::removeRecursively()
{
    return mdir.removeRecursively();
}

bool PQDir::rename(const QString &oldName, const QString &newName)
{
    return mdir.rename(oldName, newName);
}

bool PQDir::rmdir(const QString &dirName) const
{
    return mdir.rmdir(dirName);
}

bool PQDir::rmpath(const QString &dirPath) const
{
    return mdir.rmpath(dirPath);
}

void PQDir::setFilter(int filterFlags)
{
    mdir.setFilter(QDir::Filters(filterFlags));
}

void PQDir::setNameFilters(const QStringList &nameFilters)
{
    mdir.setNameFilters(nameFilters);
}

void PQDir::setPath(const QString &path)
{
    setPath(path);
}

void PQDir::setSorting(int sortFlags)
{
    mdir.setSorting(QDir::SortFlags(sortFlags));
}

int PQDir::sorting() const
{
    return (int) mdir.sorting();
}

void PQDir::addSearchPath(const QString &prefix, const QString &path)
{
    QDir::addSearchPath(prefix, path);
}

QString PQDir::cleanPath(const QString &path)
{
    return QDir::cleanPath(path);
}

QString PQDir::currentPath()
{
    return QDir::currentPath();
}

QStringList PQDir::drives()
{
    QFileInfoList drivesInfo = QDir::drives();
    QStringList drivesList;

    foreach (QFileInfo drive, drivesInfo) {
        drivesList << drive.absoluteFilePath();
    }

    return drivesList;
}

QString PQDir::fromNativeSeparators(const QString &pathName)
{
    return QDir::fromNativeSeparators(pathName);
}

QString PQDir::homePath()
{
    return QDir::homePath();
}

bool PQDir::isAbsolutePath(const QString &path)
{
    return QDir::isAbsolutePath(path);
}

bool PQDir::isRelativePath(const QString &path)
{
    return QDir::isRelativePath(path);
}

bool PQDir::match(const QString &filter, const QString &fileName)
{
    return QDir::match(filter, fileName);
}

QString PQDir::rootPath()
{
    return QDir::rootPath();
}

QStringList PQDir::searchPaths(const QString &prefix)
{
    return QDir::searchPaths(prefix);
}

QString PQDir::separator()
{
    return QDir::separator();
}

bool PQDir::setCurrent(const QString &path)
{
    return QDir::setCurrent(path);
}

QString PQDir::tempPath()
{
    return QDir::tempPath();
}

QString PQDir::toNativeSeparators(const QString &pathName)
{
    return QDir::toNativeSeparators(pathName);
}
