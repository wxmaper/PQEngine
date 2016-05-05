#include "phpqt5.h"
#include <QSize>

zval PHPQt5::pq_qstringlist_to_ht(QStringList stringList PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    zval array;
    array_init(&array);

    for(int index = 0; index < stringList.size(); index++)
    {
        QByteArray ba = stringList.at(index).toUtf8();
        add_index_string(&array, index, ba.constData());
    }

    PQDBG_LVL_DONE();
    return array;
}

zval PHPQt5::pq_qobjectlist_to_ht(const QObjectList &objectList PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    zval array;
    array_init(&array);

    foreach(QObject *child, objectList) {
        zval pzval = objectFactory()->getZObject(child PQDBG_LVL_CC);

        if(Z_TYPE(pzval) == IS_OBJECT) {
            add_next_index_zval(&array, &pzval);
        }
    }

    PQDBG_LVL_DONE();
    return array;
}

zval PHPQt5::pq_qpoint_to_ht(const QPoint &point PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    zval array;
    array_init(&array);

    add_assoc_long(&array, "x", point.x());
    add_assoc_long(&array, "y", point.y());

    PQDBG_LVL_DONE();
    return array;
}

zval PHPQt5::pq_qrect_to_ht(const QRect &rect PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    zval array;
    array_init(&array);

    add_assoc_long(&array, "width", rect.width());
    add_assoc_long(&array, "height", rect.height());
    add_assoc_long(&array, "top", rect.top());
    add_assoc_long(&array, "bottom", rect.bottom());
    add_assoc_long(&array, "left", rect.left());
    add_assoc_long(&array, "right", rect.right());
    add_assoc_long(&array, "x", rect.x());
    add_assoc_long(&array, "y", rect.y());

    PQDBG_LVL_DONE();
    return array;
}

zval PHPQt5::pq_qsize_to_ht(const QSize &size PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    zval array;
    array_init(&array);

    add_assoc_long(&array, "width", size.width());
    add_assoc_long(&array, "height", size.height());

    PQDBG_LVL_DONE();
    return array;
}

QStringList PHPQt5::pq_ht_to_qstringlist(zval *pzval PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    QStringList list;

    if(Z_TYPE_P(pzval) != IS_ARRAY) {
        php_error(E_ERROR, "Wrong zval type for method `pq_ht_to_qstringlist(zval *)`. Expected type: ARRAY\n");
    }

    zval *entry;
    HashTable *ht = Z_ARRVAL_P(pzval);
    ulong num_key;
    zend_string *key;

    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, key, entry) {
        if(Z_TYPE_P(entry) == IS_STRING) {
            list << toUTF8(Z_STRVAL_P(entry));
        }
        else if(Z_TYPE_P(entry) == IS_LONG) {
            list << QString::number(Z_LVAL_P(entry));
        }
        else {
            php_error(E_ERROR, "A wrong value in array");
        }

    } ZEND_HASH_FOREACH_END();

    Q_UNUSED(num_key);
    Q_UNUSED(key);

    PQDBG_LVL_DONE();
    return list;
}

#include "classes/pqevent.h"

zval PHPQt5::pq_qevent_to_zobject_ex(QEvent *event PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    zval zevent;
    zend_class_entry *qevent_ce = PHPQt5::objectFactory()->getClassEntry("PQEvent" PQDBG_LVL_CC);

#ifdef PQDEBUG
    PQDBGLPUP(qevent_ce->name->val);
    PQDBGLPUP("init zobject");
#endif
    object_init_ex(&zevent, qevent_ce);

#ifdef PQDEBUG
    PQDBGLPUP("init qobject");
#endif
    QObject *qobject = new PQEvent(event);

#ifdef PQDEBUG
    PQDBGLPUP("register qobject");
#endif
    objectFactory()->registerObject(&zevent, qobject PQDBG_LVL_CC);

#ifdef PQDEBUG
    PQDBGLPUP("done");
#endif
    PQDBG_LVL_DONE();
    return zevent;
}

