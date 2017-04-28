#include "phpqt5.h"

#include <QMutex>

bool PHPQt5::plastiqConnect(zval *z_sender,
                            const QString &signalSignature,
                            zval *z_receiver,
                            const QString &slotSignature,
                            bool isOnSignal)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("signal signature: %1").arg(signalSignature));
    PQDBGLPUP(QString("slot signature: %1").arg(slotSignature));
#endif

    if(slotSignature == ZEND_INVOKE_FUNC_NAME) {
        PQDBGLPUP("CLOSURE CONNECTION");

        if(Z_OBJ_HANDLER_P(z_receiver, get_closure)) {
            zend_function *closure = zend_get_closure_invoke_method(Z_OBJ_P(z_receiver));

            if(closure) {
                PQObjectWrapper *sender_pqobject = fetch_pqobject(Z_OBJ_P(z_sender));
                PlastiQObject *sender = sender_pqobject->object;
                const PlastiQMetaObject *sender_metaObject = sender->plastiq_metaObject();

                QByteArray signalName;

                if(isOnSignal) { // onSignalName -> signalName
                    signalName = (signalSignature.mid(2).left(1).toLower() + signalSignature.mid(3)).toUtf8();
                }
                else { // normal signal signature
                    if(signalSignature.at(0).isDigit()) {
                        if(signalSignature.left(1) == "2") {
                            signalName = signalSignature.mid(1, signalSignature.indexOf("(")-1).toUtf8();
                        }
                        else {
                            PQDBG_LVL_DONE_LPUP();
                            return false; // FIXME: error
                        }
                    }
                    else {
                        signalName = signalSignature.mid(0, signalSignature.indexOf("(")).toUtf8();
                    }
                }
                PQDBGLPUP(QString("signal name: %1").arg(signalName.constData()));

                PQDBGLPUP(QString("test internal connections: %1").arg(signalName.constData()));
                // extra objects may not have a connections
                if(!sender_pqobject->connections) {
                    if(sender_pqobject->isValid) {
                        sender_pqobject->connections = new QHash<QByteArray,ConnectionHash*>();
                    }
                    else {
                        php_error(E_ERROR, "Object have been destroyed");
                        PQDBG_LVL_DONE_LPUP();
                        return false;
                    }
                }

                QHashIterator<QByteArray,ConnectionHash*> iter(*sender_pqobject->connections);
                while(iter.hasNext()) {
                    iter.next();

                    QByteArray iterSignalName = iter.key().mid(0, iter.key().indexOf("("));
                    if(iterSignalName == signalName) {
                        PQDBGLPUP(QString("create internal connection: %1::%2 [idx:%3] -> %4::%5")
                                  .arg(Z_OBJCE_NAME_P(z_sender))
                                  .arg(signalName.constData())
                                  .arg(-1)
                                  .arg(Z_OBJCE_NAME_P(z_receiver))
                                  .arg(ZEND_INVOKE_FUNC_NAME));

                        PQObjectWrapper *receiver_pqobject = new PQObjectWrapper;
                        receiver_pqobject->object = Q_NULLPTR;
                        receiver_pqobject->zoptr = Z_OBJ_P(z_receiver);
                        receiver_pqobject->isClosure = true;
                        receiver_pqobject->isExtra = true;

                        PQDBGLPUP(QString("change thread %1[id:%2] -> %3")
                                  .arg(receiver_pqobject->zo.ce->name->val)
                                  .arg(reinterpret_cast<quint64>(receiver_pqobject->object->plastiq_data()))
                                  .arg(reinterpret_cast<quint64>(sender_pqobject->thread)));

                        receiver_pqobject->thread = sender_pqobject->thread;

                        Z_ADDREF_P(z_receiver);

                        QByteArray connectSignature = QString(iterSignalName).append(":").append(slotSignature).toUtf8();
                        iter.value()->insert(connectSignature, {
                                                 sender_pqobject, signalName,
                                                 receiver_pqobject, ZEND_INVOKE_FUNC_NAME
                                             });

                        PQDBG_LVL_DONE_LPUP();
                        return true;
                    }
                }

                QList<PlastiQCandidateMethod> candidates;
                if(isOnSignal) {
                    candidates = sender_metaObject->candidates(signalName, -1, PlastiQMethod::Signal);
                }
                else {
                    QString args = signalSignature.mid( signalSignature.indexOf("("), signalSignature.indexOf(")") );

                    int argc = 0;
                    if(args != "()") {
                        argc = args.split(",").size();
                    }

                    candidates = sender_metaObject->candidates(signalName, argc, PlastiQMethod::Signal);
                }

                if(candidates.size()) {
                    PQObjectWrapper *receiver_pqobject = new PQObjectWrapper;
                    receiver_pqobject->object = Q_NULLPTR;
                    receiver_pqobject->zoptr = Z_OBJ_P(z_receiver);
                    receiver_pqobject->isClosure = true;
                    receiver_pqobject->isExtra = true;

                    PQDBGLPUP(QString("change thread %1[id:%2] -> %3")
                              .arg("CLOSURE")
                              .arg("?")
                              .arg(reinterpret_cast<quint64>(sender_pqobject->thread)));

                    receiver_pqobject->thread = sender_pqobject->thread;

                    PlastiQCandidateMethod candidateSignal = candidates.at(0);

                    PMOGStack stack = new PMOGStackItem[10];
                    stack[1].s_voidp = reinterpret_cast<void*>(sender_pqobject);
                    stack[2].s_voidp = reinterpret_cast<void*>(receiver_pqobject);
                    stack[3].s_bytearray = QByteArray(ZEND_INVOKE_FUNC_NAME);

                    Z_ADDREF_P(z_receiver);

                    PQDBGLPUP(QString("create closure connection: %1::%2 [idx:%3] -> %4::%5")
                              .arg(sender_metaObject->className())
                              .arg(candidateSignal.name.constData())
                              .arg(candidateSignal.idx)
                              .arg(Z_OBJCE_NAME_P(z_receiver))
                              .arg(ZEND_INVOKE_FUNC_NAME));

                    sender_metaObject->d.static_metacall(sender,
                                                         PlastiQMetaObject::CreateConnection,
                                                         candidateSignal.idx,
                                                         stack);

                    delete [] stack;
                }
                else {
                    PQDBGLPUP(QString("QObject::connect: No such signal %1::%2").arg(sender_metaObject->className()).arg(signalSignature));
                    php_error(E_WARNING, "QObject::connect: No such signal %s::%s", sender_metaObject->className(), signalSignature.toUtf8().constData());
                    // FIXME: add error message
                }
            }
        }
    }
    else {
        PQDBGLPUP("GENERAL CONNECTION");

        zend_class_entry *sender_ce = Z_OBJCE_P(z_sender);
        QByteArray senderClassName;
        int sender_ce_lvl = 0;
        do {
            if(objectFactory()->havePlastiQMetaObject(sender_ce->name->val)) {
                senderClassName = QByteArray(sender_ce->name->val);
                break;
            }
            sender_ce_lvl++;
        } while(sender_ce = sender_ce->parent);

        if(!senderClassName.length()) {
            PQDBG_LVL_DONE_LPUP();
            return false; // FIXME: add error message
        }

        PQDBGLPUP(QString("senderClassName: %1").arg(senderClassName.constData()));

        zend_class_entry *receiver_ce = Z_OBJCE_P(z_receiver);
        QByteArray receiverClassName;
        int receiver_ce_lvl = 0;
        do {
            if(objectFactory()->havePlastiQMetaObject(receiver_ce->name->val)) {
                receiverClassName = QByteArray(receiver_ce->name->val);
                break;
            }
            receiver_ce_lvl++;
        } while(receiver_ce = receiver_ce->parent);

        if(!receiverClassName.length()) {
            PQDBG_LVL_DONE_LPUP();
            return false; // FIXME: add error message
        }

        PQDBGLPUP(QString("receiverClassName: %1").arg(receiverClassName.constData()));

        /////////////////////////////////////////////////////////////////////
        QByteArray signalName;
        QByteArray cSignalSignature;
        if(signalSignature.at(0).isDigit()) {
            if(signalSignature.left(1) == "2") {
                signalName = signalSignature.mid(1, signalSignature.indexOf("(")-1).toUtf8();
                cSignalSignature = signalSignature.mid(1).toUtf8();
            }
            else {
                PQDBG_LVL_DONE_LPUP();
                return false; // FIXME: error
            }
        }
        else {
            signalName = signalSignature.mid(0, signalSignature.indexOf("(")).toUtf8();
            cSignalSignature = signalSignature.toUtf8();
        }
        PQDBGLPUP(QString("signal name: %1").arg(signalName.constData()));

        QByteArray slotName;
        QByteArray cSlotSignature;
        if(slotSignature.at(0).isDigit()) {
            if(slotSignature.left(1) == "1") {
                slotName = slotSignature.mid(1, slotSignature.indexOf("(")-1).toUtf8();
                cSlotSignature = slotSignature.mid(1).toUtf8();
            }
            else {
                PQDBG_LVL_DONE_LPUP();
                return false; // FIXME: error
            }
        }
        else {
            slotName = slotSignature.mid(0, slotSignature.indexOf("(")).toUtf8();
        }
        PQDBGLPUP(QString("slot name: %1").arg(slotName.constData()));

        PQObjectWrapper *sender_pqobject = fetch_pqobject(Z_OBJ_P(z_sender));
        PlastiQObject *sender_object = sender_pqobject->object;

        PQObjectWrapper *receiver_pqobject = fetch_pqobject(Z_OBJ_P(z_receiver));
        PlastiQObject *receiver_object = receiver_pqobject->object;

        // отправитель должен быть объектом Q_OBJECT
        if(sender_object->plastiq_objectType() != PlastiQ::IsQObject
                && sender_object->plastiq_objectType() != PlastiQ::IsQWidget
                && sender_object->plastiq_objectType() != PlastiQ::IsQWindow) {
            PQDBGLPUP(QString("%1 is not Q_OBJECT").arg(senderClassName.constData()));

            PQDBG_LVL_DONE_LPUP();
            return false; // FIXME: add error message
        }

        // получатель должен быть объектом Q_OBJECT
        if(receiver_object->plastiq_objectType() == PlastiQ::IsQObject
                || receiver_object->plastiq_objectType() == PlastiQ::IsQWidget
                || receiver_object->plastiq_objectType() == PlastiQ::IsQWindow) {
            QObject *sender_qo = reinterpret_cast<QObject*>(sender_object->plastiq_data());
            QObject *receiver_qo = reinterpret_cast<QObject*>(receiver_object->plastiq_data());

            PQDBGLPUP("create Qt connection...");
            if(sender_qo != Q_NULLPTR && receiver_qo != Q_NULLPTR
                    && signalSignature.left(1) == "2"
                    && slotSignature.left(1) == "1") {
                int indexOfSignal = sender_qo->metaObject()->indexOfSignal(QMetaObject::normalizedSignature(signalSignature.mid(1).toUtf8().constData()).constData());
                int indexOfSlot = receiver_qo->metaObject()->indexOfSlot(QMetaObject::normalizedSignature(slotSignature.mid(1).toUtf8().constData()).constData());

                PQDBGLPUP(QString("indexOfSignal `%1`: %2 -> indexOfSlot `%3`: %4")
                          .arg(QMetaObject::normalizedSignature(signalSignature.mid(1).toUtf8().constData()).constData())
                          .arg(indexOfSignal)
                          .arg(QMetaObject::normalizedSignature(slotSignature.mid(1).toUtf8().constData()).constData())
                          .arg(indexOfSlot));
                if(indexOfSignal >= 0 && indexOfSlot >= 0
                        && QObject::connect(sender_qo, signalSignature.toUtf8().constData(),
                                            receiver_qo, slotSignature.toUtf8().constData())) {
                    PQDBGLPUP("Qt connection created");
                    PQDBG_LVL_DONE_LPUP();
                    return true;
                }
                else {
                    PQDBGLPUP("not created");
                }
            }
            else {
                PQDBGLPUP(QString("not created [ %1 -> %2 ]")
                          .arg(reinterpret_cast<quint64>(sender_qo))
                          .arg(reinterpret_cast<quint64>(receiver_qo)));
            }

            QString args = signalSignature.mid( signalSignature.indexOf("("), signalSignature.indexOf(")") );

            int argc = 0;
            if(args != "()") {
                argc = args.split(",").size();
            }

            if(sender_pqobject->connections == Q_NULLPTR) {
                sender_pqobject->connections = new QHash<QByteArray,ConnectionHash*>;
            }

            ConnectionHash *connections = sender_pqobject->connections->value(cSignalSignature, Q_NULLPTR );
            if(connections != Q_NULLPTR) {
                PQDBGLPUP(QString("create internal connection: %1::%2 [idx:%3] -> %4::%5")
                          .arg(senderClassName.constData())
                          .arg(signalName.constData())
                          .arg(-1)
                          .arg(receiverClassName.constData())
                          .arg(slotName.constData()));

                QByteArray connectSignature = QByteArray(cSignalSignature).append(":").append(cSlotSignature);
                connections->insert(connectSignature, {
                                        sender_pqobject, signalName,
                                        receiver_pqobject, slotName
                                    });

                PQDBG_LVL_DONE_LPUP();
                return true;
            }

            QList<PlastiQCandidateMethod> candidates = sender_object->plastiq_metaObject()->candidates(signalName, argc, PlastiQMethod::Signal);

            if(candidates.size()) {
                PlastiQCandidateMethod candidateSignal = candidates.at(0);

                PMOGStack stack = new PMOGStackItem[10];
                stack[1].s_voidp = reinterpret_cast<void*>(sender_pqobject);
                stack[2].s_voidp = reinterpret_cast<void*>(receiver_pqobject);
                stack[3].s_bytearray = slotName;

                PQDBGLPUP(QString("create connection: %1::%2 [idx:%3] -> %4::%5")
                          .arg(senderClassName.constData())
                          .arg(candidateSignal.name.constData())
                          .arg(candidateSignal.idx)
                          .arg(receiverClassName.constData())
                          .arg(slotName.constData()));

                sender_object->plastiq_metaObject()->d.static_metacall(sender_object,
                                                                       PlastiQMetaObject::CreateConnection,
                                                                       candidateSignal.idx,
                                                                       stack);

                delete [] stack;
                PQDBG_LVL_DONE_LPUP();
                return true;
            }
            else {
                PQDBGLPUP(QString("QObject::connect: No such signal %1::%2").arg(senderClassName.constData()).arg(cSignalSignature.constData()));
                php_error(E_WARNING, "QObject::connect: No such signal %s::%s", senderClassName.constData(), cSignalSignature.constData());
                PQDBG_LVL_DONE_LPUP();
                return false;
                // FIXME: add error message
            }
        }
        else {
            PQDBGLPUP(QString("%1 is not Q_OBJECT").arg(receiverClassName.constData()));
            PQDBG_LVL_DONE_LPUP();
            return false;
            // FIXME: add error message
        }
    }

    PQDBG_LVL_DONE_LPUP();
    return false;
}

