#include "phpqt5.h"

QByteArray PHPQt5::mTrLang;
QHash<QByteArray, QByteArray> PHPQt5::mTrData;

void PHPQt5::zif_SIGNAL(INTERNAL_FUNCTION_PARAMETERS)
{
    const char* signal;
    int signal_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &signal, &signal_len)) {
        return;
    }

    QString qstring = QString(signal);
    qstring.prepend("2");

    RETURN_STRING(qstring.toUtf8().constData(), 1);
}

void PHPQt5::zif_SLOT(INTERNAL_FUNCTION_PARAMETERS)
{
    const char* slot;
    int slot_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &slot, &slot_len)) {
        return;
    }

    QString qstring = QString(slot);
    qstring.prepend("1");

    RETURN_STRING(qstring.toUtf8().constData(), 1);
}

void PHPQt5::zif_connect(INTERNAL_FUNCTION_PARAMETERS)
{
    zval *z_sender;
    zval *z_signal;
    zval *z_receiver;
    zval *z_slot;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzzz", &z_sender, &z_signal, &z_receiver, &z_slot) == FAILURE) {
        return;
    }

    RETURN_BOOL( pq_connect(z_sender, z_signal, z_receiver, z_slot, false TSRMLS_CC) );
}

void PHPQt5::zif_pre(INTERNAL_FUNCTION_PARAMETERS)
{
    zval *pzval;
    char *_title;
    int _title_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|s", &pzval, &_title, &_title_len) == FAILURE) {
        return;
    }

    php_output_start_default(TSRMLS_C);
    zend_print_zval_r(pzval, 0 TSRMLS_CC);
    php_output_get_contents(return_value TSRMLS_CC);

    QString title = ZEND_NUM_ARGS() == 1 ? qApp->applicationName() : QString(_title);
    //pq_ub_write(toUTF8( Z_STRVAL_P(return_value) ), title);
    pq_pre(toUTF8( Z_STRVAL_P(return_value) ), title);

    php_output_discard(TSRMLS_C);

    RETURN_TRUE;
}

void PHPQt5::zif_disconnect(INTERNAL_FUNCTION_PARAMETERS)
{
    zval *z_sender;
    zval *z_signal;
    zval *z_receiver;
    zval *z_slot;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzzz", &z_sender, &z_signal, &z_receiver, &z_slot) == FAILURE) {
        return;
    }

    RETURN_BOOL( pq_connect(z_sender, z_signal, z_receiver, z_slot, true TSRMLS_CC) )
}


/*
 * Реализация функции c()
 */
void PHPQt5::zif_c(INTERNAL_FUNCTION_PARAMETERS)
{
    const char* objectName;
    int objectName_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &objectName, &objectName_len)) {
        RETURN_NULL();
    }


    QString sObjectName = QString(objectName);
    int zhandle = objectFactory()->getObjectHandleByObjectName(sObjectName);

    zval *retval = objectFactory()->getZObject(zhandle);

    if(retval != NULL) {
        RETURN_ZVAL(retval, 1, 0);
    }


    /*
    QString sObjectName = QString(objectName);
    if(!sObjectName.isEmpty()) {
        int zHandle = objectFactory()->getObjectHandleByObjectName(sObjectName);
        if(zHandle > -1) {
            zval *pzval = objectFactory()->getZObject(zHandle);
            RETURN_ZVAL(pzval, 1, 0);
        }
    }
    */

    RETURN_NULL();
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

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &tr_str, &tr_str_len, &unitId, &unitId_len)) {
        return;
    }

    if(!mTrData.isEmpty()) {
        QByteArray key = QString("%1&:@=%2").arg(unitId).arg(tr_str).toUtf8();
        if(mTrData.contains(key)) {
            RETURN_STRING(toW(mTrData.value(key)).constData(), 1);
        }
    }

    RETURN_STRING(tr_str, 1);
}

