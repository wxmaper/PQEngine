#include "plastiqobject.h"
#include "plastiqmethod.h"

#include "phpqt5.h"

zval PHPQt5::engineErrorHandler;

zval PHPQt5::plastiqCall(PQObjectWrapper *pqobject, const QByteArray &methodName, int argc, zval *argv, const PlastiQMetaObject *metaObject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    PlastiQObject *object = Q_NULLPTR;
    zval retVal;

    if(pqobject) {
        object = pqobject->object;

        if(!metaObject) {
            metaObject = pqobject->object->plastiq_metaObject();
        }
    }
    else if(!metaObject) {
        ZVAL_UNDEF(&retVal);
        PQDBG_LVL_DONE();
        return retVal;
    }

    PQDBGLPUP(QString("called object: %1").arg(metaObject->className()));

    QByteArray argsTypes = "";
    PMOGStack stack = new PMOGStackItem[argc+1];

    zval *entry;
    ulong argnum;
    int sidx;

    QList<pq_tmp_call_info> tciList;

    ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(argv), argnum, entry) {
        sidx = argnum + 1;

        switch(Z_TYPE_P(entry)) {
        case IS_TRUE:
            argsTypes += argsTypes.length()
                    ? ",bool" : "bool";
            stack[sidx].s_bool = true;
            break;

        case IS_FALSE:
            argsTypes += argsTypes.length()
                    ? ",bool" : "bool";
            stack[sidx].s_bool = false;
            break;

        case IS_STRING: {
            // определение типа данных
            QString str = QString::fromLatin1(Z_STRVAL_P(entry));

            if(str.length() == Z_STR_P(entry)->len) {
                // vargs << QString::fromUtf8(Z_STRVAL_P(entry)); // возврат к utf8
                argsTypes += argsTypes.length()
                        ? ",QString" : "QString";
                stack[sidx].s_string = QString::fromUtf8(Z_STRVAL_P(entry));;
            }
            else {
                // vargs << QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
                argsTypes += argsTypes.length()
                        ? ",QByteArray" : "QByteArray"; // > И С П Р А В И Т Ь ! <
                stack[sidx].s_bytearray = QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
            }
            break;
        }

        case IS_LONG:
            argsTypes += argsTypes.length()
                    ? ",int" : "int";
            stack[sidx].s_int = Z_LVAL_P(entry);
            break;

        case IS_DOUBLE:
            argsTypes += argsTypes.length()
                    ? ",double" : "double";
            stack[sidx].s_double = Z_DVAL_P(entry);
            break;

        case IS_ARRAY: {
            argsTypes += argsTypes.length()
                    ? ",ZendArray" : "ZendArray";
            break;
        }

        case IS_NULL:
            argsTypes += argsTypes.length()
                    ? ",Q_NULLPTR" : "Q_NULLPTR";
            break;

        case IS_OBJECT: {
            zend_class_entry *ce = Z_OBJCE_P(entry);
            QByteArray className;

            if(Z_OBJCE_NAME_P(entry) == QByteArray("QEnum")) {
                className = "QEnum";
            }
            else {
                do {
                    if(objectFactory()->havePlastiQMetaObject(ce->name->val)) {
                        className = QByteArray(ce->name->val);
                        break;
                    }
                } while(ce = ce->parent);
            }

            if(className.length()) {
                PQObjectWrapper *epqobject = fetch_pqobject(Z_OBJ_P(entry));
                PlastiQObject *eobject = epqobject->object;

                if(epqobject->isEnum) {
                    PQDBGLPUP(QString("arg object: QEnum(%1)").arg(epqobject->enumVal));

                    argsTypes += argsTypes.length()
                            ? ",enum" : "enum";

                    stack[sidx].s_int64 = epqobject->enumVal;
                }
                else if(eobject != Q_NULLPTR && eobject->plastiq_data() != Q_NULLPTR) {
                    PQDBGLPUP(QString("arg object: %1").arg(eobject->plastiq_metaObject()->className()));

                    argsTypes += argsTypes.length()
                            ? "," + className : className;

                    stack[sidx].s_voidp = eobject->plastiq_data();

                    PlastiQ::ObjectType objectType = *(eobject->plastiq_metaObject()->d.objectType);

                    switch(objectType) {
                    case PlastiQ::IsQtObject:
                    case PlastiQ::IsQObject:
                    case PlastiQ::IsQWidget:
                    case PlastiQ::IsQWindow: {
                        bool haveParent = eobject->plastiq_haveParent();
                        tciList << pq_tmp_call_info { eobject, entry, haveParent };
                    } break;

                    default: ;
                    }
                }
                else {
                    php_error(E_ERROR,
                              QString("ACCESS TO NULL PlastiQObject: %1 (%2)")
                              .arg(argnum)
                              .arg(zend_zval_type_name(entry))
                              .toUtf8().constData());
                }
            }
            else if(Z_OBJCE_NAME_P(entry) == QByteArray("Closure")) {
                PQDBGLPUP("ADD CLOSURE");
                argsTypes += argsTypes.length()
                        ? ",Closure" : "Closure";
            }
            else {
                zend_wrong_paramer_class_error(argnum, (char*) "Object", entry);
            }
        } break;

        default:
            php_error(E_ERROR,
                      QString("%4::%5(): Unknown type of argument %1 (%2 [%3])")
                      .arg(argnum)
                      .arg(zend_zval_type_name(entry))
                      .arg(Z_TYPE_P(entry))
                      .arg(metaObject->className())
                      .arg(methodName.constData())
                      .toUtf8().constData());
        }

    } ZEND_HASH_FOREACH_END();

    PlastiQMethod::Access access = object == Q_NULLPTR
            ? PlastiQMethod::StaticPublic
            : PlastiQMethod::None;

    PlastiQMetaObject::Call call = object == Q_NULLPTR
            ? PlastiQMetaObject::InvokeStaticMember
            : PlastiQMetaObject::InvokeMethod;

    /* FIXME evil shit code for refernces :-) */
    QString *strvala;
    QStringList *slvala;
    long *lvala;
    int *ivala;
    bool *bvala;
    double *dvala;
    float *fvala;

    bool fastCall = false;
    bool right = false;

    int mid = metaObject->methodId( QByteArray(methodName).append("(").append(argsTypes).append(")"), access );

    if(mid >= 0) {
        PQDBGLPUP(QString("fast call methodId: %1").arg(mid));
        metaObject->d.static_metacall(object, call, mid, stack);
        right = true;
        fastCall = true;
    }
    else {
        /* FIXME evil shit code for refernces :-) */
        strvala = new QString[argc+1];
        slvala = new QStringList[argc+1];
        lvala = new long[argc+1];
        ivala = new int[argc+1];
        bvala = new bool[argc+1];
        dvala = new double[argc+1];
        fvala = new float[argc+1];

        QList<PlastiQCandidateMethod> candidates = metaObject->candidates(methodName, argc, PlastiQMethod::Method, access);
        PQDBGLPUP(QString("general call: candidates size: %1").arg(candidates.size()));

        tciList.clear();

        right = candidates.size() > 0;
        for(int cid = 0; cid < candidates.size(); cid++) {
            QString d_argsTypes;
            foreach(QString at, candidates.at(cid).argTypes) {
                d_argsTypes += d_argsTypes.length() ? "," + at : at;
            }

            PQDBGLPUP(QString("cid: %1; candidate: %2(%3)")
                      .arg(cid)
                      .arg(candidates.at(cid).name.constData())
                      .arg(d_argsTypes));

            argsTypes.clear();
            right = true;

            for(int idx = 0; (idx < argc && right); idx++) {
                QString fMethodType = candidates.at(cid).argTypes.at(idx);
                QString methodType = QString(fMethodType).replace("&","");
                bool isref = methodType != fMethodType;

                argsTypes += argsTypes.length() ? "," + fMethodType : fMethodType;
                int sidx = idx + 1;

                PQDBGLPUP(QString("idx: %1; methodType: %2")
                          .arg(idx)
                          .arg(methodType));

                zval *entry = &Z_ARRVAL_P(argv)->arData[idx].val;

                switch(Z_TYPE_P(entry)) {
                case IS_STRING:
                    if(methodType == "char*" || methodType == "char**" || methodType == "const char*") {
                        stack[sidx].s_voidp = Z_STRVAL_P(entry);
                    }
                    else if(methodType == "QString") {
                        if(isref) {
                            strvala[sidx] = QString::fromUtf8(Z_STRVAL_P(entry));
                            stack[sidx].s_voidp = reinterpret_cast<void*>(&strvala[sidx]);
                        }
                        else {
                            stack[sidx].s_string = QString::fromUtf8(Z_STRVAL_P(entry));
                        }
                    }
                    else if(methodType == "QByteArray") {
                        QString str = QString::fromLatin1(Z_STRVAL_P(entry));

                        if(str.length() == Z_STR_P(entry)->len) {
                            if(isref) {
                                stack[sidx].s_voidp = &QByteArray(Z_STRVAL_P(entry));
                            }
                            else {
                                stack[sidx].s_bytearray = QByteArray(Z_STRVAL_P(entry));
                            }
                        }
                        else {
                            if(isref) {
                                stack[sidx].s_voidp = &QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
                            }
                            else {
                                stack[sidx].s_bytearray = QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
                            }
                        }
                    }
                    else if(methodType == "QVariant") {
                        if(!isref) {
                            stack[sidx].s_variant = QVariant(QByteArray(Z_STRVAL_P(entry)));
                        }
                        else right = false;
                    }
                    else right = false;
                    break;

                case IS_LONG:
                    if(methodType == "long") {
                        if(isref) {
                            lvala[sidx] = Z_LVAL_P(entry);
                            stack[sidx].s_voidp = reinterpret_cast<void*>(&lvala[sidx]);
                        }
                        else {
                            stack[sidx].s_long = Z_LVAL_P(entry);
                        }
                    }
                    else if(methodType == "int") {
                        if(isref) {
                            ivala[sidx] = (int) Z_LVAL_P(entry);
                            stack[sidx].s_voidp = reinterpret_cast<void*>(&ivala[sidx]);
                        }
                        else {
                            stack[sidx].s_int = Z_LVAL_P(entry);
                        }
                    }
                    else if(methodType == "enum") {
                        stack[sidx].s_int64 = qint64(Z_LVAL_P(entry));
                    }
                    else if(methodType == "QVariant") {
                        if(!isref) {
                            stack[sidx].s_variant = QVariant(Z_LVAL_P(entry));
                        }
                        else right = false;
                    }
                    else right = false;
                    break;

                case IS_TRUE: {
                    if(methodType == "bool") {
                        if(isref) {
                            bvala[sidx] = true;
                            stack[sidx].s_voidp = reinterpret_cast<void*>(&bvala[sidx]);
                        }
                        else {
                            stack[sidx].s_bool = true;
                        }
                    }
                    else if(methodType == "QVariant") {
                        if(!isref) {
                            stack[sidx].s_variant = QVariant(true);
                        }
                        else right = false;
                    }
                    else right = false;
                } break;

                case IS_FALSE: {
                    if(methodType == "bool") {
                        if(isref) {
                            bvala[sidx] = false;
                            stack[sidx].s_voidp = reinterpret_cast<void*>(&bvala[sidx]);
                        }
                        else {
                            stack[sidx].s_bool = false;
                        }
                    }
                    else if(methodType == "QVariant") {
                        if(!isref) {
                            stack[sidx].s_variant = QVariant(false);
                        }
                        else right = false;
                    }
                    else right = false;
                } break;

                case IS_DOUBLE: {
                    if(methodType == "double") {
                        if(isref) {
                            dvala[sidx] = Z_DVAL_P(entry);
                            stack[sidx].s_voidp = reinterpret_cast<void*>(&dvala[sidx]);
                        }
                        else {
                            stack[sidx].s_double = Z_DVAL_P(entry);
                        }
                    }
                    if(methodType == "float") {
                        if(isref) {
                            fvala[sidx] = (float) Z_DVAL_P(entry);
                            stack[sidx].s_voidp = reinterpret_cast<void*>(&fvala[sidx]);
                        }
                        else {
                            stack[sidx].s_float = (float) Z_DVAL_P(entry);
                        }
                    }
                    else if(methodType == "enum") {
                        stack[sidx].s_int64 = qint64(Z_LVAL_P(entry));
                    }
                    else if(methodType == "QVariant") {
                        if(!isref) {
                            stack[sidx].s_variant = QVariant(Z_DVAL_P(entry));
                        }
                        else right = false;
                    }
                    else right = false;
                } break;

                case IS_ARRAY: {
                    zend_array *arr = Z_ARRVAL_P(entry);
                    int size = arr->nNumOfElements;

                    if(methodType == "char**") {
                        char **achar = (char**)malloc(size * sizeof(char*));
                        for(int acidx = 0; acidx < size; acidx++) {
                            int acsize = arr->arData[acidx].val.value.str->len;
                            achar[acidx] = (char*)malloc(acsize * sizeof(char));
                            achar[acidx] = arr->arData[acidx].val.value.str->val;
                        }

                        stack[sidx].s_voidp = reinterpret_cast<void*>(achar);
                    }
                    else if(methodType == "QStringList" || methodType == "QVariant") {
                        QStringList sl;

                        for(int acidx = 0; acidx < size; acidx++) {
                            zval outstr;

                            php_output_start_default();
                            zend_print_zval_r(&arr->arData[acidx].val, 0);
                            php_output_get_contents(&outstr);

                            sl << QString(Z_STRVAL(outstr));

                            php_output_discard();
                        }

                        slvala[sidx] = sl;

                        if(methodType == "QStringList") {
                            stack[sidx].s_voidp = reinterpret_cast<void*>(&slvala[sidx]);
                        }
                        else if(methodType == "QVariant") {
                            stack[sidx].s_variant = QVariant(sl);
                        }
                    }
                    else right = false;
                } break;

                case IS_OBJECT: {
                    zend_class_entry *ce = Z_OBJCE_P(entry);
                    QByteArray className;

                    if(Z_OBJCE_NAME_P(entry) == QByteArray("QEnum")) {
                        className = "QEnum";
                    }
                    else {
                        do {
                            if(objectFactory()->havePlastiQMetaObject(ce->name->val)) {
                                className = QByteArray(ce->name->val);
                                break;
                            }
                        } while(ce = ce->parent);
                    }

                    if(className.length()) {
                        PQObjectWrapper *epqobject = fetch_pqobject(Z_OBJ_P(entry));
                        PlastiQObject *object = epqobject->object;

                        if(epqobject->isEnum) {
                            PQDBGLPUP(QString("arg object: QEnum(%1)").arg(epqobject->enumVal));

                            argsTypes += argsTypes.length()
                                    ? ",enum" : "enum";

                            stack[sidx].s_int64 = epqobject->enumVal;
                        }
                        else if(object != Q_NULLPTR && object->plastiq_data() != Q_NULLPTR) {
                            PQDBGLPUP(QString("arg object: %1").arg(epqobject->object->plastiq_metaObject()->className()));

                            const PlastiQMetaObject *metaObject = epqobject->object->plastiq_metaObject();
                            bool cancast = false;
                            do {
                                QByteArray objectClassName = metaObject->className();

                                if(methodType == objectClassName) {
                                    stack[sidx].s_voidp = epqobject->object->plastiq_data();
                                    cancast = true;
                                    break;
                                }
                                else if(fMethodType.contains("*")) {
                                    if(fMethodType == QString(objectClassName).append("*")
                                            || fMethodType == QString(objectClassName).prepend("const ").append("*")) {
                                        stack[sidx].s_voidp = epqobject->object->plastiq_data();

                                        if(methodName == "moveToThread") {
                                            QThread *thread = reinterpret_cast<QThread*>(epqobject->object->plastiq_data());
                                            // pqobject->ctx = PHPQt5::threadCreator()->get_tsrmls_cache(thread);
                                            pqobject->thread = thread;

                                            PQDBGLPUP(QString("thread: %1; TSRMLS_CACHE: %2")
                                                      .arg(reinterpret_cast<quint64>(thread))
                                                      .arg(reinterpret_cast<quint64>(pqobject->ctx)));
                                        }

                                        cancast = true;
                                        break;
                                    }
                                    else {
                                        //do {
                                            objectClassName = metaObject->className();
                                            foreach(PlastiQMetaObject* inherit, *metaObject->d.inherits) {
                                                PQDBGLPUP(QString("downcast %1 to %2").arg(objectClassName.constData()).arg(inherit->className()));
                                                objectClassName = inherit->className();

                                                if(fMethodType == QString(objectClassName).append("*")
                                                        || fMethodType == QString(objectClassName).prepend("const ").append("*")) {
                                                    stack[sidx].s_voidp = epqobject->object->plastiq_data();
                                                    cancast = true;
                                                    break;
                                                }

                                            }
                                        //} while((metaObject = metaObject->d.superdata) && !cancast);
                                    }

                                    if(cancast) {
                                        break;
                                    }
                                }
                                else if(fMethodType == QByteArray(objectClassName).append("&")) {
                                    stack[sidx].s_voidp = epqobject->object->plastiq_data();
                                    cancast = true;
                                    break;
                                }
                                else if(fMethodType == QByteArray(objectClassName).prepend("const ").append("&")) {
                                    stack[sidx].s_voidp = epqobject->object->plastiq_data();
                                    cancast = true;
                                    break;
                                }
                                else if(methodType == "QVariant" && epqobject->object->plastiq_objectType() == PlastiQ::IsQtObject) {
                                    int metaTypeId = QMetaType::type(objectClassName);
                                    if(metaTypeId != QMetaType::UnknownType) {
                                        stack[sidx].s_variant.create(metaTypeId, epqobject->object->plastiq_data());
                                        cancast = true;
                                        break;
                                    }
                                }
                                else continue;
                            } while(metaObject = metaObject->d.superdata);

                            if(cancast) {
                                PlastiQ::ObjectType objectType = *(object->plastiq_metaObject()->d.objectType);

                                switch(objectType) {
                                case PlastiQ::IsQtObject:
                                case PlastiQ::IsQObject:
                                case PlastiQ::IsQWidget:
                                case PlastiQ::IsQWindow: {
                                    bool haveParent = object->plastiq_haveParent();
                                    tciList << pq_tmp_call_info { object, entry, haveParent };
                                } break;

                                default: ;
                                }
                            }

                            right = cancast;
                        }
                        else right = false;
                    }
                    /* FunctorOrLambda */
                    else if(Z_OBJCE_NAME_P(entry) == QByteArray("Closure") && methodType == "FunctorOrLambda") {
                        PQDBGLPUP("methodType == FunctorOrLambda");

                        PQObjectWrapper *receiver_pqobject = new PQObjectWrapper;
                        receiver_pqobject->object = Q_NULLPTR;
                        receiver_pqobject->zoptr = Z_OBJ_P(entry);
                        receiver_pqobject->isClosure = true;
                        receiver_pqobject->isExtra = false;
                        receiver_pqobject->thread = QThread::currentThread();

                        stack[sidx].s_voidp = reinterpret_cast<void*>(receiver_pqobject);

                        Z_ADDREF_P(entry);
                    }
                    else right = false;
                } break;

                case IS_NULL:
                    if(methodType.indexOf("*") >= 0) {
                        stack[sidx].s_voidp = Q_NULLPTR;
                        break;
                    }
                    else right = false;

                default: right = false;
                }
            }

            if(right) {
                mid = candidates.at(cid).idx;
                break;
            }
        }

        if(right) {
            PQDBGLPUP(QString("call candidate method: %1(%2)")
                      .arg(methodName.constData())
                      .arg(QString(argsTypes).replace(" ", "")));

            metaObject->d.static_metacall(object, call, mid, stack);
        }
        else {
            // FIXME
            PQDBGLPUP(QString("Call to undefined method %1::%2() or wrong arguments")
                      .arg(metaObject->className())
                      .arg(methodName.constData()).toUtf8().constData());

            php_error(E_ERROR,
                      "Call to undefined method %s::%s() or wrong arguments",
                      metaObject->className(),
                      methodName.constData());

            stack[0].type = PlastiQ::Void;
            tciList.clear();
        }
    }

    PQDBGLPUP("check parents");
    foreach(pq_tmp_call_info tci, tciList) {
        bool haveParentBefore = tci.haveParent;
        bool haveParentAfter = tci.po->plastiq_haveParent();

        // не было родителя и появился
        if(!haveParentBefore && haveParentAfter) {
            PQDBGLPUP("ADDREF");
            Z_ADDREF_P(tci.zv);
        }
        // был родитель и не стало
        else if(haveParentBefore && !haveParentAfter) {
            PQDBGLPUP("DELREF");
            Z_DELREF_P(tci.zv);
        }
    }

    PQDBGLPUP(QString("rettype: %1").arg(stack[0].type));
    if(stack[0].type != PlastiQ::Void && right) {
        retVal = plastiq_cast_to_zval(stack[0]);
    }
    else {
        ZVAL_UNDEF(&retVal);
    }

    delete [] stack;

    if(!fastCall) {
        delete [] strvala;
        delete [] slvala;
        delete [] lvala;
        delete [] ivala;
        delete [] bvala;
        delete [] dvala;
        delete [] fvala;
    }

    PQDBG_LVL_DONE();
    return retVal;
}

