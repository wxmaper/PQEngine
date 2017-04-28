#include "plastiqobject.h"
#include "plastiqmethod.h"

#include "phpqt5.h"

void PHPQt5::zim_qenum___construct(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    Q_UNUSED(return_value)

    zval *enum_zval;

    if(zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &enum_zval) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    PQObjectWrapper *pqenum = fetch_pqobject(Z_OBJ_P(getThis()));
    pqenum->object = Q_NULLPTR;
    pqenum->isEnum = true;
    pqenum->isValid = true;

    switch(Z_TYPE_P(enum_zval)) {
    case IS_LONG:
        pqenum->enumVal = qint64(Z_LVAL_P(enum_zval));
        break;

    case IS_DOUBLE:
        pqenum->enumVal = qint64(Z_DVAL_P(enum_zval));
        break;

    default:
#if (PHP_VERSION_ID < 70101)
        zend_wrong_paramer_class_error(1, (char*) "long", enum_zval);
#else
        zend_wrong_parameter_class_error(1, (char*) "long", enum_zval);
#endif
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_plastiq___construct(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg("0")
              .arg(Z_OBJ_HANDLE_P(getThis())));

    pqdbg_send_message({
                           { "command", "construct" },
                           { "thread", QString::number(reinterpret_cast<quint64>(QThread::currentThread())) },
                           { "zclass", Z_OBJCE_NAME_P(getThis()) },
                           { "zobject", QString::number(reinterpret_cast<quint64>(Z_OBJ_P(getThis()))) },
                           { "zhandle", QString::number(Z_OBJ_HANDLE_P(getThis())) },
                       });
#endif

    Q_UNUSED(return_value)

    const int argc = ZEND_NUM_ARGS();
    zval *args = (zval *) safe_emalloc(argc, sizeof(zval), 0);

    if(zend_get_parameters_array_ex(argc, args) == FAILURE)
    {
        efree(args);
        PQDBG_LVL_DONE();
        WRONG_PARAM_COUNT;
    }

    QList<int> types;
    QString argsTypes = "";
    zval *entry;
    bool isWrapper = false;

    PMOGStack stack = new PMOGStackItem[10];
    QList<pq_tmp_call_info> tciList;

    for(int i = 0; i < argc; i++) {
        entry = &args[i];
        types << Z_TYPE_P(entry);
        int sidx = i + 1;

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
                argsTypes += argsTypes.length()
                        ? ",QString" : "QString";
                stack[sidx].s_string = QString::fromUtf8(Z_STRVAL_P(entry)); // возврат к utf8
            }
            else {
                argsTypes += argsTypes.length()
                        ? ",QByteArray" : "QByteArray";
                stack[sidx].s_bytearray = QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
            }
        } break;

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

        case IS_ARRAY:
            argsTypes += argsTypes.length()
                    ? ",ZendArray" : "ZendArray";
            break;

        case IS_OBJECT: {
            //argsTypes += argsTypes.length()
            //           ? ",double" : "double";
            //stack[sidx].s_voidp = Z_DVAL_P(entry);

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

                    stack[sidx].s_enum = epqobject->enumVal;
                }
                else if(eobject != Q_NULLPTR && eobject->plastiq_data() != Q_NULLPTR) {
                    PQDBGLPUP(QString("arg object: %1").arg(eobject->plastiq_metaObject()->className()));

                    argsTypes += argsTypes.length()
                            ? "," + className : className;

                    if (className == "QString") {
                        stack[sidx].s_string = *(reinterpret_cast<QString*>(eobject->plastiq_data()));
                    }
                    else if (className == "QByteArray") {
                        stack[sidx].s_bytearray = *(reinterpret_cast<QByteArray*>(eobject->plastiq_data()));
                    }
                    //else if (className == "QVariant") {
                    //    stack[sidx].s_variant = *(reinterpret_cast<QVariant*>(eobject->plastiq_data()));
                    //}
                    else {
                        stack[sidx].s_voidp = eobject->plastiq_data();

                        PlastiQ::ObjectType objectType = *(eobject->plastiq_metaObject()->d.objectType);

                        switch(objectType) {
                        case PlastiQ::IsQtObject:
                        case PlastiQ::IsQObject:
                        case PlastiQ::IsQWidget:
                        case PlastiQ::IsQWindow:
                        case PlastiQ::IsQtItem: {
                            bool haveParent = eobject->plastiq_haveParent();
                            tciList << pq_tmp_call_info { epqobject, entry, haveParent };
                        } break;

                        default: ;
                        }
                    }
                }
                else {
                    php_error(E_ERROR,
                              QString("ACCESS TO NULL PlastiQObject: %1 (%2)")
                              .arg(i)
                              .arg(zend_zval_type_name(entry))
                              .toUtf8().constData());
                }
            }
            else {
#if (PHP_VERSION_ID < 70101)
                zend_wrong_paramer_class_error(i, (char*) "Object", entry);
#else
                zend_wrong_parameter_class_error(i, (char*) "Object", entry);
#endif
            }
        } break;

        case IS_NULL:
            argsTypes += argsTypes.length()
                    ? ",Q_NULLPTR" : "Q_NULLPTR";
            break;

        default:
            php_error(E_ERROR, QString("Unknown type of argument %1").arg(i).toUtf8().constData());
        }
    }

    zend_class_entry *ce = Z_OBJCE_P(getThis());
    QByteArray className;
    do {
        if (objectFactory()->havePlastiQMetaObject(ce->name->val)) {
            className = QByteArray(ce->name->val);
            break;
        } else isWrapper = true; // php-class was extends a qt-class
    } while(ce = ce->parent);

    PlastiQMetaObject metaObject = objectFactory()->getMetaObject(className);
    QByteArray signature = QString("%1(%2)").arg(className.constData()).arg(argsTypes).toUtf8();

    int mid = metaObject.constructorId(signature);

    /* FIXME evil shit code for refernces :-) */
    QString *strvala = new QString[10];
    QByteArray *bavala = new QByteArray[10];
    QStringList *slvala = new QStringList[10];
    long *lvala = new long[10];
    int *ivala = new int[10];
    bool *bvala = new bool[10];
    double *dvala = new double[10];
    float *fvala = new float[10];

    if(mid >= 0) {
        PQDBGLPUP(QString("fast call constructorId: %1").arg(mid));
        objectFactory()->createPlastiQObject(className, signature, getThis(), isWrapper, stack);
    }
    else {
        tciList.clear();

        QList<PlastiQCandidateMethod> candidates = metaObject.candidates(className, argc, PlastiQMethod::Constructor);
        PQDBGLPUP(QString("general call: candidates size: %1").arg(candidates.size()));

        bool right;
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
                /*
                QString methodType = candidates.at(cid).argTypes.at(idx);
                argsTypes += argsTypes.length() ? "," + methodType : methodType;
                int sidx = idx + 1;

                PQDBGLPUP(QString("idx: %1; methodType: %2")
                          .arg(idx)
                          .arg(methodType));
                */


                QString fMethodType = candidates.at(cid).argTypes.at(idx);
                QString methodType = QString(fMethodType).replace("&","");
                bool isref = methodType != fMethodType;

                argsTypes += argsTypes.length() ? "," + fMethodType : fMethodType;
                int sidx = idx + 1;

                PQDBGLPUP(QString("idx: %1; methodType: %2")
                          .arg(idx)
                          .arg(methodType));

                zval *entry = &args[idx];

                switch(Z_TYPE_P(entry)) {
                case IS_STRING:
                    if(methodType == "char*" || methodType == "char**") {
                        stack[sidx].s_voidp = Z_STRVAL_P(entry);
                    }
                    else if (methodType == "char" && ZSTR_LEN(Z_STR_P(entry)) == 1) {
                        stack[sidx].s_char = Z_STRVAL_P(entry)[0];
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
                                bavala[sidx] = QByteArray(Z_STRVAL_P(entry));
                                stack[sidx].s_voidp = reinterpret_cast<void*>(&bavala[sidx]);
                                //stack[sidx].s_voidp = &QByteArray(Z_STRVAL_P(entry));
                            }
                            else {
                                stack[sidx].s_bytearray = QByteArray(Z_STRVAL_P(entry));
                            }
                        }
                        else {
                            if(isref) {
                                bavala[sidx] = QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
                                stack[sidx].s_voidp = reinterpret_cast<void*>(&bavala[sidx]);
                                //stack[sidx].s_voidp = &QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
                            }
                            else {
                                stack[sidx].s_bytearray = QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
                            }
                        }
                    }
                    else right = false;
                    break;

                case IS_LONG:
                    if (methodType == "char" && Z_LVAL_P(entry) >= 0 && Z_LVAL_P(entry) <= 9) {
                        stack[sidx].s_char = Z_LVAL_P(entry);
                    }
                    else if(methodType == "long") {
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
                        stack[sidx].s_enum = qint64(Z_LVAL_P(entry));
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
                        stack[sidx].s_enum = qint64(Z_LVAL_P(entry));
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
                    else if(methodType == "QStringList") {
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
                        stack[sidx].s_voidp = reinterpret_cast<void*>(&slvala[sidx]);
                    }
                    else right = false;
                    // argsTypes += argsTypes.length()
                    //         ? ",Array" : "Array";
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

                            stack[sidx].s_enum = epqobject->enumVal;
                        }
                        else if(object != Q_NULLPTR && object->plastiq_data() != Q_NULLPTR) {
                            PQDBGLPUP(QString("arg object: %1").arg(epqobject->object->plastiq_metaObject()->className()));

                            if (className == "QString") {
                                if (methodType == "QVariant") {
                                    stack[sidx].s_variant = *(reinterpret_cast<QString*>(object->plastiq_data()));
                                }
                                else {
                                    stack[sidx].s_string = *(reinterpret_cast<QString*>(object->plastiq_data()));
                                }
                                right = true;
                            }
                            else if (className == "QByteArray") {
                                if (methodType == "QVariant") {
                                    stack[sidx].s_variant = *(reinterpret_cast<QByteArray*>(object->plastiq_data()));
                                }
                                else {
                                    stack[sidx].s_bytearray = *(reinterpret_cast<QByteArray*>(object->plastiq_data()));
                                }
                                right = true;
                            }
                            else if (className == "QVariant" && methodType == "QVariant") {
                                stack[sidx].s_variant = *(reinterpret_cast<QVariant*>(object->plastiq_data()));
                                right = true;
                            }
                            else {
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
                                            cancast = true;
                                            break;
                                        }
                                        else {
                                            objectClassName = metaObject->className();
                                            foreach(PlastiQMetaObject* inherit, *metaObject->d.inherits) {
                                                PQDBGLPUP(QString("downcast %1 to %2").arg(objectClassName.constData()).arg(inherit->className()));
                                                objectClassName = inherit->className();

                                                if(fMethodType == QString(objectClassName).append("*")
                                                        || fMethodType == QString(objectClassName).prepend("const ").append("*")) {
                                                    // stack[sidx].s_voidp = epqobject->object->plastiq_data();

                                                    PMOGStack downCastStack = new PMOGStackItem[2];
                                                    downCastStack[1].s_bytearray = objectClassName;
                                                    metaObject->d.static_metacall(epqobject->object, PlastiQMetaObject::DownCast, -1, downCastStack);

                                                    stack[sidx].s_voidp = downCastStack[0].s_voidp;

                                                    delete [] downCastStack;

                                                    cancast = true;
                                                    break;
                                                }
                                            }
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
                                    else continue;
                                } while((metaObject = metaObject->d.superdata) && !cancast);

                                if(cancast) {
                                    PlastiQ::ObjectType objectType = *(object->plastiq_metaObject()->d.objectType);

                                    switch(objectType) {
                                    case PlastiQ::IsQtObject:
                                    case PlastiQ::IsQObject:
                                    case PlastiQ::IsQWidget:
                                    case PlastiQ::IsQWindow:
                                    case PlastiQ::IsQtItem: {
                                        bool haveParent = object->plastiq_haveParent();
                                        tciList << pq_tmp_call_info { epqobject, entry, haveParent };
                                    } break;

                                    default: ;
                                    }
                                }

                                right = cancast;
                            }
                        }
                        else right = false;
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
            signature = QString("%1(%2)").arg(className.constData()).arg(QString(argsTypes).replace(" ", "")).toUtf8();
            PQDBGLPUP(QString("generated signature: %1").arg(signature.constData()));
            objectFactory()->createPlastiQObject(className, signature, getThis(), isWrapper, stack);
        }
        else {
            php_error(E_ERROR, "Cannot create object: wrong param types");
        }
    }

    foreach(pq_tmp_call_info tci, tciList) {
        bool haveParentBefore = tci.haveParent;
        bool haveParentAfter = tci.pqo->object->plastiq_haveParent();
        const PlastiQ::ObjectType objectType = *(tci.pqo->object->plastiq_metaObject()->d.objectType);

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

    delete [] stack;

    delete [] strvala;
    delete [] bavala;
    delete [] slvala;
    delete [] lvala;
    delete [] ivala;
    delete [] bvala;
    delete [] dvala;
    delete [] fvala;

    efree(args);

    PQDBG_LVL_DONE_LPUP();
}