void PHPQt5::zif_set_tr_lang(INTERNAL_FUNCTION_PARAMETERS)
{
    const char* tr_lang;
    int tr_lang_len;
    const char* tr_path;
    int tr_path_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s",
                             &tr_lang, &tr_lang_len, &tr_path, &tr_path_len)) {
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
        RETURN_FALSE;
    }

    QFile trFile(QString("%1/%2.xml").arg(path.absolutePath()).arg(tr_lang));
    if (!trFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        pq_ub_write(QString("Cannot open translation file `%1/%2.xml`")
                             .arg(path.absolutePath())
                             .arg(tr_lang));
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
        RETURN_FALSE;
    }

    if(!pqenginetr.hasAttribute("lang")) {
        pq_ub_write(QString("Bad translation file `tr/%1.xml`: 'lang' attribute not found.")
                             .arg(tr_lang));
        trFile.close();
        RETURN_FALSE;
    }

    QString trLang = pqenginetr.attribute("lang", "");
    if(trLang != tr_lang) {
        pq_ub_write(QString("Bad translation file `tr/%1.xml`: incorrect 'lang' attribute:\n%2 != %3")
                             .arg(tr_lang)
                             .arg(trLang)
                             .arg(tr_lang));
        trFile.close();
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
            RETURN_FALSE;
        }

        unit = unit.nextSibling().toElement();
    }

    if(trData.size() == 0) {
        pq_ub_write(QString("Empty translation file `tr/%1.xml`")
                             .arg(tr_lang));
        trData.clear();
        trFile.close();
        RETURN_FALSE;
    }

    mTrData.clear();
    mTrData = trData;
    trFile.close();
    RETURN_TRUE;
}

void PHPQt5::zif_aboutPQ(INTERNAL_FUNCTION_PARAMETERS)
{
    bool do_return = false;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &do_return) == FAILURE) {
        return;
    }

    if(do_return) {
        zval *array = NULL;
        MAKE_STD_ZVAL(array);
        array_init(array);

        add_assoc_string(array, "ZEND_VERSION", ZEND_VERSION, 1);
        add_assoc_long(array, "PHP_MAJOR_VERSION", PHP_MAJOR_VERSION);
        add_assoc_long(array, "PHP_MINOR_VERSION", PHP_MINOR_VERSION);
        add_assoc_long(array, "PHP_RELEASE_VERSION", PHP_RELEASE_VERSION);
        add_assoc_string(array, "PHP_EXTRA_VERSION", PHP_EXTRA_VERSION, 1);
        add_assoc_string(array, "PHP_VERSION", PHP_VERSION, 1);
        add_assoc_long(array, "PHP_VERSION_ID", PHP_VERSION_ID);

        add_assoc_long(array, "PQENGINE_MAJOR_VERSION", PQENGINE_MAJOR_VERSION);
        add_assoc_long(array, "PQENGINE_MINOR_VERSION", PQENGINE_MINOR_VERSION);
        add_assoc_long(array, "PQENGINE_RELEASE_VERSION", PQENGINE_RELEASE_VERSION);
        add_assoc_string(array, "PQENGINE_EXTRA_VERSION", PQENGINE_EXTRA_VERSION, 1);
        add_assoc_string(array, "PQENGINE_VERSION", PQENGINE_VERSION, 1);
        add_assoc_long(array, "PQENGINE_VERSION_ID", PQENGINE_VERSION_ID);

        RETURN_ZVAL(array, 1, 0);
    }
    else {
        QString about = QString("<b>ZEND API</b>: %1<br>\n"
                                "<b>PHP version</b>: %2<br>\n"
                                "<b>PQEngine version</b>: %3<br>\n")
                .arg(ZEND_VERSION)
                .arg(PHP_VERSION)
                .arg(PQENGINE_VERSION);

        pq_ub_write(about);

        RETURN_NULL();
    }
}

