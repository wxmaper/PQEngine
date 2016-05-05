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

void PHPQt5::zif_qApp(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    if(ZEND_NUM_ARGS() > 0) {
        zend_wrong_paramers_count_error(ZEND_NUM_ARGS(), 0 ,0);
    }

    zval zo = objectFactory()->getZObject(qApp PQDBG_LVL_CC);

    PQDBG_LVL_DONE();
    RETURN_ZVAL(&zo, 1, 0);
}

void PHPQt5::zif_emit(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    int error = 0;
    QString className = "";


    void *TSRMLS_CACHE = tsrm_get_ls_cache();

    zend_object *zo = zend_get_this_object(EG(current_execute_data));

    if(zo && zo->ce) {
        zval zobject;
        ZVAL_OBJ(&zobject, zo);

        if(pq_test_ce(&zobject PQDBG_LVL_CC)) {
            className = QString(Z_OBJCE_NAME(zobject));

            char *signal_signature;
            int signal_signature_len;
            zval *args;

            if(zend_parse_parameters(ZEND_NUM_ARGS(), "sa", &signal_signature, &signal_signature_len, &args) == FAILURE) {
                PQDBG_LVL_DONE();
                return;
            }

            QObject *qo = objectFactory()->getQObject(&zobject PQDBG_LVL_CC);

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
            error = 2;
        }
    }
    else {
        error = 1;
    }



    pq_pre(QString("error: %1\nclassName: %2").arg(error).arg(className), "");

}

void PHPQt5::zif_SIGNAL(INTERNAL_FUNCTION_PARAMETERS)
{
    const char* signal;
    int signal_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &signal, &signal_len)) {
        return;
    }

    QString qstring = QString(signal);
    qstring.prepend("2");

    RETURN_STRING(qstring.toUtf8().constData());
}

void PHPQt5::zif_SLOT(INTERNAL_FUNCTION_PARAMETERS)
{
    const char* slot;
    int slot_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &slot, &slot_len)) {
        return;
    }

    QString qstring = QString(slot);
    qstring.prepend("1");

    RETURN_STRING(qstring.toUtf8().constData());
}

void PHPQt5::zif_connect(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *z_sender;
    zval *z_signal;
    zval *z_receiver;
    zval *z_slot;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zzzz", &z_sender, &z_signal, &z_receiver, &z_slot) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    PQDBG_LVL_DONE();
    RETURN_BOOL( pq_connect(z_sender, z_signal, z_receiver, z_slot, false PQDBG_LVL_CC) );
}

void PHPQt5::zif_pre(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *pzval;
    char *_title;
    int _title_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "z|s", &pzval, &_title, &_title_len) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    php_output_start_default();
    zend_print_zval_r(pzval, 0);
    php_output_get_contents(return_value);

    QString title = ZEND_NUM_ARGS() == 1 ? qApp->applicationName() : QString(_title);
    pq_pre(toUTF8( Z_STRVAL_P(return_value) ), title);

    php_output_discard();

    PQDBG_LVL_DONE();
    RETURN_TRUE;
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
    RETURN_BOOL( pq_connect(z_sender, z_signal, z_receiver, z_slot, true PQDBG_LVL_CC) )
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

    /*
    QString sObjectName = QString(objectName);
    int zhandle = objectFactory()->getObjectHandleByObjectName(sObjectName);

    zval retval = objectFactory()->getZObject(zhandle);

    if(Z_TYPE(retval) == IS_OBJECT) {
        RETURN_ZVAL(&retval, 1, 0);
    }

    RETURN_NULL();
    */

    QList<zval> zobjects = objectFactory()->getZObjectsByName(objectName PQDBG_LVL_CC);

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
        QString about = QString("<b>ZEND API</b>: %1<br>\n"
                                "<b>PHP version</b>: %2<br>\n"
                                "<b>PQEngine version</b>: %3 (%4)<br>\n"
                                "<b>PQExtensions API version</b>: %5<br>\n")
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

void PHPQt5::zif_setQStringCodePage(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char *code_page;
    int code_page_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &code_page, &code_page_len) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    W_CP = QByteArray(code_page);

    PQDBG_LVL_DONE();
    RETURN_TRUE;
}

void PHPQt5::zif_pqinclude(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char *filename;
    int filename_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &filename, &filename_len) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    QFile file(filename);

    if(!file.exists()) {
        php_error(E_WARNING,
                  QString("pqinclude(\"%1\"): failed to open stream: No such file or directory")
                  .arg(filename).toUtf8().constData());
        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }

    if(!file.open(QIODevice::ReadOnly)) {
        php_error(E_WARNING,
                  QString("pqinclude(\"%1\"): failed to open stream: %2")
                  .arg(filename).arg(file.errorString()).toUtf8().constData());
        PQDBG_LVL_DONE();
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

    if(php_execute_script(&file_handle) == SUCCESS) {
        PQDBG_LVL_DONE();
        RETURN_TRUE;
    }
    else {
        PQDBG_LVL_DONE();
        RETURN_FALSE;
    }
}

void PHPQt5::zif_pqread(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    char *filename;
    int filename_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &filename, &filename_len) == FAILURE) {
        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        return;
    }

    QFile file(filename);

    if(!file.exists()) {
        php_error(E_WARNING,
                  QString("pqread(\"%1\"): failed to open stream: No such file or directory")
                  .arg(filename).toUtf8().constData());

        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        RETURN_FALSE;
    }

    if(!file.open(QIODevice::ReadOnly)) {
        php_error(E_WARNING,
                  QString("pqread(\"%1\"): failed to open stream: %2")
                  .arg(filename).arg(file.errorString()).toUtf8().constData());

        #ifdef PQDEBUG
            PQDBG_LVL_DONE();
        #endif

        RETURN_FALSE;
    }

    QByteArray data_ba = file.readAll();

    pq_return_qvariant(data_ba, INTERNAL_FUNCTION_PARAM_PASSTHRU PQDBG_LVL_CC);

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif
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

void PHPQt5::zif_pqGetStarCoords(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    ZVAL_NEW_ARR(return_value);

    zval array;
    array_init(&array);

    ADD_STAR_POINT(array, 0, 0, 8);
    ADD_STAR_POINT(array, 300, 165, 4);
    ADD_STAR_POINT(array, 308, 198, 4);
    ADD_STAR_POINT(array, 294, 232, 4);
    ADD_STAR_POINT(array, 222, 322, 4);
    ADD_STAR_POINT(array, 225, 405, 4);

    ZVAL_ZVAL(return_value, &array, 1, 0);

#ifdef PQDEBUG
    PQDBG_LVL_DONE();
#endif
}

void PHPQt5::zif_test_one(INTERNAL_FUNCTION_PARAMETERS)
{

}
