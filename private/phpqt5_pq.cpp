#include <typeinfo>
#include <QMetaObject>

#include "pqclasses.h"
#include "phpqt5.h"
#include "pqengine_private.h"

QHash<QObject*, int> PHPQt5::acceptedPHPSlots_indexes;
QList< QHash<QString, pq_access_function_entry> > PHPQt5::acceptedPHPSlots_list;
QHash<int, pq_event_wrapper> PHPQt5::pq_eventHash;
QHash<QString, pq_access_function_entry> PHPQt5::acceptedPHPFunctions;
QStringList PHPQt5::mArguments;

#define PQ_TEST_CLASS(classname) qo_sender->metaObject()->className() == QString(classname)
#define PQ_CREATE_PHP_CONN(classname) phpqt5Connections->createPHPConnection<classname>(zo_sender, qo_sender, signal, z_receiver, slot TSRMLS_CC)

bool pq_PHP_emit(const QString &qosignal, QObject *sender, QEvent *event)
{
    int index = PHPQt5::acceptedPHPSlots_indexes.value(sender, -1);

    if(index >= 0) {
        if(PHPQt5::acceptedPHPSlots_list.at(index).contains(qosignal)) {
            TSRMLS_FETCH();

            pq_access_function_entry afe = PHPQt5::acceptedPHPSlots_list.at(index).value(qosignal);
            zend_function *copy_fptr = zend_get_closure_invoke_method(afe.zo TSRMLS_CC);
            zval *zevent = PHPQt5::pq_qevent_to_zobject(event TSRMLS_CC);

            PHPQt5::pq_eventHash.insert(Z_OBJVAL_P(zevent).handle, pq_event_wrapper{zevent, event});
            zval *zobject = PHPQt5::objectFactory()->getZObject(sender);

            zend_call_method_with_2_params(&afe.zo,
                                           Z_OBJCE_P(afe.zo),
                                           &copy_fptr,
                                           "__invoke",
                                           NULL,
                                           zobject,
                                           zevent);

            PHPQt5::pq_eventHash.remove(Z_OBJVAL_P(zevent).handle);
            zval_ptr_dtor(&zevent);

            return true;
        }
    }

    return false;
}

bool pq_callPHPFunction(const QString &fn_name, QObject *sender, QEvent *event)
{
#ifdef PQDEBUG
    PQDBG2("pq_callPHPFunction()", fn_name);
#endif

    TSRMLS_FETCH();

    if(!PHPQt5::acceptedPHPFunctions.contains(fn_name)) {
        //QMessageBox::warning(0, "callPHPFunction", QString("callPHPFunction: cannot call function %1($event)").arg(fn_name));
        return false;
    }

    pq_access_function_entry afe = PHPQt5::acceptedPHPFunctions.value(fn_name);

    zval *args[2];
    zval *retzval = NULL;
    zval *pqzobj = PHPQt5::objectFactory()->getZObject(sender);
    zval *zevent = PHPQt5::pq_qevent_to_zobject(event TSRMLS_CC);
    int handle = Z_OBJVAL_P(zevent).handle;
    bool retval = false;

    MAKE_STD_ZVAL(args[0]);
    MAKE_STD_ZVAL(args[1]);
    MAKE_STD_ZVAL(retzval);
    ZVAL_ZVAL(args[0], pqzobj, 1, 0);
    ZVAL_ZVAL(args[1], zevent, 1, 0);

    PHPQt5::pq_eventHash.insert(handle, pq_event_wrapper{zevent, event});
    call_user_function(EG(function_table), &afe.zo, afe.zfn_name, retzval, 2, args TSRMLS_CC);
    PHPQt5::pq_eventHash.remove(handle);

    if(retzval != NULL
            && Z_TYPE_P(retzval) == IS_BOOL)
    {
        retval = Z_BVAL_P(retzval);
    }

    zval_ptr_dtor(&args[0]);
    zval_ptr_dtor(&args[1]);
    zval_ptr_dtor(&zevent);
    zval_ptr_dtor(&retzval);

    return retval;
}

