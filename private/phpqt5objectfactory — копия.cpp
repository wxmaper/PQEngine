#include "phpqt5objectfactory.h"
#include "phpqt5.h"

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

#define PQZHANDLE "__pq_zhandle_"

PHPQt5ObjectFactory::PHPQt5ObjectFactory(QObject *parent) :
    QObject(parent)
{}

QObject *PHPQt5ObjectFactory::createObject(const QString &className, zval *zobj_ptr, const QVariantList &args TSRMLS_DC)
{
    QObject *qo = 0;

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

    if(constIndex > -1) {
        QMetaMethod metaMethod = metaObject.constructor(constIndex);

        QVariantList convertedArgs;
        bool converted = convertArgs(metaMethod, args, &convertedArgs);

        if( converted ) {
            PREPARE_ARGS(convertedArgs);
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
        }
    }
    else {
        for(int constructorIndex = 0;
            constructorIndex < metaObject.constructorCount();
            constructorIndex++)
        {
            QMetaMethod metaMethod = metaObject.constructor(constructorIndex);

            QVariantList convertedArgs;
            bool converted = convertArgs(metaMethod, args, &convertedArgs);

            if( converted ) {
                PREPARE_ARGS(convertedArgs);

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

                break;
            }
        }
    }

    registerObject(zobj_ptr, qo);

    return qo;
}

bool PHPQt5ObjectFactory::registerObject(zval* zobj_ptr, QObject *qo)
{
    if(qo) {
        quint32 zhandle = Z_OBJVAL_P(zobj_ptr).handle;
        qo->connect(qo, SIGNAL(destroyed(QObject*)), this, SLOT(freeObject(QObject*)));
        //qo->connect(qo, SIGNAL(objectNameChanged(QString))
        qo->setProperty(PQZHANDLE, zhandle);


        m_handles.insert(zhandle, pqof_object_entry { qo, zobj_ptr, zhandle } );
        m_objects.append(qo);

        if(qo->parent()) {
            Z_ADDREF_P(zobj_ptr);
        }

        //Z_ADDREF_P(zobj_ptr);
        //m_objects.insert(qo, zobj_ptr);

        return true;
    }

    return false;
}

void PHPQt5ObjectFactory::freeObject(QObject *qo) {
#ifdef PQDEBUG
    PQDBG2("PHPQt5ObjectFactory::freeObject() ",
           QString("%1:%2")
           .arg(QString(qo->metaObject()->className()).mid(1))
           .arg(reinterpret_cast<qint32>(qo) ));
#endif

    if(qo) {
        QObjectList childObjs = qo->children();

        foreach(QObject *childObj, childObjs) {
            childObj->disconnect(childObj, SIGNAL(destroyed(QObject*)), this, SLOT(freeObject(QObject*)));
            removeObject(childObj);
            childObj->deleteLater();
        }

        qo->disconnect(qo, SIGNAL(destroyed(QObject*)),
                       this, SLOT(freeObject(QObject*)));
        removeObject(qo);

        delete qo;
    }
#ifdef PQDEBUG
    else {
        PQDBG("NOT DELETED!");
    }
#endif
}

void PHPQt5ObjectFactory::removeObject(QObject *qo)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5ObjectFactory::removeObject() ",
           QString("%1:%2")
           .arg(QString(qo->metaObject()->className()).mid(1))
           .arg(reinterpret_cast<qint32>(qo) ));
    PQDBG2("before m_objects.size() = ", QString::number(m_objects.size()));
#endif

    if(qo) {
        QVariant val = qo->property(PQZHANDLE);

        if(val.isValid()) {
            m_handles.remove( val.toUInt() );
            m_objects.removeOne( qo );
        }
    }


    /*
    if(!qo) return;

    PHPQt5::pq_remove_connections(qo);

    QMapIterator<QObject*, zval*> iter(m_objects);

    while(iter.hasNext()) {
        iter.next();

        QObject *iqo = iter.key();
        zval *izo = iter.value();

        if(!iqo) {
            if(izo != nullptr && izo != NULL) {
               // if (Z_REFCOUNT_P(izo) <= 2)
                //Z_UNSET_ISREF_P(izo);
               // ZVAL_NULL(izo);
                //Z_DELREF_P(izo);
                //zval_ptr_dtor(&izo);
            }

            m_objects.remove(iqo);
        }

        if(iqo == qo) {
            if(izo != nullptr && izo != NULL) {
               // TSRMLS_FETCH();

               // int is_tmp;
               // HashTable *objht = Z_OBJDEBUG_P(izo, is_tmp);

               // zend_hash_del(objht, "uid", sizeof("uid"));
                //zend_hash_del(objht, "zhandle", sizeof("zhandle"));
                //Z_DELREF_P(izo);
                //if (Z_REFCOUNT_P(izo) <= 2)
               // Z_UNSET_ISREF_P(izo);
                //zval_ptr_dtor(&izo);
            }

            m_objects.remove(iqo);

            break;
        }
    }

#ifdef PQDEBUG
    PQDBG2("after m_objects.size() = ", QString::number(m_objects.size()));

    QMapIterator<QObject*, zval*> iter2(m_objects);

    while(iter2.hasNext()) {
        iter2.next();
        PQDBG("ITER() ... ");
        PQDBG( QString("qo->%1 : zo->%2")
                .arg(reinterpret_cast<qint32>(iter2.key()))
                .arg(Z_OBJVAL_P(iter2.value()).handle)
              );
    }
#endif
*/
}

