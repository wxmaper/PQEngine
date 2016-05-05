#include "phpqt5objectfactory.h"
#include "phpqt5.h"

#define FETCH_PQOMAP() PQObjectMap _pqom = m_ts_pqobjects.value(PQTHREAD, PQObjectMap());
#define PQOMAP _pqom
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
    {
        PQDBGLPUP(QString("%1:? - z:%3")
                  .arg(className)
                  .arg(Z_OBJ_HANDLE_P(pzval)))
    }
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

    #ifdef PQDEBUG
    #ifdef PQDETAILEDDEBUG
        PQDBGLPUP(QString("prepared signature: %1").arg(signature.constData()));
    #endif
    #endif

    if(constIndex > -1) {
        #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
        PQDBGLPUP("found faster method (#1)");
        #endif

        QMetaMethod metaMethod = metaObject.constructor(constIndex);
        PQDBGLPUP(QString("testing constructor #%1").arg(constIndex));

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
        #ifdef PQDEBUG
        #ifdef PQDETAILEDDEBUG
            PQDBGLPUP("found general method (#2)");
        #endif
        #endif

        for(int constructorIndex = 0;
            constructorIndex < metaObject.constructorCount();
            constructorIndex++)
        {
            QMetaMethod metaMethod = metaObject.constructor(constructorIndex);

            #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
            PQDBGLPUP(QString("testing constructor #%1").arg(constructorIndex));
            PQDBGLPUP(QString("%1").arg(metaMethod.methodSignature().constData()));
            #endif

            QVariantList convertedArgs;
            bool converted = convertArgs(metaMethod, args, &convertedArgs, true PQDBG_LVL_CC);

            if( converted ) {
                PREPARE_ARGS(convertedArgs);

                #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
                PQDBGLPUP("creating qobject");
                #endif

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
                    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
                    PQDBGLPUP("ok");
                    #endif

                    break;
                }
            }
        }
    }

    if(qo) {
        registerObject(pzval, qo PQDBG_LVL_CC);
    }
    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    else { PQDBGLPUP("qobject not created"); }
    #endif

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("done");
    #endif

    PQDBG_LVL_DONE();
    return qo;
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
    pqobject->isinit = true;
    pqobject->qo_sptr = qobject;

    connect(qobject, SIGNAL(destroyed(QObject*)),
            this, SLOT(s_freeObject(QObject*)));

    if(qobject->parent()) {
        #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
        PQDBGLPUP("addref to zobject");
        #endif

        Z_ADDREF_P(pzval);
    }

    m_objects.insert(qobject, Z_OBJ_P(pzval));

    {
#if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
        PQDBGLPUP("update properties");
#endif

        zend_update_property_long(Z_OBJCE_P(pzval), pzval, "uid", sizeof("uid")-1, reinterpret_cast<quint64>(qobject));
        zend_update_property_long(Z_OBJCE_P(pzval), pzval, "zhandle", sizeof("zhandle")-1, Z_OBJ_HANDLE_P(pzval));

        /*
        {
            int is_tmp;
            HashTable *objht = Z_OBJDEBUG_P(pzval, is_tmp);

            zval uid;
            ZVAL_LONG(&uid, reinterpret_cast<quint32>(qobject));
            zend_string *uid_str = zend_string_init("uid", sizeof("uid")-1, 0);

            zval zhandle;
            ZVAL_LONG(&zhandle, Z_OBJ_HANDLE_P(pzval));
            zend_string *zhandle_str = zend_string_init("zhandle", sizeof("zhandle")-1, 0);

            zend_hash_add(objht, uid_str, &uid);
            zend_hash_add(objht, zhandle_str, &zhandle);
        }
        */



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
                                               "%3::signals expects value %4 to be String")
                              .arg(Z_OBJCE_P(pzval)->name->val)
                              .arg(PHP_EOL)
                              .arg(Z_OBJCE_P(pzval)->name->val)
                              .arg(index)
                              .toUtf8().constData());
                }
            } ZEND_HASH_FOREACH_END();

            break;
        }

        case IS_NULL: break;

        default:
            php_error(E_ERROR, QString("Cannot prepare signals for `<b>%1</b>`%2"
                                       "%3::signals expects parameter to be Array")
                      .arg(Z_OBJCE_P(pzval)->name->val)
                      .arg(PHP_EOL)
                      .arg(Z_OBJCE_P(pzval)->name->val)
                      .toUtf8().constData());
        }
    }


    PQDBG_LVL_DONE();
    return true;
}

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
        PQDBGLPUP(QString("%1:%2 - z:%3")
                  .arg(c)
                  .arg(reinterpret_cast<quint64>(qobject))
                  .arg(z))
    }
