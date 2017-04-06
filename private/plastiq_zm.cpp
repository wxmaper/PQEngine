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
zend_object_handlers PHPQt5::pqenum_handlers;

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

    Q_UNUSED(wrapper)
    Q_UNUSED(options)
    Q_UNUSED(opened_path)
    Q_UNUSED(context)

    QString resourcePath = QString(path).mid(6); // 6 - length of "qrc://"
    QByteArray qrc_data;

    PQDBGLPUP("open resource");
    QString rFilePath = QString(":/%1").arg(resourcePath);

    PQDBGLPUP(QString("rFilePath: %1").arg(rFilePath));
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

    PQDBGLPUP("emalloc qrc_stream_data");
    data = (qrc_stream_data *) emalloc(sizeof(*data));
    data->qrc_file = s;
    data->stream = stream;

    PQDBGLPUP("php_stream_alloc");
    stream = php_stream_alloc(&php_stream_qrc_ops, data, NULL, mode);

    PQDBG_LVL_DONE();
    return stream;
}

size_t PHPQt5::php_qrc_write(php_stream *stream, const char *buf, size_t count)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    Q_UNUSED(stream)
    Q_UNUSED(buf)
    Q_UNUSED(count)

    php_error(E_ERROR, "Unable to write to resource file!");

    PQDBG_LVL_DONE();
    return 0;
}

int PHPQt5::php_qrc_flush(php_stream *stream)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    Q_UNUSED(stream)

    PQDBG_LVL_DONE();
    return 0;
}

int PHPQt5::php_qrc_close(php_stream *stream, int close_handle)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    Q_UNUSED(close_handle)

    struct qrc_stream_data *self = (struct qrc_stream_data *) stream->abstract;
    int ret = EOF;

    PQDBGLPUP("close device");
    self->qrc_file->device()->close();
    delete self->qrc_file;

    PQDBGLPUP("free stream");
    efree(self);

    PQDBG_LVL_DONE();
    return ret;
}

size_t PHPQt5::php_qrc_read(php_stream *stream, char *buf, size_t count)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    struct qrc_stream_data *self = (struct qrc_stream_data *) stream->abstract;

    PQDBGLPUP("read device");
    QDataStream *s = self->qrc_file;
    size_t read_bytes = s->device()->read(buf, count);

    stream->eof = 1;

    PQDBGLPUP("done");
    PQDBG_LVL_DONE();
    return read_bytes;
}

static void phpqt5_error_handler(int error_num, const char *error_filename, const uint error_lineno, const char *format, va_list args) {
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    QString error_type;

    switch(error_num) {
    case E_ERROR: error_type = "Error"; break;
    case E_WARNING: error_type = "Warning"; break;
    case E_CORE_ERROR: error_type = "Core Error"; break;
    case E_CORE_WARNING: error_type = "Core Warning"; break;
    case E_COMPILE_ERROR: error_type = "Compile Error"; break;
    case E_COMPILE_WARNING: error_type = "Compile Warning"; break;
    case E_USER_ERROR: error_type = "User Error"; break;
    case E_USER_WARNING: error_type = "User Warning"; break;
    case E_RECOVERABLE_ERROR: error_type = "Recoverable Error"; break;
    default: return;
    }

    char buffer[256];
    vsprintf(buffer, format, args);

    pq_pre(QString("<b>%1</b>: %2 in <b>%3</b> on line <b>%4</b>")
           .arg(error_type)
           .arg(buffer)
           .arg(error_filename)
           .arg(error_lineno),
           error_type);

    PQDBG_LVL_DONE();
}

int PHPQt5::zm_startup_phpqt5(INIT_FUNC_ARGS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    Q_UNUSED(module_number)
    Q_UNUSED(type)

    zend_error_cb = PHPQt5::plastiqErrorHandler; //

    qRegisterMetaType<zval>("zval");
    qRegisterMetaType<zval*>("zval*");

    PQDBGLPUP("register stream wrapper");
    php_register_url_stream_wrapper("qrc", &php_qrc_stream_wrapper);

    PQDBGLPUP("register handlers");
    memcpy(&pqobject_handlers,
           zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    pqobject_handlers.offset = XtOffsetOf(PQObjectWrapper, zo);

    pqobject_handlers.free_obj = pqobject_object_free;
    pqobject_handlers.dtor_obj = pqobject_object_dtor;
    pqobject_handlers.clone_obj = NULL;
    pqobject_handlers.compare_objects = pqobject_compare_objects;
    pqobject_handlers.compare = pqobject_compare;

    // QEnum
//    memcpy(&pqenum_handlers,
//           zend_get_std_object_handlers(),
//           sizeof(zend_object_handlers));

//    pqenum_handlers.offset = XtOffsetOf(PQEnumWrapper, zo);

//    pqenum_handlers.free_obj = pqenum_object_free;
//    pqenum_handlers.dtor_obj = pqenum_object_dtor;
//    pqenum_handlers.clone_obj = NULL;



    // pqobject_handlers.call_method = pqobject_call_method; // BUG: memory leak with zend_get_parameters_array_ex(); efree(args) - crash app
    // pqobject_handlers.get_method = pqobject_get_method;

    // pqobject_handlers.get_property_ptr_ptr = pqobject_get_property_ptr_ptr;

    // pqobject_handlers.get_properties = pqobject_get_properties;
    // pqobject_handlers.read_property	= pqobject_read_property;
    // pqobject_handlers.has_property = pqobject_has_property;

    PQDBGLPUP("register extensions");
    PQEnginePrivate::pq_register_extensions();

    PQDBG_LVL_DONE_LPUP();
    return SUCCESS;
}


