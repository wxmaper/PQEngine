#include "plastiqmethod.h"

bool PlastiQMethod::exists()
{
    return (index >= 0);
}

PlastiQMethod::PlastiQMethod()
    : name(""), argTypes(""), returnType(""), index(-1), access(None), type(Unknown)
{ }

PlastiQMethod::PlastiQMethod(const QByteArray &_n, const QByteArray &_at, const QByteArray &_r, int _i, PlastiQMethod::Access _a, PlastiQMethod::Type _t)
    : name(_n), argTypes(_at), returnType(_r), index(_i), access(_a), type(_t)
{ }

PlastiQMethod &PlastiQMethod::operator =(const PlastiQMethod &other) {
    this->access = other.access;
    this->index = other.index;
    this->name = other.name;
    this->offset = other.offset;
    this->returnType = other.returnType;
    this->argTypes = other.argTypes;
    this->type = other.type;
    return *this;
}
