#ifndef PQENGINE_H
#define PQENGINE_H

#include "ipqengineext.h"
#include "pqengine_global.h"

#ifndef NO_IMPORT_QT_PLUGINS
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QDDSPlugin)
Q_IMPORT_PLUGIN(QICNSPlugin)
Q_IMPORT_PLUGIN(QICOPlugin)
Q_IMPORT_PLUGIN(QJp2Plugin)
Q_IMPORT_PLUGIN(QMngPlugin)
Q_IMPORT_PLUGIN(QTgaPlugin)
Q_IMPORT_PLUGIN(QTiffPlugin)
Q_IMPORT_PLUGIN(QWbmpPlugin)
Q_IMPORT_PLUGIN(QWebpPlugin)
#endif



typedef struct _PQEngineModule {
    void (*ub_write)(const QString &msg,
                     const QString &title);
    void (*pre)(const QString &msg,
                     const QString &title);
} PQEngineModule;

class PQDLAPI PQEngine
{
public:
    PQEngine(PQExtensionHash pqExtensions = PQExtensionHash(),
             MemoryManager m_mmng = MemoryManager::Hybrid);

    bool                    init(int argc,
                                 char **argv,
                                 const char *coreName = "",
                                 const char *hashKey = "0x0",
                                 const char *appName = "PQEngine application",
                                 const char *appVersion = "0.1",
                                 const char *orgName = "PHPQt5",
                                 const char *orgDomain = "phpqt.ru");

    int                     exec(const char *script = "-");

    static void***          getTSRMLS();
    static PQExtensionHash  m_PQExtensions;

private:
    static bool pqeInitialized;
    MemoryManager m_mmng;
};


#endif // PQENGINE_H
