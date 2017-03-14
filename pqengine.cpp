/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PQEngine.
**
** BEGIN LICENSE: MPL 2.0
**
** This Source Code Form is subject to the terms of the Mozilla Public
** License, v. 2.0. If a copy of the MPL was not distributed with this
** file, You can obtain one at http://mozilla.org/MPL/2.0/.
**
** END LICENSE
**
****************************************************************************/

#include <QTextStream>
#include "pqtypes.h"
#include "pqengine.h"
#include "pqengine_private.h"

/* Static vars */
PQEnginePrivate *PQEngine::pqeEngine;
bool PQEngine::pqeInitialized = false;
PQExtensionList PQEngine::pqeExtensions;
QString PQEngine::pqeCoreName;

#include <QMutex>
#include <QtGlobal>

/* PQEngine class */
PQEngine::PQEngine(PQExtensionList extensions)
{
    pqeExtensions = extensions;
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
    pqeCoreName = coreName;
    pqeEngine = new PQEnginePrivate(pqeExtensions);

    pqeInitialized = pqeEngine->init(argc, argv, pmd5, coreName, checkName, hashKey, appName, appVersion, orgName, orgDomain);
    pmd5.fill(0);

    return pqeInitialized;
}

int PQEngine::exec(const char *script)
{
    if(pqeInitialized) {
#ifdef PQDEBUG
        return pqeEngine->exec(script, 0);
#else
        return pqeEngine->exec(script);
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
        QTextStream( stdout ) << QString(title).prepend("[").append("] ") << m.toUtf8().constData() << endl;
    }
    else {
        QTextStream( stdout ) << m.toUtf8().constData() << endl;
    }
}



#ifdef PQDEBUG
int __pqdbg_current_d_lvl = 0;
int __pqdbg_current_d_line = 0;

int pqdbg_get_current_lvl() {
    static QMutex mutex;

    mutex.lock();
    int retval = __pqdbg_current_d_lvl;
    mutex.unlock();

    return retval;
}

QString pqdbg_get_current_line() {
    QString line = QString::number(pqdbg_get_current_lvl());

    while(line.length() < 7) {
        line.prepend("0");
    }

    return line;
}

void pqdbg_current_line_inc() {
    static QMutex mutex;

    mutex.lock();
    __pqdbg_current_d_line++;
    mutex.unlock();
}

void pqdbg_set_current_lvl(int lvl) {
    static QMutex mutex;

    mutex.lock();
    __pqdbg_current_d_lvl = lvl;
    mutex.unlock();
}
#endif

QString getCorename() {
    return PQEngine::pqeCoreName;
}
