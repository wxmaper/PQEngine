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

#include "phpqt5objectfactory.h"
#include "phpqt5.h"

/*
#define PREPARE_ARGS(convertedArgs) \
    QList<QGenericArgument> preparedArgs; \
    for (int i = 0; i < convertedArgs.size(); i++) { \
        if(convertedArgs[i].typeName() == QString("pq_nullptr")) {\
            QVariant &argument = convertedArgs[i]; \
            PQDBGLPUP(QString("qvariant_cast: %1").arg(argument.typeName()));\
            pq_nullptr ptr = qvariant_cast<pq_nullptr>(argument);\
    PQDBGLPUP(QString("toTypeName: %1").arg(ptr.toTypeName.constData()));\
            QByteArray aName = "pq_nullptr";\
            if(ptr.toTypeName.length()) {\
                PQDBGLPUP(QString("ptr.toTypeName: %1").arg(ptr.toTypeName.constData()));\
                aName = ptr.toTypeName;\
            }\
            /*preparedArgs << QGenericArgument(Q_NULLPTR);* \
            int *v = new int(0);\
            QGenericArgument genericArgument = QGenericArgument( \
                "int", \
                static_cast<void*>(v)); \
            preparedArgs << genericArgument; \
        }\
        else {\
            QVariant &argument = convertedArgs[i]; \
            QGenericArgument genericArgument = QGenericArgument( \
                QMetaType::typeName(argument.userType()), \
                const_cast<void*>(argument.constData()) \
                ); \
            preparedArgs << genericArgument; \
        }\
    }

#define PREPARE_ARGS(convertedArgs) \
    QList<QGenericArgument> preparedArgs; \
    for (int i = 0; i < convertedArgs.size(); i++) { \
        QVariant &argument = convertedArgs[i]; \
        QGenericArgument genericArgument = QGenericArgument( \
            QMetaType::typeName(argument.userType()), \
            const_cast<void*>(argument.constData()) \
            ); \
        preparedArgs << genericArgument; \
    }
*/

#define PREPARE_ARGS(convertedArgs) \
    QList<QGenericArgument> preparedArgs; \
    for (int i = 0; i < convertedArgs.size(); i++) { \
        if(convertedArgs[i].typeName() == QString("pq_nullptr")) {\
            preparedArgs << QGenericArgument(Q_NULLPTR);\
        }\
        else {\
            QVariant &argument = convertedArgs[i]; \
            QGenericArgument genericArgument = QGenericArgument( \
                QMetaType::typeName(argument.userType()), \
                const_cast<void*>(argument.constData()) \
                ); \
            preparedArgs << genericArgument; \
        }\
    }

PHPQt5ObjectFactory::PHPQt5ObjectFactory(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<pq_nullptr>("pq_nullptr");
    qRegisterMetaType<pq_nullptr*>("pq_nullptr*");
    qRegisterMetaType<QObjectList>("QObjectList");
    qRegisterMetaType<QObjectList*>("QObjectList*");
}

QObject *PHPQt5ObjectFactory::createObject(const QString &className,
                                           zval *pzval,
                                           const QVariantList &args
                                           PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("%1:? - z:%3")
              .arg(className)
              .arg(Z_OBJ_HANDLE_P(pzval)))
