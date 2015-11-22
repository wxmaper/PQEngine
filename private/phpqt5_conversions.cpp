#include "phpqt5.h"

zval *PHPQt5::pq_qstringlist_to_ht(QStringList stringList TSRMLS_DC)
{
    zval *array = NULL;
    MAKE_STD_ZVAL(array);
    array_init(array);

    for(int index = 0; index < stringList.size(); index++)
    {
        QByteArray ba = stringList.at(index).toUtf8();
        add_index_string(array, index, ba.constData(), 1);
    }

    return array;
}

zval *PHPQt5::pq_qpoint_to_ht(const QPoint &point TSRMLS_DC)
{
    zval *array = NULL;
    MAKE_STD_ZVAL(array);
    array_init(array);

    add_assoc_long(array, "x", point.x());
    add_assoc_long(array, "y", point.y());

    return array;
}

zval *PHPQt5::pq_qrect_to_ht(const QRect &rect TSRMLS_DC)
{
    zval *array = NULL;
    MAKE_STD_ZVAL(array);
    array_init(array);

    add_assoc_long(array, "width", rect.width());
    add_assoc_long(array, "height", rect.height());
    add_assoc_long(array, "top", rect.top());
    add_assoc_long(array, "bottom", rect.bottom());
    add_assoc_long(array, "left", rect.left());
    add_assoc_long(array, "right", rect.right());
    add_assoc_long(array, "x", rect.x());
    add_assoc_long(array, "y", rect.y());

    return array;
}

zval *PHPQt5::pq_qsize_to_ht(const QSize &size TSRMLS_DC)
{
    zval *array = NULL;
    MAKE_STD_ZVAL(array);
    array_init(array);

    add_assoc_long(array, "width", size.width());
    add_assoc_long(array, "height", size.height());

    return array;
}

QStringList PHPQt5::pq_ht_to_qstringlist(zval *pzval TSRMLS_DC)
{
    QStringList list;

    if(Z_TYPE_P(pzval) != IS_ARRAY) {
        php_error(E_ERROR, "Wrong zval type for method `pq_ht_to_qstringlist(zval *)`. Expected type: ARRAY\n");
    }

    zval **entry;
    HashPosition pos;
    HashTable *ht = Z_ARRVAL_P(pzval);

    zend_hash_internal_pointer_reset_ex(ht, &pos);
    while (zend_hash_get_current_data_ex(ht, (void **)&entry, &pos) == SUCCESS) {
        if(Z_TYPE_PP(entry) == IS_STRING) {
            list << Z_STRVAL_PP(entry);
        }
        else if(Z_TYPE_PP(entry) == IS_LONG) {
            list << QString::number(Z_LVAL_PP(entry));
        }
        else {
            php_error(E_ERROR, "Wrong value in array");
        }

        zend_hash_move_forward_ex(ht, &pos);
    }

    return list;
}