QString PHPQt5ObjectFactory::registerMetaObject(const QMetaObject &qmo)
{
    QString pq_class_name = qmo.className();
    QString qt_class_name = pq_class_name.mid(1);
    pqof_class_entry ce(qt_class_name, pq_class_name, qmo);
    m_classes.insert(qt_class_name, ce);

    return qt_class_name;
}

QMap<QString, pqof_class_entry> PHPQt5ObjectFactory::getRegisteredMetaObjects()
{
    return m_classes;
}

QObject *PHPQt5ObjectFactory::getObject(zval *zobj_ptr TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5ObjectFactory::getObject() z_ptr: ", QString::number(Z_OBJVAL_P(zobj_ptr).handle));
#endif

    if(zobj_ptr != NULL
            && Z_TYPE_P(zobj_ptr) == IS_OBJECT) {
        int zhandle = Z_OBJVAL_P(zobj_ptr).handle;
        return getObject(zhandle);
    }

    return 0;
}

QObject *PHPQt5ObjectFactory::getObject(int zhandle)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5ObjectFactory::getObject() z: ",
           QString("%1").arg(zhandle));
#endif

    if(m_handles.contains(zhandle)) {
        pqof_object_entry oe = m_handles.value(zhandle);

        if(oe.zhandle == zhandle) {
            return oe.qo;
        }
    }

    return 0;

    /*
    QMapIterator<QObject*, zval*> iter(m_objects);
    while(iter.hasNext()) {
        iter.next();

        QObject *iqo = iter.key();
        zval *izo = iter.value();

        if(izo != nullptr && izo != NULL) {
            PQDBG(QString("%1:%2  ==  %3")
                  .arg("Z_OBJVAL_P(izo).handle")
                  .arg(Z_OBJVAL_P(iter.value()).handle)
                  .arg(zhandle));

            PQDBG(iqo->metaObject()->className());

            if(Z_OBJVAL_P(izo).handle == zhandle) {
                if(iqo) {
                    return iqo;
                }
                else {
                    //Z_DELREF_P(izo);
                    //ZVAL_NULL(izo);

                    m_objects.remove(iqo);
                }
            }
        }
        else {
            //Z_DELREF_P(izo);
            m_objects.remove(iqo);

            if(iqo) {
                removeObject(iqo);
            }
        }
    }
    */


    return 0;
}

zval *PHPQt5ObjectFactory::getZObject(int zhandle)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5ObjectFactory::getZObject() z: ",
           QString("%1").arg(zhandle));
#endif

    /*
    QMapIterator<QObject*, zval*> iter(m_objects);
    while(iter.hasNext()) {
        iter.next();

        QObject *iqo = iter.key();
        zval *izo = iter.value();

        if(izo != nullptr && izo != NULL) {
            if(Z_OBJVAL_P(izo).handle == zhandle) {
                if(iqo != nullptr || iqo != NULL) {
                    return izo;
                }
                else {
                    //if (Z_REFCOUNT_P(izo) <= 2) Z_UNSET_ISREF_P(izo);
                    //zval_ptr_dtor(&izo);

                    m_objects.remove(iqo);
                }
            }
        }
        else {
            //if (Z_REFCOUNT_P(izo) <= 2) Z_UNSET_ISREF_P(izo);
            //zval_ptr_dtor(&izo);

            m_objects.remove(iqo);

            if(iqo != nullptr || iqo != NULL) {
                iqo->deleteLater();
                delete iqo;
            }
        }
    }

    */

    if(m_handles.contains(zhandle)) {
        pqof_object_entry oe = m_handles.value(zhandle);


        if(oe.zo != NULL
                && Z_TYPE_P(oe.zo) == IS_OBJECT
                && oe.zhandle == zhandle) {
            return oe.zo;
        }
    }

    return NULL;
}

zval *PHPQt5ObjectFactory::getZObject(const QObject *qo)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5ObjectFactory::getZObject() qo: ",
           QString("%1:%2")
           .arg(QString(qo->metaObject()->className()).mid(1))
           .arg(reinterpret_cast<qint32>(qo) ));
