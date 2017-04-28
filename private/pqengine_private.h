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

#ifndef PQENGINE_PRIVATE_H
#define PQENGINE_PRIVATE_H

#ifdef mkdir
#undef mkdir
#endif

#include "simplecrypt.h"
#include <QTextCodec>
#include <QTextStream>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <qglobal.h>

#include "pqengine_global.h"
#include "ipqengineext.h"

/* REMOVE MULTIPLE DEFINITION ERRORS :-[ */
#include <QRectF>
#include <QVector2D>

#ifdef PHP_WIN32
#include <time.h>
#include <ws2tcpip.h>
#include <io.h>
#endif

#include <math.h>
#include <inttypes.h>
#include <sys/stat.h>
/* end */

extern "C" {
#include <php.h>
#include <php_main.h>
#include <TSRM.h>
#include <SAPI.h>
}

struct PQEngineInitConf;

class PQDLAPI PQEnginePrivate : public QObject
{
    Q_OBJECT
public:
    PQEnginePrivate(PQExtensionList extensions = PQExtensionList(), QObject *parent = Q_NULLPTR);

    bool                    init(int argc, char **argv, const QString &coreName, const PQEngineInitConf &ic);

    bool                    sapi_init();
    void                    shutdown(PQDBG_LVL_D);

    int                     exec(const char *script PQDBG_LVL_DC);
    int                     execpq(PQDBG_LVL_D);
    void                    debugMessage();

    static void             pq_register_extensions();

    static QByteArray *     readMainFile(PQDBG_LVL_D);
    static QByteArray *     pqe_unpack(const QByteArray &pqeData, qlonglong key);

    static PQExtensionList  pqExtensions;
    static QString          pqCoreName;

#ifdef PQDEBUG
public slots:
    void                    debugConnected();
#endif

private:
    static void             pq_register_extension(IPQExtension *extension);
    static void             pq_register_plastiq_classes(const PlastiQMetaObjectList &classes);

    static int              php_pqengine_startup(sapi_module_struct *sapi_module);
    static int              php_pqengine_deactivate();
    static size_t           php_pqengine_ub_write(const char *str, size_t str_length);
    static void             php_pqengine_error(int type, const char *format, ...);
    static void             php_pqengine_flush(void *server_context);
    static void             php_pqengine_send_header(sapi_header_struct *sapi_header, void *server_context);
    static void             php_pqengine_register_variables(zval *track_vars_array);
    static void             php_pqengine_log_message(char *message, int syslog_type_int);

    static size_t           pqengine_stream_reader(void *dataStreamPtr, char *buffer, size_t wantlen);
    static void             pqengine_stream_closer(void *dataStreamPtr);
    static size_t           pqengine_stream_fsizer(void *dataStreamPtr);

    /* VARIABLES */
    sapi_module_struct php_pqengine_module;
    static qlonglong pqHashKey;
};

#endif // PQENGINE_PRIVATE_H

