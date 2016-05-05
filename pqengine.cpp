#include <QTextStream>
#include "pqtypes.h"
#include "pqengine.h"
#include "pqengine_private.h"

/* Static vars */
PQEnginePrivate *m_engine;
bool PQEngine::pqeInitialized = false;
PQExtensionList PQEngine::m_extensions;
QByteArray m_corename;
#include <QtGlobal>
/* PQEngine class */
PQEngine::PQEngine(PQExtensionList extensions)
{
    m_extensions = extensions;
}

bool PQEngine::init(int argc,
                    char **argv,
                    QString pmd5,
                    const QString &coreName,
                    bool checkName,
                    const QString &hashKey,
                    const QString &appName,
                    const QString &appVersion,
                    const QString &orgName,
                    const QString &orgDomain)
{
    qRegisterMetaType<PQClosure>("PQClosure");
    //qRegisterMetaType<PQClosure*>("PQClosure*");

    m_corename = coreName.toUtf8();
    m_engine = new PQEnginePrivate(m_extensions);

    pqeInitialized = m_engine->init(argc, argv, pmd5, coreName, checkName, hashKey, appName, appVersion, orgName, orgDomain);
    pmd5.fill(0);

    return pqeInitialized;
}

int PQEngine::exec(const char *script)
{
    if(pqeInitialized) {
#ifdef PQDEBUG
        return m_engine->exec(script, 0);
#else
        return m_engine->exec(script);
#endif
    }

    return 1;
}

QString cleanTag(QString body) {
    body.replace("&nbsp;"," ");
    body.replace("<br>","##%break%##");
    body.replace("</br>","##%break%##");
    body.replace("</p>","##%break%##");
    body.replace("</td>","##%break%##");
    body.replace("\r\n","##%break%##");
    body.replace("\n","##%break%##");

    body.remove(QRegExp("<head>(.*)</head>",Qt::CaseInsensitive));
    body.remove(QRegExp("<form(.)[^>]*</form>",Qt::CaseInsensitive));
    body.remove(QRegExp("<script(.)[^>]*</script>",Qt::CaseInsensitive));
    body.remove(QRegExp("<style(.)[^>]*</style>",Qt::CaseInsensitive));
    body.remove(QRegExp("<(.)[^>]*>"));

    body.replace("##%break%##", "\n");

    return body;
}

void default_ub_write(const QString &msg, const QString &title)
{
    QString m = cleanTag(msg);

/*
#ifdef PQDEBUG
    QString filename = qApp->applicationDirPath() + "/pqdebug.log";

    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text) )
    {
        QTextStream stream(&file);
        stream << m.toUtf8().constData() << endl;
        stream.flush();
        file.close();
    }
#endif
*/

    if(title.length()) {
        QTextStream( stdout ) << title << m.toUtf8().constData() << endl;
    }
    else {
        QTextStream( stdout ) << m.toUtf8().constData() << endl;
    }
}

#ifdef PQDEBUG
int __pqdbg_current_d_lvl = 0;
int __pqdbg_current_d_line = 0;

int pqdbg_get_current_lvl() {
    return __pqdbg_current_d_lvl;
}

QString pqdbg_get_current_line() {
    QString line = QString::number(__pqdbg_current_d_line);

    while(line.length() < 5) {
        line.prepend("0");
    }

    return line;
}

void pqdbg_current_line_inc() {
    __pqdbg_current_d_line++;
}

void pqdbg_set_current_lvl(int lvl) {
    __pqdbg_current_d_lvl = lvl;
}
#endif

QByteArray getCorename() {
    return m_corename;
}