PQObjectWrapper *PlastiQ_getWrapper(const PMOGStackItem &stackItem)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    PQDBGLPUP(QString("stackItem name: %1").arg(stackItem.name.constData()));

    PQObjectWrapper *pqobject = Q_NULLPTR;

    void *d = stackItem.s_voidp;
    quint64 objectId = reinterpret_cast<quint64>(d);
    QByteArray className;

    switch(stackItem.type) {
    case PlastiQ::ObjectStar:
    case PlastiQ::QObjectStar:
    case PlastiQ::QtObjectStar: {
        PQDBGLPUP("ItemType: ObjectStar || QObjectStar || QtObjectStar");

        if(PHPQt5::objectFactory()->havePlastiQMetaObject(stackItem.name)) {
            if(pqobject = PHPQt5::objectFactory()->getObject(objectId)) {
            }
            else {
                className = stackItem.name;
            }
        }
    } break;

    case PlastiQ::QEventStar: {
        PQDBGLPUP("ItemType: QEventStar");

        if(stackItem.name == "QEvent") {
            if(pqobject = PHPQt5::objectFactory()->getObject(objectId)) {
                PHPQt5::qevent_cast(pqobject);
            }
            else {
                className = PHPQt5::plastiqGetQEventClassName(reinterpret_cast<QEvent*>(d));

                if(!PHPQt5::objectFactory()->havePlastiQMetaObject(className)) {
                    className = "QEvent";
                }
            }
        }
    } break;

    default: ;
    }

    PQDBGLPUP(QString("className: %1").arg(className.length()));

    if(className.length()) {
        if(PHPQt5::objectFactory()->havePlastiQMetaObject(className)) {
            PlastiQMetaObject metaObject = PHPQt5::objectFactory()->getMetaObject(className);
            PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

            PlastiQObject *object = metaObject.createInstanceFromData(d);
            PQDBGLPUP(QString("created object: %1").arg(object->plastiq_metaObject()->className()));

            zval zobject;
            zend_class_entry *ce = PHPQt5::objectFactory()->getClassEntry(className);
            object_init_ex(&zobject, ce);

            pqobject = fetch_pqobject(Z_OBJ(zobject));
            pqobject->object = object;
            pqobject->isExtra = true;
            pqobject->isValid = true;

            PHPQt5::objectFactory()->addObject(pqobject, objectId);

            Z_DELREF(zobject);
        }
    }

    PQDBG_LVL_DONE();
    return pqobject;
}

