#ifndef PLASTIQMETHOD_H
#define PLASTIQMETHOD_H

#include <QByteArray>
#include <QList>

struct PlastiQCandidateMethod {
    const QByteArray name;
    const int argc;
    const QStringList argTypes;
    const int idx;
};

struct PlastiQMethod {
    enum Access {
        None = 0x001,
        Public = 0x002,
        Private = 0x004,
        Protected = 0x008,
        Static = 0x010
    };

    enum Type {
        Unknown = 0,
        Method,
        Constructor,
        Slot,
        Signal
    };

    QByteArray name = "";
    QByteArray signature = "";
    QByteArray metaSignature = "";
    QByteArray returnType = "";
    int index = -1;
    int offset = 0;
    Access access = None;
    Type type = Unknown;

    bool exists();

    PlastiQMethod();

    PlastiQMethod(const QByteArray &_n,
               const QByteArray &_s,
               const QByteArray &_m,
               const QByteArray &_r,
               int _i,
               Access _a,
               Type _t);

    PlastiQMethod &operator =(const PlastiQMethod &other);
};

#endif // PLASTIQMETHOD_H