bool pq_registerQObject(QObject *qo)
{
    QString className = qo->metaObject()->className();

    zend_class_entry *ce_ptr = PHPQt5::objectFactory()->getClassEntry(className);

    if(ce_ptr) {
        TSRMLS_FETCH();

        zval *zobj_ptr = NULL;
        MAKE_STD_ZVAL(zobj_ptr);

        object_init_ex(zobj_ptr, ce_ptr);

        return PHPQt5::objectFactory()->registerObject(zobj_ptr, qo);
    }

    return false;
}


size_t pq_stream_reader(void *dataStreamPtr, char *buffer, size_t wantlen TSRMLS_DC)
{
    QDataStream *dataStream = (QDataStream*) dataStreamPtr;
    return dataStream->device()->read(buffer, wantlen);
}

void pq_stream_closer(void *dataStreamPtr TSRMLS_DC)
{
    QDataStream *dataStream = (QDataStream*) dataStreamPtr;
    dataStream->device()->close();
}

size_t pq_stream_fsizer(void *dataStreamPtr TSRMLS_DC)
{
    QDataStream *dataStream = (QDataStream*) dataStreamPtr;
    return dataStream->device()->size();
}


void (*ub_write_fn_ptr)(const QString &msg) = 0;
void pq_ub_write(const QString &msg)
{
    if(!ub_write_fn_ptr) {
        foreach (IPQExtension *pqext, PQEngine::m_PQExtensions.values()) {
            if(pqext->entry().use_ub_write) {
                ub_write_fn_ptr = pqext->entry().ub_write;
                break;
            }
        }
    }

    if(ub_write_fn_ptr) {
        ub_write_fn_ptr(msg);
    }
}

void (*pre_fn_ptr)(const QString &msg, const QString &title) = 0;
void pq_pre(const QString &msg, const QString &title)
{
    if(!pre_fn_ptr) {
        foreach (IPQExtension *pqext, PQEngine::m_PQExtensions.values()) {
            if(pqext->entry().use_pre) {
                pre_fn_ptr = pqext->entry().pre;
                break;
            }
        }
    }

    if(pre_fn_ptr) {
        pre_fn_ptr(msg, title);
    }
}

void do_register_long_constant(const QString &className, const QString &constName, int value, QString ceName)
{
    if(ceName == "") ceName = QString("P%1").arg(className);

    if(zend_class_entry *ce_ptr = PHPQt5::objectFactory()->getClassEntry(ceName)) {
        TSRMLS_FETCH();

        zend_declare_class_constant_long(ce_ptr,
                                         constName.toUtf8().constData(),
                                         constName.length(),
                                         value
                                         TSRMLS_CC);
    }
}

void do_register_string_constant(const QString &className, const QString &constName, const QString &value, QString ceName)
{
    if(ceName == "") ceName = QString("P%1").arg(className);

    if(zend_class_entry *ce_ptr = PHPQt5::objectFactory()->getClassEntry(ceName)) {
        TSRMLS_FETCH();

        zend_declare_class_constant_stringl(ce_ptr,
                                            constName.toUtf8().constData(),
                                            constName.length(),
                                            value.toUtf8().constData(),
                                            value.length()
                                            TSRMLS_CC);
    }
}

void pq_register_long_constant(const QString &className, const QString &constName, int value) {
    do_register_long_constant(className, constName, value, "");
}

void pq_register_long_constant_ex(const QString &className, const QString &constName, int value, QString ceName) {
    do_register_long_constant(className, constName, value, ceName);
}

void pq_register_string_constant(const QString &className, const QString &constName, const QString &value) {
    do_register_string_constant(className, constName, value, "");
}

void pq_register_string_constant_ex(const QString &className, const QString &constName, const QString &value, QString ceName) {
    do_register_string_constant(className, constName, value, ceName);
}