zval PHPQt5::plastiq_cast_to_zval(const PMOGStackItem &stackItem)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval retval;

    switch (stackItem.type) {
    case PlastiQ::True:
    case PlastiQ::False:
    case PlastiQ::Bool:
        ZVAL_BOOL(&retval, stackItem.s_bool);
        break;

    case PlastiQ::Enum:
        ZVAL_DOUBLE(&retval, ((double)stackItem.s_int64));
        break;

    case PlastiQ::Long:
        ZVAL_LONG(&retval, stackItem.s_long);
        break;

    case PlastiQ::Int:
        ZVAL_LONG(&retval, stackItem.s_int);
        break;

    case PlastiQ::Double:
        ZVAL_DOUBLE(&retval, stackItem.s_double);
        break;

    case PlastiQ::String:
        ZVAL_STRING(&retval, stackItem.s_string.toUtf8().constData());
        break;

    case PlastiQ::ByteArray:
        ZVAL_STRINGL(&retval, stackItem.s_bytearray.constData(), stackItem.s_bytearray.size());
        break;

    case PlastiQ::ObjectStar:
    case PlastiQ::QObjectStar:
    case PlastiQ::QtObjectStar: {
        PQDBGLPUP(QString("ItemType::ObjectStar||QObjectStar: %1").arg(stackItem.name.constData()));

        void *d = stackItem.s_voidp;
        quint64 objectId = reinterpret_cast<quint64>(d);
        PQDBGLPUP(QString("objectId: %1").arg(objectId));

        QByteArray className = stackItem.name;

        if(stackItem.type == PlastiQ::QObjectStar && objectId != 0) {
            PQDBGLPUP(QString("ItemType = PlastiQ::QObjectStar; get QObject instance"));

            QObject *qobject = reinterpret_cast<QObject*>(stackItem.s_voidp);
            className = qobject->metaObject()->className();

            PQDBGLPUP(QString("QObject className: %1").arg(className.constData()));

            if(className == "QWidgetWindow") {
                className = "QWindow"; // FIXME
            }
        }

        if(stackItem.type == PlastiQ::ObjectStar && objectId != 0 && stackItem.name == "char") {
            QByteArray buff(reinterpret_cast<char*>(d));
            ZVAL_STRING(&retval, buff.constData());
        }
        else if(objectFactory()->havePlastiQMetaObject(className) && objectId != 0) {
            PQObjectWrapper *pqobject;

            if(pqobject = objectFactory()->getObject(objectId)) {
                ZVAL_OBJ(&retval, &pqobject->zo);
            }
            else {
                PlastiQMetaObject metaObject = objectFactory()->getMetaObject(className);
                PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

                PlastiQObject *object = metaObject.createInstanceFromData(stackItem.s_voidp);
                PQDBGLPUP(QString("created object: %1").arg(object->plastiq_metaObject()->className()));

                zend_class_entry *ce = objectFactory()->getClassEntry(className);
                object_init_ex(&retval, ce);

                zend_update_property_long(ce, &retval, "__pq_uid", sizeof("__pq_uid")-1, objectId);
                zend_update_property_long(ce, &retval, "__pq_zhandle", sizeof("__pq_zhandle")-1, Z_OBJ_HANDLE(retval));

                pqobject = fetch_pqobject(Z_OBJ(retval));
                pqobject->object = object;
                pqobject->isExtra = true;
                pqobject->isValid = true;

                objectFactory()->addObject(pqobject, objectId);

                Z_DELREF(retval);
            }
        }
        else {
            ZVAL_NULL(&retval);
        }
    } break;

    case PlastiQ::Object:
    case PlastiQ::QObject:
    case PlastiQ::QtObject: {
        PQDBGLPUP(QString("ItemType::QtObject %1").arg(stackItem.name.constData()));
        if(stackItem.name == "QStringList") {
            array_init(&retval);

            QStringList stringList = *reinterpret_cast<QStringList*>(stackItem.s_voidp);
            foreach (const QString &string, stringList) {
                QByteArray ba = string.toUtf8();
                //zval zstr;
                //ZVAL_STRINGL(&zstr, ba.constData(), ba.length());
                //zend_hash_next_index_insert(Z_ARRVAL(&array), &zstr);
                add_next_index_stringl(&retval, ba.constData(), ba.length());
            }
        }
        else if(stackItem.name == "QObjectList") {
            array_init(&retval);

            QObjectList *objectList = reinterpret_cast<QObjectList*>(stackItem.s_voidp);
            foreach(QObject *qobject, *objectList) {
                zval zobject;
                quint64 objectId = reinterpret_cast<quint64>(qobject);
                PQObjectWrapper *pqobject;
                zend_class_entry *ce;

                QByteArray className = qobject->metaObject()->className();

                if(pqobject = objectFactory()->getObject(objectId)) {
                    ZVAL_OBJ(&zobject, &pqobject->zo);
                    add_next_index_zval(&retval, &zobject);
                }
                else if(ce = objectFactory()->getClassEntry(className)) {
                    PlastiQMetaObject metaObject = objectFactory()->getMetaObject(className);
                    PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

                    PlastiQObject *object = metaObject.createInstanceFromData(reinterpret_cast<void*>(qobject));
                    PQDBGLPUP(QString("created object: %1").arg(object->plastiq_metaObject()->className()));

                    PQDBGLPUP("object_init_ex");
                    object_init_ex(&zobject, ce);

                    PQDBGLPUP("fetch_pqobject");
                    pqobject = fetch_pqobject(Z_OBJ(zobject));
                    pqobject->object = object;
                    pqobject->isExtra = true;
                    pqobject->isValid = true;

                    quint32 zhandle = Z_OBJ_HANDLE(zobject);

                    zend_update_property_long(ce, &zobject, "__pq_uid", sizeof("__pq_uid")-1, objectId);
                    zend_update_property_long(ce, &zobject, "__pq_zhandle", sizeof("__pq_zhandle")-1, zhandle);

                    objectFactory()->addObject(pqobject, objectId);

                    add_next_index_zval(&retval, &zobject);
                }
                else {

                }
            }

            delete objectList;
        }
        else if(objectFactory()->havePlastiQMetaObject(stackItem.name)) {
            //void *d = stackItem.s_voidp;


            //if(PHPQt5::objectFactory()->havePlastiQObject(objectId)) {

            //}
            //else {
            PlastiQMetaObject metaObject = objectFactory()->getMetaObject(stackItem.name);
            PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

            PlastiQObject *object = metaObject.createInstanceFromData(stackItem.s_voidp);
            PQDBGLPUP(QString("created object: %1").arg(object->plastiq_metaObject()->className()));

            zend_class_entry *ce = objectFactory()->getClassEntry(stackItem.name);
            object_init_ex(&retval, ce);

            PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ(retval));
            pqobject->object = object;
            pqobject->isExtra = stackItem.isRef; // <--- false becouse this object is a copy
            pqobject->isValid = true;

            quint64 objectId = reinterpret_cast<quint64>(stackItem.s_voidp);
            objectFactory()->addObject(pqobject, objectId);
            // objectFactory()->addObjectInStorage(pqobject, reinterpret_cast<quint64>(stackItem.s_voidp));

            Z_DELREF(retval);
            // }
        }
        else {
            ZVAL_NULL(&retval);
        }
    } break;

    case PlastiQ::QEventStar: {
        if(stackItem.name == "QEvent") {
            PQObjectWrapper *pqobject = Q_NULLPTR;
            if(pqobject = PlastiQ_getWrapper(stackItem)) {
                ZVAL_OBJ(&retval, &pqobject->zo);
            }
            else {
                ZVAL_NULL(&retval);
            }
        }
    } break;

    default:
        ZVAL_NULL(&retval);
    }

    PQDBG_LVL_DONE_LPUP();

    return retval;
}

