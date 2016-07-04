/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PHPQt5.
**
** BEGIN LICENSE: MPL 2.0
**
** This Source Code Form is subject to the terms of the Mozilla Public
** License, v. 2.0. If a copy of the MPL was not distributed with this
** file, You can obtain one at http://mozilla.org/MPL/2.0/.
**
** END LICENSE
**
****************************************************************************/

#include "phpqt5.h"
#include "phpqt5connection.h"
#include <QObject>
#include <QThread>


QHash<QByteArray, QList<pq_connect_entry> > PHPQt5Connection::pq_connections;
QHash<QByteArray, PHPQt5ConnectionWorker*> connectWorkers;


PHPQt5ConnectionWorker::PHPQt5ConnectionWorker()
    : QObject()
{
    this->ctx = nullptr;
}

PHPQt5ConnectionWorker::PHPQt5ConnectionWorker(zend_object *zo_sender,
                                               zend_object *zo_receiver,
                                               const QByteArray slotName,
                                               int argc,
                                               QVariantList args,
                                               bool createNewCtx)
    : QObject()
{
    this->zo_sender = zo_sender;
    this->zo_receiver = zo_receiver;
    this->slotName = slotName;
    this->argc = argc;
    this->args = args;
    this->createNewCtx = createNewCtx;

    this->ctx = nullptr;
}

void PHPQt5ConnectionWorker::process()
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    if(this->ctx == nullptr) {
        PQDBGLPUP("tsrm_get_ls_cache");
        this->ctx = tsrm_get_ls_cache();

        if(!this->ctx) {

            PQDBGLPUP("tsrm_new_interpreter_context");
            this->ctx = tsrm_new_interpreter_context();

            PQDBGLPUP("php_request_startup");
            php_request_startup();

            /*
            PQDBGLPUP("get interpreter context from thread");
            void *TSRMLS_CACHE;
            bool php_started;

            QMetaObject::invokeMethod(this->thread(), "get_ls_cache",
                                      Qt::DirectConnection,
                                      Q_RETURN_ARG(void*, TSRMLS_CACHE));

            if(!TSRMLS_CACHE) {
                php_error(E_ERROR, "Failed getting interpreter context from thread");
            }

            QMetaObject::invokeMethod(this->thread(), "php_started",
                                      Qt::DirectConnection,
                                      Q_RETURN_ARG(bool, php_started));

            PQDBGLPUP(QString("tsrm_set_interpreter_context %1")
                      .arg(reinterpret_cast<quint64>(TSRMLS_CACHE)));

            this->ctx = TSRMLS_CACHE;
            tsrm_set_interpreter_context(TSRMLS_CACHE);

            if(!php_started) {
                PQDBGLPUP("php_request_startup");
                php_request_startup();

                QMetaObject::invokeMethod(this->thread(), "set_php_started",
                                          Qt::DirectConnection);
            }

            //PQDBGLPUP("php_request_startup");
            //php_request_startup();
            */
        }
    }

    argc++; // the first argument for PHP-slots it is a PHP-object sender

    zval function_name, retval, z_receiver;
    zval *params = new zval[argc];

    ZVAL_STRING(&function_name, slotName.constData());
    ZVAL_OBJ(&z_receiver, zo_receiver);

    ZVAL_OBJ(&params[0], zo_sender);

    for(int i = 1; i < argc; i++) {
        QVariant arg = args.at(i-1);
        params[i] = PHPQt5::pq_cast_to_zval(arg, true PQDBG_LVL_CC);
    }

#ifdef PQDEBUG
    PQDBGLPUP(QString("call function (%1)").arg(reinterpret_cast<quint64>(&z_receiver)));
#endif
    //zend_call_method(&z_receiver, Z_OBJCE(z_receiver), NULL, ZEND_STRL("run"), &retval, 0, NULL, NULL);
    if(call_user_function(nullptr, &z_receiver, &function_name, &retval, argc, params) == FAILURE) {
        QString s = QString("PHPQt5 could not call method: %1 of class: %2")
                .arg(Z_STRVAL(function_name))
                .arg(Z_OBJCE(z_receiver)->name->val);

        php_error(E_ERROR, s.toUtf8().constData());
    }

#ifdef PQDEBUG
    PQDBGLPUP("dtor params");
