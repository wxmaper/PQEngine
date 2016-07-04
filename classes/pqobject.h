/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PQEngine.
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

#ifndef PQOBJECT_H
#define PQOBJECT_H

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QEvent>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QMetaType>

#include "pqtypes.h"

struct _zend_object;

extern void PHPQt5Connection_invoke(QObject *qo_sender,
                                    const QByteArray signalSignature,
                                    QVariantList args);

extern QObject* pq_createObject(const QString &className, const QVariantList &args);
template<typename T>
T *pq_createObjectT(const QString &className, const QVariantList &args) {
    QObject *qo = pq_createObject(className, args);

    T *t = dynamic_cast<T*>(qo);
    if(!t || t == Q_NULLPTR) {
        pq_php_error(QString("<b>PQExtension Error</b>: cannot casting '%1' to template typename '%2'")
                     .arg(qo->metaObject()->className())
                     .arg(typeid(T).name()));
    }

    return t;
}

extern bool pq_registerQObject(QObject *qo);
extern void pq_ub_write(const QString &msg);
extern bool pq_send_event(QObject *sender, QEvent *event);
extern void pq_register_extra_zend_ce(const QString &className);
extern void pq_register_qevent_zend_ce(const QString &className);
extern void pq_register_long_constant(const QString &className, const QString &constName, int value);
extern bool pq_register_long_constant_ex(const QString &className, const QString &constName, int value, QString ceName);
extern void pq_register_string_constant(const QString &className, const QString &constName, const QString &value);
extern void pq_register_string_constant_ex(const QString &className, const QString &constName, const QString &value, QString ceName);

extern void pq_invoke_closure(PQClosure closure, const QVariantList &args);

extern void pq_php_error(const QString &error);
extern void pq_php_warning(const QString &warning);
extern void pq_php_notice(const QString &notice);

enum PQEventTypes {
    Clicked = 1040, // QToolButton, QPushButton, PQCheckBox
    Pressed = 1041, // QToolButton, QPushButton, PQCheckBox
    Released = 1042, // QToolButton, QPushButton, PQCheckBox
    IndexChaged = 1050, // ComboBox::onCurrentIndexChanged
    Toggled = 1060, // PQCheckBox, PQAction, QPushButton
    Triggered = 1061, // PQAction
    ProcessError = 1070, // PQProcess
    ProcessFinished = 1071, // PQProcess
    ProcessStarted = 1072, // PQProcess
    ProcessStateChanged = 1073, // PQProcess
};

#define PQOBJECT_ON_SIGNALS() \
    PQ_ADD_ON_SIGNAL("onDestroyed", "destroyed()");\
    PQ_ADD_ON_SIGNAL("onObjectNameChanged", "objectNameChanged(string)");\

#define PQ_ADD_ON_SIGNAL(onSignalName, signalSignature) \
    m_enabledOnSignals.insert(onSignalName, signalSignature)

#define PQ_ON_SIGNAL_DECL_START private: void declareOnSignals() {\
    PQOBJECT_ON_SIGNALS();

#define PQ_ON_SIGNAL_DECL_END }

#define PQ_ON_SIGNAL(s) declareOnSignal(#s)

#define PQ_OBJECT_EX(Q) \
    Q_PROPERTY( QString objectName READ objectName WRITE setObjectName ) \
public: \
    ~P##Q();\
    Q_INVOKABLE QString objectName();\
    Q_INVOKABLE QObject* sender();\
    Q_INVOKABLE void removeEventListenerType(int eventType);\
    Q_INVOKABLE void clearEventListenerTypes();\
    Q_INVOKABLE QByteArray getOnSignalSignature(const QByteArray &onSignalName);\
    Q_INVOKABLE QStringList getOnSignals() const;\
    Q_INVOKABLE void addEventListenerType(int eventType);\
    Q_INVOKABLE void enableEventListener();\
    Q_INVOKABLE void disableEventListener();\
    Q_INVOKABLE bool declareSignal(const QByteArray &signalName);\
    Q_INVOKABLE bool haveSignal(const QByteArray &signalName);\
    Q_INVOKABLE bool haveSignalConnection(const QByteArray &signalName);\
    Q_INVOKABLE void incConnection(const QByteArray &signalName);\
    Q_INVOKABLE void decConnection(const QByteArray &signalName);\
    Q_INVOKABLE bool event(PQObject *event);\
    Q_INVOKABLE bool parentEvent(PQObject *event);\
    Q_INVOKABLE QVariant property(const QString &name);\
    bool event(QEvent *event);\
    /*Q_INVOKABLE QObject *parent();*/\
    Q_INVOKABLE _zend_object *__pq_getZObject();\
    \
    Q_SIGNAL void destroyed(_zend_object*);\
    \
public Q_SLOTS: \
    Q_INVOKABLE void setObjectName(QString objectName);\
    Q_INVOKABLE bool setProperty(const QString &name, const QVariant &value);\
    Q_INVOKABLE void __pq_setThisPtr(QObject *o);\
    Q_INVOKABLE void __pq_setZObject(_zend_object *zobject);\
    \
private:\
    void declareOnSignal(const QString &pslot);\
    \
    QMap<QByteArray,QByteArray> m_enabledOnSignals;\
    QList<int> m_eventListenerTypes;\
    QHash<QByteArray,int> m_signals;\
    bool m_useEventListener = false;\
    static QObject *__pq_thisPtr;\
    _zend_object *__pq_zobject = Q_NULLPTR;

class PQObject;
class PQObject : public QObject {
    Q_OBJECT
    PQ_OBJECT_EX(QObject)

public:
    Q_INVOKABLE explicit PQObject(QObject *parent = Q_NULLPTR);
};

Q_DECLARE_METATYPE(PQObject*)

#endif // PQOBJECT_H
