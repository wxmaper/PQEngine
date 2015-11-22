#ifndef PHPQT5_CONNECTION_H
#define PHPQT5_CONNECTION_H

#include "phpqt5objectfactory.h"

typedef struct _pq_connect_entry {
    zval *zo_sender;
    const QObject *qo_sender;
    QByteArray signal_signature;
    zval *zo_receiver;
    QByteArray slot_signature;
    QByteArray slot_name;
    int argc;
    int zo_sender_handle;
    int zo_receiver_handle;
    bool zo_receiver_is_closure;
} pq_connect_entry;

extern void PHPQt5Connection_invoke(const QObject *qo_sender, const QByteArray signalSignature, QVariantList args);

class PHPQt5Connection
{
public:
    PHPQt5Connection(TSRMLS_D);

    void                        removeConnections(const QObject *qo);

    void                        removePHPConnection(const QObject *qo_sender,
                                                    const QByteArray qSignalSignature_ba,
                                                    const QByteArray qSlotSignature_ba
                                                    TSRMLS_DC);

    void                        removePHPConnection(const QByteArray connectKey,
                                                    const QByteArray qSignalSignature_ba,
                                                    const QByteArray pslotSignature_ba);

    bool                        createPHPConnection(zval *zo_sender,
                                                    const QObject *qo_sender,
                                                    const QByteArray qSignalSignature_ba,
                                                    zval *zo_receiver,
                                                    const QByteArray qSlotSignature_ba
                                                    TSRMLS_DC);

    static void                 call_php_funtion(zval *zo_sender,
                                                 zval *zo_receiver,
                                                 const QByteArray slotName,
                                                 int argc,
                                                 QVariantList args);

    static QByteArray           generateConnectKey(const QObject *qo_sender,
                                                   const QByteArray signalSignature);

    static void ***thread_ctx;
    static QHash<QByteArray, QList<pq_connect_entry> > pq_connections;
};

#endif // PHPQT5_CONNECTION_H
