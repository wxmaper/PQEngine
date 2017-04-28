#ifndef PLASTIQOBJECT_H
#define PLASTIQOBJECT_H

#include "plastiqmetaobject.h"
#include "plastiqmethod.h"
#include "plastiq.h"
#include <QByteArray>
#include <QHash>
#include <QVector>

#define PQ_CONSTRUCTOR(x)
#define PQ_PUBLIC_METHOD(x)
#define PQ_PUBLIC_SLOT(x)
#define PQ_PUBLIC_SIGNAL(x)
#define PQ_PUBLIC_STATIC_METHOD(x)
#define PQ_PRIVATE_METHOD(x)
#define PQ_ENUM(name,...)

#define PLASTIQ_INHERITS(...)
#define PLASTIQ_OBJECT(objectType,className,parentClassName) \
    public:\
        int plastiq_metacall(PlastiQMetaObject::Call call, int id, const PMOGStack &stack);\
        virtual const PlastiQMetaObject *plastiq_metaObject() const;\
        static PlastiQMetaObject plastiq_static_metaObject;\
        static const PlastiQ::ObjectType plastiq_static_objectType;\
    private:\
        static void plastiq_static_metacall(PlastiQObject *object, PlastiQMetaObject::Call call, int id, const PMOGStack &stack);\
        static QVector<PlastiQMetaObject*> plastiqInherits;\
        static QHash<QByteArray, PlastiQMethod> plastiqMethods;\
        static QHash<QByteArray, PlastiQMethod> plastiqConstructors;\
        static QHash<QByteArray, PlastiQMethod> plastiqSignals;\
        static QHash<QByteArray, PlastiQProperty> plastiqProperties;\
        static QHash<QByteArray, long> plastiqConstants;

extern bool PlastiQ_activate(PQObjectWrapper *sender, const char *signal,
                             PQObjectWrapper *receiver, const char *slot,
                             int argc, const PMOGStack &stack);
extern bool PlastiQ_anon_activate(QObject *receiverObject, const char *slot,
                                  int argc, const PMOGStack &stack);
extern bool PlastiQ_event(QObject *eventFilter, QObject *obj, QEvent *event);
extern PQObjectWrapper *PlastiQ_getWrapper(const PMOGStackItem &stackItem);
extern bool PlastiQ_have_virtual_call(PQObjectWrapper *pqobject, const QByteArray &methodSignature);
extern void PlastiQ_virtual_call(PQObjectWrapper *pqobject, const QByteArray &methodSignature, PMOGStack stack);
extern void PlastiQ_self_destroy(PQObjectWrapper *pqobject);
extern void PlastiQ_connectSlotsByName(PQObjectWrapper *pqobject);

class VirtualMethod {
    QByteArray _functionName;
    int _argc;

public:
    VirtualMethod() {}
    VirtualMethod(const QByteArray &functionName,
                  int argc) :
        _functionName(functionName),
        _argc(argc) {}

    void call(PQObjectWrapper *pqobject, PMOGStack stack) const;
};

class VirtualMethodList : public QHash<QByteArray,VirtualMethod> {};

class PlastiQObject // : public QObject
{
    //Q_OBJECT
    PLASTIQ_OBJECT(IsObject,PlastiQObject,Q_NULLPTR)

public:
    PlastiQObject();
    PlastiQObject(const QByteArray &typeName, int typeId, void *plastiq_data);
    PlastiQObject(const QByteArray &typeName, void *plastiq_data = Q_NULLPTR);
    virtual ~PlastiQObject();

    void plastiq_setData(void *plastiq_data, PlastiQObject *ddata);
    void *plastiq_data();
    PlastiQMetaObject *plastiq_dynamicMetaObject();

    PlastiQ::ObjectType plastiq_objectType();
    QObject *plastiq_toQObject();
    bool plastiq_haveParent(); // only for QObject|QWidget|QWindow

    bool isWrapper();
    void setWrapperMark(bool isWrapper);

private:
    QByteArray m_typeName;
    int m_typeId;
    bool m_isNull;
    bool m_isWrapper;

protected:
    void *dptr = Q_NULLPTR;
    PlastiQObject *ddptr = Q_NULLPTR;
};

#endif // PLASTIQOBJECT_H
