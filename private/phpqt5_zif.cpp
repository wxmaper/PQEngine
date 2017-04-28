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
#include "ipqengineext.h"

QByteArray PHPQt5::mTrLang;
QHash<QByteArray, QByteArray> PHPQt5::mTrData;

void PHPQt5::zif_R(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    const char* path;
    int path_len;

    bool addQRC = false;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s|b", &path, &path_len, &addQRC)) {
        return;
    }

    /* old
    echo R($path);        // :/corename/some/file.txt
    echo R($path, false); // :/corename/some/file.txt
    echo R($path, true);  // qrc://corename/some/file.txt
    */
    /* new
    echo R($path);        // :/some/file.txt
    echo R($path, false); // :/some/file.txt
    echo R($path, true);  // qrc://some/file.txt
    */

    QByteArray rpath = QByteArray(path).replace("\\", "/").replace("//","/");
    if(addQRC) {
        //rpath.prepend(getCorename().prepend("qrc://").append("/"));
        rpath.prepend("qrc://");
    }
    else {
        //rpath.prepend(getCorename().prepend(":/").append("/"));
        rpath.prepend(":/");
    }

    PQDBG_LVL_DONE();
    RETURN_STRING(rpath.constData());
}

/*
void PHPQt5::zif_emit(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    QString className = "";

    void *TSRMLS_CACHE = tsrm_get_ls_cache();

    zend_object *zo = zend_get_this_object(EG(current_execute_data));

    if (zo && zo->ce) {
        zval zobject;
        ZVAL_OBJ(&zobject, zo);

        if (pq_test_ce(&zobject PQDBG_LVL_CC)) {
            className = QString(Z_OBJCE_NAME(zobject));

            char *signal_signature;
            int signal_signature_len;
            zval *args;

            if(zend_parse_parameters(ZEND_NUM_ARGS(), "sa", &signal_signature, &signal_signature_len, &args) == FAILURE) {
                PQDBG_LVL_DONE();
                return;
            }

            QObject *qo = Q_NULLPTR;//objectFactory()->getQObject(&zobject);

            if(qo != nullptr) {
                bool haveSignalConnection = false;
                bool haveSignal = false;
                QByteArray signalSignature(signal_signature);

                signalSignature.replace(",string",",QString")
                        .replace("string,","QString,")
                        .replace("(string)","(QString)");

                if(!QMetaObject::invokeMethod(qo,
                                          "haveSignal",
                                          Qt::DirectConnection,
                                          Q_RETURN_ARG(bool, haveSignal),
                                          Q_ARG(QByteArray, signalSignature))) {
                    php_error(E_WARNING,
                              QString("ERROR %1::invokeMethod( haveSignal )")
                              .arg(className)
                              .toUtf8().constData());
                }

                if(!haveSignal) {
                    php_error(E_WARNING,
                              QString("Undefined signal %1::%2")
                              .arg(className)
                              .arg(signal_signature).toUtf8().constData());

                    RETURN_NULL();
                }

                if(QMetaObject::invokeMethod(qo,
                                             "haveSignalConnection",
                                             Qt::DirectConnection,
                                             Q_RETURN_ARG(bool, haveSignalConnection),
                                             Q_ARG(QByteArray, signalSignature))) {
                    if(haveSignalConnection) {
                        #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
                        PQDBGLPUP(QString("emit: %1").arg(signal_signature));
                        #endif
                        pq_emit(qo, signalSignature, args);
                    }
                    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
                    else {
                        #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
                        PQDBGLPUP(QString("no have connections for %1").arg(signal_signature));
                        #endif
                    }
                    #endif
                }
                #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
                else {
                    #if defined(PQDEBUG) && defined(PQDETAILEDDEBUG)
                    PQDBGLPUP(QString("ERROR invoke method %1::haveSignalConnection").arg(className));
                    #endif
                }
                #endif
            }
        }
        else {
            pq_php_error("Call EMIT from non QObject object!");
        }
    }
    else {
        pq_php_error("Call EMIT without object!");
    }
}
*/

void PHPQt5::zif_qenum(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *enum_zval;

    if(zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &enum_zval) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    zval zenum;
    zend_class_entry *ce = PHPQt5::objectFactory()->getClassEntry("QEnum");
    object_init_ex(&zenum, ce);

    PQObjectWrapper *pqenum = fetch_pqobject(Z_OBJ(zenum));
    pqenum->object = Q_NULLPTR;
    pqenum->isEnum = true;
    pqenum->isValid = true;

    // Z_DELREF(pqenum);

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

    RETVAL_ZVAL(&zenum, 0, 0);
    PQDBG_LVL_DONE();
}