void PHPQt5::zim_plastiq___call(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg("0")
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    char* method;
    int method_len;
    zval *args;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &method, &method_len, &args) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    PQDBGLPUP(QString("call: %1").arg(method));

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(getThis()));

    zval retVal = plastiqCall(pqobject, QByteArray(method), Z_ARRVAL_P(args)->nNumOfElements, args);

    if(Z_TYPE(retVal) != IS_UNDEF) {
        RETVAL_ZVAL(&retVal, 1, 0);
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_plastiq___callStatic(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char* method;
    int method_len;
    zval *args;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &method, &method_len, &args) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    zend_class_entry* ce = Q_NULLPTR;

    void *TSRMLS_CACHE = Q_NULLPTR;
    TSRMLS_CACHE_UPDATE();

#if PHP_VERSION_ID < 70100
    if (execute_data->called_scope) {
        ce = execute_data->called_scope;
    } else if (EG(scope)) {
        ce = EG(scope);
    }
#else
    ce = zend_get_executed_scope();
#endif

    QByteArray objectClassName(ce->name->val);
    int argc = Z_ARRVAL_P(args)->nNumOfElements;

    PQDBGLPUP(QString("static call: %1::%2").arg(objectClassName.constData()).arg(method));

    QByteArray className;
    PlastiQMetaObject metaObject;
    do {
        if(objectFactory()->havePlastiQMetaObject(ce->name->val)) {
            className = QByteArray(ce->name->val);
            metaObject = objectFactory()->getMetaObject(className);
            break;
        }
    } while(ce = ce->parent);

    PQDBGLPUP(QString("static call: %1::%2").arg(className.constData()).arg(method));

    zval retVal = plastiqCall(Q_NULLPTR, QByteArray(method), argc, args, &metaObject);

    if(Z_TYPE(retVal) != IS_UNDEF) {
        RETVAL_ZVAL(&retVal, 1, 0);
    }

    PQDBG_LVL_DONE_LPUP();
}