void pq_register_extra_zend_ce(const QString &className)
{
#ifdef PQDEBUG
    PQDBG2("pq_register_extra_zend_ce()", className);
#endif

    TSRMLS_FETCH();
    zend_class_entry ce, *ce_ptr;

    INIT_CLASS_ENTRY_EX(ce, className.toUtf8().constData(), className.length(), PHPQt5::phpqt5_no_methods());
    ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

    PHPQt5::objectFactory()->registerZendClassEntry(className, ce_ptr);
}

#define PQ_REGISTER_ZEND_CE(zcname) \
    zend_class_entry zcname##_ce;\
    zend_class_entry *zcname##_ce_ptr;\
    QString zcname##_str = #zcname;\
    INIT_CLASS_ENTRY_EX(zcname##_ce, zcname##_str.toUtf8().constData(), strlen(#zcname), phpqt5_qevent_methods());\
    zcname##_ce_ptr = zend_register_internal_class(&zcname##_ce TSRMLS_CC);\
    objectFactory()->registerZendClassEntry(zcname##_str, zcname##_ce_ptr);

void PHPQt5::pq_prepare_args(int argc, char** argv)
{
    for(int i = 0; i < argc; i++) {
        mArguments.append(QString(argv[i]));
    }
}

QStringList PHPQt5::pq_get_arguments()
{
    return mArguments;
}

PQAPI void PHPQt5::pq_core_init(TSRMLS_D)
{
#ifdef PQDEBUG
    PQDBG("PHPQt5::pq_core_init()");
#endif

    QByteArray className = "qApp";

    zend_class_entry qApp_ce;
    INIT_CLASS_ENTRY_EX(qApp_ce, className.constData(), className.size(), phpqt5_qApp_methods());
    zend_register_internal_class(&qApp_ce TSRMLS_CC);

    /*
    PQ_REGISTER_ZEND_CE(QEvent);
    PQ_REGISTER_ZEND_CE(QMouseEvent);
    PQ_REGISTER_ZEND_CE(QKeyEvent);
    PQ_REGISTER_ZEND_CE(QResizeEvent);
    PQ_REGISTER_ZEND_CE(QMoveEvent);
    PQ_REGISTER_ZEND_CE(QHoverEvent);
    PQ_REGISTER_ZEND_CE(QFocusEvent);
    PQ_REGISTER_ZEND_CE(QEnterEvent);
    PQ_REGISTER_ZEND_CE(QContextMenuEvent);
    PQ_REGISTER_ZEND_CE(QTimerEvent);
    */
}

PQAPI void PHPQt5::pq_register_class(const QMetaObject &metaObject TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::pq_register_class()", metaObject.className());
#endif

    QString className = objectFactory()->registerMetaObject(metaObject);

    QByteArray ba = className.toUtf8();
    const char* class_name = ba.constData();
    int class_name_len = className.length();

    zend_class_entry ce;

    INIT_CLASS_ENTRY_EX(ce, class_name, class_name_len, phpqt5_generic_methods());

    zend_class_entry *ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

    objectFactory()->registerZendClassEntry(metaObject.className(), ce_ptr);
}

PQAPI bool PHPQt5::pq_test_ce(zval *pzval TSRMLS_DC)
{
    if(Z_TYPE_P(pzval) == IS_OBJECT) {
        zend_class_entry *ce = Z_OBJCE_P(pzval);

        while (!objectFactory()->getRegisteredMetaObjects().contains(ce->name)
               && ce->parent != NULL)
        {
            ce = ce->parent;
        }

        return objectFactory()->getRegisteredMetaObjects().contains(ce->name);
    }

    return false;
}

void PHPQt5::pq_call_with_return(QObject *qo, const char *method, zval *pzval, INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::pq_call_with_return()", QString("%1->%2").arg(qo->metaObject()->className()).arg(method));
#endif

    QVariant retVal = pq_call(qo, method, pzval TSRMLS_CC);
    pq_return_qvariant(retVal, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

typedef struct _pq_call_qo_entry {
    QObject *qo;
    zval *zv;
    bool before_have_parent;
} pq_call_qo_entry;

QVariant PHPQt5::pq_call(QObject *qo, const char *method, zval *pzval TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::pq_call()", QString("%1->%2").arg(qo->metaObject()->className()).arg(method));
#endif

    /*
     * Парсим параметры и записываем в QVariantList values
     * пока поддерживаются только bool, string, long, double, null, QWidget
     */
    zval **entry;
    char *string_key;
    uint str_key_len;
    ulong num_key;
    HashPosition pos;

    QVariantList args;

    QList<pq_call_qo_entry> arg_qos;

    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(pzval), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(pzval), (void **)&entry, &pos) == SUCCESS) {
        if (zend_hash_get_current_key_ex(Z_ARRVAL_P(pzval), &string_key, &str_key_len, &num_key, 0, &pos) == HASH_KEY_IS_LONG) {

            switch(Z_TYPE_PP(entry)) {
            case IS_BOOL:
                args << QVariant( Z_BVAL_PP(entry) );
                break;

            case IS_STRING:
                args << QVariant( QString(toUTF8(Z_STRVAL_PP(entry))) );
                break;

            case IS_LONG:
                args << QVariant( (int) Z_LVAL_PP(entry) );
                break;

            case IS_DOUBLE:
                args << QVariant( Z_DVAL_PP(entry) );
                break;

            case IS_OBJECT: {
                if(pq_test_ce(*entry TSRMLS_CC)) {
                    QObject *arg_qo = objectFactory()->getObject(*entry TSRMLS_CC);
                    args << QVariant::fromValue<QObject*>( arg_qo );

                    bool before_have_parent = arg_qo->parent() ? true : false;
                    arg_qos << pq_call_qo_entry { arg_qo, *entry, before_have_parent };
                }
                else {
                    php_error(E_ERROR, QString("Unknown type of argument %1").arg(pos->h).toUtf8().constData());
                }
                break;
            }

            case IS_ARRAY:
                args << pq_ht_to_qstringlist(*entry TSRMLS_CC);
                break;

            case IS_NULL:
                args << NULL;
                break;

            default:
                php_error(E_ERROR, QString("Unknown type of argument %1").arg(pos->h).toUtf8().constData());
            }
        }
        else {
            return NULL;
        }

        zend_hash_move_forward_ex(Z_ARRVAL_P(pzval), &pos);
    }

    QVariant retVal = pq_call(qo, method, args TSRMLS_CC);

    foreach(pq_call_qo_entry cqe, arg_qos) {
        bool after_have_parent = cqe.qo->parent();

        switch (m_mmng) {
        case MemoryManager::PHPQt5:

            break;

        case MemoryManager::Zend:
            if(!cqe.before_have_parent
                    && after_have_parent) {
                Z_ADDREF_P(pzval);
            }
            break;

        case MemoryManager::Hybrid:
        default:
            if(!cqe.qo->isWidgetType()
                    && !cqe.before_have_parent
                    && after_have_parent) {
                Z_ADDREF_P(pzval);
            }
        }

    }

    return retVal;
}

QVariant PHPQt5::pq_call(QObject *qo, const char *method, QVariantList args TSRMLS_DC)
{
    QVariant retVal;

    /*
     * Пытаемся найти метод
     */
    QMetaMethod qmm;

    int parameterCount = args.size();

    bool ok = false;

    // перебираем все доступные методы
    for(int i = qo->metaObject()->methodOffset(); i < qo->metaObject()->methodCount(); ++i) {
        qmm = qo->metaObject()->method(i);

        // ищем совпадение названия метода
        if(qmm.name() == method) {

            // ищем совпадение количества параметров
            if(qmm.parameterCount() == parameterCount)
            {
                // останавливаем цикл только в случае успеха
                // обеспечивает возможность перебора перегруженных методов
                if(objectFactory()->call(qo, qmm, args, &retVal)) {
                    ok = true;
                    break;
                }
            }
        }
    }

    if(!ok) {
        QString qt_class_name = qo->metaObject()->className();
        qt_class_name = qt_class_name.mid(1);

        QString types = "";
        foreach(QVariant arg, args) {
            types.append(",").append(arg.typeName());
        }

        php_error(E_ERROR,
                  QString("Call to undefined method %1::%2(%3)")
                  .arg(qt_class_name)
                  .arg(method)
                  .arg(types)
                  .toUtf8().constData());
    }

    return retVal;
}

void PHPQt5::pq_return_qvariant(const QVariant &retVal, INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef PQDEBUG
    PQDBG("PHPQt5::pq_return_qvariant()");
#endif

    zval *ret_pzval;

    if(retVal.isValid()) {
        QVariant r = retVal; // make copy

        if(retVal.type() == QMetaType::QVariant) {
            r = retVal.value<QVariant>(); // re-qvariant :-)
        }

        switch (r.type()) {
        case QMetaType::QString:
            RETURN_STRING( toW(r.toByteArray()).constData(), 1 );
            break;

        case QMetaType::QByteArray:
            RETURN_STRING( toW(r.toByteArray().constData()), 1 );
            break;

        case QMetaType::Long:
        case QMetaType::Int:
        case QMetaType::LongLong:
            RETURN_LONG( r.toInt() );
            break;

        case QMetaType::Double:
        case QMetaType::Float:
            RETURN_DOUBLE( r.toDouble() );
            break;

        case QMetaType::Bool:
            RETURN_BOOL( r.toBool() );
            break;

        case QMetaType::QStringList:
            ret_pzval = pq_qstringlist_to_ht(r.toStringList() TSRMLS_CC);
            Z_DELREF_P(ret_pzval);
            RETURN_ZVAL(ret_pzval, 1, 0);
            break;

        case QMetaType::QPoint:
            ret_pzval = pq_qpoint_to_ht(r.toPoint() TSRMLS_CC);
            Z_DELREF_P(ret_pzval);
            RETURN_ZVAL(ret_pzval, 1, 0);
            break;

        case QMetaType::QRect:
            ret_pzval = pq_qrect_to_ht(r.toRect() TSRMLS_CC);
            Z_DELREF_P(ret_pzval);
            RETURN_ZVAL(ret_pzval, 1, 0);
            break;

        case QMetaType::QObjectStar:
            ret_pzval = objectFactory()->getZObject(qvariant_cast<QObject*>(r));
            if(ret_pzval != NULL) {
                RETURN_ZVAL(ret_pzval, 1, 0);
            }
            else RETURN_NULL();
            break;

        case QMetaType::User:
            if(r.canConvert(QMetaType::QObjectStar)) {
                ret_pzval = objectFactory()->getZObject(qvariant_cast<QObject*>(r));

                if(ret_pzval != NULL) {
                    RETURN_ZVAL(ret_pzval, 1, 0);
                }
                else RETURN_NULL();
            }
            break;

        default:
            RETURN_NULL();
        }
    }
    else {
#ifdef PQDEBUG
        PQDBG("INVALID QVARIANT VALUE");
#endif
    }

    RETURN_NULL();
}

bool PHPQt5::pq_set_parent(QObject *qo, zval *pzval TSRMLS_DC)
{
    zval **ppzval_object;
    HashPosition pos;
    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(pzval), &pos);
    zend_hash_get_current_data_ex(Z_ARRVAL_P(pzval), (void **)&ppzval_object, &pos);

    if(Z_TYPE_PP(ppzval_object) == IS_OBJECT) {
        zend_class_entry *parent_ce = Z_OBJCE_PP(ppzval_object);

        while(!objectFactory()->getRegisteredMetaObjects().contains(parent_ce->name)
              && parent_ce->parent != NULL)
        {
            parent_ce = parent_ce->parent;
        }

        if(objectFactory()->getRegisteredMetaObjects().contains(parent_ce->name)) {
            QObject *parent_qo = objectFactory()->getObject(*ppzval_object TSRMLS_CC);

            if(parent_qo != NULL) {
                qo->setParent(parent_qo);
                return true;
            }
        }
    }
    else if(Z_TYPE_PP(ppzval_object) == IS_LONG
            || Z_TYPE_PP(ppzval_object) == IS_NULL) {
        qo->setParent(0);
        return true;
    }

    php_error(E_WARNING, "Unknown argument 1, cannot prepare method call setParent(QObject *parent)\n");

    return false;
}

bool PHPQt5::pq_connect_ex(zval *zobj_ptr, zval *pzval TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG("PHPQt5::pq_connect_ex()");
#endif

    if(Z_TYPE_P(pzval) != IS_ARRAY) {
        php_error(E_WARNING, "Wrong zval type for method `pq_connect_ex(zval *, zval *)`. Expected type: ARRAY\n");
        return false;
    }

    zval **entry;
    HashPosition pos;
    HashTable *ht = Z_ARRVAL_P(pzval);

    zval *z_sender;
    zval *z_receiver;
    zval *z_signal;
    zval *z_slot;

    int argc = ht->nNumOfElements;

    if(argc == 3) {
        z_sender = zobj_ptr;
    }

    zend_hash_internal_pointer_reset_ex(ht, &pos);
    while (zend_hash_get_current_data_ex(ht, (void **)&entry, &pos) == SUCCESS) {
        int index = 4 - argc + pos->h;

        switch(index) {
        case 0:
            z_sender = *entry;
            break;

        case 1:
            z_signal = *entry;
            break;

        case 2:
            z_receiver = *entry;
            break;

        case 3:
            z_slot = *entry;
            break;
        }

        zend_hash_move_forward_ex(ht, &pos);
    }

    return pq_connect(z_sender, z_signal, z_receiver, z_slot, false TSRMLS_CC);
}

#define QO_SIGNAL(className, signal) className::signal

bool PHPQt5::pq_connect(zval *z_sender, zval *z_signal, zval *z_receiver, zval *z_slot, bool cDisconnect TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG("PHPQt5::pq_connect()");
#endif

    const QObject *qo_sender;
    const QObject *qo_receiver;
    const char *signal;
    const char *slot;

    /*
     * Первый параметр должен быть объектом
     */
    if(Z_TYPE_P(z_sender) != IS_OBJECT
            || !pq_test_ce(z_sender TSRMLS_CC)) {
        php_error(E_WARNING, "Unknown argument 1, cannot prepare method call connect(QObject *sender, SIGNAL(), QObject *receiver, SLOT())\n");
        return false;
    }
    qo_sender = objectFactory()->getObject(z_sender TSRMLS_CC);


    /*
     * Второй параметр должен быть строкой
     */
    if(Z_TYPE_P(z_signal) != IS_STRING) {
        php_error(E_WARNING, "Unknown argument 2, cannot prepare method call connect(QObject *sender, SIGNAL(), QObject *receiver, SLOT())\n");
        return false;
    }
    QByteArray signal_ba = QMetaObject::normalizedSignature( Z_STRVAL_P(z_signal) );
    signal = signal_ba.constData();


    /*
     * Третий параметр должен быть объектом...
     */
    if(Z_TYPE_P(z_receiver) == IS_OBJECT
            && pq_test_ce(z_receiver TSRMLS_CC)) {

        qo_receiver = objectFactory()->getObject(z_receiver TSRMLS_CC);
    }
    /*
     * или строкой qApp
     */
    else if(Z_TYPE_P(z_receiver) == IS_STRING
            && Z_STRVAL_P(z_receiver) == QString("qApp")) {
        qo_receiver = qApp;
        z_receiver = objectFactory()->getZObject(qApp);
    }
    else {
        php_error(E_WARNING, "Unknown argument 3, cannot prepare method call connect(QObject *sender, SIGNAL(), QObject *receiver, SLOT())\n");
        return false;
    }

    /*
     * Четвёртый параметр должен быть строкой
     */
    if(Z_TYPE_P(z_slot) != IS_STRING) {
        php_error(E_WARNING, "Unknown argument 4, cannot prepare method call connect(QObject *sender, SIGNAL(), QObject *receiver, SLOT())\n");
        return false;
    }
    QByteArray slot_ba = QMetaObject::normalizedSignature( Z_STRVAL_P(z_slot) );
    slot = slot_ba.constData();

    if(qo_sender != NULL && qo_receiver != NULL) {
        // баг из-за которого объект, полученный функцией c() "теряется":
        // берем ссылку на объект из основного хранилища.
        zval *zo_sender = objectFactory()->getZObject(Z_OBJVAL_P(z_sender).handle);

        if(cDisconnect) {
            qo_sender->disconnect(qo_sender, signal, qo_receiver, slot);
            phpqt5Connections->removePHPConnection(qo_sender, signal, slot TSRMLS_CC);
            return true;
        }

        int indexOfSlot = qo_sender->metaObject()->indexOfMethod(slot_ba.mid(1));

        if(indexOfSlot < 0 || !qo_sender->connect(qo_sender, signal, qo_receiver, slot)) {
            bool ok = phpqt5Connections->createPHPConnection(zo_sender, qo_sender, signal_ba, z_receiver, slot_ba TSRMLS_CC);

            if(!ok) {
                php_error(E_WARNING, QString("Cannot connect SIGNAL `%1` with SLOT `%2`\n").arg(signal).arg(slot).toUtf8().constData());
            }

            return ok;
        }
        else {
            return true;
        }
    }
    else {
        return false;
    }
}

bool PHPQt5::pq_move_to_thread(QObject *qo, zval *pzval TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG("PHPQt5::pq_move_to_thread()");
#endif

    zval **ppzval;
    HashPosition pos;
    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(pzval), &pos);

    /*
     * Первый параметр должен быть объектом QThread
     */
    zend_hash_get_current_data_ex(Z_ARRVAL_P(pzval), (void **)&ppzval, &pos);
    if(Z_TYPE_PP(ppzval) == IS_OBJECT) {
        zend_class_entry *ce_thread = Z_OBJCE_PP(ppzval);
        QString qt_class_name = QString(ce_thread->name);

        if(qt_class_name != "QThread") {
            php_error(E_WARNING, "Unknown argument 1, cannot prepare method call moveToThread(QThread *thread)\n");
            return false;
        }
    }
    else {
        php_error(E_WARNING, "Unknown argument 1, cannot prepare method call moveToThread(QThread *thread)\n");
        return false;
    }

    QObject *thread_qo = objectFactory()->getObject(*ppzval TSRMLS_CC);
    if(thread_qo != NULL)
    {
        qo->moveToThread(qobject_cast<QThread*>(thread_qo));
        return true;
    }

    return false;
}