void PHPQt5::zif_SIGNAL(INTERNAL_FUNCTION_PARAMETERS)
{
    const char* signal;
    int signal_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &signal, &signal_len)) {
        return;
    }

    QByteArray str = QMetaObject::normalizedSignature(signal);
    str.prepend("2");

    RETURN_STRINGL(str.constData(), str.length());
}

void PHPQt5::zif_SLOT(INTERNAL_FUNCTION_PARAMETERS)
{
    const char* slot;
    int slot_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &slot, &slot_len)) {
        return;
    }

    QByteArray str = QMetaObject::normalizedSignature(slot);
    str.prepend("1");

    RETURN_STRINGL(str.constData(), str.length());
}

void PHPQt5::zif_disconnect(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *z_sender;
    zval *z_signal;
    zval *z_receiver;
    zval *z_slot;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zzzz", &z_sender, &z_signal, &z_receiver, &z_slot) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    PQDBG_LVL_DONE();
    // FIXME: исправить 0_0
    RETURN_FALSE;
    //RETURN_BOOL( pq_connect(z_sender, z_signal, z_receiver, z_slot, true PQDBG_LVL_CC) )
}


/*
 * Реализация функции c()
 */
void PHPQt5::zif_c(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    const char* objectName;
    int objectName_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &objectName, &objectName_len)) {
        PQDBG_LVL_DONE();
        RETURN_NULL();
    }


    QList<zval> zobjects;// = objectFactory()->getZObjectsByName(objectName);

    array_init(return_value);
    foreach (zval zobject, zobjects) {
        add_next_index_zval(return_value, &zobject);
    }

    PQDBG_LVL_DONE();
}

/*
 * Интернационализация приложений tr()
 */
void PHPQt5::zif_tr(INTERNAL_FUNCTION_PARAMETERS)
{
    const char* tr_str;
    int tr_str_len;

    const char* unitId = "";
    int unitId_len = 0;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &tr_str, &tr_str_len, &unitId, &unitId_len)) {
        return;
    }

    if(!mTrData.isEmpty()) {
        QByteArray key = QString("%1&:@=%2").arg(unitId).arg(tr_str).toUtf8();
        if(mTrData.contains(key)) {
            RETURN_STRING(toW(mTrData.value(key)).constData());
        }
    }

    RETURN_STRING(tr_str);
}