#endif
    for(int i = 1; i < argc; i++) { // do not remove sender!
        zval_dtor(&params[i]);
    }

#ifdef PQDEBUG
    PQDBGLPUP("dtor temps");
#endif

    delete params;
    params = nullptr;

    zval_dtor(&retval);
    zval_dtor(&function_name);

    /*
    if(createNewCtx) {
        #ifdef PQDEBUG
                PQDBGLPUP("php_request_shutdown");
        #endif
        php_request_shutdown(nullptr);

        #ifdef PQDEBUG
                PQDBGLPUP("tsrm_free_interpreter_context");
        #endif
        tsrm_free_interpreter_context(ctx);
    }
    */

    PQDBG_LVL_DONE();
}

void PHPQt5ConnectionWorker::doProcess(zend_object *zo_sender,
                                       zend_object *zo_receiver,
                                       const QByteArray slotName,
                                       int argc,
                                       QVariantList args,
                                       bool createNewCtx)
{
}

void PHPQt5ConnectionWorker::setReady() {
    emit ready();
}

void PHPQt5ConnectionWorker::setReady(zend_object *zo_sender,
                                      zend_object *zo_receiver,
                                      const QByteArray slotName,
                                      int argc,
                                      QVariantList args,
                                      bool createNewCtx)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    this->zo_sender = zo_sender;
    this->zo_receiver = zo_receiver;
    this->slotName = slotName;
    this->argc = argc;
    this->args = args;
    this->createNewCtx = createNewCtx;

    PQDBG_LVL_DONE();

    emit ready();
}

PQAPI void PHPQt5Connection_invoke(QObject *qo_sender,
                                   const QByteArray signalSignature,
                                   QVariantList args)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("invoke of signal %1").arg(signalSignature.constData()));
#endif

    QByteArray connectKey = PHPQt5Connection::generateConnectKey(qo_sender,
                                                                 signalSignature
                                                                 PQDBG_LVL_CC);

    QList<pq_connect_entry> ceList = PHPQt5Connection::pq_connections.value(connectKey, QList<pq_connect_entry>());
    foreach(pq_connect_entry ce, ceList) {

        if(ce.connection_type == C_TYPE_THREADED) {
            QThread *thread = qobject_cast<QThread*>(qo_sender);
            if(thread) {
                PHPQt5ConnectionWorker *worker = new PHPQt5ConnectionWorker(ce.zo_sender,
                                                                            ce.zo_receiver,
                                                                            ce.slot_name,
                                                                            ce.argc,
                                                                            args,
                                                                            true);
                worker->moveToThread(thread);

                QObject::connect(worker, SIGNAL(ready()), worker, SLOT(process()));
               // QObject::connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
               // QObject::connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
               // QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

                worker->setReady();
            }
            else {
                php_error(E_ERROR, "Threaded connection is lost!");
            }
        }
        else {
            bool doCallPhpFunction = true;

            if(ce.qo_receiver) {
                quint64 senderTh = reinterpret_cast<quint64>(ce.qo_sender->thread());
                quint64 receiverTh = reinterpret_cast<quint64>(ce.qo_receiver->thread());

                QByteArray signalThPath = QString("%1->%2").arg(senderTh).arg(receiverTh).toUtf8();
                PQDBGLPUP(QString("signal thread path: %1").arg(signalThPath.constData()));

                if(senderTh != receiverTh) {
                    PHPQt5ConnectionWorker *worker;

                    if(connectWorkers.contains(signalThPath)) {
                        PQDBGLPUP("get PHPQt5ConnectionWorker");
                        worker = connectWorkers.value(signalThPath);
                    }
                    else {
                        PQDBGLPUP("new PHPQt5ConnectionWorker");
                        worker = new PHPQt5ConnectionWorker;
                        worker->moveToThread(ce.qo_receiver->thread());

                        connectWorkers.insert(signalThPath, worker);

                        QObject::connect(worker, SIGNAL(ready()),
                                         worker, SLOT(process()));
                    }

                    if(worker) {
                        PQDBGLPUP("worker->setReady()");
                        worker->setReady(ce.zo_sender,
                                         ce.zo_receiver,
                                         ce.slot_name,
                                         ce.argc,
                                         args,
                                         true);

                        //if(worker->thread()->isRunning())
                        //    worker->thread()->start();
                       // QMetaObject::invokeMethod(worker, "process", Qt::QueuedConnection);
                       // qApp->processEvents(QEventLoop::AllEvents);

                        //QCoreApplication::processEvents();
                    }

                    doCallPhpFunction = false;
                }
            }

            if(doCallPhpFunction) {
                PHPQt5Connection::call_php_funtion(ce.zo_sender,
                                                   ce.zo_receiver,
                                                   ce.slot_name,
                                                   ce.argc,
                                                   args
                                                   PQDBG_LVL_CC);
            }
        }
    }

    PQDBG_LVL_DONE();
}