#endif

    QObject *qo = Q_NULLPTR;

    QMetaObject metaObject = m_classes.value(className).metaObject;

    QByteArray signature;
    signature.append(metaObject.className());
    signature.append("(");

    for(int i = 0; i < args.size(); i++)
    {
        const QVariant &arg = args[i];
        if(i > 0) signature.append(",");
        signature.append(arg.typeName());
    }

    signature.append(")");

    int constIndex = metaObject.indexOfConstructor(QMetaObject::normalizedSignature(signature));

    PQDBGLPUP(QString("prepared signature: %1").arg(signature.constData()));

    if(constIndex > -1) {
        PQDBGLPUP("found faster method (#1)");
        PQDBGLPUP(QString("testing constructor #%1").arg(constIndex));

        QMetaMethod metaMethod = metaObject.constructor(constIndex);

        QVariantList convertedArgs;
        bool converted = convertArgs(metaMethod, args, &convertedArgs, true PQDBG_LVL_CC);

        if( converted ) {
            PQDBGLPUP(QString("preparing... (%1)").arg(convertedArgs.size()));

            PREPARE_ARGS(convertedArgs);

            PQDBGLPUP(QString("create Instance of %1 class...").arg(metaObject.className()));

            qo = metaObject.newInstance(
                        preparedArgs.value(0),
                        preparedArgs.value(1),
                        preparedArgs.value(2),
                        preparedArgs.value(3),
                        preparedArgs.value(4),
                        preparedArgs.value(5),
                        preparedArgs.value(6),
                        preparedArgs.value(7),
                        preparedArgs.value(8),
                        preparedArgs.value(9));

            PQDBGLPUP("ok");
        }
    }

    else {
        PQDBGLPUP("found general method (#2)");

        for(int constructorIndex = 0;
            constructorIndex < metaObject.constructorCount();
            constructorIndex++)
        {
            QMetaMethod metaMethod = metaObject.constructor(constructorIndex);

            PQDBGLPUP(QString("testing constructor #%1").arg(constructorIndex));
            PQDBGLPUP(QString("%1").arg(metaMethod.methodSignature().constData()));

            QVariantList convertedArgs;
            bool converted = convertArgs(metaMethod, args, &convertedArgs, true PQDBG_LVL_CC);

            if( converted ) {
                PREPARE_ARGS(convertedArgs);
                PQDBGLPUP("creating qobject");

                qo = metaObject.newInstance(
                            preparedArgs.value(0),
                            preparedArgs.value(1),
                            preparedArgs.value(2),
                            preparedArgs.value(3),
                            preparedArgs.value(4),
                            preparedArgs.value(5),
                            preparedArgs.value(6),
                            preparedArgs.value(7),
                            preparedArgs.value(8),
                            preparedArgs.value(9));

                if(qo) {
                    PQDBGLPUP("ok");
                    break;
                }
            }
        }
    }

    if(qo) {
        registerObject(pzval, qo PQDBG_LVL_CC);
    }
    #if defined(PQDEBUG)
    else { PQDBGLPUP("qobject not created"); }
    #endif


    PQDBG_LVL_RETURN_VAL_LPUP(qo);
}

bool PHPQt5ObjectFactory::registerObject(zval* pzval, QObject *qobject PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(pzval))
              .arg(reinterpret_cast<quint64>(qobject))
              .arg(Z_OBJ_HANDLE_P(pzval)));
