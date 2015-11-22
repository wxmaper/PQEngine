#ifndef PQPROCESS_H
#define PQPROCESS_H

#include "pqobject.h"
#include <QProcess>

class PQProcess;
class PQProcess : public QProcess {
    Q_OBJECT
    PQ_OBJECT

public:
    Q_INVOKABLE explicit PQProcess(QObject *parent = 0);
    virtual ~PQProcess();

    Q_INVOKABLE QStringList arguments() const;
    Q_INVOKABLE qint64 processId() const;
    Q_INVOKABLE QString program() const;
    Q_INVOKABLE QByteArray readAllStandardError();
    Q_INVOKABLE QByteArray readAllStandardOutput();
    Q_INVOKABLE void setArguments(const QStringList &arguments);
    Q_INVOKABLE void setNativeArguments(const QString &arguments);
    Q_INVOKABLE void setProgram(const QString &program);
    Q_INVOKABLE void setWorkingDirectory(const QString &dir);
    Q_INVOKABLE void start(const QString &program, const QStringList &arguments, int mode = QProcess::ReadWrite);
    Q_INVOKABLE void start(int mode = QProcess::ReadWrite);
    Q_INVOKABLE void start(const QString &command, int mode = QProcess::ReadWrite);
    Q_INVOKABLE int state();
    Q_INVOKABLE bool waitForFinished(int msecs = 30000);
    Q_INVOKABLE bool waitForStarted(int msecs = 30000);
    Q_INVOKABLE QString workingDirectory() const;

    Q_INVOKABLE void close();
    Q_INVOKABLE bool isSequential() const;
    Q_INVOKABLE bool open(int mode = QProcess::ReadWrite);

public Q_SLOTS:
    Q_INVOKABLE void kill();
    Q_INVOKABLE void terminate();

    void error_simplify(QProcess::ProcessError errorCode);
    void finished_simplify(int exitCode, QProcess::ExitStatus exitStatus);
    void stateChanged_simplify(QProcess::ProcessState newState);

    void error_pslot(int error);
    void started_pslot();
    void finished_pslot(int exitCode, int exitStatus);
    void stateChanged_pslot(int newState);

signals:
    void error(int error);
    void finished(int exitCode, int exitStatus);
    void stateChanged(int newState);
};

#endif // PQPROCESS_H
