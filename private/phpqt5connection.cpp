#include "phpqt5.h"
#include "phpqt5connection.h"

QHash<QByteArray, QList<pq_connect_entry> > PHPQt5Connection::pq_connections;
void*** PHPQt5Connection::thread_ctx;

PQAPI void PHPQt5Connection_invoke(const QObject *qo_sender, const QByteArray signalSignature, QVariantList args)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5Connection_invoke: ", signalSignature);
#endif

    QByteArray connectKey = PHPQt5Connection::generateConnectKey(qo_sender, signalSignature);

    QList<pq_connect_entry> ceList = PHPQt5Connection::pq_connections.value(connectKey, QList<pq_connect_entry>());
    foreach(pq_connect_entry ce, ceList) {
        PHPQt5Connection::call_php_funtion(ce.zo_sender,
                                           ce.zo_receiver,
                                           ce.slot_name,
                                           ce.argc,
                                           args);
    }
}

PHPQt5Connection::PHPQt5Connection(TSRMLS_D)
{
    TSRMLS_SET_CTX(thread_ctx);
}

QByteArray PHPQt5Connection::generateConnectKey(const QObject *qo_sender, const QByteArray signalSignature) {
    QByteArray connectKey = qo_sender->metaObject()->className();
    connectKey.append("-").append(QByteArray::number(reinterpret_cast<quint32>(qo_sender))).append("-").append(signalSignature);

    return connectKey;
}

void PHPQt5Connection::removeConnections(const QObject *qo)
{
    QHashIterator<QByteArray, QList<pq_connect_entry> > i(pq_connections);

    while(i.hasNext()) {
        i.next();

        if(i.value().at(0).qo_sender == qo) {
            pq_connections.remove(i.key());
        }
    }
}

void PHPQt5Connection::removePHPConnection(const QObject *qo_sender,
                                           const QByteArray qSignalSignature_ba,
                                           const QByteArray qSlotSignature_ba
                                           TSRMLS_DC)
{
    QString args = qSlotSignature_ba.mid( qSlotSignature_ba.indexOf("("), qSlotSignature_ba.indexOf(")") );
    QByteArray signalName = qSignalSignature_ba.mid(1, qSignalSignature_ba.indexOf("(")-1); // get a "cleared" signal name
    QByteArray connectKey = generateConnectKey(qo_sender, qSignalSignature_ba.mid(1));

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
}

void PHPQt5Connection::removePHPConnection(const QByteArray connectKey,
                                           const QByteArray qSignalSignature_ba,
                                           const QByteArray pslotSignature_ba) {
    if(pq_connections.contains(connectKey)) {
        QList<pq_connect_entry> ce_list = pq_connections.value(connectKey);

        foreach(pq_connect_entry ce, ce_list) {
            if(ce.zo_receiver_is_closure) {
               Z_DELREF_P(ce.zo_receiver);
               QObject::disconnect(ce.qo_sender,
                                   qSignalSignature_ba.constData(),
                                   ce.qo_sender,
                                   pslotSignature_ba.constData());
               pq_connections.remove(connectKey);
            }
        }
    }
}

