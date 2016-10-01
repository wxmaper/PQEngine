#include "plastiqproperty.h"

PlastiQProperty::PlastiQProperty()
    : name(""), type(""), setter(""), getter("")
{ }

PlastiQProperty::PlastiQProperty(const QByteArray &_n, const QByteArray &_t, const QByteArray &_s, const QByteArray &_g)
    : name(_n), type(_t), setter(_s), getter(_g)
{ }

PlastiQProperty &PlastiQProperty::operator =(const PlastiQProperty &other)
{
    this->name = other.name;
    this->type = other.type;
    this->setter = other.setter;
    this->getter = other.getter;

    return *this;
}