bool PlastiQ_event(QObject *eventFilter, QObject *obj, QEvent *event)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    quint64 receiverId = reinterpret_cast<quint64>(eventFilter);
    PQObjectWrapper *receiver = Q_NULLPTR;
    bool prevent = false;

    if(receiver = PHPQt5::objectFactory()->getObject(receiverId)) {
        PQDBGLPUP(QString("receiver: %1; metaObject: %2")
                  .arg(ZSTR_VAL(receiver->zo.ce->name))
                  .arg(receiver->object->plastiq_metaObject()->className()));
        if(!strcmp(ZSTR_VAL(receiver->zo.ce->name), receiver->object->plastiq_metaObject()->className())) {
            // not inheritance (no extend PQEventFilter)
            PQDBGLPUP("not inheritance");
            PQDBG_LVL_DONE();
            return prevent;
        }

        QByteArray senderClassName = obj->metaObject()->className();

        quint64 senderId = reinterpret_cast<quint64>(obj);
        PQObjectWrapper *sender = PHPQt5::objectFactory()->getObject(senderId);

        if (sender == Q_NULLPTR) {
            if (PHPQt5::objectFactory()->havePlastiQMetaObject(senderClassName)) {
                zval sender_zv = PHPQt5::pq_create_extra_object(senderClassName, obj, true, true);
                sender = fetch_pqobject(Z_OBJ(sender_zv));
            }
            else {
                PQDBG_LVL_DONE();
                // FIXME: ошибка, класс не найден! Нельзя передать sender в функцию-слот
                return false;
            }
        }

        PMOGStack stack = new PMOGStackItem[1];

        stack[0].s_voidp = reinterpret_cast<void*>(event);
        stack[0].name = "QEvent";
        stack[0].type = PlastiQ::QEventStar;

        prevent = PlastiQ_activate(sender, "event", receiver, "eventFilter", 1, stack);

        delete [] stack;
        stack = Q_NULLPTR;
    }

    PQDBGLPUP(QString("propagate: %1").arg(prevent));

    PQDBG_LVL_DONE();
    return prevent;
}