void PHPQt5::zim_plastiq___get(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char* propertyName;
    int len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &propertyName, &len) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    PQDBGLPUP(QString("%1::%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg(propertyName)
              .arg(Z_OBJ_HANDLE_P(getThis())));

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(getThis()));
    if(pqobject->isValid) {
        const PlastiQMetaObject *metaObject = pqobject->object->plastiq_metaObject();

        zval retVal;
        bool exists = false;
        bool haveGetter = false;

        do {
            PlastiQProperty property = metaObject->d.pq_properties->value(propertyName);
            if(property.name.length()) {
                exists = true;

                if(property.getter.length()) {
                    zval argv;
                    array_init(&argv);

                    haveGetter = true;
                    retVal = plastiqCall(pqobject, property.getter, 0, &argv);

                    //zval_dtor(&argv); // Crash (!?)
                    Z_DELREF(argv); // instead dtor
                    break;
                }
            }
        } while(metaObject = metaObject->d.superdata);

        if(exists) {
            if(haveGetter) {
                RETVAL_ZVAL(&retVal, 1, 0);
            }
            else {
                php_error(E_NOTICE, QString("Cannot get value from write only property: %1::%2")
                          .arg(Z_OBJCE_NAME_P(getThis()))
                          .arg(propertyName).toUtf8().constData());

                PQDBG_LVL_DONE();
                RETURN_NULL();
            }
        }
        else {
            if(pqobject->userProperties == Q_NULLPTR) {
                pqobject->userProperties = new QHash<QByteArray,zval>();
            }

            if(pqobject->userProperties->contains(propertyName)) {
                zval rval = pqobject->userProperties->value(propertyName);
                RETVAL_ZVAL(&rval, 1, 0);
            }
            else {
                php_error(E_NOTICE, QString("Undefined property: %1::%2")
                          .arg(Z_OBJCE_NAME_P(getThis()))
                          .arg(propertyName).toUtf8().constData());

                PQDBG_LVL_DONE();
                RETURN_NULL();
            }
        }
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_plastiq___set(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg("0")
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    RETVAL_NULL();

    char* propertyName;
    int len;
    zval *pzval;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &propertyName, &len, &pzval) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    PQDBGLPUP(QString("propertyName: %1").arg(propertyName));

    QString signalName(propertyName);
    if(signalName.mid(0, 2) == "on"
            && signalName.at(2).isUpper()
            && Z_TYPE_P(pzval) == IS_OBJECT) {
        PQDBGLPUP("create closure connection...");
        if(plastiqConnect(getThis(), signalName.toUtf8(), pzval, ZEND_INVOKE_FUNC_NAME, true)) {
            PQDBG_LVL_DONE();
            return;
        }
        else {
            PQDBG_LVL_DONE();
            return;
        }
    }

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(getThis()));
    const PlastiQMetaObject *metaObject = pqobject->object->plastiq_metaObject();

    zval argv;
    array_init(&argv);
    add_next_index_zval(&argv, pzval);
    bool exists = false;
    bool haveSetter = false;

    do {
        PlastiQProperty property = metaObject->d.pq_properties->value(propertyName);
        if(property.name.length()) {
            exists = true;

            if(property.setter.length()) {
                haveSetter = true;
                plastiqCall(pqobject, property.setter, 1, &argv);

                //zval_dtor(&argv); // Crash (!?)

                if(Z_REFCOUNTED(argv))
                    Z_DELREF(argv); // instead dtor

                break;
            }
        }
    } while(metaObject = metaObject->d.superdata);

    if(exists) {
        if(!haveSetter) {
            php_error(E_NOTICE, QString("Cannot set value to read only property: %1::%2")
                      .arg(Z_OBJCE_NAME_P(getThis()))
                      .arg(propertyName).toUtf8().constData());
        }
    }
    else {
        PQDBGLPUP(QString("set user property: %1").arg(propertyName));

        if(pqobject->userProperties == Q_NULLPTR) {
            pqobject->userProperties = new QHash<QByteArray,zval>();
        }

        zval propzval;
        ZVAL_COPY_VALUE(&propzval, pzval);
        if(Z_REFCOUNTED(propzval))
            Z_ADDREF(propzval);

        pqobject->userProperties->insert(propertyName, propzval);
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_plastiq___toString(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(getThis()));
    QString ret;
    QString objectName;

    const PlastiQMetaObject *metaObject = pqobject->object->plastiq_metaObject();
    const PlastiQ::ObjectType objectType = pqobject->object->plastiq_objectType();

    if (objectType == PlastiQ::String || metaObject->className() == "QString") {
        QString str = *(reinterpret_cast<QString*>(pqobject->object->plastiq_data()));
        PQDBG_LVL_DONE();
        RETURN_STRING(str.toUtf8().constData());
    }
    else if (objectType == PlastiQ::ByteArray || metaObject->className() == "QByteArray") {
        QByteArray ba = *(reinterpret_cast<QByteArray*>(pqobject->object->plastiq_data()));
        PQDBG_LVL_DONE();
        RETURN_STRING(ba.constData());
    }

    if(objectType == PlastiQ::IsQObject
            || objectType == PlastiQ::IsQWidget
            || objectType == PlastiQ::IsQWindow) {
        int mid = metaObject->methodId("objectName()");
        if(mid >= 0) {
            PMOGStack stack = new PMOGStackItem[1];
            metaObject->d.static_metacall(pqobject->object, PlastiQMetaObject::InvokeMethod, mid, stack);
            objectName = stack[0].s_string;

            delete [] stack;
        }
    }

    QString typeName;
    switch(objectType) {
    case PlastiQ::IsObject: typeName = "IsObject"; break;
    case PlastiQ::IsQObject: typeName = "IsQObject"; break;
    case PlastiQ::IsQWidget: typeName = "IsQWidget"; break;
    case PlastiQ::IsQWindow: typeName = "IsQWindow"; break;
    case PlastiQ::IsQtObject: typeName = "IsQtObject"; break;
    case PlastiQ::IsNamespace: typeName = "IsNamespace"; break;
    case PlastiQ::IsQEvent: typeName = "IsQEvent"; break;
    }

    ret = QString("[%1:%2:%3:\"%4\"]")
            .arg(Z_OBJCE_NAME_P(getThis()))
            .arg(typeName)
            .arg(reinterpret_cast<quint64>(pqobject->object->plastiq_data()))
            .arg(objectName);

    PQDBG_LVL_DONE();
    RETURN_STRING(ret.toUtf8().constData());
}


