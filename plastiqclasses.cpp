#include "plastiqclasses.h"

QHash<QByteArray, int> PlastiQClasses::plastiqTypes = {
    { "QObject", 0 },
};

int PlastiQClasses::typeId(const QByteArray &typeName)
{
    return plastiqTypes.value(typeName, -1);
}