bool PlastiQ_anon_activate(QObject *receiverObject, const char *slot,
                           int argc, const PMOGStack &stack)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    quint64 receiverId = reinterpret_cast<quint64>(receiverObject);
    PQObjectWrapper *receiver = Q_NULLPTR;
    bool ok = false;

    if(receiver = PHPQt5::objectFactory()->getObject(receiverId)) {
        PQDBGLPUP(QString("receiver: %1; metaObject: %2")
                  .arg(ZSTR_VAL(receiver->zo.ce->name))
                  .arg(receiver->object->plastiq_metaObject()->className()));
        ok = PlastiQ_activate(Q_NULLPTR, "", receiver, slot, argc, stack);
    }
    else {
        PQDBG_LVL_DONE_LPUP();
        pq_php_warning(QString("Failure to call <b>%1</b> method: cannot find receiver object.")
                       .arg(slot));
    }

    PQDBG_LVL_DONE_LPUP();
    return ok;
}

bool PlastiQ_activate(PQObjectWrapper *sender, const char *signal,
                      PQObjectWrapper *receiver, const char *slot,
                      int argc, const PMOGStack &stack)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
    PQDBGLPUP(QString("sender: %1").arg(reinterpret_cast<quint64>(sender)));
    PQDBGLPUP(QString("receiver: %1").arg(reinterpret_cast<quint64>(receiver)));
