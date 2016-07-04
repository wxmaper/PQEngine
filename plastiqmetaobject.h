#ifndef PLASTIQMETAOBJECT_H
#define PLASTIQMETAOBJECT_H

#include <QHash>
#include <QPair>
#include <QByteArray>
#include <QMetaObject>

#include "plastiqmethod.h"
// #include "plastiqobject.h"
// struct PlastiQMethod;
class PlastiQObject;

struct PMOGStackItem {
    short typeId;

    void* s_voidp;
    void** s_voidpp;

    bool s_bool;

    long s_long;
    int s_int;

    float s_float;
    double s_double;

    quint16 s_uint16;
    quint32 s_uint32;
    quint64 s_uint64;

    qint16 s_int16;
    qint32 s_int32;
    qint64 s_int64;

    QByteArray s_bytearray;
    QString s_string;
    char *s_charstar;
};

typedef PMOGStackItem* PMOGStack;

struct PlastiQObjectData {
    const QByteArray className;
    int classId;
};

enum Call;
// enum PlastiQObject::ObjectType;
// enum Type;
struct PlastiQMetaObject;
struct PlastiQCandidateMethod;
struct PQObjectWrapper;

struct PlastiQMetaObject
{
    enum Call {
        InvokeMethod,
        ReadProperty,
        WriteProperty,
        CreateInstance,
        CreateConnection,
        IndexOfMethod
    };

    const char *className() const;
    int classId() const;
    int methodId(const QByteArray &signature) const;
    int constructorId(const QByteArray &signature) const;

    static bool invokeMethod(PlastiQObject *object, const QByteArray &signature, const PMOGStack &stack);
    static bool connect(PQObjectWrapper *sender, const QByteArray &signal,
                        PQObjectWrapper *receiver, const QByteArray &slot);
    PlastiQObject *newInstance(const QByteArray &signature, const PMOGStack &stack);
    bool static_metacall(Call call, int id, const PMOGStack &stack);
    const QList<PlastiQCandidateMethod> candidates(const QByteArray &methodName, int argc, PlastiQMethod::Type type) const;

    struct {
        const PlastiQMetaObject *superdata;
        const char *className;
        const uint *classId;
        const uint *objectType;
        const QHash<QByteArray, PlastiQMethod> *pq_constructors;
        const QHash<QByteArray, PlastiQMethod> *pq_methods;
        const QHash<QByteArray, PlastiQMethod> *pq_signals;
        typedef void (*MetacallFunction)(PlastiQObject *, Call, int, const PMOGStack &);
        MetacallFunction static_metacall;
    } d;
};

#endif // PLASTIQMETAOBJECT_H