zval *PHPQt5::pq_get_child_objects(QObject *qo, zval *pzval TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG("PHPQt5::pq_get_child_objects()");
#endif

    zval *array = NULL;
    MAKE_STD_ZVAL(array);
    array_init(array);

    if(Z_TYPE_P(pzval) != IS_ARRAY) {
        php_error(E_WARNING, "Wrong zval type for method `pq_get_child_objects(QObject *, zval *)`. Expected type: ARRAY\n");
        return array;
    }

    zval **entry;
    HashPosition pos;
    HashTable *ht = Z_ARRVAL_P(pzval);

    bool subchilds;
    if(ht->nNumOfElements == 0) {
        subchilds = true;
    }
    else if(ht->nNumOfElements == 1) {
        zend_hash_internal_pointer_reset_ex(ht, &pos);
        zend_hash_get_current_data_ex(ht, (void **)&entry, &pos);
        if( Z_TYPE_PP(entry) != IS_BOOL ) {
            ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(array);
        }
        else {
            subchilds = Z_BVAL_PP(entry);
        }
    }
    else {
        ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(array);
    }

    QObjectList childs = qo->children();

    int index = 0;
    foreach(QObject *child, childs) {
        if(!subchilds) {
            if(child->parent() != qo) {
                continue;
            }
        }

        zval* pzval = objectFactory()->getZObject(child);

        if(pzval != NULL) {
            add_index_zval(array, index, pzval);
            index++;
        }
    }

    return array;
}