#endif

    if(sender != Q_NULLPTR) {
        argc++; // becouse "PHP-7 doesn't support symbol_table substitution for functions"...
    }

    // params
    PQDBGLPUP(QString("prepare params [%1]").arg(argc));
    zval *params = argc ? new zval[argc] : NULL;

    int startIdx = 0;
    if(sender != Q_NULLPTR) {
        PQDBGLPUP("append sender object");
        zval z_sender;
        ZVAL_OBJ(&z_sender, &sender->zo);

        params[0] = z_sender; // need becouse "PHP-7 doesn't support symbol_table substitution for functions"...
        startIdx = 1;
    }

    for(int i = startIdx; i < argc; i++) {
        PQDBGLPUP(QString("plastiq_cast_to_zval(stack[%1])").arg(i-startIdx));
        params[i] = PHPQt5::plastiq_cast_to_zval(stack[i-startIdx]); // int i = 1; and [i-1] becouse "PHP-7 doesn't support symbol_table substitution for functions"...
    }

    bool ret = PHPQt5::activateConnection(sender, signal, receiver, slot, argc, params);

    if(argc) {
        delete [] params;
        params = Q_NULLPTR;
    }

    PQDBG_LVL_DONE();
    return ret;
}

bool PHPQt5::activateConnection(PQObjectWrapper *sender, const char *signal,
                                PQObjectWrapper *receiver, const char *slot,
                                int argc, zval *params, bool dtor_params)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    //PQDBGLPUP(QString("sender: %1").arg(reinterpret_cast<quint64>(sender != Q_NULLPTR ? sender->object->plastiq_data() : 0)));
    //PQDBGLPUP(QString("sender thread: %1").arg(sender != Q_NULLPTR ? reinterpret_cast<quint64>(sender->thread) : 0));
    //!!!! PQDBGLPUP(QString("receiver: %1").arg(receiver != Q_NULLPTR ? reinterpret_cast<quint64>(receiver->object) : 0));
    //PQDBGLPUP(QString("receiver thread: %1").arg(receiver != Q_NULLPTR ? reinterpret_cast<quint64>(receiver->thread) : 0));

    static QMutex workersMutex;
    static QHash<QByteArray,PlastiQThreadWorker*> workers;

    if(sender != Q_NULLPTR &&
            sender->thread != receiver->thread) {
        PQDBGLPUP("THREAD CONNECTION to receiver");

        PQDBGLPUP(QString("sender thread: %1").arg(reinterpret_cast<quint64>(sender->thread)));

        QByteArray thSig = QString("%1:%2").arg(reinterpret_cast<quint64>(sender->thread)).arg(reinterpret_cast<quint64>(receiver->thread)).toUtf8();

        workersMutex.lock();
        PlastiQThreadWorker *worker = workers.value(thSig, Q_NULLPTR);

        if(!worker) {
            PQDBGLPUP("create worker");
            worker = new PlastiQThreadWorker;
            worker->moveToThread(receiver->thread);

            QObject::connect(worker, SIGNAL(ready(int)),
                             worker, SLOT(activate(int)));

            QObject::connect(receiver->thread, SIGNAL(destroyed(QObject*)),
                             worker, SLOT(deleteLater()));

            workers.insert(thSig, worker);
        }

        workersMutex.unlock();

        //PQDBGLPUP("ADDREF for $sender");
        //Z_ADDREF(params[0]);

        PQDBGLPUP("worker->setReady()");
        worker->setReady(sender, signal, receiver, slot, argc, params, dtor_params);

        PQDBG_LVL_DONE_LPUP();
        return true;
    }
    else if(sender != Q_NULLPTR &&
            (sender->thread != QThread::currentThread() ||
             receiver->thread != QThread::currentThread())) {
        PQDBGLPUP("THREAD CONNECTION to currentThread");

        PQDBGLPUP(QString("sender thread: %1").arg(reinterpret_cast<quint64>(sender->thread)));

        QByteArray thSig = QString("%1:%2").arg(reinterpret_cast<quint64>(sender->thread)).arg(reinterpret_cast<quint64>(receiver->thread)).toUtf8();

        workersMutex.lock();
        PlastiQThreadWorker *worker = workers.value(thSig, Q_NULLPTR);

        if(!worker) {
            PQDBGLPUP("create worker");
            worker = new PlastiQThreadWorker;
            worker->moveToThread(QThread::currentThread());

            QObject::connect(worker, SIGNAL(ready(int)),
                             worker, SLOT(activate(int)));

            QObject::connect(receiver->thread, SIGNAL(destroyed(QObject*)),
                             worker, SLOT(deleteLater()));

            workers.insert(thSig, worker);
        }

        workersMutex.unlock();

        //PQDBGLPUP("ADDREF for $sender");
        //Z_ADDREF(params[0]);

        PQDBGLPUP("worker->setReady()");
        worker->setReady(sender, signal, receiver, slot, argc, params, dtor_params);

        PQDBG_LVL_DONE_LPUP();
        return true;

    }
    else {
        PQDBGLPUP("GENERAL CONNECTION");

        bool ok = doActivateConnection(sender, signal, receiver, slot, argc, params, dtor_params);

        PQDBG_LVL_DONE_LPUP();
        return ok;
    }

    PQDBG_LVL_DONE_LPUP();
    return false;
}