#endif

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(pzval));
    pqobject->qo_sptr = qobject;
    pqobject->isvalid = true;
    m_objects << qobject;

    QMetaObject::invokeMethod(qobject, "__pq_setThisPtr", Q_ARG(QObject*, qobject));
    QMetaObject::invokeMethod(qobject, "__pq_setZObject",
                              QGenericArgument("_zend_object*", static_cast<const void*>(&Z_OBJ_P(pzval))));

    connect(qobject, SIGNAL(destroyed(_zend_object*)),
            this, SLOT(freeObject_slot(_zend_object*)));

    if(qobject->parent()) {
        PQDBGLPUP("addref to zobject");
        Z_ADDREF_P(pzval);
    }

    PQDBGLPUP("set standard properties");
    {   // transfer into a function?
        zend_class_entry *ce = Z_OBJCE_P(pzval);

        quint64 uid = reinterpret_cast<quint64>(qobject);
        quint32 zhandle = Z_OBJ_HANDLE_P(pzval);

        do {
            zend_update_property_long(ce, pzval, "__pq_uid", sizeof("__pq_uid")-1, uid);
            zend_update_property_long(ce, pzval, "__pq_zhandle", sizeof("__pq_zhandle")-1, zhandle);
        } while (ce = ce->parent);

        qobject->setProperty(PQZHANDLE, zhandle);
    }

    {   // transfer into a function?
        zval *zsignals, rv;
        zsignals = zend_read_property(Z_OBJCE_P(pzval), pzval, "signals", 7, 1, &rv);

        switch(Z_TYPE_P(zsignals)) {
        case IS_ARRAY: {
            zval *zsignal;
            zend_ulong index;

            ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(zsignals), index, zsignal) {
                if(Z_TYPE_P(zsignal) == IS_STRING) {
                    PHPQt5::pq_declareSignal(qobject, QByteArray(Z_STRVAL_P(zsignal)));
                }
                else {
                    zend_wrong_paramer_type_error(index, zend_expected_type(IS_STRING), zsignal);
                    php_error(E_ERROR, QString("Error loading signals for `<b>%1</b>`%2"
                                               "%3::signals expects value %4 to be string")
                              .arg(Z_OBJCE_P(pzval)->name->val)
                              .arg(PHP_EOL)
                              .arg(Z_OBJCE_P(pzval)->name->val)
                              .arg(index)
                              .toUtf8().constData());
                }
            } ZEND_HASH_FOREACH_END();

            break;
        }

        case IS_NULL:
        case IS_UNDEF:
            break;

        default:
            php_error(E_ERROR, QString("Cannot prepare signals for `<b>%1</b>`%2"
                                       "%3::signals expects parameter to be array")
                      .arg(Z_OBJCE_P(pzval)->name->val)
                      .arg(PHP_EOL)
                      .arg(Z_OBJCE_P(pzval)->name->val)
                      .toUtf8().constData());
        }
    }

    pqobject->isinit = true;

    PQDBG_LVL_RETURN_VAL(true);
}

/*
void PHPQt5ObjectFactory::freeObject(QObject *qobject PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    {
        int z = -1;
        QString c = "nullptr";
        if(qobject) {
            z = qobject->property(PQZHANDLE).toInt();
            c = QString(qobject->metaObject()->className()).mid(1);
        }
        PQDBGLPUP(QString("%1:%2(%3) - z:%4")
                  .arg(c)
                  .arg(reinterpret_cast<quint64>(qobject))
                  .arg(qobject->objectName())
                  .arg(z))
    }
#endif

    if(qobject) {

        QObjectList childObjs = qobject->children();

        PQDBGLPUP("remove childs...");
        foreach(QObject *childObj, childObjs) {
            if(childObj != nullptr && childObj->parent() == qobject) {
                if(!m_classes.contains(childObj->metaObject()->className()))
                    continue;
                childObj->disconnect(childObj, SIGNAL(destroyed(_zend_object*)), this, SLOT(s_freeObject(_zend_object*)));
                childObj->deleteLater();
                removeObjectFromStorage(childObj PQDBG_LVL_CC);
            }
        }

        PQDBGLPUP("...all childs removed");

        PQDBGLPUP("disconnect signals");
        qobject->disconnect(qobject, SIGNAL(destroyed(_zend_object*)),
                            this, SLOT(s_freeObject(_zend_object*)));

        removeObjectFromStorage(qobject PQDBG_LVL_CC);

        // double-free protection
        // parent class (QObject) can send destruction signal over subclass (PQObject)
        if(QString(qobject->metaObject()->className()).mid(0,1) == "P") {
            PQDBGLPUP(QString("delete %1").arg(qobject->metaObject()->className()));
            delete qobject;
        }

    }

    PQDBG_LVL_DONE_LPUP();
}
*/

void PHPQt5ObjectFactory::freeObject(zval *zobject PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(zobject));

    if(!pqobject->qo_sptr.isNull()) {
        QObject *qobject = pqobject->qo_sptr.data();

        qobject->disconnect(qobject, SIGNAL(destroyed(_zend_object*)),
                            this, SLOT(freeObject_slot(_zend_object*)));

        if(qobject->parent() && pqobject->isvalid) {
            PQDBGLPUP("DELREF");
            Z_DELREF_P(zobject);
        }


        m_objects.removeOne(qobject);
        delete qobject;
    }

    pqobject->qo_sptr.clear();
    pqobject->isvalid = false;

    // ZVAL_UNDEF(zobject); // no need: memory leak
    PQDBG_LVL_DONE_LPUP();
}