PHPQt5Connection::PHPQt5Connection(PQDBG_LVL_D)
    : QObject()
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    PQDBG_LVL_DONE();
}

QByteArray PHPQt5Connection::generateConnectKey(const QObject *qo_sender,
                                                const QByteArray signalSignature
                                                PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    QByteArray connectKey = qo_sender->metaObject()->className();
    connectKey.append("-").append(QByteArray::number(reinterpret_cast<quint64>(qo_sender))).append("-").append(signalSignature);


#ifdef PQDEBUG
    PQDBGLPUP(QString("connectKey: %1").arg(connectKey.constData()));
    PQDBG_LVL_DONE();
#endif

    return connectKey;
}

void PHPQt5Connection::removeConnections(const QObject *qo
                                         PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    QHashIterator<QByteArray, QList<pq_connect_entry> > i(pq_connections);

    while(i.hasNext()) {
        i.next();

        if(i.value().at(0).qo_sender == qo) {
            pq_connections.remove(i.key());
        }
    }

    PQDBG_LVL_DONE();
}

void PHPQt5Connection::removeConnection(const QObject *qo_sender,
                                        const QByteArray qSignalSignature_ba,
                                        const QByteArray qSlotSignature_ba
                                        PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    QString args = qSlotSignature_ba.mid( qSlotSignature_ba.indexOf("("), qSlotSignature_ba.indexOf(")") );
    QByteArray signalName = qSignalSignature_ba.mid(1, qSignalSignature_ba.indexOf("(")-1); // get a "cleared" signal name
    QByteArray connectKey = generateConnectKey(qo_sender,
                                               qSignalSignature_ba.mid(1)
                                               PQDBG_LVL_CC);

    QByteArray pslotSignature_ba(signalName);
    pslotSignature_ba.prepend("1");
    pslotSignature_ba.append("_pslot");
    pslotSignature_ba.append(args);

    QHashIterator<QByteArray, QList<pq_connect_entry> > i(pq_connections);
    while(i.hasNext()) {
        i.next();

        if(i.key() == connectKey) {
            foreach(pq_connect_entry ce, i.value()) {
                if(ce.slot_signature == qSlotSignature_ba.mid(1)) {
                    for(int index = 0; index < pq_connections[connectKey].size(); index++) {
                        if(pq_connections[connectKey].at(index).slot_signature == qSlotSignature_ba.mid(1)) {
                            pq_connections[connectKey].removeAt(index);
                            break;
                        }
                    }
                }
            }

            if(pq_connections[connectKey].size() == 0) {
                pq_connections.remove(connectKey);

                QObject::disconnect(qo_sender,
                                    qSignalSignature_ba.constData(),
                                    qo_sender,
                                    pslotSignature_ba.constData());
            }
        }
    }

    PQDBG_LVL_DONE();
}

void PHPQt5Connection::removeConnection(const QByteArray connectKey,
                                        const QByteArray qSignalSignature_ba,
                                        const QByteArray pslotSignature_ba
                                        PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    if(pq_connections.contains(connectKey)) {
        QList<pq_connect_entry> ce_list = pq_connections.value(connectKey);

        foreach(pq_connect_entry ce, ce_list) {
            if(ce.connection_type == C_TYPE_CLOSURE) {
                zval z_receiver;
                ZVAL_OBJ(&z_receiver, ce.zo_receiver);
                Z_DELREF_P(&z_receiver);

                QObject::disconnect(ce.qo_sender,
                                    qSignalSignature_ba.constData(),
                                    ce.qo_sender,
                                    pslotSignature_ba.constData());

                if(ce.connection_type == C_TYPE_PHPOBJECT_SIGNAL_METHOD) {
                    QMetaObject::invokeMethod(const_cast<QObject*>(ce.qo_sender), "decConnection", Qt::DirectConnection);
                }

                pq_connections.remove(connectKey);
            }
        }
    }

    PQDBG_LVL_DONE();
}