void *PHPQt5::plastiq_cast_to_s_voidp(const PMOGStackItem &stackItem)
{
    return 0;
}

void PHPQt5::plastiqErrorHandler(int error_num, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    PQDBGLPUP("load buffer:");
    char buffer[1024];
    int length = vsprintf(buffer, format, args);

#ifdef PQDEBUG
    qDebug() << QString("In file `%1` at line %2: error %3: %4")
                .arg(error_filename).arg(error_lineno).arg(error_num).arg(buffer).toUtf8().constData();
#endif

    if(Z_TYPE(engineErrorHandler) != IS_UNDEF) {
        QByteArray fileName(error_filename);
        //QByteArray text("buffer");

        zval retval;
        zval *params = (zval *) safe_emalloc(4, sizeof(zval), 0);

        ZVAL_LONG(&params[0], error_num);
        ZVAL_STRINGL(&params[1], buffer, length);
        ZVAL_STRINGL(&params[2], fileName.constData(), fileName.length());
        ZVAL_LONG(&params[3], error_lineno);

        void *TSRMLS_CACHE = Q_NULLPTR;
        TSRMLS_CACHE_UPDATE();

        PQDBGLPUP(QString("engineErrorHandler type: %1").arg(Z_TYPE(engineErrorHandler)));
        if(Z_TYPE(engineErrorHandler) == IS_OBJECT) {
            PQDBGLPUP("call closure");
            zval function_name;
            ZVAL_STRINGL(&function_name, ZEND_INVOKE_FUNC_NAME, strlen(ZEND_INVOKE_FUNC_NAME));

            if(call_user_function_ex(NULL, &engineErrorHandler, &function_name, &retval, 4, params, 1, NULL) == SUCCESS) {
                if(Z_TYPE(retval) == IS_TRUE) {
                    PQDBGLPUP("php_request_shutdown");
                    php_request_shutdown(Q_NULLPTR);
                }
            }
            else {
                PQDBGLPUP("engineErrorHandler call error");
                default_ub_write(QString(buffer), "");
            }

            zval_ptr_dtor(&function_name);
        }
        else {
            PQDBGLPUP("call user function");
            if(call_user_function_ex(CG(function_table), NULL, &engineErrorHandler, &retval, 4, params, 1, NULL) == SUCCESS) {
                if(Z_TYPE(retval) == IS_TRUE) {
                    PQDBGLPUP("php_request_shutdown");
                    php_request_shutdown(Q_NULLPTR);
                }
            }
            else {
                PQDBGLPUP("engineErrorHandler call error");
                default_ub_write(QString(buffer), "");
            }
        }

        for (int i = 0; i < 4; i++) {
            zval_ptr_dtor(&params[i]);
        }

        efree(params);
    }
    else {
        PQDBGLPUP("engineErrorHandler function have not been set");
        default_ub_write(QString(buffer), "");

        zval argv, _parent, _title, _message;
        if(PHPQt5::objectFactory()->havePlastiQMetaObject("QMessageBox")) {
            ZVAL_NULL(&_parent);
            ZVAL_STRING(&_message, QString(buffer).replace("\n", "<br>").toUtf8().constData());

            array_init(&argv);
            add_next_index_zval(&argv, &_parent);

            switch(error_num) {
            case E_ERROR:
            case E_USER_ERROR:
            case E_CORE_ERROR:
            case E_COMPILE_ERROR:
                ZVAL_STRING(&_title, "Error");
                add_next_index_zval(&argv, &_title);
                add_next_index_zval(&argv, &_message);
                PHPQt5::plastiqCall(Q_NULLPTR, "critical", 3, &argv, &PHPQt5::objectFactory()->getMetaObject("QMessageBox"));
                PQDBGLPUP("php_request_shutdown");
                php_request_shutdown(Q_NULLPTR);
                break;

            case E_WARNING:
            case E_PARSE:
            case E_USER_WARNING:
            case E_CORE_WARNING:
            case E_COMPILE_WARNING:
                ZVAL_STRING(&_title, "Warning");
                add_next_index_zval(&argv, &_title);
                add_next_index_zval(&argv, &_message);
                PHPQt5::plastiqCall(Q_NULLPTR, "warning", 3, &argv, &PHPQt5::objectFactory()->getMetaObject("QMessageBox"));
                PQDBGLPUP("php_request_shutdown");
                php_request_shutdown(Q_NULLPTR);
                break;

            case E_NOTICE:
            case E_USER_NOTICE:
                ZVAL_STRING(&_title, "Notice");
                add_next_index_zval(&argv, &_title);
                add_next_index_zval(&argv, &_message);
                PHPQt5::plastiqCall(Q_NULLPTR, "information", 3, &argv, &PHPQt5::objectFactory()->getMetaObject("QMessageBox"));
                break;

            default: ;
            }

            zval_dtor(&argv);
        }
    }

    PQDBG_LVL_DONE();
}