bool PHPQt5::doActivateConnection(PQObjectWrapper *sender, const char *signal,
                                  PQObjectWrapper *receiver, const char *slot,
                                  int argc, zval *params, bool dtor_params)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#else
    Q_UNUSED(signal)
#endif

    PQDBGLPUP("TSRMLS_CACHE_UPDATE");
    void *old_tsrmls_cache = Q_NULLPTR;
    void *TSRMLS_CACHE = Q_NULLPTR;
    TSRMLS_CACHE_UPDATE();

    PQDBGLPUP(QString("TSRMLS_CACHE: %1").arg(reinterpret_cast<quint64>(TSRMLS_CACHE)));
    PQDBGLPUP(QString("QThread: %1").arg(reinterpret_cast<quint64>(QThread::currentThread())));

    if (TSRMLS_CACHE == Q_NULLPTR || TSRMLS_CACHE == 0) {
        TSRMLS_CACHE = PlastiQThreadCreator::get_tsrmls_cache(QThread::currentThread());
        PQDBGLPUP(QString("TSRMLS_CACHE: %1").arg(reinterpret_cast<quint64>(TSRMLS_CACHE)));
        //PQDBGLPUP("tsrm_new_interpreter_context");
        //TSRMLS_CACHE = tsrm_new_interpreter_context();

        // PQDBGLPUP("php_request_startup");
        // php_request_startup();
    }

    /*
    if(sender != Q_NULLPTR &&
            (sender->thread != receiver->thread ||
             sender->thread != QThread::currentThread() ||
             receiver->thread != QThread::currentThread())) {
        old_tsrmls_cache = tsrm_set_interpreter_context(TSRMLS_CACHE);
        PQDBGLPUP(QString("tsrm_set_interpreter_context from `%1` to `%2`")
                  .arg(reinterpret_cast<quint64>(old_tsrmls_cache))
                  .arg(reinterpret_cast<quint64>(TSRMLS_CACHE)));
    }
    */

    static QMap<QByteArray, zend_fcall_info_cache> call_cache;

    zval z_receiver;
    quint64 receiverId = 0;
    if (receiver->isClosure) {
        PQDBGLPUP("CLOSURE CONNECTION");
        ZVAL_OBJ(&z_receiver, receiver->zoptr);
        receiverId = reinterpret_cast<quint64>(receiver->zoptr);
    }
    else {
        PQDBGLPUP("NORMAL CONNECTION");
        ZVAL_OBJ(&z_receiver, &receiver->zo);
        receiverId = reinterpret_cast<quint64>(receiver->object->plastiq_data());
    }

