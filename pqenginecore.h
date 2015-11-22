#ifndef PQENGINECORE_H
#define PQENGINECORE_H

#include "pqengine.h"

class PQEngineCore : public IPQExtension {
public:
    QMetaObjectList classes();
    bool start();
    bool finalize();

    static QCoreApplication * instance(int argc, char** argv);
    static void ub_write(const QString &msg);
    static void pre(const QString &msg, const QString &title);

    PQEXT_USE(instance)
    PQEXT_USE(ub_write)
    PQEXT_USE(pre)

    PQEXT_ENTRY_START(PQEngineCore)
        PQEXT_INSTANCE(PQEngineCore)
        PQEXT_UB_WRITE(PQEngineCore)
        PQEXT_NO_PRE
    PQEXT_ENTRY_END
};

#endif // PQENGINECORE_H