bool PHPQt5Connection::createConnection(zval *zo_sender,
                                        const QObject *qo_sender,
                                        const QByteArray qSignalSignature_ba,
                                        zval *zo_receiver,
                                        const QObject *qo_receiver,
                                        const QByteArray qSlotSignature_ba
                                        PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    int connectionType = C_TYPE_UNKNOWN;
    bool haveConnection = false;

    PQDBGLPUP("const casting");
    QObject *_qo_sender = const_cast<QObject*>(qo_sender);

    QByteArray slotSignature = QByteArray(qSlotSignature_ba)
            .replace(",string",",QString")
            .replace("string,","QString,")
            .replace("(string)","(QString)");

    QByteArray signalSignature = QByteArray(qSignalSignature_ba)
            .replace(",string",",QString")
            .replace("string,","QString,")
            .replace("(string)","(QString)");

    if(slotSignature == "1__invoke()") {
        slotSignature = "1__invoke";
        slotSignature.append( signalSignature.mid(signalSignature.indexOf("(")) );
        connectionType = C_TYPE_CLOSURE;
    }
    else {
        if(qo_sender != nullptr) {
#ifdef PQDEBUG
            PQDBGLPUP("qobject casting");
#endif

            if(qobject_cast<QThread*>(_qo_sender)) {
                if(qo_sender->thread() != qo_receiver->thread()) {
                    connectionType = C_TYPE_THREADED;
                }
            }
        }
    }

    QString args = slotSignature.mid( slotSignature.indexOf("("), slotSignature.indexOf(")") );
    QByteArray slotName = slotSignature.mid(1, slotSignature.indexOf("(")-1); // get a "cleared" slot name
    QByteArray signalName = signalSignature.mid(1, signalSignature.indexOf("(")-1); // get a "cleared" signal name
    QByteArray connectKey = generateConnectKey(qo_sender,
                                               signalSignature.mid(1)
                                               PQDBG_LVL_CC);

    QByteArray pslotSignature_ba(signalName);
    pslotSignature_ba.prepend("1");
    pslotSignature_ba.append("_pslot");
    pslotSignature_ba.append(args);

    int argc = 0;
    if(args != "()") {
        argc = args.split(",").size();
    }

    if(connectionType == C_TYPE_CLOSURE) {
        removeConnection(connectKey,
                         signalSignature,
                         pslotSignature_ba
                         PQDBG_LVL_CC);
    }

    if(pq_connections.contains(connectKey)) {
        if(connectionType == C_TYPE_UNKNOWN) {
            connectionType = pq_connections.value(connectKey).at(0).connection_type;
        }

        haveConnection = true;
    }
    else {
        {
            /* Пробуем создать связь PHP-Signal -> PHP-Slot */
            bool haveSignal = false;
            if(QMetaObject::invokeMethod(_qo_sender, "haveSignal",
                                         Qt::DirectConnection,
                                         Q_RETURN_ARG(bool, haveSignal),
                                         Q_ARG(QByteArray, signalSignature)
                                         )) {

                if(haveSignal) {
                    connectionType = C_TYPE_PHPOBJECT_SIGNAL_METHOD;
                    if(QMetaObject::invokeMethod(_qo_sender, "incConnection",
                                                 Qt::DirectConnection,
                                                 Q_ARG(QByteArray, signalSignature)
                                                 )) {
                        haveConnection = true;
                    }
                    else {
                        PQDBGLPUP(QString("ERROR: cannot increase connections count for object %1")
                                  .arg(QString(qo_sender->metaObject()->className()).mid(1))
                                  );
                    }
                }
            }
        }

        if(!haveConnection) {
            haveConnection = QObject::connect(qo_sender, signalSignature.constData(),
                                              qo_sender, pslotSignature_ba.constData());

            PQDBGLPUP(QString("test PHP SIGNAL-SLOT CONNECTION: %1").arg(haveConnection));
        }
    }


#ifdef PQDEBUG
    if(connectionType == C_TYPE_THREADED) {
        quint64 th1 = reinterpret_cast<quint64>(qo_sender->thread());
        quint64 th2 = reinterpret_cast<quint64>(qo_receiver->thread());
        PQDBGLPUP("createConnection: THREADED CONNECTION");
        PQDBGLPUP(QString("[(%1)->(%2)] %3 -> %4")
                  .arg(th1)
                  .arg(th2)
                  .arg(qSignalSignature_ba.constData())
                  .arg(slotSignature.constData()));

    }
    else if(connectionType == C_TYPE_CLOSURE) {
        PQDBGLPUP("createConnection: CLOSURE CONNECTION");
        PQDBGLPUP(QString("%1 -> %2")
                  .arg(qSignalSignature_ba.constData())
                  .arg(slotSignature.constData()));
    }
    else if(connectionType == C_TYPE_PHPOBJECT_SIGNAL_METHOD) {
        PQDBGLPUP("createConnection: PHP SIGNAL-SLOT CONNECTION");
        PQDBGLPUP(QString("%1 -> %2")
                  .arg(qSignalSignature_ba.constData())
                  .arg(slotSignature.constData()));
    }
    else {
        PQDBGLPUP("createConnection: NORMAL CONNECTION");
        PQDBGLPUP(QString("%1 -> %2")
                  .arg(qSignalSignature_ba.constData())
                  .arg(slotSignature.constData()));
    }
#endif

    if(haveConnection) {
        if(connectionType == C_TYPE_UNKNOWN)
            connectionType = C_TYPE_QOBJECT_METHOD;

        QList<pq_connect_entry> ceList = pq_connections.value(connectKey, QList<pq_connect_entry>());
        ceList.append(pq_connect_entry {
                          Z_OBJ_P(zo_sender),
                          qo_sender,
                          signalSignature.mid(1), // remove first symbol (is a "2" for signals)
                          Z_OBJ_P(zo_receiver),
                          qo_receiver,
                          slotSignature.mid(1), // remove first symbol (is a "1" for slots)
                          slotName.constData(),
                          argc,
                          Z_OBJ_HANDLE_P(zo_sender),
                          Z_OBJ_HANDLE_P(zo_receiver),
                          connectionType
                      });

        pq_connections.insert(connectKey, ceList);
    }

    PQDBG_LVL_DONE();
    return haveConnection;
}

