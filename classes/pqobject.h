#ifndef PQOBJECT_H
#define PQOBJECT_H

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QEvent>

extern void PHPQt5Connection_invoke(const QObject *qo_sender, const QByteArray signalSignature, QVariantList args);
extern bool pq_PHP_emit(const QString &qosignal, QObject *sender, QEvent *event);
extern bool pq_callPHPFunction(const QString &fn_name, QObject *sender, QEvent *event);
extern bool pq_registerQObject(QObject *qo);
extern void pq_ub_write(const QString &msg);
extern void pq_register_extra_zend_ce(const QString &className);
extern void pq_register_long_constant(const QString &className, const QString &constName, int value);
extern void pq_register_long_constant_ex(const QString &className, const QString &constName, int value, QString ceName);
extern void pq_register_string_constant(const QString &className, const QString &constName, const QString &value);
extern void pq_register_string_constant_ex(const QString &className, const QString &constName, const QString &value, QString ceName);

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

#define PQ_ADD_ON_SIGNAL(onSignalName, signalSignature) \
    mEnabledOnSignals.insert(onSignalName, signalSignature)

#define PQ_OBJECT \
    Q_PROPERTY( QString objectName READ objectName WRITE setObjectName ) \
public: \
    Q_INVOKABLE QString objectName() { \
        return QObject::objectName(); \
    } \
    Q_INVOKABLE void setUserProperty(const QString &property, QVariant value) {\
        m_userProps.insert(property, value);\
    }\
    Q_INVOKABLE QObject* sender() {\
        return QObject::sender();\
    }\
    Q_INVOKABLE void setPHPEventListener(const QString &fn_name) {\
        mPHPEL = fn_name;\
    }\
    Q_INVOKABLE void addPHPEventListenerType(int eventType) {\
        if(!mPHPEventListenerTypes.contains(eventType)) {\
            mPHPEventListenerTypes.append(eventType);\
        }\
    }\
    Q_INVOKABLE void removePHPEventListenerType(int eventType) {\
        mPHPEventListenerTypes.removeOne(eventType);\
    }\
    Q_INVOKABLE void clearPHPEventListenerTypes() {\
        mPHPEventListenerTypes.clear();\
    }\
    Q_INVOKABLE QByteArray getOnSignalSignature(const QByteArray &onSignalName) {\
        QByteArray signalName = onSignalName.toLower();\
        QMapIterator<QByteArray,QByteArray> i(mEnabledOnSignals);\
        while(i.hasNext()) {\
            i.next();\
            if(i.key().toLower() == signalName) {\
                return i.value();\
            }\
        }\
        return "";\
    }\
public Q_SLOTS: \
    Q_INVOKABLE void setObjectName(QString objectName) { \
        QObject::setObjectName(objectName); \
    }\
    Q_INVOKABLE QVariant getUserProperty(const QString &property) {\
        return m_userProps.value(property);\
    }\
private:\
    QHash<QString, QVariant> m_userProps;\
    QList<int> m_enabledEvents;\
    QString mPHPEL;\
    QList<int> mPHPEventListenerTypes;\
    QMap<QByteArray,QByteArray> mEnabledOnSignals;

class PQObject;
class PQObject : public QObject {
    Q_OBJECT
    PQ_OBJECT

public:
    Q_INVOKABLE explicit PQObject(QObject *parent = 0);
    virtual ~PQObject();
};

#endif // PQOBJECT_H
