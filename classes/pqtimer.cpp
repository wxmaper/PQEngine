#include "pqtimer.h"

PQTimer::PQTimer(QObject *parent) :
    QTimer(parent)
{
    mEnabledOnSignals.insert("onTimeout", "timeout()");
}

PQTimer::~PQTimer() {}

int PQTimer::interval() {
    return QTimer::interval();
}

bool PQTimer::isActive() {
    return QTimer::isActive();
}

bool PQTimer::isRunning() {
    return QTimer::isActive();
}

void PQTimer::setInterval(int interval) {
    QTimer::setInterval(interval);
}

void PQTimer::start() {
    QTimer::start();
}

void PQTimer::start(int interval) {
    QTimer::start(interval);
}

void PQTimer::stop() {
    QTimer::stop();
}

bool PQTimer::setRunning(bool running)
{
    running ? this->start() : this->stop();
}

bool PQTimer::setActive(bool active)
{
    active ? this->start() : this->stop();
}

void PQTimer::timeout_pslot()
{
    QVariantList args;
    PHPQt5Connection_invoke(this, "timeout()", args);
}
