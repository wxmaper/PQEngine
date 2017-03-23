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
        Static = 0x010,
        Virtual = 0x020,
        VirtualProtected = Virtual | Protected,
        VirtualPublic = Virtual | Public,
        StaticPublic = Static | Public
    };

    enum Type {
        Unknown = 0,
        Method,
        Constructor,
        Slot,
        Signal
    };

    QByteArray name;
    QByteArray argTypes;
    QByteArray returnType;
    int index = -1;
    int offset = 0;
    Access access = None;
    Type type = Unknown;

    bool exists();

    PlastiQMethod();

    PlastiQMethod(const QByteArray &_n,
                  const QByteArray &_at,
                  const QByteArray &_r,
                  int _i,
                  Access _a,
                  Type _t);

    PlastiQMethod &operator =(const PlastiQMethod &other);
};

#endif // PLASTIQMETHOD_H