void PHPQt5::zif_setQStringCodePage(INTERNAL_FUNCTION_PARAMETERS)
{
    char *code_page;
    int code_page_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &code_page, &code_page_len) == FAILURE) {
        return;
    }

    W_CP = QByteArray(code_page);

    RETURN_TRUE;
}

void PHPQt5::zif_pqinclude(INTERNAL_FUNCTION_PARAMETERS)
{
    char *filename;
    int filename_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
        return;
    }

    QFile file(filename);

    if(!file.exists()) {
        php_error(E_WARNING,
                  QString("pqinclude(\"%1\"): failed to open stream: No such file or directory")
                  .arg(filename).toUtf8().constData());

        RETURN_FALSE;
    }

    if(!file.open(QIODevice::ReadOnly)) {
        php_error(E_WARNING,
                  QString("pqinclude(\"%1\"): failed to open stream: %2")
                  .arg(filename).arg(file.errorString()).toUtf8().constData());

        RETURN_FALSE;
    }

    QByteArray data_ba = file.readAll();
    QDataStream dataStream(data_ba);

    zend_stream zs;
    zs.reader = pq_stream_reader;
    zs.closer = pq_stream_closer;
    zs.fsizer = pq_stream_fsizer;
    zs.handle = &dataStream;

    zend_file_handle file_handle;
    file_handle.type = ZEND_HANDLE_STREAM;
    file_handle.filename = QByteArray(filename).constData();
    file_handle.free_filename = 0;
    file_handle.opened_path = NULL;
    file_handle.handle.stream = zs;

    if(php_execute_script(&file_handle TSRMLS_CC) == SUCCESS) {
        RETURN_TRUE;
    }
    else {
        RETURN_FALSE;
    }
}

void PHPQt5::zif_pqread(INTERNAL_FUNCTION_PARAMETERS)
{
    char *filename;
    int filename_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
        return;
    }

    QFile file(filename);

    if(!file.exists()) {
        php_error(E_WARNING,
                  QString("pqread(\"%1\"): failed to open stream: No such file or directory")
                  .arg(filename).toUtf8().constData());

        RETURN_FALSE;
    }

    if(!file.open(QIODevice::ReadOnly)) {
        php_error(E_WARNING,
                  QString("pqread(\"%1\"): failed to open stream: %2")
                  .arg(filename).arg(file.errorString()).toUtf8().constData());

        RETURN_FALSE;
    }

    QByteArray data_ba = file.readAll();

    pq_return_qvariant(data_ba, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

void PHPQt5::zif_pqpack(INTERNAL_FUNCTION_PARAMETERS)
{
    char *key;
    int key_len;

    char *filename;
    int filename_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                             &key, &key_len, &filename, &filename_len) == FAILURE) {
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

        RETURN_FALSE;
    }

    if(!main_php_file.open(QIODevice::ReadOnly)) {
        php_error(E_WARNING,
                  QString("pqread(\"%1\"): failed to open stream: %2")
                  .arg(filename).arg(main_php_file.errorString()).toUtf8().constData());

        RETURN_FALSE;
    }

    if(!main_pqe_file.open(QIODevice::WriteOnly)) {
        php_error(E_WARNING,
                  QString("pqread(\"%1\"): failed to open stream: %2")
                  .arg(filename).arg(main_pqe_file.errorString()).toUtf8().constData());

        RETURN_FALSE;
    }

    QByteArray main_php_ba = main_php_file.readAll();

    // Упаковка PHP в PQE
    QByteArray key_ba(key);
    key_ba.append(PQ_APPEND_KEY);
    qlonglong hexkey = strtoll(key_ba.constData(), 0, 16);

    SimpleCrypt crypto(hexkey);
    QByteArray main_pqe_crypt_data = crypto.encryptToByteArray(main_php_ba);
    QByteArray main_pqe_data = qCompress(main_pqe_crypt_data.toBase64(), 9);

    // Запись в файл *.pqe
    main_pqe_file.write(main_pqe_data);
    main_pqe_file.close();

    RETURN_TRUE;
}
