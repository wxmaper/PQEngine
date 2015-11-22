#include <QTextStream>
#include <QTextDocument>
#include "pqengine.h"
#include "pqengine_private.h"

/* Static vars */
PQEnginePrivate *pqe;
bool PQEngine::pqeInitialized = false;
PQExtensionHash PQEngine::m_PQExtensions;

/* PQEngine class */
PQEngine::PQEngine(PQExtensionHash pqExtensions, MemoryManager mmng)
{
    m_PQExtensions = pqExtensions;
    m_mmng = mmng;
}

bool PQEngine::init(int argc,
                    char **argv,
                    const char *coreName,
                    const char *hashKey,
                    const char *appName,
                    const char *appVersion,
                    const char *orgName,
                    const char *orgDomain)
{
    pqe = new PQEnginePrivate(m_PQExtensions, m_mmng);
    pqeInitialized = pqe->init(argc, argv, coreName, hashKey, appName, appVersion, orgName, orgDomain);
    return pqeInitialized;
}

int PQEngine::exec(const char *script)
{
    if(pqeInitialized) {
        return pqe->exec(script);
    }

    return 1;
}

void ***PQEngine::getTSRMLS()
{
    return pqe->getTSRMLS();
}

void default_ub_write(const QString &msg, const QString &title)
{
    QTextDocument m;
    m.setHtml(msg);

    if(title.length()) {
        QTextStream( stdout ) << title << m.toPlainText().toUtf8().constData() << endl;
    }
    else {
        QTextStream( stdout ) << m.toPlainText().toUtf8().constData() << endl;
    }
}