#endif

    /*
    QMapIterator<QObject*, zval*> iter(m_objects);
    while(iter.hasNext()) {
        iter.next();
        if(iter.key() == qo) {
            return iter.value();
        }

        QObject *iqo = iter.key();
        zval *izo = iter.value();

        if(iqo == nullptr || iqo == NULL) {
            if(izo != nullptr && izo != NULL) {
                //if (Z_REFCOUNT_P(izo) <= 2) Z_UNSET_ISREF_P(izo);
                //zval_ptr_dtor(&izo);
            }

            m_objects.remove(iqo);
        }

        if(iqo == qo) {
            if(izo == nullptr && izo == NULL) {
                //if (Z_REFCOUNT_P(izo) <= 2) Z_UNSET_ISREF_P(izo);
                //zval_ptr_dtor(&izo);

                m_objects.remove(iqo);

                iqo->deleteLater();
                delete iqo;

                return NULL;
            }

            return izo;

            break;
        }
    }
    */

    return NULL;
}

int PHPQt5ObjectFactory::getObjectHandleByObjectName(const QString &objectName)
{
    /*
    QMapIterator<QObject*, zval*> iter(m_objects);
    while(iter.hasNext()) {
        iter.next();
        if(iter.key()->objectName() == objectName) {
            return Z_OBJVAL_P(iter.value()).handle;
        }
    }
    */

    foreach(QObject *qo, m_objects) {
        if(qo->objectName() == objectName) {
            QVariant val = qo->property(PQZHANDLE);

            if(val.isValid()) {
                return qo->property(PQZHANDLE).toInt();
            }
        }
    }

    return -1;
}

zend_class_entry *PHPQt5ObjectFactory::getClassEntry(const QString &qtClassName)
{
    return z_classes.value(qtClassName, NULL);
}

bool PHPQt5ObjectFactory::call(QObject* qo, QMetaMethod metaMethod, QVariantList args, QVariant *returnValue, QGenericReturnArgument *qgrv)
{
    bool ok = false;
    QList<QGenericArgument> preparedArgs;

    QVariantList convertedArgs;
    bool converted = convertArgs(metaMethod, args, &convertedArgs);

    if( converted ) {
        PREPARE_ARGS(convertedArgs);

        QVariant &retVal = *returnValue;

        if(metaMethod.returnType() != QMetaType::Void) {
            retVal = QVariant(QMetaType::type(metaMethod.typeName()),
                              static_cast<void*>(NULL));
        }
        else {
            retVal = NULL;
        }

        if(qgrv != 0) {
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
            QGenericReturnArgument returnArgument(metaMethod.typeName(),
                                                  const_cast<void*>(retVal.constData())
                                                  );

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

        }
    }



    if (!ok) {
        qWarning() << "Calling" << metaMethod.methodSignature() << "failed.";
        return false;
    } else {

       // if(((QObject*)retVal.constData()) != 0) {
          //  qDebug() << "PHPQt5ObjectFactory::call : " << metaMethod.typeName() << returnValue->constData() << returnValue->type();
          //  qDebug() << QVariant::nameToType(metaMethod.typeName());
       // }

        return true;
    }
}


bool PHPQt5ObjectFactory::convertArgs(QMetaMethod metaMethod, QVariantList args, QVariantList *convertedArg)
{
    QList<QByteArray> methodTypes = metaMethod.parameterTypes();
    if (methodTypes.size() != args.size()) {
        return false;
    }

    for (int i = 0; i < methodTypes.size(); i++) {
        const QVariant& arg = args.at(i);

        QByteArray methodTypeName = methodTypes.at(i);
        QByteArray argTypeName = arg.typeName();

        QVariant::Type methodType = QVariant::nameToType(methodTypeName);

        QVariant copy = QVariant(arg);

        if (copy.type() != methodType) {
            if (copy.canConvert(methodType)) {
                if (!copy.convert(methodType)) {
                    qWarning() << "Cannot convert" << argTypeName
                               << "to" << methodTypeName;

                    return false;
                }
            }
            else {
                if(methodTypeName.contains("*")
                        && argTypeName == "int") {
                    //copy = QVariant::fromValue<QObject*>(arg);

                   // void* vparent = static_cast<void*>(0);
                    //if(methodTypeName == QString("QWidget*")){
                    //    copy = QVariant::fromValue<QWidget*>(NULL);
                    //}
                    //else if(methodTypeName == QString("QObject*")) {
                    //    copy = QVariant::fromValue<QObject*>(NULL);
                    //}

                   // QObject *qo = QMetaType::metaObjectForType( QMetaType::type(methodTypeName) )->newInstance();

                  //
                    copy = QVariant::fromValue<QObject*>(0);
                }
                else if(methodTypeName.contains("*")
                        && argTypeName.contains("*")) {
                    //copy = QVariant::fromValue<QObject*>(arg);
                }
                else if(methodTypeName == "QVariant") {
                    *convertedArg << arg;
                }
                else {
                    return false;
                }
            }
        }

        *convertedArg << copy;
    }

    return true;
}

void PHPQt5ObjectFactory::registerZendClassEntry(QString qtClassName, zend_class_entry *ce_ptr)
{
    z_classes.insert(qtClassName, ce_ptr);
}


