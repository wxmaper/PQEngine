#ifndef PLASTIQOBJECT_H
#define PLASTIQOBJECT_H

#include "plastiqmetaobject.h"
#include "plastiqmethod.h"
#include <QByteArray>
#include <QHash>
#include <QDebug>

#define PQ_CONSTRUCTOR(x)
#define PQ_PUBLIC_METHOD(x)
#define PQ_PUBLIC_SLOT(x)
#define PQ_PUBLIC_SIGNAL(x)
#define PQ_PUBLIC_STATIC_METHOD(x)
#define PQ_PRIVATE_METHOD(x)

#define PLASTIQ_OBJECT(objectType,className,parentClassName) \
    public:\
        enum ObjectType {\
            IsObject,\
            IsQObject,\
            IsQWidget,\
            IsQWindow,\
            IsQtObject\
        };\
        \
        int plastiq_metacall(PlastiQMetaObject::Call call, int id, const PMOGStack &stack);\
        virtual const PlastiQMetaObject *metaObject() const;\
        static PlastiQMetaObject plastiq_metaObject;\
        static const uint plastiq_classId;\
        static const uint plastiq_objectType;\
    private:\
        static void plastiq_static_metacall(PlastiQObject *object, PlastiQMetaObject::Call call, int id, const PMOGStack &stack);\
        static QHash<QByteArray, PlastiQMethod> platiqMethods;\
        static QHash<QByteArray, PlastiQMethod> platiqConstructors;\
        static QHash<QByteArray, PlastiQMethod> platiqSignals;\

#define PLASTIQ_OBJECT_EX \
    public:\
        enum ObjectType {\
            IsObject,\
            IsQObject,\
            IsQWidget,\
            IsQWindow,\
            IsQtObject\
        };\
        \
        int plastiq_metacall(PlastiQMetaObject::Call call, int id, const PMOGStack &stack);\
        virtual const PlastiQMetaObject *metaObject() const;\
        static PlastiQMetaObject plastiq_metaObject;\
        static const uint plastiq_classId;\
        static const uint plastiq_objectType;\
    private:\
        static void plastiq_static_metacall(PlastiQObject *object, PlastiQMetaObject::Call call, int id, const PMOGStack &stack);\
        static QHash<QByteArray, PlastiQMethod> platiqMethods;\
        static QHash<QByteArray, PlastiQMethod> platiqConstructors;\
        static QHash<QByteArray, PlastiQMethod> platiqSignals;\

extern void PHPQt5Connection_invoke_new(PQObjectWrapper *sender, PQObjectWrapper *receiver,
                                        const QByteArray &slot, const QByteArray &signature,
                                        const int argc, const PMOGStack &stack);

class PlastiQObject
{
    PLASTIQ_OBJECT_EX

public:

    PlastiQObject();
    PlastiQObject(const QByteArray &typeName, int typeId, void *data);
    PlastiQObject(const QByteArray &typeName, void *data = Q_NULLPTR);
    PlastiQObject(const QByteArray &typeName, const QByteArray &constructorSignature, const PMOGStack &stack);

    void setData(void *data, PlastiQObject *ddata);
    void *data();
    PlastiQMetaObject *dynamicMetaObject();

    ObjectType objectType();

//    bool invoke(PlastiQMetaObject::Call call, const QByteArray &signature, const PMOGStack &stack);
//    bool invokeMethod(const QByteArray &signature, const PMOGStack &stack);

private:
    QByteArray m_typeName;
    int m_typeId;
    bool m_isNull;
    void *dptr = Q_NULLPTR;

protected:
    PlastiQObject *ddptr = Q_NULLPTR;
};

#endif // PLASTIQOBJECT_H
