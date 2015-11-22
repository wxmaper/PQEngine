#include "pqprocess.h"

PQProcess::PQProcess(QObject *parent)
    : QProcess(parent)
{
    mEnabledOnSignals.insert("onError", "error(int)");
    mEnabledOnSignals.insert("onStarted", "started()");
    mEnabledOnSignals.insert("onFinished", "finished(int,int)");
    mEnabledOnSignals.insert("onStateChanged", "stateChanged(int)");

    connect(this, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(error_simplify(QProcess::ProcessError)));
    connect(this, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(finished_simplify(int,QProcess::ExitStatus)));
    connect(this, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(stateChanged_simplify(QProcess::ProcessState)));
}

PQProcess::~PQProcess(){   }

QStringList PQProcess::arguments() const
{
    return QProcess::arguments();
}

qint64 PQProcess::processId() const
{
    return QProcess::processId();
}

QString PQProcess::program() const
{
    return QProcess::program();
}

QByteArray PQProcess::readAllStandardError()
{
    return QProcess::readAllStandardError();
}

QByteArray PQProcess::readAllStandardOutput()
{
    return QProcess::readAllStandardOutput();
}

void PQProcess::setArguments(const QStringList &arguments)
{
    QProcess::setArguments(arguments);
}

void PQProcess::setNativeArguments(const QString &arguments)
{
    QProcess::setNativeArguments(arguments);
}

void PQProcess::setProgram(const QString &program)
{
    QProcess::setProgram(program);
}

void PQProcess::setWorkingDirectory(const QString &dir)
{
    QProcess::setWorkingDirectory(dir);
}

void PQProcess::start(const QString &program, const QStringList &arguments, int mode)
{
    QProcess::start(program, arguments, OpenMode(mode));
}

void PQProcess::start(int mode)
{
    QProcess::start(OpenMode(mode));
}

void PQProcess::start(const QString &command, int mode)
{
    QProcess::start(command, OpenMode(mode));
}

int PQProcess::state()
{
    return QProcess::state();
}

bool PQProcess::waitForFinished(int msecs)
{
    QProcess::waitForFinished(msecs);
}

bool PQProcess::waitForStarted(int msecs)
{
    QProcess::waitForStarted(msecs);
}

QString PQProcess::workingDirectory() const
{
    return QProcess::workingDirectory();
}

void PQProcess::close()
{
    QProcess::close();
}

bool PQProcess::isSequential() const
{
    return QProcess::isSequential();
}

bool PQProcess::open(int mode)
{
    return QProcess::open(OpenMode(mode));
}

void PQProcess::kill()
{
    QProcess::kill();
}

void PQProcess::terminate()
{
    QProcess::terminate();
}

void PQProcess::error_simplify(QProcess::ProcessError errorCode)
{
    emit error((int) errorCode);
}

void PQProcess::finished_simplify(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit finished(exitCode, (int) exitStatus);
}

void PQProcess::stateChanged_simplify(QProcess::ProcessState newState)
{
    emit stateChanged((int) newState);
}

void PQProcess::error_pslot(int error) {
    QVariantList args;
    args << error;
    PHPQt5Connection_invoke(this, "error(int)", args);
}

void PQProcess::started_pslot() {
    QVariantList args;
    PHPQt5Connection_invoke(this, "started()", args);
}

void PQProcess::finished_pslot(int exitCode, int exitStatus) {
    QVariantList args;
    args << exitCode << exitStatus;
    PHPQt5Connection_invoke(this, "finished(int,int)", args);
}

void PQProcess::stateChanged_pslot(int newState) {
    QVariantList args;
    args << newState;
    PHPQt5Connection_invoke(this, "stateChanged(int)", args);
}
