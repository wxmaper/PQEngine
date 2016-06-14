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

#ifndef PHPQT5_CONNECTION_H
#define PHPQT5_CONNECTION_H

#include "phpqt5objectfactory.h"
#include <QObject>

#define C_TYPE_UNKNOWN 0
#define C_TYPE_CLOSURE 1
#define C_TYPE_QOBJECT_METHOD 2
#define C_TYPE_PHPOBJECT_METHOD 3
#define C_TYPE_PHPOBJECT_SIGNAL_METHOD 4
#define C_TYPE_THREADED 5

typedef struct _pq_connect_entry {
    zend_object *zo_sender;
    const QObject *qo_sender;
    QByteArray signal_signature;
    zend_object *zo_receiver;
    const QObject *qo_receiver;
    QByteArray slot_signature;
    QByteArray slot_name;
    int argc;
    uint zo_sender_handle;
    uint zo_receiver_handle;
    int connection_type;
} pq_connect_entry;


class PQDLAPI PHPQt5ConnectionWorker : public QObject
{
    Q_OBJECT

public:
    PHPQt5ConnectionWorker();
    PHPQt5ConnectionWorker(zend_object *zo_sender,
                           zend_object *zo_receiver,
                           const QByteArray slotName,
                           int argc,
                           QVariantList args,
                           bool createNewCtx);

    ~PHPQt5ConnectionWorker(){}

    void doProcess(zend_object *zo_sender,
                   zend_object *zo_receiver,
                   const QByteArray slotName,
                   int argc,
                   QVariantList args,
                   bool createNewCtx);


    zend_object *zo_sender;
    zend_object *zo_receiver;
    QByteArray slotName;
    int argc;
    QVariantList args;
    bool createNewCtx;

    void *ctx = nullptr;

public slots:
    Q_INVOKABLE void process();
    void setReady();
    void setReady(zend_object *zo_sender,
                  zend_object *zo_receiver,
                  const QByteArray slotName,
                  int argc,
                  QVariantList args,
                  bool createNewCtx);

signals:
    void ready();
    void finished();
    void finishedWithDestroy(PHPQt5ConnectionWorker*);
};

class PQDLAPI PHPQt5Connection : public QObject
{
    Q_OBJECT

public:
    explicit PHPQt5Connection(PQDBG_LVL_D);

    void                        removeConnections(const QObject *qo
                                                  PQDBG_LVL_DC);

    void                        removeConnection(const QObject *qo_sender,
                                                 const QByteArray qSignalSignature_ba,
                                                 const QByteArray qSlotSignature_ba
                                                 PQDBG_LVL_DC);

    void                        removeConnection(const QByteArray connectKey,
                                                 const QByteArray qSignalSignature_ba,
                                                 const QByteArray pslotSignature_ba
                                                 PQDBG_LVL_DC);

    bool                        createConnection(zval *zo_sender,
                                                 const QObject *qo_sender,
                                                 const QByteArray qSignalSignature_ba,
                                                 zval *zo_receiver,
                                                 const QObject *qo_receiver,
                                                 const QByteArray qSlotSignature_ba
                                                 PQDBG_LVL_DC);

    static void                 call_php_funtion(zend_object *zo_sender,
                                                 zend_object *zo_receiver,
                                                 const QByteArray slotName,
                                                 int argc,
                                                 QVariantList args
                                                 PQDBG_LVL_DC);

    static QByteArray           generateConnectKey(const QObject *qo_sender,
                                                   const QByteArray signalSignature
                                                   PQDBG_LVL_DC);

    static QHash<QByteArray, QList<pq_connect_entry> > pq_connections;

public slots:
    void deleteConnectionWorker(PHPQt5ConnectionWorker* w);
};

#endif // PHPQT5_CONNECTION_H
