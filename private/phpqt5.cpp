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

#include "phpqt5.h"
QByteArray PHPQt5::W_CP = "UTF-8";

QByteArray PHPQt5::toW(const QByteArray &ba)
{
    // QTextCodec *codec = QTextCodec::codecForName(W_CP);
    // return codec->fromUnicode(ba);
    return ba;
}

QByteArray PHPQt5::toUTF8(const QByteArray &ba)
{
    // QTextCodec *codec = QTextCodec::codecForName(W_CP);
    // return codec->toUnicode(ba).toUtf8();
    return ba;
}

zend_object *PHPQt5::pqobject_create(zend_class_entry *ce)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(ce->name->val);
#endif

    void *TSRMLS_CACHE = Q_NULLPTR;
    TSRMLS_CACHE_UPDATE();

    PQObjectWrapper *pqobject = (PQObjectWrapper*)
            ecalloc(1,
                    sizeof(PQObjectWrapper) +
                    zend_object_properties_size(ce));

    zend_object_std_init(&pqobject->zo, ce);
    object_properties_init(&pqobject->zo, ce);

    pqobject->zo.handlers = &pqobject_handlers;
    pqobject->ctx = TSRMLS_CACHE;

    PQDBG_LVL_DONE();
    return &pqobject->zo;
}

void PHPQt5::pqobject_object_free(zend_object *zobject) {
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    objectFactory()->freeObject(zobject);
    zend_object_std_dtor(zobject);

    PQDBG_LVL_DONE();
}

void PHPQt5::pqobject_object_dtor(zend_object *zobject) {
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zend_objects_destroy_object(zobject);

    PQDBG_LVL_DONE();
}

zend_object *PHPQt5::pqenum_create(zend_class_entry *ce)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(ce->name->val);
#endif

    PQEnumWrapper *pqenum = (PQEnumWrapper*)
            ecalloc(1,
                    sizeof(PQEnumWrapper) +
                    zend_object_properties_size(ce));

    zend_object_std_init(&pqenum->zo, ce);
    object_properties_init(&pqenum->zo, ce);

    pqenum->zo.handlers = &pqenum_handlers;

    PQDBG_LVL_DONE();
    return &pqenum->zo;
}

void PHPQt5::pqenum_object_free(zend_object *zenum) {
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zend_object_std_dtor(zenum);

    PQDBG_LVL_DONE();
}

void PHPQt5::pqenum_object_dtor(zend_object *zenum) {
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zend_objects_destroy_object(zenum);

    PQDBG_LVL_DONE();
}