void PHPQt5::zim_plastiq_free(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zend_update_property_long(Z_OBJCE_P(getThis()), getThis(), "__pq_uid", sizeof("__pq_uid")-1, 0);
    objectFactory()->freeObject(Z_OBJ_P(getThis()));

    zend_class_entry *ce = objectFactory()->getClassEntry("PlastiQDestroyedObject");
    Z_OBJ_P(getThis())->ce = ce;

    PQDBG_LVL_DONE();
    RETURN_NULL();
}

void PHPQt5::zim_plastiq_connect(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *sender;
    char *signal;
    int signal_len;
    zval *receiver;
    char *slot;
    int slot_len;

    int argc = ZEND_NUM_ARGS();

    switch(argc) {
    case 2: {
        if(zend_parse_parameters(argc, "so", &signal, &signal_len, &receiver) == FAILURE) {
            PQDBG_LVL_DONE();
            RETURN_FALSE;
        }
        sender = getThis();

        bool ok = plastiqConnect(sender, QString(signal), receiver, QString(ZEND_INVOKE_FUNC_NAME), false);
        PQDBG_LVL_DONE();
        RETURN_BOOL(ok);
    } break;

    case 3: {
        if(zend_parse_parameters(argc, "sos", &signal, &signal_len, &receiver, &slot, &slot_len) == FAILURE) {
            PQDBG_LVL_DONE();
            RETURN_FALSE;
        }
        sender = getThis();

        bool ok = plastiqConnect(sender, QString(signal), receiver, QString(slot), false);
        PQDBG_LVL_DONE();
        RETURN_BOOL(ok);
    } break;

    case 4:
        zif_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        break;

    default:
#if (PHP_VERSION_ID < 70101)
        zend_wrong_paramers_count_error(argc, 2, 4);
#else
        zend_wrong_parameters_count_error(argc, 2, 4);
#endif
    }

    PQDBG_LVL_DONE();
}

