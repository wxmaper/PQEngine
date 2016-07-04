#include "pqclasses.h"
#include "phpqt5.h"

#include "plastiqobject.h"
#include "plastiqmethod.h"

void PHPQt5::zif_plastiq_connect(INTERNAL_FUNCTION_PARAMETERS)
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

    PQDBGLPUP("start");
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "osos", &sender, &signal, &signal_len, &receiver, &slot, &slot_len) == FAILURE) {
        PQDBG_LVL_DONE();
        return;
    }

    QByteArray signalSignature = QByteArray(signal).mid(1);
    QByteArray signalName = signalSignature.mid(0, signalSignature.indexOf("("));

    QByteArray slotSignature = QByteArray(slot).mid(1);
    QByteArray slotName = slotSignature.mid(0, slotSignature.indexOf("("));

    PQDBGLPUP("start");
    zend_class_entry *sender_ce = Z_OBJCE_P(sender);
    QByteArray senderClassName;
    do {
        if(objectFactory()->havePlastiQMetaObject(sender_ce->name->val PQDBG_LVL_CC)) {
            senderClassName = QByteArray(sender_ce->name->val);
            break;
        }
    } while(sender_ce = sender_ce->parent);
    PQDBGLPUP(QString("senderClassName: %1").arg(senderClassName.constData()));

    zend_class_entry *receiver_ce = Z_OBJCE_P(receiver);
    QByteArray receiverClassName;
    do {
        if(objectFactory()->havePlastiQMetaObject(receiver_ce->name->val PQDBG_LVL_CC)) {
            receiverClassName = QByteArray(receiver_ce->name->val);
            break;
        }
    } while(receiver_ce = receiver_ce->parent);
    PQDBGLPUP(QString("receiverClassName: %1").arg(receiverClassName.constData()));

    PQObjectWrapper *sender_pqobject = fetch_pqobject(Z_OBJ_P(sender));
    PQObjectWrapper *receiver_pqobject = fetch_pqobject(Z_OBJ_P(receiver));

    PlastiQObject *sender_object = sender_pqobject->object;
    PlastiQObject *receiver_object = receiver_pqobject->object;

    if((sender_object->objectType() == PlastiQObject::IsQObject
        || sender_object->objectType() == PlastiQObject::IsQWidget
        || sender_object->objectType() == PlastiQObject::IsQWindow)
            && (receiver_object->objectType() == PlastiQObject::IsQObject
                || receiver_object->objectType() == PlastiQObject::IsQWidget
                || receiver_object->objectType() == PlastiQObject::IsQWindow))
    {
        if(sender_object->metaObject()->d.pq_signals->contains(signalSignature)) {
            PlastiQMetaObject::connect(sender_pqobject, signalSignature, receiver_pqobject, slotName);
        }
    }

    PQDBG_LVL_DONE_LPUP();
}

void PHPQt5Connection_invoke_new(PQObjectWrapper *sender, PQObjectWrapper *receiver,
                                 const QByteArray &slot, const QByteArray &signature,
                                 const int argc, const PMOGStack &stack)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    PQDBGLPUP(signature.constData());

    zval function_name, retval, z_receiver, z_sender;
    zval *params = argc ? new zval[argc] : NULL;

    ZVAL_STRING(&function_name, slot.constData());
    ZVAL_OBJ(&z_receiver, &receiver->zo);
    ZVAL_OBJ(&z_sender, &sender->zo);
    Z_ADDREF(z_sender);

    for(int i = 0; i < argc; i++) { // с этим что-то нужно делать :)
       // QVariant arg = args.at(i-1);
       // params[i] = PHPQt5::pq_cast_to_zval(arg, true PQDBG_LVL_CC);
    }

    /*
    if(call_user_function(NULL, &z_receiver, &function_name, &retval, argc, params) == FAILURE) {
        QString s = QString("PHPQt5 could not call method: %1 of class: %2")
                .arg(Z_STRVAL(function_name))
                .arg(Z_OBJCE(z_receiver)->name->val);

        php_error(E_ERROR, s.toUtf8().constData());
    }

    */

    zval symbol_table;
    array_init(&symbol_table);

    zend_string *var_name = zend_string_init("sender", sizeof("sender")-1, 1);
    zend_hash_update(Z_ARRVAL(symbol_table), var_name, &z_sender);

    zend_fcall_info fci;
    fci.size = sizeof(fci);
    fci.function_name = function_name;
    fci.retval = &retval;
    fci.params = params;
    fci.function_table = NULL;
    fci.object = &receiver->zo;
    fci.param_count = argc;
    fci.no_separation = 0;
    fci.symbol_table = Z_ARRVAL(symbol_table);

    if(zend_call_function(&fci, NULL) == SUCCESS) {
        qDebug() << "ok";
    }
    else {
        qDebug() << "ne ok";
    }

    delete [] params;

    zval_dtor(&retval);
    zval_dtor(&function_name);
    // zval_dtor(&symbol_table); // не нужно
    zend_string_free(var_name);

    PQDBG_LVL_DONE_LPUP();
}
