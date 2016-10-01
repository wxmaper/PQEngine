#ifndef PLASTIQCLASSES_H
#define PLASTIQCLASSES_H

#include <QByteArray>
#include <QHash>

class PlastiQClasses
{
public:
    static int typeId(const QByteArray &typeName);
private:
    static QHash<QByteArray, int> plastiqTypes;
};
#endif // PLASTIQCLASSES_H
