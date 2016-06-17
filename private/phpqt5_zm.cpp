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
#include "pqengine_private.h"

#include "php_streams.h"

zend_object_handlers PHPQt5::pqobject_handlers;

php_stream_ops php_stream_qrc_ops = {
    PHPQt5::php_qrc_write,
    PHPQt5::php_qrc_read,
    PHPQt5::php_qrc_close,
    PHPQt5::php_qrc_flush,
    "qrc",
    NULL,
    NULL,
    NULL,
    NULL
};

php_stream_wrapper_ops php_stream_qrc_wops = {
    PHPQt5::qrc_opener,
    NULL,
    NULL,
    NULL,
    NULL,
    "qrc wrapper",
    NULL,
    NULL,
    NULL,
    NULL
};

static php_stream_wrapper php_qrc_stream_wrapper = {
    &php_stream_qrc_wops,
    NULL,
    0
};

php_stream *PHPQt5::qrc_opener(php_stream_wrapper *wrapper,
                               const char *path,
                               const char *mode,
                               int options,
                               zend_string **opened_path,
                               php_stream_context *context)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(path);
#endif

    QString resourcePath = QString(path).mid(6);
    QByteArray qrc_data;

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("open resource");
    #endif

    // QString rFilePath = QString(":/%1/%2").arg(getCorename().constData()).arg(resourcePath);
    QString rFilePath = QString(":/%1").arg(resourcePath);

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP(QString("rFilePath: %1").arg(rFilePath));
    #endif

    QFile file(rFilePath);
    if(file.open(QIODevice::ReadOnly)) {
        qrc_data = file.readAll();
    }
    else {
        php_error(E_ERROR, QString("Resource not found: %1%2").arg(path).arg(PHP_EOL).toUtf8().constData());
        return nullptr;
    }

    QDataStream *s = new QDataStream(qrc_data);
    php_stream *stream = nullptr;
    struct qrc_stream_data *data;

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("emalloc qrc_stream_data");
    #endif

    data = (qrc_stream_data *) emalloc(sizeof(*data));
    data->qrc_file = s;
    data->stream = stream;

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("php_stream_alloc");
    #endif

    stream = php_stream_alloc(&php_stream_qrc_ops, data, NULL, mode);

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("done");
    #endif

    PQDBG_LVL_DONE();
    return stream;
}

size_t PHPQt5::php_qrc_write(php_stream *stream, const char *buf, size_t count)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBG_LVL_DONE();
#endif

    php_error(E_ERROR, QString("Unable to write to resource file!").toUtf8().constData());

    return 0;
}

int PHPQt5::php_qrc_flush(php_stream *stream)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBG_LVL_DONE();
#endif

    return 0;
}

int PHPQt5::php_qrc_close(php_stream *stream, int close_handle)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    struct qrc_stream_data *self = (struct qrc_stream_data *) stream->abstract;
    int ret = EOF;

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("close device");
    #endif

    self->qrc_file->device()->close();
    delete self->qrc_file;

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("free stream");
    #endif

    efree(self);

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("done");
    #endif

    PQDBG_LVL_DONE();
    return ret;
}

size_t PHPQt5::php_qrc_read(php_stream *stream, char *buf, size_t count)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    struct qrc_stream_data *self = (struct qrc_stream_data *) stream->abstract;

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("read device");
    #endif

    QDataStream *s = self->qrc_file;
    size_t read_bytes = s->device()->read(buf, count);

    stream->eof = 1;

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("done");
    #endif

    PQDBG_LVL_DONE();
    return read_bytes;
}

int PHPQt5::zm_startup_phpqt5(INIT_FUNC_ARGS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    qRegisterMetaType<zval>("zval");
    qRegisterMetaType<zval*>("zval*");

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("register stream wrapper");
    #endif
    php_register_url_stream_wrapper("qrc", &php_qrc_stream_wrapper);

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("register handlers");
    #endif
    memcpy(&pqobject_handlers,
           zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    pqobject_handlers.offset = XtOffsetOf(PQObjectWrapper, zo);

    pqobject_handlers.free_obj = pqobject_free_storage;
    pqobject_handlers.call_method = pqobject_call_method;
    pqobject_handlers.get_method = pqobject_get_method;
    pqobject_handlers.clone_obj = NULL;

   // pqobject_handlers.get_property_ptr_ptr = pqobject_get_property_ptr_ptr;

   // pqobject_handlers.get_properties = pqobject_get_properties;
   // pqobject_handlers.read_property	= pqobject_read_property;
   // pqobject_handlers.has_property	= pqobject_has_property;

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("register extensions");
    #endif
    PQEnginePrivate::pq_register_extensions(PQDBG_LVL_C);
    phpqt5Connections = new PHPQt5Connection(PQDBG_LVL_C);

    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
    PQDBGLPUP("done");
    #endif

    PQDBG_LVL_DONE();
    return SUCCESS;
}