zval PHPQt5::pq_qevent_to_zobject(QEvent *event PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    zval zobj;
    zend_class_entry *qevent_ce = objectFactory()->getClassEntry("QEvent" PQDBG_LVL_CC);

    object_init_ex(&zobj, qevent_ce);
    add_property_long(&zobj, "type", event->type());

    switch(event->type()) {
    case QEvent::ContextMenu: {
        QContextMenuEvent *e = (QContextMenuEvent*) event;
        //qevent_ce = objectFactory()->getClassEntry("QEvent");

        add_property_long(&zobj, "globalX", e->globalX());
        add_property_long(&zobj, "globalY", e->globalY());
        add_property_long(&zobj, "x", e->x());
        add_property_long(&zobj, "y", e->y());
        add_property_long(&zobj, "timestamp", e->timestamp());
        break;
    }

    case QEvent::MouseMove:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::NonClientAreaMouseButtonDblClick:
    case QEvent::NonClientAreaMouseButtonPress:
    case QEvent::NonClientAreaMouseButtonRelease:
    case QEvent::NonClientAreaMouseMove: {
        QMouseEvent *e = (QMouseEvent*) event;
        //qevent_ce = objectFactory()->getClassEntry("QEvent");

        add_property_long(&zobj, "globalX", e->globalX());
        add_property_long(&zobj, "globalY", e->globalY());
        add_property_long(&zobj, "x", e->x());
        add_property_long(&zobj, "y", e->y());
        add_property_long(&zobj, "button", e->button());
        add_property_long(&zobj, "buttons", e->buttons());
        add_property_long(&zobj, "flags", e->flags());
        add_property_long(&zobj, "timestamp", e->timestamp());
        break;
    }

    case QEvent::Move: {
        QMoveEvent *e = (QMoveEvent*) event;
        //qevent_ce = objectFactory()->getClassEntry("QEvent");


        zval oldPos = pq_qpoint_to_ht(e->oldPos() PQDBG_LVL_CC);
        zval pos = pq_qpoint_to_ht(e->pos() PQDBG_LVL_CC);
        //Z_DELREF(oldPos);
        //Z_DELREF(pos);

        add_property_zval(&zobj, "oldPos", &oldPos);
        add_property_zval(&zobj, "pos", &pos);
        break;
    }

    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
        QKeyEvent *e = (QKeyEvent*) event;
        //qevent_ce = objectFactory()->getClassEntry("QEvent");

        add_property_long(&zobj, "count", e->count());
        add_property_long(&zobj, "key", e->key());
        add_property_long(&zobj, "nativeVirtualKey", e->nativeVirtualKey());
        add_property_long(&zobj, "modifiers", e->modifiers());
        add_property_string(&zobj, "text", e->text().toUtf8().constData());
        add_property_long(&zobj, "timestamp", e->timestamp());
        break;
    }

    case QEvent::Resize: {
        QResizeEvent *e = (QResizeEvent*) event;
        //qevent_ce = objectFactory()->getClassEntry("QEvent");

        zval oldSize = pq_qsize_to_ht(e->oldSize() PQDBG_LVL_CC);
        zval size = pq_qsize_to_ht(e->size() PQDBG_LVL_CC);
        //Z_DELREF(oldSize);
        //Z_DELREF(size);

        add_property_zval(&zobj, "oldSize", &oldSize);
        add_property_zval(&zobj, "size", &size);
        break;
    }

    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove: {
        QHoverEvent *e = (QHoverEvent*) event;
        //qevent_ce = objectFactory()->getClassEntry("QEvent");

        zval oldPos = pq_qpoint_to_ht(e->oldPos() PQDBG_LVL_CC);
        zval pos = pq_qpoint_to_ht(e->pos() PQDBG_LVL_CC);
        //Z_DELREF(oldPos);
        //Z_DELREF(pos);

        add_property_zval(&zobj, "oldPos", &oldPos);
        add_property_zval(&zobj, "pos", &pos);
        add_property_long(&zobj, "timestamp", e->timestamp());
        break;
    }

    case QEvent::FocusIn:
    case QEvent::FocusOut: {
        QFocusEvent *e = (QFocusEvent*) event;
        //qevent_ce = objectFactory()->getClassEntry("QEvent");

        add_property_bool(&zobj, "gotFocus", e->gotFocus());
        add_property_bool(&zobj, "lostFocus", e->lostFocus());
        add_property_long(&zobj, "reason", e->reason());
        break;
    }

    case QEvent::Enter: {
        QEnterEvent *e = (QEnterEvent*) event;
        //qevent_ce = objectFactory()->getClassEntry("QEvent");

        add_property_long(&zobj, "globalX", e->globalX());
        add_property_long(&zobj, "globalY", e->globalY());
        add_property_long(&zobj, "x", e->x());
        add_property_long(&zobj, "y", e->y());
        break;
    }

    default: {
        //object_init_ex(&zobj, qevent_ce);
        break;
    }
    }

    PQDBG_LVL_DONE();
    return zobj;
}
