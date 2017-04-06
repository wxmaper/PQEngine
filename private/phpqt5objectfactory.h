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

    void                                freeObject(zend_object *zobject);

    QByteArray                          registerPlastiQMetaObject(const PlastiQMetaObject &metaObject);

    void                                registerZendClassEntry(QString qtClassName, zend_class_entry *ce_ptr);


    zend_class_entry                    * getClassEntry(const QByteArray &className);

    /* PlastiQ */
    bool                                havePlastiQMetaObject(const QByteArray &className);
    bool                                havePlastiQObject(quint64 &objectId);
    PlastiQMetaObject                   getMetaObject(const QByteArray &className);
    bool                                createPlastiQObject(const QByteArray &className,
                                                            const QByteArray &signature,
                                                            zval *pzval,
                                                            bool isWrapper,
                                                            const PMOGStack &stack);
    void                                addObject(PQObjectWrapper *pqobject, quint64 objectId = 0);
    void                                removeObject(PQObjectWrapper *pqobject, quint64 objectId = 0);
    PQObjectWrapper *                   getObject(quint64 objectId);
    void                                extractSignals(PQObjectWrapper *pqobject, zval *zobject);
    void                                extractVirtualMethods(PQObjectWrapper *pqobject, zval *zobject);

//public slots:
//    void                                freeObject_slot(QObject *qobject);

protected:
    QMap<QString, pqof_class_entry>     m_classes;
    QMap<QString, zend_class_entry*>    z_classes;
    // QObjectList m_objects; // REMOVE IT

    /* PlastiQ */
    QHash<QByteArray, PlastiQMetaObject> m_plastiqClasses;
    QHash<quint64, PQObjectWrapper*> m_plastiqObjects;
};

#endif // PHPQT5_OBJECTFACTORY_H
