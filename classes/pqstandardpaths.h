#ifndef PQSTANDARDPATHS_H
#define PQSTANDARDPATHS_H

#include "pqobject.h"
#include <QStandardPaths>

class PQStandardPaths : public PQObject
{
    Q_OBJECT
    PQ_OBJECT_EX(QStandardPaths)

public:
    Q_INVOKABLE explicit PQStandardPaths();
    ~PQStandardPaths();

    Q_INVOKABLE static QString writableLocation(int type);
    Q_INVOKABLE static QString static_writableLocation(int type);

    Q_INVOKABLE static QStringList standardLocations(int type);
    Q_INVOKABLE static QStringList static_standardLocations(int type);

    Q_INVOKABLE static QString locate(int type, const QString &fileName, int options = QStandardPaths::LocateFile);
    Q_INVOKABLE static QString static_locate(int type, const QString &fileName, int options = QStandardPaths::LocateFile);

    Q_INVOKABLE static QStringList locateAll(int type, const QString &fileName, int options = QStandardPaths::LocateFile);
    Q_INVOKABLE static QStringList static_locateAll(int type, const QString &fileName, int options = QStandardPaths::LocateFile);

    Q_INVOKABLE static QString displayName(int type);
    Q_INVOKABLE static QString static_displayName(int type);

    Q_INVOKABLE static QString findExecutable(const QString &executableName, const QStringList &paths = QStringList());
    Q_INVOKABLE static QString static_findExecutable(const QString &executableName, const QStringList &paths = QStringList());

    Q_INVOKABLE static void setTestModeEnabled(bool testMode);
    Q_INVOKABLE static void static_setTestModeEnabled(bool testMode);

    Q_INVOKABLE static bool isTestModeEnabled();
    Q_INVOKABLE static bool static_isTestModeEnabled();

public slots:
    PQ_ON_SIGNAL_DECL_START {
        PQOBJECT_ON_SIGNALS();
    } PQ_ON_SIGNAL_DECL_END
};

#endif // PQSTANDARDPATHS_H