#endif

    if(qobject) {
        QObjectList childObjs = qobject->children();

        #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
        PQDBGLPUP("remove childs...");
        #endif

        foreach(QObject *childObj, childObjs) {
            if(childObj != nullptr) {
                if(!m_classes.contains(childObj->metaObject()->className()))
                    continue;
                childObj->disconnect(childObj, SIGNAL(destroyed(QObject*)), this, SLOT(s_freeObject(QObject*)));
                removeObjectFromStorage(childObj PQDBG_LVL_CC);
                childObj->deleteLater();
            }
        }

        #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
        PQDBGLPUP("...all childs removed");
        PQDBGLPUP("disconnect signals");
        #endif

        qobject->disconnect(qobject, SIGNAL(destroyed(QObject*)),
                            this, SLOT(s_freeObject(QObject*)));

        removeObjectFromStorage(qobject PQDBG_LVL_CC);

        // double-free protection
        // parent class (QObject) can send destruction signal over subclass (PQObject)
        if(QString(qobject->metaObject()->className()).mid(0,1) == "P") {
            #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
            PQDBGLPUP(QString("delete %1").arg(qobject->metaObject()->className()));
            #endif

            delete qobject;
        }
    }

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("done");
    #endif
    PQDBG_LVL_DONE();
}

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

        if(Z_REFCOUNT(zobject) > 2)
            Z_SET_REFCOUNT(zobject, 2);

        ZVAL_UNDEF(&zobject);

        m_objects.remove(qobject);
    }

    PQDBG_LVL_DONE();
}

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

void PHPQt5ObjectFactory::s_freeObject(QObject *qobject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

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

    PQDBG_LVL_DONE();

    //freeObject(qobject PQDBG_LVL_NC);
}

void PHPQt5ObjectFactory::s_removeObjectFromStorage(QObject *qobject)
{
    removeObjectFromStorage(qobject PQDBG_LVL_NC);
}

void PHPQt5ObjectFactory::s_removeObjectByHandle(quint32 zhandle)
{
    //removeObjectByHandle(zhandle PQDBG_LVL_NC);
}

void PHPQt5ObjectFactory::s_updateObjectStorage()
{
    //updateObjectStorage(PQDBG_LVL_N);
}

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
                  .arg(metaMethod.name().constData()))
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
                QObjectList list = QObjectList();
                qDebug() << returnArgument.data();
                qDebug() << retVal.constData();
               // retVal = QVariant::fromValue<QObjectList>(list);
            }


            PQDBGLPUP("done");
        }
    }

    if (!ok) {
        #ifdef PQDEBUG
        qWarning() << "Calling" << metaMethod.methodSignature() << "failed!";
        #endif

        PQDBG_LVL_DONE();
        return false;
    }

    PQDBG_LVL_DONE();
    return true;
}