#ifdef PQDEBUG
    if (sender != Q_NULLPTR) {
        zval z_sender;
        ZVAL_OBJ(&z_sender, &sender->zo);

        PQDBGLPUP(QString("activate connection: %1::%2 -> %3::%4")
                  .arg(Z_OBJCE_NAME(z_sender))
                  .arg(signal)
                  .arg(Z_OBJCE_NAME(z_receiver))
                  .arg(slot));

        PQDBGLPUP(QString("sender: %1:%2")
                  .arg(reinterpret_cast<quint64>(sender->object->plastiq_data()))
                  .arg(Z_OBJ_HANDLE(z_sender)));
    }
    else {
        PQDBGLPUP(QString("activate connection: %1::%2 -> %3::%4")
                  .arg("Q_NULLPTR")
                  .arg("unknown")
                  .arg(Z_OBJCE_NAME(z_receiver))
                  .arg(slot));
    }

    PQDBGLPUP(QString("receiver: %1:%2")
              .arg(receiverId)
              .arg(Z_OBJ_HANDLE(z_receiver)));
#endif

    // call info
    PQDBGLPUP("generate call info");
    PQDBGLPUP(QString("param_count: %1").arg(argc));
    zval retval;
    zend_fcall_info fci;
    fci.size = sizeof(fci);
    fci.object = Z_OBJ(z_receiver);
    fci.param_count = argc;
    fci.params = params;
    fci.retval = &retval;
    fci.no_separation = 0;