#include <QReturnArgument>

void PHPQt5::pq_set_user_property(QObject *qo, const QString &property, const QVariant &value TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::pq_set_user_property()", property);
#endif

    if(!qo->metaObject()->invokeMethod(qo, "setUserProperty",
                                       Q_ARG(QString, property),
                                       Q_ARG(QVariant, value))) {
        php_error(E_WARNING,
                  QString("Can't set object property `%1`")
                  .arg(property).toUtf8().constData());
    }
}

bool PHPQt5::pq_create_php_slot(QObject *qo, const QString &qosignal, zval *pzval TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::pq_create_php_slot()", qosignal);
#endif

    QByteArray qSignalSignature_ba;

    qo->metaObject()->invokeMethod(qo, "getOnSignalSignature",
                                   Q_RETURN_ARG(QByteArray, qSignalSignature_ba),
                                   Q_ARG(QByteArray, qosignal.toUtf8()));

    if(qSignalSignature_ba.length() > 2 && Z_OBJ_HANDLER_P(pzval, get_closure)) {
        zend_function *closure = zend_get_closure_invoke_method(pzval TSRMLS_CC);

        if(closure) {
            qSignalSignature_ba = QMetaObject::normalizedSignature(qSignalSignature_ba.constData());
            qSignalSignature_ba.prepend("2");

            zval *zo_sender = objectFactory()->getZObject(qo);

            Z_ADDREF_P(pzval);

            return phpqt5Connections->createPHPConnection(zo_sender,
                                                          qo,
                                                          qSignalSignature_ba,
                                                          pzval,
                                                          "1__invoke()"
                                                          TSRMLS_CC
                                                          );
        }
    }

    return false;
}