void PHPQt5Connection::call_php_funtion(zend_object *zo_sender,
                                        zend_object *zo_receiver,
                                        const QByteArray slotName,
                                        int argc,
                                        QVariantList args
                                        PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    argc++; // the first argument for PHP-slots it is a PHP-object sender

    zval function_name, retval, z_receiver;
    zval *params = new zval[argc];

    PQDBGLPUP("alloc params");
    ZVAL_STRING(&function_name, slotName.constData());
    ZVAL_OBJ(&z_receiver, zo_receiver);
    ZVAL_OBJ(&params[0], zo_sender);

    PQDBGLPUP(QString("params: %1"));
    PQDBGLPUP(QString("params: %1").arg(reinterpret_cast<quint64>(zo_sender)));

    for(int i = 1; i < argc; i++) {
        QVariant arg = args.at(i-1);
        params[i] = PHPQt5::pq_cast_to_zval(arg, true PQDBG_LVL_CC);
    }

    if(call_user_function(NULL, &z_receiver, &function_name, &retval, argc, params) == FAILURE) {
        QString s = QString("PHPQt5 could not call method: %1 of class: %2")
                .arg(Z_STRVAL(function_name))
                .arg(Z_OBJCE(z_receiver)->name->val);

        php_error(E_ERROR, s.toUtf8().constData());
    }

    PQDBGLPUP("free params");

    for(int i = 1; i < argc; i++) {
        zval_ptr_dtor(&params[i]);
    }

    PQDBGLPUP("free temp vars");

    delete params;
    params = nullptr;

    zval_dtor(&retval);
    zval_dtor(&function_name);

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("done");
    #endif

    PQDBG_LVL_DONE();
}

void PHPQt5Connection::deleteConnectionWorker(PHPQt5ConnectionWorker *w)
{
    w->deleteLater();
    delete w;
    w = nullptr;
}