#if (PHP_VERSION_ID < 70101)
    fci.function_table = NULL; // removed in php 7.1.1
#endif

    PQDBGLPUP(QString("function_name: %1").arg(slot));
    ZVAL_STRINGL(&fci.function_name, slot, strlen(slot));

#if (PHP_VERSION_ID < 70101)
    PQDBGLPUP("symbol_table");
    fci.symbol_table = NULL; // removed in php 7.1.1
#endif

    // call cache
    PQDBGLPUP("check call cache");
    QByteArray functionSig = QByteArray(Z_OBJCE_NAME(z_receiver)).append(receiverId).append(slot);
    bool fcached = call_cache.contains(functionSig);
    zend_fcall_info_cache fcc = fcached ? call_cache.value(functionSig) : zend_fcall_info_cache({ 0 });

    //    PQDBGLPUP("update scope");
    //    zend_class_entry *old_scope = EG(scope);
    //    EG(scope) = Z_OBJCE(z_receiver);

    PQDBGLPUP("zend_call_function");
    if (zend_call_function(&fci, &fcc) == SUCCESS) {
        if(!fcached) {
            PQDBGLPUP("update call cache");
            call_cache.insert(functionSig, fcc);
        }
    }
    else {
        PQDBGLPUP("ERROR");
    }

    //    PQDBGLPUP("restore scope");
    //    EG(scope) = old_scope;

    bool ret = false;
    if (Z_TYPE(retval) == IS_TRUE
            || (Z_TYPE(retval) == IS_LONG && Z_LVAL(retval) != 0)
            || (Z_TYPE(retval) == IS_STRING && strlen(Z_STRVAL(retval)))) {
        ret = true;
    }

    zval_dtor(&retval);
    zval_dtor(&fci.function_name);

    if (dtor_params) {
        zval_dtor(params);
    }

    if (sender == Q_NULLPTR && receiver->isClosure) {
        Z_DELREF(z_receiver);
        delete receiver;
        receiver = 0;
    }

    // zval_dtor(&symbol_table); // не нужно (!)

    if (old_tsrmls_cache != Q_NULLPTR) {
        TSRMLS_CACHE = tsrm_set_interpreter_context(old_tsrmls_cache);

        PQDBGLPUP(QString("tsrm_set_interpreter_context from `%1` to `%2`")
                  .arg(reinterpret_cast<quint64>(TSRMLS_CACHE))
                  .arg(reinterpret_cast<quint64>(old_tsrmls_cache)));
    }