void PHPQt5::zim_plastiq_emit(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    RETVAL_NULL();

    char *signal_signature;
    int signal_signature_len;
    zval *argv;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "sa", &signal_signature, &signal_signature_len, &argv) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    PQDBGLPUP(QString("%1::emit -> %2").arg(Z_OBJCE_NAME_P(getThis())).arg(signal_signature));

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(getThis()));

    PlastiQ::ObjectType objectType = pqobject->object->plastiq_objectType();
    if(objectType != PlastiQ::IsQObject
            && objectType != PlastiQ::IsQWidget
            && objectType != PlastiQ::IsQWindow) {
        php_error(E_ERROR, "Non QObject object cannot emit signals");

        PQDBG_LVL_DONE();
        return;
    }

    QByteArray normalizedSignature = QMetaObject::normalizedSignature(signal_signature);
    ConnectionHash *connections = pqobject->connections->value(normalizedSignature, Q_NULLPTR );

    if(connections != Q_NULLPTR) {
        int argc = Z_ARRVAL_P(argv)->nNumOfElements + 1;

        zval *params = new zval[argc];
        ZVAL_COPY_VALUE(&params[0], getThis());

        Z_ADDREF_P(getThis());

        zval *entry;
        ulong argnum;
        zend_string *key;

        PQDBGLPUP("ZEND_HASH_FOREACH_KEY_VAL");
        ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(argv), argnum, key, entry) {
            PQDBGLPUP("ZVAL_COPY_VALUE");
            ZVAL_COPY_VALUE(&params[argnum+1], entry);

            if(Z_REFCOUNTED_P(entry)) {
                Z_ADDREF_P(entry);
            }
        } ZEND_HASH_FOREACH_END();

        QHashIterator<QByteArray,ConnectionData> iter(*connections);

        while(iter.hasNext()) {
            iter.next();

            const ConnectionData &c = iter.value();
            activateConnection(pqobject, normalizedSignature.constData(), c.receiver, c.slot.constData(), argc, params, false);
            // FIXME: нет проверки на кол-во передаваемых аргументов
        }

        for(int i = 0; i < argc; i++) {
            zval_dtor(&params[i]);
        }

        delete [] params;
        params = 0;
    }
    else {
        php_error(E_ERROR,
                  "%s::signals not constains signal <b>%s</b>",
                  Z_OBJCE_NAME_P(getThis()),
                  normalizedSignature.constData());
    }

    PQDBG_LVL_DONE();
}
