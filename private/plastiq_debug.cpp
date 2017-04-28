#include "phpqt5.h"

#include <QMutex>

void pqdbg_send_message(int lvl, const QString &msg, const QString &title)
{
#ifdef PQDEBUG
    static QMutex mutex;

    mutex.lock();
    QLocalSocket *debugSocket = PHPQt5::debugSocket();

    if(debugSocket->isOpen()) {
        /*
        QByteArray data;

        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << reinterpret_cast<quint64>(PQEngine::pqeEngine);
        out << PQEngine::pqeCoreName;
        out << lvl;
        out << title;
        out << msg;
        out.device()->reset();

//        = QString("%1:::%2:::%3:::%4:::%5:::%")
//                .arg(reinterpret_cast<quint64>(PQEngine::pqeEngine))
//                .arg(PQEngine::pqeCoreName)
//                .arg(lvl)
//                .arg(title)
//                .arg(msg).toUtf8();

        debugSocket->write(data);
        debugSocket->waitForBytesWritten();
        */

        QString str = QString("%1:::%2:::%3:::%4:::%5:::%6:::%")
                        .arg(reinterpret_cast<quint64>(PQEngine::pqeEngine))
                        .arg(PQEngine::pqeCoreName)
                        .arg(reinterpret_cast<quint64>(QThread::currentThread()))
                        .arg(lvl)
                        .arg(title)
                        .arg(msg).toUtf8();

        /*
        QByteArray block;
        QDataStream sendStream(&block, QIODevice::ReadWrite);
        sendStream << quint16(0) << str;
        sendStream.device()->seek(0);
        sendStream << (quint16)(block.size() - sizeof(quint16));
        */

        debugSocket->write(str.toUtf8());
        debugSocket->waitForBytesWritten();
    }

    mutex.unlock();
#else
    Q_UNUSED(lvl);
    Q_UNUSED(msg);
    Q_UNUSED(title);
#endif
}

void pqdbg_send_message(const QMap<QString,QString> &msgmap)
{
#ifdef PQDEBUG
    QMapIterator<QString, QString> i(msgmap);
    QString msg = "";

    while(i.hasNext()) {
        i.next();
        QString keyval = QString("\"%1\":\"%2\"").arg(i.key()).arg(i.value());
        msg += msg.length() ? QString(",%1").arg(keyval) : keyval;
    }

    msg.append(QString(",\"lineno\":\"%1\",\"filename\":\"%2\"")
               .arg(zend_get_executed_lineno())
               .arg(zend_get_executed_filename()));

    msg.prepend("jsondata{").append("}");

    pqdbg_send_message(0, msg, "");
#else
    Q_UNUSED(msgmap);
#endif
}