void PHPQt5::zif_set_tr_lang(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    const char* tr_lang;
    int tr_lang_len;
    const char* tr_path;
    int tr_path_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s|s",
                             &tr_lang, &tr_lang_len, &tr_path, &tr_path_len)) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    QDir path;
    if(ZEND_NUM_ARGS() == 2) {
        path.setPath(tr_path);
    }
    else {
        path.setPath("tr");
    }

    if(!path.exists()) {
        pq_ub_write(QString("Cannot access to translations directory `%1`")
                    .arg(path.absolutePath()));
        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    QFile trFile(QString("%1/%2.xml").arg(path.absolutePath()).arg(tr_lang));
    if (!trFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        pq_ub_write(QString("Cannot open translation file `%1/%2.xml`")
                             .arg(path.absolutePath())
                             .arg(tr_lang));
        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    QHash<QByteArray, QByteArray> trData;
    QDomDocument xmlPQETR;
    xmlPQETR.setContent(&trFile);

    QDomElement pqenginetr = xmlPQETR.documentElement();

    if(pqenginetr.nodeName() != "pqenginetr") {
        pq_ub_write(QString("Bad translation file `tr/%1.xml`: 'pqenginetr' node not found.")
                             .arg(tr_lang));
        trFile.close();

        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    if(!pqenginetr.hasAttribute("lang")) {
        pq_ub_write(QString("Bad translation file `tr/%1.xml`: 'lang' attribute not found.")
                             .arg(tr_lang));
        trFile.close();

        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    QString trLang = pqenginetr.attribute("lang", "");
    if(trLang != tr_lang) {
        pq_ub_write(QString("Bad translation file `tr/%1.xml`: incorrect 'lang' attribute:\n%2 != %3")
                             .arg(tr_lang)
                             .arg(trLang)
                             .arg(tr_lang));
        trFile.close();

        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    QDomElement unit = pqenginetr.firstChild().toElement();
    while(!unit.isNull()) {
        if(unit.tagName() == "unit") {
            QString unitId = unit.attribute("id", "");
            QString source;
            QString translate;

            QDomElement unitData = unit.firstChild().toElement();
            while(!unitData.isNull())
            {
                if(unitData.tagName() == "source") source = unitData.firstChild().toText().data();
                else if(unitData.tagName() == "tr") translate = unitData.firstChild().toText().data();
                unitData = unitData.nextSibling().toElement();
            }

            if(source.isEmpty()
                    || translate.isEmpty()) {
                pq_ub_write(QString("Bad translation file `tr/%1.xml`: incorrect unit at line %2:\nempty value of <source> or <tr> node.")
                                     .arg(tr_lang)
                                     .arg(unit.lineNumber()));
                trData.clear();
                trFile.close();

                PQDBG_LVL_DONE();
                RETURN_FALSE;
            }

            trData.insert(QString("%1&:@=%2").arg(unitId).arg(source).toUtf8(), translate.toUtf8());
        }
        else {
            pq_ub_write(QString("Bad translation file `tr/%1.xml`: unexpected node <%2> at line %3")
                                 .arg(tr_lang)
                                 .arg(unit.tagName())
                                 .arg(unit.lineNumber()));
            trData.clear();
            trFile.close();

            PQDBG_LVL_DONE();
            RETURN_FALSE;
        }

        unit = unit.nextSibling().toElement();
    }

    if(trData.size() == 0) {
        pq_ub_write(QString("Empty translation file `tr/%1.xml`")
                             .arg(tr_lang));
        trData.clear();
        trFile.close();

        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    mTrData.clear();
    mTrData = trData;
    trFile.close();

    PQDBG_LVL_DONE();
    RETURN_TRUE;
}

void PHPQt5::zif_aboutPQ(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    bool do_return = false;
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &do_return) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    if(do_return) {
        zval array;
        array_init(&array);

        add_assoc_string(&array, "QT_VERSION", (char *)QT_VERSION_STR);
        add_assoc_string(&array, "QT_VERSION_MAJOR", (char *)QT_VERSION_MAJOR);
        add_assoc_string(&array, "QT_VERSION_MINOR", (char *)QT_VERSION_MINOR);
        add_assoc_string(&array, "QT_VERSION_PATCH", (char *)QT_VERSION_PATCH);

        add_assoc_string(&array, "MSC_FULL_VER", (char *)_MSC_FULL_VER);

        add_assoc_string(&array, "ZEND_VERSION", (char *)ZEND_VERSION);
        add_assoc_long(&array, "PHP_MAJOR_VERSION", PHP_MAJOR_VERSION);
        add_assoc_long(&array, "PHP_MINOR_VERSION", PHP_MINOR_VERSION);
        add_assoc_long(&array, "PHP_RELEASE_VERSION", PHP_RELEASE_VERSION);
        add_assoc_string(&array, "PHP_EXTRA_VERSION", (char *)PHP_EXTRA_VERSION);
        add_assoc_string(&array, "PHP_VERSION", (char *)PHP_VERSION);
        add_assoc_long(&array, "PHP_VERSION_ID", PHP_VERSION_ID);

        add_assoc_long(&array, "PQENGINE_MAJOR_VERSION", PQENGINE_MAJOR_VERSION);
        add_assoc_long(&array, "PQENGINE_MINOR_VERSION", PQENGINE_MINOR_VERSION);
        add_assoc_long(&array, "PQENGINE_RELEASE_VERSION", PQENGINE_RELEASE_VERSION);
        add_assoc_string(&array, "PQENGINE_CODENAME", (char *)PQENGINE_CODENAME);
        add_assoc_string(&array, "PQENGINE_VERSION", (char *)PQENGINE_VERSION);
        add_assoc_long(&array, "PQENGINE_VERSION_ID", PQENGINE_VERSION_ID);

        add_assoc_long(&array, "PQENGINEEXT_MAJOR_VERSION", PQENGINEEXT_MAJOR_VERSION);
        add_assoc_long(&array, "PQENGINEEXT_MINOR_VERSION", PQENGINEEXT_MINOR_VERSION);
        add_assoc_long(&array, "PQENGINEEXT_RELEASE_VERSION", PQENGINEEXT_RELEASE_VERSION);
        add_assoc_string(&array, "PQENGINEEXT_VERSION", (char *)PQENGINEEXT_VERSION);
        add_assoc_long(&array, "PQENGINEEXT_VERSION_ID", PQENGINEEXT_VERSION_ID);

        PQDBG_LVL_DONE();
        RETURN_ZVAL(&array, 1, 0);
    }
    else {
        QString about = QString("<b>Qt version</b>: %1<br>\n"
                                "<b>MSVC version</b>: %2<br>\n"
                                "<b>ZEND API</b>: %3<br>\n"
                                "<b>PHP version</b>: %4<br>\n"
                                "<b>PQEngine version</b>: %5 (%6)<br>\n"
                                "<b>PQExtensions API version</b>: %7<br>\n")
                .arg(QT_VERSION_STR)
                .arg(_MSC_FULL_VER)
                .arg(ZEND_VERSION)
                .arg(PHP_VERSION)
                .arg(PQENGINE_VERSION)
                .arg(PQENGINE_CODENAME)
                .arg(PQENGINEEXT_VERSION);

        pq_ub_write(about);

        PQDBG_LVL_DONE();
        RETURN_NULL();
    }
}

#include "zend_language_scanner.h"
#include "zend_highlight.h"

void PHPQt5::zif_pqpack(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char *key;
    int key_len;

    char *filename;
    int filename_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "ss",
                             &key, &key_len, &filename, &filename_len) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    QString scriptPath = QString(filename);
    QString pqeScriptPath = scriptPath.left(scriptPath.lastIndexOf(".")) + ".pqe";

    QFile main_php_file(scriptPath);
    QFile main_pqe_file(pqeScriptPath);

    if(!main_php_file.exists()) {
        php_error(E_WARNING,
                  QString("pqpack(\"%1\"): failed to open stream: No such file or directory")
                  .arg(filename).toUtf8().constData());

        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    if(!main_php_file.open(QIODevice::ReadOnly)) {
        php_error(E_WARNING,
                  QString("pqread(\"%1\"): failed to open stream: %2")
                  .arg(filename).arg(main_php_file.errorString()).toUtf8().constData());

        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    if(!main_pqe_file.open(QIODevice::WriteOnly)) {
        php_error(E_WARNING,
                  QString("pqread(\"%1\"): failed to open stream: %2")
                  .arg(filename).arg(main_pqe_file.errorString()).toUtf8().constData());

        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    QByteArray main_php_ba = main_php_file.readAll();
    QByteArray stripped_php_ba;

    // Удаление комментов, переносов и лишних пробелов
    {
        QDataStream dataStream(main_php_ba);

        zend_stream zs;
        zs.reader = pq_stream_reader;
        zs.closer = pq_stream_closer;
        zs.fsizer = pq_stream_fsizer;
        zs.handle = &dataStream;

        zend_file_handle file_handle;
        file_handle.type = ZEND_HANDLE_STREAM;
        file_handle.filename = "-";
        file_handle.free_filename = 0;
        file_handle.opened_path = NULL;
        file_handle.handle.stream = zs;

        zend_lex_state original_lex_state;
        zend_save_lexical_state(&original_lex_state);

        if(open_file_for_scanning(&file_handle) != FAILURE) {
            zval stripped_code;

            php_output_start_default();
            zend_strip();
            php_output_get_contents(&stripped_code);
            php_output_discard();

            dataStream.device()->close();
            dataStream.unsetDevice();

            stripped_php_ba = QByteArray(Z_STRVAL(stripped_code));
        }
        else {
            pq_pre("Lexical analyzer detect errors", "Error");
        }

        zend_restore_lexical_state(&original_lex_state);
    }

    // Упаковка PHP в PQE
    QByteArray key_ba(key);
    key_ba.append(PQ_APPEND_KEY);
    qlonglong hexkey = strtoll(key_ba.constData(), 0, 16);

    SimpleCrypt crypto(hexkey);
    crypto.setCompressionMode(SimpleCrypt::CompressionNever);

    QByteArray main_pqe_crypt_data = crypto.encryptToByteArray(stripped_php_ba);
    QByteArray main_pqe_data = qCompress(main_pqe_crypt_data.toBase64(), 9);

    // Запись в файл *.pqe
    main_pqe_file.write(main_pqe_data);
    main_pqe_file.close();

    PQDBG_LVL_DONE();
    RETURN_TRUE;
}

#define ADD_STAR_POINT(array, x, y, s) {\
    zval point;\
    array_init(&point);\
    add_assoc_long(&point, "x", x);\
    add_assoc_long(&point, "y", y);\
    add_assoc_long(&point, "s", s);\
    \
    add_next_index_zval(&array, &point);\
}

void PHPQt5::zif_qDebug(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    const int argc = ZEND_NUM_ARGS();
    zval *args = (zval *) safe_emalloc(argc, sizeof(zval), 0);
    if (ZEND_NUM_ARGS() < 1
            || zend_get_parameters_array_ex(argc, args) == FAILURE) {
        efree(args);
        WRONG_PARAM_COUNT;
        PQDBG_LVL_DONE();
    }

    for (int argn = 0; argn < argc; argn++) {
        pq_qdbg_message(&args[argn], return_value, QStringLiteral("qDebug"));
    }

    efree(args);

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_qWarning(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    const int argc = ZEND_NUM_ARGS();
    zval *args = (zval *) safe_emalloc(argc, sizeof(zval), 0);
    if (ZEND_NUM_ARGS() < 1
            || zend_get_parameters_array_ex(argc, args) == FAILURE) {
        efree(args);
        WRONG_PARAM_COUNT;
        PQDBG_LVL_DONE();
    }

    for (int argn = 0; argn < argc; argn++) {
        pq_qdbg_message(&args[argn], return_value, QStringLiteral("qWarning"));
    }

    efree(args);

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_qCritical(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    const int argc = ZEND_NUM_ARGS();
    zval *args = (zval *) safe_emalloc(argc, sizeof(zval), 0);
    if (ZEND_NUM_ARGS() < 1
            || zend_get_parameters_array_ex(argc, args) == FAILURE) {
        efree(args);
        WRONG_PARAM_COUNT;
        PQDBG_LVL_DONE();
    }

    for (int argn = 0; argn < argc; argn++) {
        pq_qdbg_message(&args[argn], return_value, QStringLiteral("qCritical"));
    }

    efree(args);

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_qInfo(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    const int argc = ZEND_NUM_ARGS();
    zval *args = (zval *) safe_emalloc(argc, sizeof(zval), 0);
    if (ZEND_NUM_ARGS() < 1
            || zend_get_parameters_array_ex(argc, args) == FAILURE) {
        efree(args);
        WRONG_PARAM_COUNT;
        PQDBG_LVL_DONE();
    }

    for (int argn = 0; argn < argc; argn++) {
        pq_qdbg_message(&args[argn], return_value, QStringLiteral("qInfo"));
    }

    efree(args);

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_qFatal(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *value;
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
        php_error(E_PARSE, "wrong parameters for qFatal");
        PQDBG_LVL_RETURN();
    }

    pq_qdbg_message(value, return_value, "qFatal");
    php_error(E_ERROR, Z_STRVAL_P(return_value));

    PQDBG_LVL_DONE();
}

void PHPQt5::zif_qvariant_cast(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *zobject;
    QByteArray typeName;

    int argc = ZEND_NUM_ARGS();

    switch(argc) {
    case 1:
        if(zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zobject) == FAILURE) {
            PQDBG_LVL_DONE();
            RETURN_NULL();
        }

        break;

    case 2: {
        const char* type_name;
        int type_name_len;

        if(zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &type_name, &type_name_len, &zobject) == FAILURE) {
            PQDBG_LVL_DONE();
            RETURN_NULL();
        }

        typeName = QByteArray(type_name, type_name_len);
    } break;

    default:
#if (PHP_VERSION_ID < 70101)
        zend_wrong_paramers_count_error(argc, 1, 2);
#else
        zend_wrong_parameters_count_error(argc, 1, 2);
#endif

        PQDBG_LVL_DONE();
        RETURN_NULL();
    }

    if(Z_TYPE_P(zobject) != IS_OBJECT) {
#if (PHP_VERSION_ID < 70101)
        zend_wrong_paramer_type_error(1, zend_expected_type(IS_OBJECT), zobject);
#else
        zend_wrong_parameter_type_error(1, zend_expected_type(IS_OBJECT), zobject);
#endif
        PQDBG_LVL_DONE();
        RETURN_NULL();
    }

    if(Z_OBJCE_NAME_P(zobject) != QByteArray("QVariant")) {
        zend_throw_error(NULL, "qvariant_cast() expects parameter 1 to be a <b>QVariant</b>, <b>%s</b> given",
                         Z_OBJCE_NAME_P(zobject));
    }

    PQObjectWrapper *pqobject = fetch_pqobject(Z_OBJ_P(zobject));
    QVariant *v = reinterpret_cast<QVariant*>(pqobject->object->plastiq_data());
    zval retval = plastiq_cast_to_zval(*v, typeName);

    RETVAL_ZVAL(&retval, 1, 0);

    PQDBG_LVL_DONE();
}