#ifdef PQDEBUG
    if (sender != Q_NULLPTR) {
        zval z_sender;
        ZVAL_OBJ(&z_sender, &sender->zo);

        PQDBGLPUP(QString("end connection: %1::%2 -> %3::%4")
                  .arg(Z_OBJCE_NAME(z_sender))
                  .arg(signal)
                  .arg(Z_OBJCE_NAME(z_receiver))
                  .arg(slot));
    }
    else {
        PQDBGLPUP(QString("end activate connection: %1::%2 -> %3::%4")
                  .arg("Q_NULLPTR")
                  .arg("unknown")
                  .arg(Z_OBJCE_NAME(z_receiver))
                  .arg(slot));
    }
#endif

    PQDBG_LVL_DONE_LPUP();
    return ret;
}

void VirtualMethod::call(PQObjectWrapper *pqobject, PMOGStack stack) const
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    zval *params = new zval[_argc];

    for (int i = 0; i < _argc; i++) {
        zval tmpz = PHPQt5::plastiq_cast_to_zval(stack[i+1]);
        ZVAL_ZVAL(&params[i], &tmpz, 1, 0);
    }

    PHPQt5::doActivateConnection(Q_NULLPTR, "VirtualMethod::call", pqobject,
                                 _functionName.constData(), _argc, params, true);

    /*
    PQDBGLPUP("generate call info");
    zval retval;
    zend_fcall_info fci;
    fci.size = sizeof(fci);
    fci.object = &pqobject->zo;
    fci.param_count = _argc;
    fci.params = params;
    fci.retval = &retval;
    fci.no_separation = 0;

    PQDBGLPUP("symbol_table");
    ZVAL_STRINGL(&fci.function_name,
                 _functionName.constData(),
                 _functionName.length());

    // call cache
    PQDBGLPUP("check call cache");
    zend_fcall_info_cache fcc = zend_fcall_info_cache({ 0 });

    PQDBGLPUP("zend_call_function");
    if(zend_call_function(&fci, &fcc) == SUCCESS) {
        stack[0] = PHPQt5::plastiq_cast_to_stackItem(&retval);
    }
    else {
        PQDBGLPUP("ERROR");
    }
    */

    PQDBG_LVL_DONE_LPUP();
}

void PlastiQ_connectSlotsByName(PQObjectWrapper *pqobject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    // connect slots by name
    ulong num_key;
    zend_string *key;
    zval *zv;
    zend_op_array *op_array;

    HashTable *ht = (&pqobject->zo.ce->function_table);

    //PQObjectWrapper *pqobjectWidget = fetch_pqobject(Z_OBJ(zWidget));
    //QObject *widget = pqobjectWidget->object->plastiq_toQObject();
    QHash<QString,zval> objectList; // objectName -> zval
    objectList = PHPQt5::loadChilds(pqobject->object->plastiq_toQObject());

    zval zobject;
    ZVAL_OBJ(&zobject, &pqobject->zo);

    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, key, zv) {
        if (key) { //HASH_KEY_IS_STRING
            const QString functionName(key->val);

            int endObjectName = functionName.indexOf("_", 4);

            if (functionName.startsWith("on_") && endObjectName > 0) {
                op_array = (zend_op_array*) Z_PTR_P(zv);
                if (op_array->doc_comment && op_array->doc_comment->val) {
                    const QString docComment(op_array->doc_comment->val);
                    const QRegExp rx(QStringLiteral("@slot on_(.*)_(.*)(\\(.*\\))"));

                    if (rx.indexIn(docComment) > 0) {
                        const QString objectName = rx.cap(1);
                        const QString signalName = rx.cap(2);
                        QString args = rx.cap(3);

                        QRegExp rx2(QStringLiteral("\\((.*,)"));
                        rx2.setMinimal(true);
                        if (args.indexOf(",") > 0) args.replace(rx2, "(");
                        else args = "()";

                        if (objectList.contains(objectName)) {
                            zval lzv = objectList.take(objectName);
                            PHPQt5::plastiqConnect(&lzv, signalName + args,
                                                   &zobject, functionName + args, false);
                        }
                    }
                }
            }
        }
    } ZEND_HASH_FOREACH_END();

    PQDBG_LVL_DONE();
}