bool PHPQt5ObjectFactory::convertArgs(QMetaMethod metaMethod, QVariantList args, QVariantList *convertedArg, bool is_constructor PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    QList<QByteArray> methodTypes = metaMethod.parameterTypes();
    if (methodTypes.size() != args.size()) {

        PQDBG_LVL_DONE();
        return false;
    }

    for (int i = 0; i < methodTypes.size(); i++) {
        const QVariant& arg = args.at(i);

        QByteArray methodTypeName = methodTypes.at(i);
        QByteArray argTypeName = arg.typeName();

        QVariant::Type methodType = QVariant::nameToType(methodTypeName);

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
                    PQDBG_LVL_DONE();
                    return false;
                }
            }
        }
        else {
            copy = QVariant(arg);

            if (copy.type() != methodType) {
                if (copy.canConvert(methodType)) {
                    if (!copy.convert(methodType)) {
                        qWarning() << "Cannot convert" << argTypeName
                                   << "to" << methodTypeName;

                        PQDBG_LVL_DONE();
                        return false;
                    }
                }
                else {
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
                            PQDBG_LVL_DONE();
                            return false;
                        }
                    }
                    else if(methodTypeName.contains("*")
                            && argTypeName.contains("*")) {
                        //TODO: NOT SUPPORTED CONVERSION
                        //copy = QVariant::fromValue<QObject*>(arg);
                        //return false;
                    }
                    else {
                        PQDBG_LVL_DONE();
                        return false;
                    }
                }
            }
        }

        *convertedArg << copy;
    }

    PQDBG_LVL_DONE();
    return true;
}

QList<zval> PHPQt5ObjectFactory::getZObjectsByName(const QString &objectName PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

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

    return zobjects;
}

void PHPQt5ObjectFactory::registerZendClassEntry(QString qtClassName, zend_class_entry *ce_ptr PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(QString("qtClassName: %1").arg(qtClassName));
#endif

    z_classes.insert(qtClassName, ce_ptr);
}

zend_class_entry *PHPQt5ObjectFactory::getClassEntry(const QString &qtClassName PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    Q_UNUSED(PQDBG_LVL_C);
    //PQDBG_LVL_PROCEED(__FUNCTION__);
    //PQDBGLPUP(QString("qtClassName: %1").arg(qtClassName));
#endif

    //PQDBG_LVL_DONE();
    return z_classes.value(qtClassName, nullptr);
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

    PQDBG_LVL_DONE();
    return qt_class_name;
}

QMap<QString, pqof_class_entry> PHPQt5ObjectFactory::getRegisteredMetaObjects(PQDBG_LVL_D)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    PQDBG_LVL_DONE();
    return m_classes;
}

QObject *PHPQt5ObjectFactory::getQObject(zval *zobj_ptr PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    {
        PQDBGLPUP(QString("%1:? - z:%3:%4")
                  .arg(Z_OBJCE_NAME_P(zobj_ptr))
                  .arg(Z_OBJ_HANDLE_P(zobj_ptr))
                  .arg(reinterpret_cast<quint64>(zobj_ptr)))
    }
#endif

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(zobj_ptr));

#ifdef PQDEBUG
    if(pqobject->qo_sptr.isNull()) {
        PQDBGLPUP(QString("fetch: Q_NULLPTR:%2 z:%3:%4")
                  .arg(reinterpret_cast<quint64>(pqobject->qo_sptr.data()))
                  .arg(pqobject->zo.ce->name->val)
                  .arg(pqobject->zo.handle));
    }
    else {
        PQDBGLPUP(QString("fetch: %1:%2 z:%3:%4")
                  .arg(pqobject->qo_sptr.data()->metaObject()->className())
                  .arg(reinterpret_cast<quint64>(pqobject->qo_sptr.data()))
                  .arg(pqobject->zo.ce->name->val)
                  .arg(pqobject->zo.handle));
    }
#endif

    PQDBG_LVL_DONE();
    return pqobject->qo_sptr.data();
}

zval PHPQt5ObjectFactory::getZObject(QObject *qo PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    zval zv_object;

    if(m_objects.contains(qo)) {
        ZVAL_OBJ(&zv_object, m_objects.value(qo));
    }
    else {
        ZVAL_NULL(&zv_object);
    }

    return zv_object;
}
