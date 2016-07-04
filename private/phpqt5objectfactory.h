/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PHPQt5.
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

#ifndef PHPQT5_OBJECTFACTORY_H
#define PHPQT5_OBJECTFACTORY_H

#include <QThread>
#include <QDebug>
#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QCoreApplication>
#include <QDomDocument>

#include <QTextCodec>
#include <QTextStream>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QHoverEvent>
#include <QFocusEvent>
#include <QPointer>

#include <QDir>
#include <QFile>
#include <qglobal.h>

#include "pqengine_global.h"
#include "simplecrypt.h"
#include "pqclasses.h"

/* REMOVE MULTIPLE DEFINITION ERRORS :-[ */
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
#include <php_variables.h>
#include <TSRM.h>
#include <zend.h>
#include <zend_API.h>
#include <zend_types.h>
#include <zend_closures.h>
#include <zend_interfaces.h>
}

#include <php_streams.h>

typedef struct _pqof_class_entry {
    QString qt_class_name;
    QString pq_class_name;
    QMetaObject metaObject;
} pqof_class_entry;

typedef struct _pqobjectpdata {
    QPointer<QObject> qo_sptr;
    zend_object *zo_ptr;
    void *ctx;
    void *fromctx;
} PQObjectPData;

typedef struct _pq_nullptr {
    QByteArray toTypeName = QByteArray();
} pq_nullptr;

Q_DECLARE_METATYPE(pq_nullptr)
Q_DECLARE_METATYPE(pq_nullptr*)
Q_DECLARE_METATYPE(QObjectList)
Q_DECLARE_METATYPE(QObjectList*)

//struct PlastiQMetaObject;
#include "plastiqmetaobject.h"

class PHPQt5ObjectFactory;
class PQDLAPI PHPQt5ObjectFactory : public QObject
{
    Q_OBJECT

public:
    explicit PHPQt5ObjectFactory(QObject *parent = 0);

    QObject                             * createObject(const QString &className,
                                                       zval *pzval,
                                                       const QVariantList &args
                                                       PQDBG_LVL_DC);

    bool                                registerObject(zval *pzval, QObject *qobject PQDBG_LVL_DC);
    void                                freeObject(zval *zobject PQDBG_LVL_DC);

    QString                             registerMetaObject(const QMetaObject &qmo PQDBG_LVL_DC);
    QByteArray                          registerPlastiQMetaObject(const PlastiQMetaObject &metaObject PQDBG_LVL_DC);

    void                                registerZendClassEntry(QString qtClassName, zend_class_entry *ce_ptr PQDBG_LVL_DC);

    bool                                call(QObject *qo, QMetaMethod metaMethod, QVariantList args, QVariant *retVal, QGenericReturnArgument *qgrv PQDBG_LVL_DC);
    bool                                convertArgs(QMetaMethod metaMethod, QVariantList args, QVariantList *converted, bool is_constructor PQDBG_LVL_DC);

    QList<zval>                         getZObjectsByName(const QString &objectName PQDBG_LVL_DC);
    QMap<QString, pqof_class_entry>     getRegisteredMetaObjects(PQDBG_LVL_D);

    QObject                             * getQObject(zval *zobject PQDBG_LVL_DC);
    zval                                getZObject(QObject* qobject PQDBG_LVL_DC);

    zend_class_entry                    * getClassEntry(const QString &qtClassName PQDBG_LVL_DC);


    /* PlastiQ */
    bool                                havePlastiQMetaObject(const QByteArray &className PQDBG_LVL_DC);
    PlastiQMetaObject                   getMetaObject(const QByteArray &className PQDBG_LVL_DC);
    bool                                createPlastiQObject(const QByteArray &className,
                                                            const QByteArray &signature,
                                                            zval *pzval,
                                                            const PMOGStack &stack
                                                            PQDBG_LVL_DC);

public slots:
    void                                freeObject_slot(_zend_object *zobject);

protected:
    QMap<QString, pqof_class_entry>     m_classes;
    QMap<QString, zend_class_entry*>    z_classes;
    QObjectList m_objects;

    /* PlastiQ */
    QHash<QByteArray, PlastiQMetaObject> m_plastiqClasses;
};

#endif // PHPQT5_OBJECTFACTORY_H
