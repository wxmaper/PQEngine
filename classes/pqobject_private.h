#ifndef PQOBJECT_PRIVATE_H
#define PQOBJECT_PRIVATE_H

#include "pqobject.h"
#include "pqevent.h"

#define PQOBJECT_STANDARD_METHODS(Q,QQ) \
    QString P##Q::objectName() \
    { \
        return QObject::objectName(); \
    } \
    \
    QObject* P##Q::sender() \
    {\
        return QQ::sender();\
    }\
    \
    void P##Q::addEventListenerType(int eventType) \
    {\
        if(!m_eventListenerTypes.contains(eventType)) {\
            m_eventListenerTypes.append(eventType);\
        }\
    }\
    \
    void P##Q::removeEventListenerType(int eventType) \
    {\
        m_eventListenerTypes.removeOne(eventType);\
    }\
    \
    void P##Q::clearEventListenerTypes() \
    {\
        m_eventListenerTypes.clear();\
    }\
    \
    QByteArray P##Q::getOnSignalSignature(const QByteArray &onSignalName) \
    {\
        QByteArray signalName = onSignalName.toLower();\
        QMapIterator<QByteArray,QByteArray> i(m_enabledOnSignals);\
        while(i.hasNext()) {\
            i.next();\
            if(i.key().toLower() == signalName) {\
                return i.value();\
            }\
        }\
        return "";\
    }\
    \
    bool P##Q::event(PQObject *pevent) \
    { \
        QEvent *e = dynamic_cast<QEvent*>(pevent);\
        if(e) {\
            return this->event(e);\
        }\
        else {\
            pq_php_error("P##Q::event: cannot convert property 1 to QEvent*");\
        }\
    }\
    \
    bool P##Q::parentEvent(PQObject *pevent) \
    { \
        PQEvent *e = dynamic_cast<PQEvent*>(pevent);\
        if(e) {\
            return QQ::event(e->getEvent());\
        }\
        else {\
            pq_php_error("P##Q::event: cannot convert property 1 to QEvent*");\
        }\
    }\
    bool P##Q::event(QEvent *event) \
    { \
        if(m_useEventListener && m_eventListenerTypes.contains(event->type())) {\
            if(pq_send_event(this, event)) {\
                return true; \
            }\
        }\
        return QQ::event(event);\
    }\
    \
    void P##Q::setObjectName(QString objectName) \
    { \
        QQ::setObjectName(objectName); \
    }\
    \
    bool P##Q::setProperty(const QString &name, const QVariant &value) \
    {\
        return QQ::setProperty(name.toUtf8().constData(), value);\
    }\
    \
    QVariant P##Q::property(const QString &name) \
    {\
        return QQ::property(name.toUtf8().constData());\
    }\
    \
    void P##Q::declareOnSignal(const QString &pslot) \
    {\
        QString slotMethod = QString(pslot).replace("_pslot(", "("); \
        QString slotMethodName = slotMethod.mid(0, slotMethod.indexOf("(")); \
        QString clearedSignature = pslot.mid(pslot.indexOf("("), pslot.indexOf(")") - pslot.indexOf("(") + 1);\
        QString upperedPslot = slotMethod.mid(0,1).toUpper() + slotMethod.mid(1);\
        QString onSignalMethod = "on" + upperedPslot;\
        QString onSignalMethodName = onSignalMethod.mid(0, onSignalMethod.indexOf("("));\
        QString signalSignature = QString("%1%2")\
                .arg(slotMethodName)\
                .arg(clearedSignature);\
        m_enabledOnSignals.insert(onSignalMethodName.toUtf8(), signalSignature.toUtf8());\
    }\
    \
    QStringList P##Q::getOnSignals() const \
    {\
        QStringList onsignals;\
        QMapIterator<QByteArray,QByteArray> i(m_enabledOnSignals);\
        while(i.hasNext()) {\
            i.next();\
            onsignals.append(QString("%1%2").arg(i.key().constData())\
                                            .arg(i.value().mid(i.value().indexOf("("),i.value().indexOf(")")).constData()));\
        }\
        return onsignals;\
    }\
    void P##Q::enableEventListener() {\
        m_useEventListener = true; \
    }\
    void P##Q::disableEventListener() {\
        m_useEventListener = false; \
    }\
    \
    bool P##Q::declareSignal(const QByteArray &signalName) {\
        QByteArray s = QByteArray(signalName).prepend("2");\
        if(m_signals.contains(signalName)\
            || m_signals.contains(s)) return false;\
        m_signals.insert(s,0);\
        return true;\
    }\
    \
    bool P##Q::haveSignal(const QByteArray &signalName) {\
        QByteArray s = QByteArray(signalName).prepend("2");\
        return m_signals.contains(signalName) ? true : m_signals.contains(s);\
    }\
    \
    bool P##Q::haveSignalConnection(const QByteArray &signalName) {\
        bool ret = false;\
        QByteArray s = QByteArray(signalName).prepend("2");\
        if(m_signals.contains(signalName)\
                && m_signals.value(signalName) > 0) ret = true;\
        else if(m_signals.contains(s) \
                && m_signals.value(s) > 0) ret = true;\
        return ret;\
    }\
    \
    void P##Q::incConnection(const QByteArray &signalName) {\
        QByteArray s = QByteArray(signalName).prepend("2");\
        if(m_signals.contains(signalName)) {\
            m_signals[signalName] = m_signals[signalName] + 1;\
        }\
        else if(m_signals.contains(s)) {\
            m_signals[s] = m_signals[s] + 1;\
        }\
    }\
    \
    void P##Q::decConnection(const QByteArray &signalName) {\
        QByteArray s = QByteArray(signalName).prepend("2");\
        if(m_signals.contains(signalName)) \
            m_signals[signalName] = m_signals[signalName] - 1;\
        else if(m_signals.contains(s)) \
            m_signals[s] = m_signals[s] - 1;\
    }

#endif // PQOBJECT_PRIVATE_H