zval *PHPQt5::pq_qevent_to_zobject(QEvent *event TSRMLS_DC)
{
    zval *zobj = NULL;
    zend_class_entry *qevent_ce;

    MAKE_STD_ZVAL(zobj);

    if(event->type() == QEvent::ContextMenu) {
        QContextMenuEvent *e = (QContextMenuEvent*) event;
        qevent_ce = objectFactory()->getClassEntry("QContextMenuEvent");

        object_init_ex(zobj, qevent_ce);
        add_property_long(zobj, "globalX", e->globalX());
        add_property_long(zobj, "globalY", e->globalY());
        add_property_long(zobj, "x", e->x());
        add_property_long(zobj, "y", e->y());
        add_property_long(zobj, "timestamp", e->timestamp());
    }

    else if(event->type() == QEvent::MouseMove
            || event->type() == QEvent::MouseButtonDblClick
            || event->type() == QEvent::MouseButtonPress
            || event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *e = (QMouseEvent*) event;
        qevent_ce = objectFactory()->getClassEntry("QMouseEvent");

        object_init_ex(zobj, qevent_ce);
        add_property_long(zobj, "globalX", e->globalX());
        add_property_long(zobj, "globalY", e->globalY());
        add_property_long(zobj, "x", e->x());
        add_property_long(zobj, "y", e->y());
        add_property_long(zobj, "button", e->button());
        add_property_long(zobj, "buttons", e->buttons());
        add_property_long(zobj, "flags", e->flags());
        add_property_long(zobj, "timestamp", e->timestamp());
    }

    else if(event->type() == QEvent::KeyPress
            || event->type() == QEvent::KeyRelease) {
        QKeyEvent *e = (QKeyEvent*) event;
        qevent_ce = objectFactory()->getClassEntry("QKeyEvent");

        object_init_ex(zobj, qevent_ce);
        add_property_long(zobj, "count", e->count());
        add_property_long(zobj, "key", e->key());
        add_property_long(zobj, "nativeVirtualKey", e->nativeVirtualKey());
        add_property_long(zobj, "modifiers", e->modifiers());
        add_property_string(zobj, "text", e->text().toUtf8().constData(), 1);
        add_property_long(zobj, "timestamp", e->timestamp());
    }

    else if(event->type() == QEvent::Resize) {
        QResizeEvent *e = (QResizeEvent*) event;
        qevent_ce = objectFactory()->getClassEntry("QResizeEvent");

        zval *oldSize = pq_qsize_to_ht(e->oldSize() TSRMLS_CC);
        zval *size = pq_qsize_to_ht(e->size() TSRMLS_CC);
        Z_DELREF_P(oldSize);
        Z_DELREF_P(size);

        object_init_ex(zobj, qevent_ce);
        add_property_zval(zobj, "oldSize", oldSize);
        add_property_zval(zobj, "size", size);
    }

    else if(event->type() == QEvent::Move) {
        QMoveEvent *e = (QMoveEvent*) event;
        qevent_ce = objectFactory()->getClassEntry("QMoveEvent");

        object_init_ex(zobj, qevent_ce);

        zval *oldPos = pq_qpoint_to_ht(e->oldPos() TSRMLS_CC);
        zval *pos = pq_qpoint_to_ht(e->pos() TSRMLS_CC);
        Z_DELREF_P(oldPos);
        Z_DELREF_P(pos);

        add_property_zval(zobj, "oldPos", oldPos);
        add_property_zval(zobj, "pos", pos);
    }

    else if(event->type() == QEvent::HoverEnter
            || event->type() == QEvent::HoverLeave
            || event->type() == QEvent::HoverMove) {
        QHoverEvent *e = (QHoverEvent*) event;
        qevent_ce = objectFactory()->getClassEntry("QHoverEvent");

        zval *oldPos = pq_qpoint_to_ht(e->oldPos() TSRMLS_CC);
        zval *pos = pq_qpoint_to_ht(e->pos() TSRMLS_CC);
        Z_DELREF_P(oldPos);
        Z_DELREF_P(pos);

        object_init_ex(zobj, qevent_ce);
        add_property_zval(zobj, "oldPos", oldPos);
        add_property_zval(zobj, "pos", pos);
        add_property_long(zobj, "timestamp", e->timestamp());
    }

    else if(event->type() == QEvent::FocusIn
            || event->type() == QEvent::FocusOut) {
        QFocusEvent *e = (QFocusEvent*) event;
        qevent_ce = objectFactory()->getClassEntry("QFocusEvent");

        object_init_ex(zobj, qevent_ce);
        add_property_bool(zobj, "gotFocus", e->gotFocus());
        add_property_bool(zobj, "lostFocus", e->lostFocus());
        add_property_long(zobj, "reason", e->reason());
    }

    else if(event->type() == QEvent::Enter) {
        QEnterEvent *e = (QEnterEvent*) event;
        qevent_ce = objectFactory()->getClassEntry("QEnterEvent");

        object_init_ex(zobj, qevent_ce);
        add_property_long(zobj, "globalX", e->globalX());
        add_property_long(zobj, "globalY", e->globalY());
        add_property_long(zobj, "x", e->x());
        add_property_long(zobj, "y", e->y());
    }

    else {
        qevent_ce = objectFactory()->getClassEntry("QEvent");
        object_init_ex(zobj, qevent_ce);
    }

    add_property_long(zobj, "type", event->type());

    return zobj;
}
