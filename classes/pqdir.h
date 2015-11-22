#ifndef PQDIR_H
#define PQDIR_H

#include "pqobject.h"
#include <QDir>

class PQDir;
class PQDir : public QObject {
    Q_OBJECT
    PQ_OBJECT

public:
    Q_INVOKABLE explicit PQDir(const QString &path = "");
    Q_INVOKABLE explicit PQDir(const QString &path,
                               const QString &nameFilter,
                               int sortFlags = QDir::SortFlags( QDir::Name | QDir::IgnoreCase ),
                               int filterFlags = QDir::AllEntries);
    virtual ~PQDir();

    Q_INVOKABLE QString absoluteFilePath(const QString &fileName) const;
    Q_INVOKABLE QString absolutePath() const;
    Q_INVOKABLE QString canonicalPath() const;

    Q_INVOKABLE bool cd(const QString &dirName);
    Q_INVOKABLE bool cdUp();
    Q_INVOKABLE uint count() const;
    Q_INVOKABLE QString dirName() const;
    Q_INVOKABLE QStringList entryList(const QStringList &nameFilters, int filterFlags = QDir::NoFilter, int sortFlags = QDir::NoSort) const;
    Q_INVOKABLE QStringList entryList(int filterFlags = QDir::NoFilter, int sortFlags = QDir::NoSort) const;
    Q_INVOKABLE bool exists(const QString &name) const;
    Q_INVOKABLE bool exists() const;
    Q_INVOKABLE QString filePath(const QString &fileName) const;
    Q_INVOKABLE int filter() const;
    Q_INVOKABLE bool isAbsolute() const;
    Q_INVOKABLE bool isReadable() const;
    Q_INVOKABLE bool isRelative() const;
    Q_INVOKABLE bool isRoot() const;
    Q_INVOKABLE bool makeAbsolute();
    Q_INVOKABLE bool mkdir(const QString &dirName) const;
    Q_INVOKABLE bool mkpath(const QString &dirPath) const;
    Q_INVOKABLE QStringList nameFilters() const;
    Q_INVOKABLE QString path() const;
    Q_INVOKABLE void refresh() const;
    Q_INVOKABLE QString relativeFilePath(const QString &fileName) const;
    Q_INVOKABLE bool remove(const QString &fileName);
    Q_INVOKABLE bool removeRecursively();
    Q_INVOKABLE bool rename(const QString &oldName, const QString &newName);
    Q_INVOKABLE bool rmdir(const QString &dirName) const;
    Q_INVOKABLE bool rmpath(const QString &dirPath) const;
    Q_INVOKABLE void setFilter(int filterFlags);
    Q_INVOKABLE void setNameFilters(const QStringList &nameFilters);
    Q_INVOKABLE void setPath(const QString &path);
    Q_INVOKABLE void setSorting(int sortFlags);
    Q_INVOKABLE int sorting() const;

    Q_INVOKABLE static void addSearchPath(const QString &prefix, const QString &path);
    Q_INVOKABLE static QString cleanPath(const QString &path);
    Q_INVOKABLE static QString currentPath();
    Q_INVOKABLE static QStringList drives();
    Q_INVOKABLE static QString fromNativeSeparators(const QString &pathName);
    Q_INVOKABLE static QString homePath();
    Q_INVOKABLE static bool isAbsolutePath(const QString &path);
    Q_INVOKABLE static bool isRelativePath(const QString &path);
    Q_INVOKABLE static bool match(const QString &filter, const QString &fileName);
    Q_INVOKABLE static QString rootPath();
    Q_INVOKABLE static QStringList searchPaths(const QString &prefix);
    Q_INVOKABLE static QString separator();
    Q_INVOKABLE static bool setCurrent(const QString &path);
    Q_INVOKABLE static QString tempPath();
    Q_INVOKABLE static QString toNativeSeparators(const QString &pathName);

private:
    QDir mdir;
};


#endif // PQDIR_H
