#include "pqthread.h"

PQThread::PQThread(QObject *parent) :
    QThread(parent){
    mRunning = false;
}

PQThread::~PQThread() {}

bool PQThread::isRunning() {
    return mRunning;
}

void PQThread::start() {
    mRunning = true;
    QThread::start();
}

void PQThread::stop() {
    mRunning = false;
    QThread::quit();
}
