#ifndef PQENGINE_H
#define PQENGINE_H

#include "ipqengineext.h"
#include "pqengine_global.h"

typedef struct _PQEngineModule {
    void (*ub_write)(const QString &msg,
                     const QString &title);
    void (*pre)(const QString &msg,
                     const QString &title);
} PQEngineModule;

class PQDLAPI PQEngine
{
public:
    PQEngine(PQExtensionList extensions = PQExtensionList());

    bool                    init(int argc,
                                 char **argv,
                                 QString pmd5,
                                 const QString &coreName = "",
                                 bool checkName = false,
                                 const QString &hashKey = "0x0",
                                 const QString &appName = "PQEngine application",
                                 const QString &appVersion = "0.1",
                                 const QString &orgName = "PHPQt5",
                                 const QString &orgDomain = "phpqt.ru");

    int                     exec(const char *script = "-");

    static PQExtensionList  m_extensions;

private:
    static bool pqeInitialized;
};

#endif // PQENGINE_H