bool PHPQt5Connection::createPHPConnection(zval *zo_sender,
                                           const QObject *qo_sender,
                                           const QByteArray qSignalSignature_ba,
                                           zval *zo_receiver,
                                           const QByteArray qSlotSignature_ba
                                           TSRMLS_DC)
{
    bool receiverIsClosure = false;
    QByteArray slotSignature = qSlotSignature_ba;
    if(slotSignature == "1__invoke()") {
        slotSignature = slotSignature.mid(0,9); // remove brackeds ()
        slotSignature.append( qSignalSignature_ba.mid(qSignalSignature_ba.indexOf("(")) );
        receiverIsClosure = true;
    }

#ifdef PQDEBUG
    PQDBG2("PHPQt5Connection::createPHPConnection: ",
           QString("%1 -> %2").arg(qSignalSignature_ba.constData()).arg(slotSignature.constData()));
#endif

    QString args = slotSignature.mid( slotSignature.indexOf("("), slotSignature.indexOf(")") );
    QByteArray slotName = slotSignature.mid(1, slotSignature.indexOf("(")-1); // get a "cleared" slot name
    QByteArray signalName = qSignalSignature_ba.mid(1, qSignalSignature_ba.indexOf("(")-1); // get a "cleared" signal name
    QByteArray connectKey = generateConnectKey(qo_sender, qSignalSignature_ba.mid(1));

    QByteArray pslotSignature_ba(signalName);
    pslotSignature_ba.prepend("1");
    pslotSignature_ba.append("_pslot");
    pslotSignature_ba.append(args);

    if(receiverIsClosure) {
        removePHPConnection(connectKey, qSignalSignature_ba, pslotSignature_ba);
    }

    bool haveConnection = pq_connections.contains(connectKey)
            ? true
            : QObject::connect(qo_sender, qSignalSignature_ba.constData(), qo_sender, pslotSignature_ba.constData());

    if(haveConnection) {
        int argc = 0;
        if(args != "()") {
            argc = args.split(",").size();
        }

        QList<pq_connect_entry> ceList = pq_connections.value(connectKey, QList<pq_connect_entry>());
        ceList.append(pq_connect_entry {
                          zo_sender,
                          qo_sender,
                          qSignalSignature_ba.mid(1), // remove first symbol (is a "2" for signals)
                          zo_receiver,
                          slotSignature.mid(1), // remove first symbol (is a "1" for slots)
                          slotName.constData(),
                          argc,
                          Z_OBJVAL_P(zo_sender).handle,
                          Z_OBJVAL_P(zo_receiver).handle,
                          receiverIsClosure
                      });

        pq_connections.insert(connectKey, ceList);

        return true;
    }

    return false;
}

void PHPQt5Connection::call_php_funtion(zval *zo_sender,
                                        zval *zo_receiver,
                                        const QByteArray slotName,
                                        int argc,
                                        QVariantList args)
{
    TSRMLS_FETCH_FROM_CTX(thread_ctx);

    argc++; // the first argument for PHP-slots it is a PHP-object sender

    zval *function_name;
    MAKE_STD_ZVAL(function_name);
    ZVAL_STRING(function_name, slotName.constData(), 0);

    zval *retval;
    MAKE_STD_ZVAL(retval);

    zval *params[argc];
    MAKE_STD_ZVAL(params[0]);

    ZVAL_ZVAL(params[0], zo_sender, 1, 0);

    for(int i = 1; i < argc; i++) {
        QVariant arg = args.at(i-1);

        MAKE_STD_ZVAL(params[i]);

        switch (arg.type()) {
        case QMetaType::QString:
            ZVAL_STRING(params[i], PHPQt5::toW(arg.toByteArray()).constData(), 1);
            break;

        case QMetaType::Long:
        case QMetaType::Int:
        case QMetaType::LongLong:
            ZVAL_LONG(params[i], arg.toInt());
            break;

        case QMetaType::Double:
        case QMetaType::Float:
            ZVAL_DOUBLE(params[i], arg.toDouble());
            break;

        case QMetaType::Bool:
            ZVAL_BOOL(params[i], arg.toBool());
            break;

        default:
            ZVAL_NULL(params[i]);
        }
    }

    if(call_user_function(NULL, &zo_receiver, function_name, retval, argc, params TSRMLS_CC) == FAILURE) {
        QString s = QString("PHPQt5 could not call method: %1 of class: %2")
                .arg(Z_STRVAL_P(function_name))
                .arg(Z_OBJCE_P(zo_receiver)->name);

        php_error(E_ERROR, s.toUtf8().constData());
    }

    for(int i = 0; i < argc; i++) {
        efree(params[i]);
    }

    efree(retval);
    efree(function_name);
}