/** BUG: memory leak with zend_get_parameters_array_ex(); efree(args) - crash app */
/* int PHPQt5::pqobject_call_method(zend_string *method, zend_object *object, INTERNAL_FUNCTION_PARAMETERS)
{
    Q_UNUSED(object);

    #ifdef PQDEBUG
        PQDBG_LVL_START(__FUNCTION__);
        PQDBGLPUP(QString("%1->%2").arg(Z_OBJCE_P(getThis())->name->val).arg(method->val));
    #endif

    QObject *qo = objectFactory()->getQObject(getThis() PQDBG_LVL_CC);

    const int argc = ZEND_NUM_ARGS();
    zval *args;

    if(argc) {
        args = (zval *)safe_emalloc(sizeof(zval), argc, 0);
        zend_get_parameters_array_ex(argc, args); // MEMORY LEAK
    }

    //if(zend_get_parameters_array_ex(argc, args) == FAILURE)
    //{
    //    efree(args);
    //    zend_wrong_param_count();
    //    return FAILURE;
    //}

    QList<pq_call_qo_entry> arg_qos;
    bool this_before_have_parent = qo->parent() ? true : false;
    arg_qos << pq_call_qo_entry { qo, getThis(), this_before_have_parent };

    // Список "опасных" методов: moveToThread, getChildObjects
    // В версии 0.3.* вызывали падение при обращении напрямую,
    // В новых версиях не тестировалось :-)
    // connect() - вспомогательный

    /*
     * Вызов метода moveToThread( ... )
     *
    if(method->val == QString("moveToThread")) {
        if(argc != 1) {
            // if(args) { efree(args); }
            zend_wrong_param_count();
            PQDBG_LVL_DONE();
            return FAILURE;
        }

        RETVAL_BOOL( pq_move_to_thread(qo, &args[0] PQDBG_LVL_CC) );
        PQDBG_LVL_DONE();
        return SUCCESS;
    }

    /*
     * Вызов метода connect( ... )
     *
    else if(method->val == QString("connect")) {
        bool ok = false;
        switch(argc) {
        case 4:
            ok = pq_connect(&args[0], &args[1], &args[2], &args[3], false PQDBG_LVL_CC);
            break;

        case 3:
            ok = pq_connect(getThis(), &args[0], &args[1], &args[2], false PQDBG_LVL_CC);
            break;

        default:
            // if(args) { efree(args); }
            zend_wrong_param_count();

            RETVAL_BOOL(false);
            PQDBG_LVL_DONE();
            return FAILURE;
        }

        RETVAL_BOOL(ok);
        PQDBG_LVL_DONE();
        return SUCCESS;
    }

    /*
     * Вызов иного метода
     *
    else {
        QVariantList vargs;

        for(int i = 0; i < argc; i++) {
            switch(Z_TYPE(args[i])) {
            case IS_TRUE:
                vargs << QVariant( true );
                break;

            case IS_FALSE:
                vargs << QVariant( false );
                break;

            case IS_STRING: {
                // определение типа данных
                QString str = QString::fromLatin1(Z_STRVAL(args[i]));

                if(str.length() == Z_STR(args[i])->len)
                    vargs << QString::fromUtf8(Z_STRVAL(args[i])); // возврат к utf8
                else
                    vargs << QByteArray::fromRawData(Z_STRVAL(args[i]), Z_STR(args[i])->len);
                break;
            }

            case IS_LONG:
                vargs << QVariant( (int) Z_LVAL(args[i]) );
                break;

            case IS_DOUBLE:
                vargs << QVariant( Z_DVAL(args[i]) );
                break;

            case IS_OBJECT: {
                if(pq_test_ce(&args[i] PQDBG_LVL_CC)) {
                    QObject *arg_qo = objectFactory()->getQObject(&args[i] PQDBG_LVL_CC);
                    vargs << QVariant::fromValue<QObject*>( arg_qo );

                    bool before_have_parent = arg_qo->parent() ? true : false;
                    arg_qos << pq_call_qo_entry { arg_qo, &args[i], before_have_parent };
                }
                else {
                    zend_function *closure = zend_get_closure_invoke_method(Z_OBJ(args[i]));

                    Z_ADDREF(args[i]);
                    if(closure) {
                        PQClosure pqc = { reinterpret_cast<void*>( Z_OBJ(args[i]) ) };
                        vargs << QVariant::fromValue<PQClosure>( pqc );
                    }
                    else php_error(E_ERROR, QString("Unknown type of argument %1").arg(i).toUtf8().constData());
                }

                break;
            }

            case IS_ARRAY:
                vargs << pq_ht_to_qstringlist(&args[i] PQDBG_LVL_CC);
                break;

            case IS_NULL: {
                pq_nullptr nptr;
                vargs << QVariant::fromValue<pq_nullptr>(nptr);
                break;
            }

            default:
                php_error(E_ERROR, QString("Unknown type of argument %1").arg(i).toUtf8().constData());
            }
        }

        QVariant retVal = pq_call(qo, method->val, vargs PQDBG_LVL_CC);
        pq_return_qvariant(retVal, INTERNAL_FUNCTION_PARAM_PASSTHRU PQDBG_LVL_CC);

    }

    foreach(pq_call_qo_entry cqe, arg_qos) {
        bool after_have_parent = cqe.qo->parent();
        bool before_have_parent = cqe.before_have_parent;

        // не было родителя и появился
        if(!before_have_parent && after_have_parent) {
            Z_ADDREF_P(cqe.zv);
        }
        // был родитель и не стало
        else if(before_have_parent && !after_have_parent) {
            Z_DELREF_P(cqe.zv);
        }
    }

    // if(argc) { efree(args); } // CRASH

    PQDBG_LVL_DONE();
    return SUCCESS;
}


zend_function *PHPQt5::pqobject_get_method(zend_object **zobject, zend_string *method, const zval *key)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("%1->%2").arg((*zobject)->ce->name->val).arg(method->val));
#endif

    zend_internal_function f, *fptr = NULL;
    zend_function *func;

    func = zend_get_std_object_handlers()->get_method(zobject, method, key);
    if(!func) {
        f.type = ZEND_OVERLOADED_FUNCTION;
        f.num_args = 0;
        f.arg_info = NULL;
        f.scope = (*zobject)->ce;
        f.fn_flags = ZEND_ACC_CALL_VIA_HANDLER;
        f.function_name = zend_string_copy(method);
        zend_set_function_arg_flags(((zend_function*)&f));
        fptr = &f;

        func = (zend_function*) emalloc(sizeof(*fptr));
        memcpy(func, fptr, sizeof(*fptr));
    }

    PQDBG_LVL_DONE();

    return func;
}

zval *PHPQt5::pqobject_get_property_ptr_ptr(zval *object,
                                            zval *member,
                                            int type,
                                            void **cache_slot)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *retval;
    ZVAL_NULL(retval);
    PQDBG_LVL_DONE();
    return retval;
}

HashTable *PHPQt5::pqobject_get_properties(zval *object)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif


    return NULL;
}

zval *PHPQt5::pqobject_read_property(zval *object,
                                     zval *member,
                                     int type,
                                     void **cache_slot,
                                     zval *rv)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *retval;
    ZVAL_NULL(retval);
    PQDBG_LVL_DONE();
    return retval;
}

int PHPQt5::pqobject_has_property(zval *object,
                                  zval *member,
                                  int type,
                                  void **cache_slot)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    int retval = 0;
    PQDBG_LVL_DONE();
    return retval;
}
*/