void PHPQt5ObjectFactory::freeObject_slot(_zend_object *zobject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    PQObjectWrapper *pqobject = fetch_pqobject(zobject);
    QObject *qobject = pqobject->qo_sptr.data();
    pqobject->qo_sptr.clear();

    zval zv;
    ZVAL_OBJ(&zv, zobject);

    if(qobject->parent() && pqobject->isvalid) {
        PQDBGLPUP("DELREF");
        Z_DELREF(zv);
    }

    zend_update_property_long(Z_OBJCE(zv), &zv, "__pq_uid", sizeof("__pq_uid")-1, 0);

    /*
    PQDBGLPUP(QString("====== %1 %2")
              .arg(Z_OBJ_HANDLE_P(pqobject->zv))
              .arg(reinterpret_cast<quint64>(Z_OBJ_P(pqobject->zv))));

    PQDBGLPUP(QString("====== %1 %2")
              .arg(Z_OBJ_HANDLE_P(&zv))
              .arg(reinterpret_cast<quint64>(Z_OBJ_P(&zv))));
    */

    m_objects.removeOne(qobject);
    pqobject->isvalid = false;

    PQDBG_LVL_DONE();
}

/*
void PHPQt5ObjectFactory::removeObjectFromStorage(QObject *qobject PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    {
        int z = -1;
        if(qobject) z = qobject->property(PQZHANDLE).toInt();
        PQDBGLPUP(QString("%1:%2 - z:%3")
                  .arg(QString(qobject->metaObject()->className()).mid(1))
                  .arg(reinterpret_cast<quint64>(qobject))
                  .arg(z))
    }
#endif


    if(m_objects.contains(qobject)) {
        zval zobject;
        ZVAL_OBJ(&zobject, m_objects.value(qobject));

        if(Z_REFCOUNT(zobject) > 2) {
            Z_SET_REFCOUNT(zobject, 2);
        }

        ZVAL_UNDEF(&zobject);

        m_objects.remove(qobject);
    }

    PQDBG_LVL_DONE();
}
    */

/*
void PHPQt5ObjectFactory::moveObjectToThread(quint32 zhandle,
                                             QThread *old_th,
                                             QThread *new_th
                                             PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("zhandle: %1").arg(zhandle));
    PQDBGLPUP(QString("from: %1 to: %2")
              .arg(reinterpret_cast<quint64>(old_th))
              .arg(reinterpret_cast<quint64>(new_th)));
#endif

    PQDBGLPUP("FAILURE");
}
*/

