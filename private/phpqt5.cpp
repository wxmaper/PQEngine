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

//#pragma comment(lib, "php7ts.lib")

QByteArray PHPQt5::W_CP = "UTF-8";
PHPQt5Connection *PHPQt5::phpqt5Connections;

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

    PQObjectWrapper *pqobject = (PQObjectWrapper*) ecalloc(1,
             sizeof(PQObjectWrapper) +
             zend_object_properties_size(ce));

    zend_object_std_init(&pqobject->zo, ce);

    object_properties_init(&pqobject->zo, ce);
    pqobject->zo.handlers = &pqobject_handlers;

    PQDBGLPUP("done");
    PQDBG_LVL_DONE();
    return &pqobject->zo;
}

void PHPQt5::pqobject_free_storage(zend_object *object) {
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif
    PQDBG_LVL_DONE();
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
