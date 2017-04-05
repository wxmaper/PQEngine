#ifndef PLASTIQMETAOBJECT_H
#define PLASTIQMETAOBJECT_H

#include <QHash>
#include <QPair>
#include <QByteArray>
#include <QVariant>
#include <QMetaObject>

#include "plastiqmethod.h"
#include "plastiqproperty.h"
#include "plastiq.h"

enum Call;
class PlastiQObject;
struct PlastiQMetaObject;
struct PlastiQCandidateMethod;
struct PQObjectWrapper;

struct PMOGStackItem {
    PlastiQ::ItemType type;
    QByteArray name;
    bool isRef = false;

    void* s_voidp = Q_NULLPTR;
    void** s_voidpp;
    PlastiQObject* s_object;
    PQObjectWrapper* s_pqobject;
    char *s_charstar;

    bool s_bool;

    long s_long;
    int s_int;
    unsigned long s_ulong;
    unsigned int s_uint;

    float s_float;
    double s_double;
    char s_char;
    uchar s_uchar;

    //quint16 s_uint16;
    //quint32 s_uint32;
    //quint64 s_uint64;

    //qint16 s_int16;
    //qint32 s_int32;
    //qint64 s_int64;

#ifdef Q_PROCESSOR_X86_64
    int s_enum;
#else
    qint64 s_enum;
#endif

    QString s_string;
    QByteArray s_bytearray;
    QVariant s_variant;
};

typedef PMOGStackItem* PMOGStack;

struct PlastiQObjectData {
    const QByteArray className;
    int classId;
};

struct PlastiQMetaObject
{
    enum Call {
        InvokeMethod,
        InvokeStaticMember,
        ReadProperty,
        WriteProperty,
        CreateInstance,
        CreateDataInstance,
        CreateConnection,
        DownCast,
        IndexOfMethod,
        ToQObject,
        HaveParent
    };

    const char *className() const;
    int methodId(const QByteArray &signature, PlastiQMethod::Access filter = PlastiQMethod::None) const;
    int signalId(const QByteArray &signature) const;
    int constructorId(const QByteArray &signature) const;
    bool haveVirtualMethod(const QByteArray &methodName, int argc, QByteArray &signature) const;

    static QObject *toQObject(PlastiQObject *object);
    static bool haveParent(PlastiQObject *object);
    static bool invokeMethod(PlastiQObject *object, const QByteArray &signature, const PMOGStack &stack);
    static bool connect(PQObjectWrapper *sender, const QByteArray &signal,
                        PQObjectWrapper *receiver, const QByteArray &slot);
    PlastiQObject *newInstance(const QByteArray &signature, const PMOGStack &stack);
    PlastiQObject *createInstanceFromData(void *data);
    bool static_metacall(Call call, int id, const PMOGStack &stack);
    const QList<PlastiQCandidateMethod> candidates(const QByteArray &methodName, int argc, PlastiQMethod::Type type, PlastiQMethod::Access filter = PlastiQMethod::None, bool ignoreCase = false) const;

    struct {
        const PlastiQMetaObject *superdata;
        const QVector<PlastiQMetaObject*> *inherits;
        const char *className;
        const PlastiQ::ObjectType *objectType;
        const QHash<QByteArray, PlastiQMethod> *pq_constructors;
        const QHash<QByteArray, PlastiQMethod> *pq_methods;
        const QHash<QByteArray, PlastiQMethod> *pq_signals;
        const QHash<QByteArray, PlastiQProperty> *pq_properties;
        const QHash<QByteArray, long> *pq_constants;
        typedef void (*MetacallFunction)(PlastiQObject *, Call, int, const PMOGStack &);
        MetacallFunction static_metacall;
    } d;
};

#endif // PLASTIQMETAOBJECT_H
