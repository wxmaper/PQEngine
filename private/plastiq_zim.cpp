#include "pqclasses.h"
#include "plastiqobject.h"
#include "plastiqmethod.h"

#include "phpqt5.h"

void PHPQt5::zim_plastiq___construct(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1:%2 - z:%3")
              .arg(Z_OBJCE_NAME_P(getThis()))
              .arg("0")
              .arg(Z_OBJ_HANDLE_P(getThis())));
#endif

    const int argc = ZEND_NUM_ARGS();
    zval *args = (zval *) safe_emalloc(argc, sizeof(zval), 0);

    if(zend_get_parameters_array_ex(argc, args) == FAILURE)
    {
        efree(args);
        WRONG_PARAM_COUNT;
    }

    QList<int> types;
    QString argsTypes = "";
    zval *entry;

    PMOGStack stack = new PMOGStackItem[10];

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

        default:
            php_error(E_ERROR, QString("Unknown type of argument %1").arg(i).toUtf8().constData());
        }
    }

    zend_class_entry *ce = Z_OBJCE_P(getThis());
    QByteArray className;
    do {
        if(objectFactory()->havePlastiQMetaObject(ce->name->val PQDBG_LVL_CC)) {
            className = QByteArray(ce->name->val);
            break;
        }
    } while(ce = ce->parent);

    PlastiQMetaObject metaObject = objectFactory()->getMetaObject(className PQDBG_LVL_CC);
    QByteArray signature = QString("%1(%2)").arg(className.constData()).arg(argsTypes).toUtf8();

    int mid = metaObject.constructorId(signature);
    if(mid >= 0) {
        PQDBGLPUP(QString("fast call constructorId: %1").arg(mid));
        objectFactory()->createPlastiQObject(className, signature, getThis(), stack PQDBG_LVL_CC);
    }
    else {
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

                QString methodType = candidates.at(cid).argTypes.at(idx);
                argsTypes += argsTypes.length() ? "," + methodType : methodType;
                int sidx = idx + 1;

                PQDBGLPUP(QString("idx: %1; methodType: %2")
                          .arg(idx)
                          .arg(methodType));

                zval *entry = &args[idx];

                switch(Z_TYPE_P(entry)) {
                case IS_STRING:
                    if(methodType == "char*" || methodType == "char**") {
                        stack[sidx].s_voidp = Z_STRVAL_P(entry);
                        qDebug() << "add char" << methodType;
                    }
                    else if(methodType == "QString") {
                        stack[sidx].s_string = QString::fromUtf8(Z_STRVAL_P(entry));
                        qDebug() << "add QString" << methodType;
                    }
                    else if(methodType == "QByteArray") {
                        QString str = QString::fromLatin1(Z_STRVAL_P(entry));

                        if(str.length() == Z_STR_P(entry)->len) {
                            stack[sidx].s_bytearray = QByteArray(Z_STRVAL_P(entry));
                            qDebug() << "add QByteArray" << methodType;
                        }
                        else {
                            stack[sidx].s_bytearray = QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
                            qDebug() << "add QByteArray from raw data" << methodType;
                        }
                    }
                    else right = false;
                    break;

                case IS_LONG:
                    if(methodType == "long") {
                        stack[sidx].s_long = Z_LVAL_P(entry);
                        qDebug() << "add long" << methodType;
                    }
                    else if(methodType == "int") {
                        stack[sidx].s_int = Z_LVAL_P(entry);
                        qDebug() << "add int" << methodType;
                    }
                    else if(methodType == "int&") {
                        stack[sidx].s_voidp = &Z_LVAL_P(entry);
                        argsTypes = argsTypes.left(argsTypes.length() - 1);
                        qDebug() << "add int&" << methodType;
                    }
                    else right = false;
                    break;

                case IS_TRUE:
                    if(methodType == "bool") {
                        stack[sidx].s_bool = true;
                        qDebug() << "add bool" << methodType;
                    }
                    else right = false;
                    break;

                case IS_FALSE:
                    if(methodType == "bool") {
                        stack[sidx].s_bool = false;
                        qDebug() << "add bool" << methodType;
                    }
                    else right = false;
                    break;

                case IS_DOUBLE:
                    if(methodType == "double") {
                        stack[sidx].s_double = Z_DVAL_P(entry);
                        qDebug() << "add double" << methodType;
                    }
                    if(methodType == "float") {
                        stack[sidx].s_float = (float) Z_DVAL_P(entry);
                        qDebug() << "add float" << methodType;
                    }
                    else right = false;
                    break;

                case IS_OBJECT: {
                    qDebug() << "OBJECT" << methodType;
                }

                default: right = false;
                }
            }

            if(right) {
                mid = candidates.at(cid).idx;
                break;
            }
        }

        signature = QString("%1(%2)").arg(className.constData()).arg(QString(argsTypes).replace(" ", "")).toUtf8();
        objectFactory()->createPlastiQObject(className, signature, getThis(), stack PQDBG_LVL_CC);
    }

    delete [] stack;
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

    /*
     * Парсим параметры и записываем в QVariantList values
     */
    int argc = Z_ARRVAL_P(args)->nNumOfElements;

    QList<int> types;
    QString argsTypes = "";
    PMOGStack stack = new PMOGStackItem[10];

    zval *entry;
    ulong argnum;
    zend_string *key;


    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(args), argnum, key, entry) {
        types << Z_TYPE_P(entry);
        int sidx = argnum + 1;

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

        default:
            php_error(E_ERROR,
                      QString("Unknown type of argument %1 (%2)")
                      .arg(argnum)
                      .arg(zend_zval_type_name(getThis()))
                      .toUtf8().constData());
        }

    } ZEND_HASH_FOREACH_END();

    // Создаем виджет QWidget
    // PMOGClass pmogClass = PMOG::findClass(ce->name->val);
    // PMOGMethod pmogMethod = pmogClass.findMethod(QString("%1(%2)").arg(ce->name->val).arg(argsTypes).toUtf8());

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(getThis()));

    PQDBGLPUP(QString("called object: %1").arg(pqobject->object->metaObject()->className()));

    int mid = pqobject->object->metaObject()->methodId( QString("%1(%2)").arg(method).arg(argsTypes).toUtf8() );
    if(mid >= 0) {
        PQDBGLPUP(QString("fast call methodId: %1").arg(mid));
        PlastiQMetaObject::invokeMethod(pqobject->object,
                                        QString("%1(%2)").arg(method).arg(QString(argsTypes).replace(" ", "")).toUtf8(),
                                        stack);
    }
    else {
        QList<PlastiQCandidateMethod> candidates = pqobject->object->metaObject()->candidates(QByteArray(method), argc, PlastiQMethod::Method);
        PQDBGLPUP(QString("general call: candidates size: %1").arg(candidates.size()));

        bool right = true;
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

                QString methodType = candidates.at(cid).argTypes.at(idx);
                argsTypes += argsTypes.length() ? "," + methodType : methodType;
                int sidx = idx + 1;

                PQDBGLPUP(QString("idx: %1; methodType: %2")
                          .arg(idx)
                          .arg(methodType));

                zval *entry = &Z_ARRVAL_P(args)->arData[idx].val;

                switch(Z_TYPE_P(entry)) {
                case IS_STRING:
                    if(methodType == "char*" || methodType == "char**") {
                        stack[sidx].s_voidp = Z_STRVAL_P(entry);
                        qDebug() << "add char" << methodType;
                    }
                    else if(methodType == "QString") {
                        stack[sidx].s_string = QString::fromUtf8(Z_STRVAL_P(entry));
                        qDebug() << "add QString" << methodType;
                    }
                    else if(methodType == "QByteArray") {
                        QString str = QString::fromLatin1(Z_STRVAL_P(entry));

                        if(str.length() == Z_STR_P(entry)->len) {
                            stack[sidx].s_bytearray = QByteArray(Z_STRVAL_P(entry));
                            qDebug() << "add QByteArray" << methodType;
                        }
                        else {
                            stack[sidx].s_bytearray = QByteArray::fromRawData(Z_STRVAL_P(entry), Z_STR_P(entry)->len);
                            qDebug() << "add QByteArray from raw data" << methodType;
                        }
                    }
                    else right = false;
                    break;

                case IS_LONG:
                    if(methodType == "long") {
                        stack[sidx].s_long = Z_LVAL_P(entry);
                        qDebug() << "add long" << methodType;
                    }
                    else if(methodType == "int") {
                        stack[sidx].s_int = Z_LVAL_P(entry);
                        qDebug() << "add int" << methodType;
                    }
                    else if(methodType == "int&") {
                        stack[sidx].s_voidp = &Z_LVAL_P(entry);
                        argsTypes = argsTypes.left(argsTypes.length() - 1);
                        qDebug() << "add int&" << methodType;
                    }
                    else right = false;
                    break;

                case IS_TRUE:
                    if(methodType == "bool") {
                        stack[sidx].s_bool = true;
                        qDebug() << "add bool" << methodType;
                    }
                    else right = false;
                    break;

                case IS_FALSE:
                    if(methodType == "bool") {
                        stack[sidx].s_bool = false;
                        qDebug() << "add bool" << methodType;
                    }
                    else right = false;
                    break;

                case IS_DOUBLE:
                    if(methodType == "double") {
                        stack[sidx].s_double = Z_DVAL_P(entry);
                        qDebug() << "add double" << methodType;
                    }
                    if(methodType == "float") {
                        stack[sidx].s_float = (float) Z_DVAL_P(entry);
                        qDebug() << "add float" << methodType;
                    }
                    else right = false;
                    break;

                case IS_OBJECT: {
                    qDebug() << "OBJECT" << methodType;
                }

                default: right = false;
                }
            }

            if(right) {
                mid = candidates.at(cid).idx;
                break;
            }
        }

        PlastiQMetaObject::invokeMethod(pqobject->object,
                                        QString("%1(%2)").arg(method).arg(QString(argsTypes).replace(" ", "")).toUtf8(),
                                        stack);
    }

    delete [] stack;

    PQDBG_LVL_DONE_LPUP();
}