/*
void PHPQt5ObjectFactory::s_freeObject(QObject *qobject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    {
        int z = -1;
        if(qobject) z = qobject->property(PQZHANDLE).toInt();
        PQDBGLPUP(QString("%1:%2(%3) - z:%4")
                  .arg(QString(qobject->metaObject()->className()).mid(1))
                  .arg(reinterpret_cast<quint64>(qobject))
                  .arg(qobject->objectName())
                  .arg(z))
    }
#endif

    QVariant pqzhandle = qobject->property(PQZHANDLE);
    if(!pqzhandle.isValid()) {
        PQDBGLPUP("invalid QObject");
        PQDBG_LVL_DONE();
        return;
    }

    /*
    QMapIterator<QObject*,zend_object*> i(m_objects);
    while(i.hasNext()) {
        i.next();

        if(i.key()
                && i.value()
                && i.key() == qobject) {
            zval zobject;
            ZVAL_OBJ(&zobject, i.value());
            zval_dtor(&zobject);
            break;
        }
    }
    *

    QObject *qo = qobject;
    QString methods;

    if(qo != nullptr) {
        const QMetaObject* metaObject = qo->metaObject();

        for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i) {
            QMetaMethod metaMethod = metaObject->method(i);

            if(metaMethod.methodSignature().left(7) == PQ_STATIC_PREFIX) // skip *_pslot methods
                continue;

            methods.append(QString("\n >>> %1").arg(metaMethod.methodSignature().constData()));
        }
    }

    PQDBGLPUP(methods);

    PQDBGLPUP("fetch");
    zval zobject = fetch_zobject(qobject);

    //if(!zobject) return;
   // if(Z_TYPE(zobject) == IS_OBJECT) {
   //     zval_dtor(&zobject);
   // }



    PQDBGLPUP("Z_DELREF_P");
    PQDBGLPUP(Z_TYPE(zobject));

    Z_DELREF(zobject);
   // if (Z_REFCOUNT_P(zobject) == 0) {
    PQDBGLPUP("zval_ptr_dtor");
        zval_ptr_dtor(&zobject);
        PQDBGLPUP("ZVAL_UNDEF");
        ZVAL_UNDEF(&zobject);
       // efree(zobject);
   // }

    PQDBG_LVL_DONE();

    //freeObject(qobject PQDBG_LVL_NC);
}
*/


bool PHPQt5ObjectFactory::call(QObject* qo,
                               QMetaMethod metaMethod,
                               QVariantList args,
                               QVariant *returnValue,
                               QGenericReturnArgument *qgrv
                               PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    {
        int z = -1;
        QString c = "nullptr";
        if(qo) {
            z = qo->property(PQZHANDLE).toInt();
            c = qo->metaObject()->className();
        }

        PQDBGLPUP(QString("%1:%2 - z:%3 -> %4")
                  .arg(c)
                  .arg(reinterpret_cast<quint64>(qo))
                  .arg(z)
                  .arg(metaMethod.name().constData()));
    }
#endif

    bool ok = false;
    QList<QGenericArgument> preparedArgs;

    QVariantList convertedArgs;
    bool converted = convertArgs(metaMethod, args, &convertedArgs, false PQDBG_LVL_CC);

    if( converted ) {
        PREPARE_ARGS(convertedArgs);

        QVariant &retVal = *returnValue;

        bool voidReturn = metaMethod.returnType() == QMetaType::Void;

        if(!voidReturn) {
            retVal = QVariant(QMetaType::type(metaMethod.typeName()),
                              static_cast<void*>(nullptr));
        }
        else {
            //retVal = NULL;
        }

        if(qgrv != nullptr) {
            PQDBGLPUP("invoke method");
            ok = metaMethod.invoke(
                        qo,
                        Qt::DirectConnection,
                        *qgrv,
                        preparedArgs.value(0),
                        preparedArgs.value(1),
                        preparedArgs.value(2),
                        preparedArgs.value(3),
                        preparedArgs.value(4),
                        preparedArgs.value(5),
                        preparedArgs.value(6),
                        preparedArgs.value(7),
                        preparedArgs.value(8),
                        preparedArgs.value(9)
                        );
        }
        else {
            QGenericReturnArgument returnArgument;

            if(voidReturn) {
                returnArgument = QGenericReturnArgument(metaMethod.typeName());
            }
            else {
                returnArgument = QGenericReturnArgument(metaMethod.typeName(),
                                                        const_cast<void*>(retVal.constData())
                                                        );
            }

            PQDBGLPUP("invoke method");
            PQDBGLPUP(QString("expected return type: %1").arg(metaMethod.typeName()));

            ok = metaMethod.invoke(
                        qo,
                        Qt::DirectConnection,
                        returnArgument,
                        preparedArgs.value(0),
                        preparedArgs.value(1),
                        preparedArgs.value(2),
                        preparedArgs.value(3),
                        preparedArgs.value(4),
                        preparedArgs.value(5),
                        preparedArgs.value(6),
                        preparedArgs.value(7),
                        preparedArgs.value(8),
                        preparedArgs.value(9)
                        );

            if(metaMethod.typeName() == QString("QObjectList*")) {
                // QObjectList list = QObjectList();
                qDebug() << returnArgument.data();
                qDebug() << retVal.constData();
                // retVal = QVariant::fromValue<QObjectList>(list);
            }

            PQDBGLPUP("ok");
        }
    }

    if (!ok) {
        PQDBGLPUP(QString("Calling %1 failed!").arg(metaMethod.methodSignature().constData()));
        PQDBG_LVL_RETURN_VAL(false);
    }

    PQDBG_LVL_RETURN_VAL(true);
}

