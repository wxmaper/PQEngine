#ifndef PHPQT5_OBJECTFACTORY_H
#define PHPQT5_OBJECTFACTORY_H

#include <QDebug>
#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QCoreApplication>
#include <QDomDocument>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QHoverEvent>
#include <QFocusEvent>

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
#endif

#include <math.h>
#include <io.h>
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

typedef struct _pqof_class_entry {
    QString qt_class_name;
    QString pq_class_name;
    QMetaObject metaObject;
} pqof_class_entry;

typedef struct _pqof_object_entry {
    QObject *qo;
    zval *zo;
    quint32 zhandle;
} pqof_object_entry;

class PHPQt5ObjectFactory;
class PHPQt5ObjectFactory : public QObject
{
    Q_OBJECT

public:
    explicit PHPQt5ObjectFactory(QObject *parent = 0,
                                 MemoryManager mmng = MemoryManager::Hybrid);

    QObject                             * createObject(const QString &className, zval *zobj_ptr,
                                                       const QVariantList &args TSRMLS_DC);

    bool                                registerObject(zval *zobj_ptr, QObject *qo);
    QString                             registerMetaObject(const QMetaObject &qmo);
    void                                registerZendClassEntry(QString qtClassName, zend_class_entry *ce_ptr);
    bool                                call(QObject *qo, QMetaMethod metaMethod, QVariantList args, QVariant *retVal, QGenericReturnArgument *qgrv = 0);
    bool                                convertArgs(QMetaMethod metaMethod, QVariantList args, QVariantList *converted);

    int                                 getObjectHandleByObjectName(const QString &objectName);
    QMap<QString, pqof_class_entry>     getRegisteredMetaObjects();

    QObject                             * getObject(zval *zobj_ptr TSRMLS_DC);
    QObject                             * getObject(int zhandle);

    zval                                * getZObject(int zhandle);
    zval                                * getZObject(QObject* qo);

    zend_class_entry                    * getClassEntry(const QString &qtClassName);

public slots:
    void                                freeObject(QObject *qo);
    void                                removeObject(QObject *qo);
    void                                removeObjectByHandle(quint32 zhandle);

protected:
    MemoryManager                       m_mmng;

    QMap<QString, pqof_class_entry>     m_classes;
    QMap<QString, zend_class_entry*>    z_classes;

    QObjectList                         m_objects;
    QHash<quint32, pqof_object_entry>   m_handles;
};

#endif // PHPQT5_OBJECTFACTORY_H
