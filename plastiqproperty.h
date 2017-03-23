#ifndef PLASTIQPROPERTY_H
#define PLASTIQPROPERTY_H

#include <QByteArray>
#include <QList>

struct PlastiQProperty {
    QByteArray name;
    QByteArray type;
    QByteArray setter;
    QByteArray getter;

    PlastiQProperty();

    PlastiQProperty(const QByteArray &_n,
                    const QByteArray &_t,
                    const QByteArray &_s,
                    const QByteArray &_g);

    PlastiQProperty &operator =(const PlastiQProperty &other);
};

#endif // PLASTIQPROPERTY_H