bool PHPQt5ObjectFactory::convertArgs(QMetaMethod metaMethod, QVariantList args, QVariantList *convertedArg, bool is_constructor PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    QList<QByteArray> methodTypes = metaMethod.parameterTypes();
    if (methodTypes.size() != args.size()) {
        PQDBG_LVL_RETURN_VAL(false);
    }

    for (int i = 0; i < methodTypes.size(); i++) {
        const QVariant& arg = args.at(i);

        QByteArray methodTypeName = methodTypes.at(i);
        QByteArray argTypeName = arg.typeName();

        // QVariant::Type methodType = QVariant::nameToType(methodTypeName);
        int methodTypeId = QMetaType::type(methodTypeName);

        QVariant copy;

        if(argTypeName == "pq_nullptr") {
            if(is_constructor) copy = arg;
            else {
                if(methodTypeName == "QString") {
                    copy = QString();
                }
                else if(methodTypeName == "QByteArray") {
                    copy = QByteArray();
                }
                else if(methodTypeName == "int") {
                    copy = 0;
                }
                else if(methodTypeName == "double") {
                    copy = 0.0f;
                }
                else if(methodTypeName == "float") {
                    copy = 0.0f;
                }
                else if(methodTypeName == "long") {
                    copy = 0;
                }
                else if(QString(methodTypeName).contains('*')) {
                    copy = QVariant::fromValue<int>(0);
                }
                else {
                    PQDBG_LVL_RETURN_VAL(false);
                }
            }
        }
        else {
            copy = QVariant(arg);

            if (copy.type() != methodTypeId) {
                if (copy.canConvert(methodTypeId)) {
                    if (!copy.convert(methodTypeId)) {
                        //qMetaTypeId<QWidget*>() or QVariant::canConvert<QWidget*>() instead.
                        //qWarning() << "Error converting" << argTypeName
                        //           << "to" << methodTypeName;

                        PQDBGLPUP(QString("Error converting %1 to %2").arg(copy.type()).arg(methodTypeId));
                        PQDBG_LVL_RETURN_VAL(false);
                    }
                }
                else {
                    PQDBGLPUP(QString("Can't convert %1(%2) to %3(%4)")
                              .arg(argTypeName.constData()).arg(copy.type())
                              .arg(methodTypeName.constData()).arg(methodTypeId));

                    if(methodTypeName == "QVariant") {
                        copy = arg;
                    }
                    else if(methodTypeName.contains("*")
                            && argTypeName == "int") {
                        int val = arg.toInt();
                        if(val == 0) {
                            pq_nullptr ptr;
                            copy = QVariant::fromValue<pq_nullptr>(ptr);
                        }
                        else {
                            PQDBG_LVL_RETURN_VAL(false);
                        }
                    }
                    else if(methodTypeName.contains("*")
                            && argTypeName.contains("*")) {


                        // copy = QVariant::fromValue(arg.value());
                        PQDBGLPUP(QString("TODO: NOT SUPPORTED CONVERSION at line (%1)").arg(__LINE__));
                        PQDBGLPUP("methodTypeName.contains(\"*\") && argTypeName.contains(\"*\")");

                        //TODO: NOT SUPPORTED CONVERSION
                        //copy = QVariant::fromValue<QObject*>(arg);
                        //return false;
                    }
                    else {
                        PQDBG_LVL_RETURN_VAL(false);
                    }
                }
            }
        }

        *convertedArg << copy;
    }

    PQDBG_LVL_RETURN_VAL(true);
}