void PHPQt5::pq_remove_connections(QObject *qo) {
    phpqt5Connections->removeConnections(qo);

    int index = PHPQt5::acceptedPHPSlots_indexes.value(qo, -1);
    if(index != -1) {
        QMutableHashIterator<QString, pq_access_function_entry> i(acceptedPHPSlots_list[index]);
        while(i.hasNext()) {
            i.next();
            if(i.value().zfn != NULL) {
                zval_ptr_dtor(&i.value().zfn);
            }

            if(i.value().zfn_name != NULL) {
                zval_ptr_dtor(&i.value().zfn_name);
            }

            if(i.value().zo != NULL) {
                Z_DELREF_P(i.value().zo);
            }

            i.value().fn_name.clear();
        }
    }
}

bool PHPQt5::pq_register_closure(QObject *qo, const QString &name, zval *pzval TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG2("PHPQt5::pq_register_closure()", name);
#endif

    zend_function *closure = zend_get_closure_invoke_method(pzval TSRMLS_CC);

    if(closure) {
        pq_access_function_entry afe {
            name,
            NULL,
            NULL,
            pzval,
            qo
        };

        int index = acceptedPHPSlots_indexes.value(qo, -1);

        if(index >= 0) {
            if(acceptedPHPSlots_list.at(index).contains(name)) {
                pq_access_function_entry oldAfe = acceptedPHPSlots_list.at(index).value(name);
                Z_DELREF_P(oldAfe.zo);
                acceptedPHPSlots_list.value(index).remove(name);
            }
        }
        else {
            QHash<QString, pq_access_function_entry> hash;
            index = acceptedPHPSlots_list.count();
            acceptedPHPSlots_indexes.insert(qo, index);
            acceptedPHPSlots_list.append(hash);
        }

        QHash<QString, pq_access_function_entry> hash =
                acceptedPHPSlots_list.value(index);

        hash.insert(name, afe);

        acceptedPHPSlots_list.replace(index, hash);

        Z_ADDREF_P(pzval);

        return true;
    }

    return false;
}

template <typename T>
bool PHPQt5::pq_test_enabledPHPSlots(QObject *qo, QString slotName)
{
    QString className1 = QString(typeid(T).name()).mid(1);
    QString className2 = qo->metaObject()->className();

    if(className1 == className2) {
        if(T *pqo = dynamic_cast<T*>(qo)) {
            if(pqo->enabledPHPSlots.contains(slotName))
            {
                return true;
            }
        }
    }

    return false;
}