QList<zval> PHPQt5ObjectFactory::getZObjectsByName(const QString &objectName PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    /*
    QList<zval> zobjects;

    QMapIterator<QObject*,zend_object*> i(m_objects);

    while(i.hasNext()) {
        i.next();

        if(i.key() && i.key()->objectName() == objectName) {
            zval zobject;
            ZVAL_OBJ(&zobject, i.value());

            zobjects.append(zobject);
        }
    }
    */

    QList<zval> zobjects;
    foreach(QObject *qo, m_objects) {
        if(qo->objectName() == objectName) {
            zval zobject = fetch_zobject(qo);

            if(Z_TYPE(zobject) == IS_OBJECT) {
                zobjects << zobject;
            }
        }
    }

    PQDBG_LVL_RETURN_VAL(zobjects);
}

void PHPQt5ObjectFactory::registerZendClassEntry(QString qtClassName, zend_class_entry *ce_ptr PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("qtClassName: %1").arg(qtClassName));
#endif

    z_classes.insert(qtClassName, ce_ptr);
    PQDBG_LVL_DONE();
}

zend_class_entry *PHPQt5ObjectFactory::getClassEntry(const QString &qtClassName PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    // PQDBG_LVL_PROCEED(__FUNCTION__);
    // PQDBGLPUP(QString("className: %1").arg(qtClassName));
#endif

    zend_class_entry *ce = z_classes.value(qtClassName, Q_NULLPTR);
    PQDBG_LVL_RETURN_VAL(ce);
}


QString PHPQt5ObjectFactory::registerMetaObject(const QMetaObject &qmo PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("className: %1").arg(qmo.className()));
#endif

    QString pq_class_name = qmo.className();
    QString qt_class_name = pq_class_name.mid(1);

    m_classes.insert(qt_class_name,
                     pqof_class_entry { qt_class_name, pq_class_name, qmo });

    PQDBG_LVL_RETURN_VAL(qt_class_name);
}

QByteArray PHPQt5ObjectFactory::registerPlastiQMetaObject(const PlastiQMetaObject &metaObject PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("PlastiQ className: %1").arg(metaObject.className()));
#endif

    m_plastiqClasses.insert(metaObject.className(), metaObject);

    PQDBG_LVL_RETURN_VAL(metaObject.className());
}

QMap<QString, pqof_class_entry> PHPQt5ObjectFactory::getRegisteredMetaObjects(PQDBG_LVL_D)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    PQDBG_LVL_RETURN_VAL(m_classes);
}

QObject *PHPQt5ObjectFactory::getQObject(zval *zobject PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2:%3 - %4:%5(%6)")
              .arg(Z_OBJCE_NAME_P(zobject))
              .arg(Z_OBJ_HANDLE_P(zobject))
              .arg(reinterpret_cast<quint64>(Z_OBJ_P(zobject)))
              .arg("?")
              .arg("?")
              .arg("?"));
#endif

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(zobject));
    QObject *qobject = pqobject->qo_sptr.data();

    PQDBG_LVL_RETURN_VAL(qobject);
}

zval PHPQt5ObjectFactory::getZObject(QObject *qobject PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2:%3 - %4:%5(%6)")
              .arg("?")
              .arg("?")
              .arg("?")
              .arg(qobject->metaObject()->className())
              .arg(reinterpret_cast<quint64>(qobject))
              .arg(qobject->objectName()));
#endif

    /*
    zval zv_object;

    if(m_objects.contains(qo)) {
        ZVAL_OBJ(&zv_object, m_objects.value(qo));
    }
    else {
        ZVAL_NULL(&zv_object);
    }
    */

    zval zobject = fetch_zobject(qobject);

    if(Z_TYPE(zobject) != IS_OBJECT) {
        ZVAL_NULL(&zobject);
    }

    PQDBG_LVL_RETURN_VAL(zobject);
}
