#include "phpqt5.h"

QByteArray PHPQt5::plastiqGetQEventClassName(QEvent *event)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    QByteArray name;

    switch(event->type()) {
    case QEvent::ContextMenu: name = "QContextMenuEvent"; break;
    case QEvent::DragEnter: name = "QDragEnterEvent"; break;
    case QEvent::DragLeave: name = "QDragLeaveEvent"; break;
    case QEvent::DragMove: name = "QDragMoveEvent"; break;
    case QEvent::Drop: name = "QDropEvent"; break;
    case QEvent::Enter: name = "QEnterEvent"; break;
    case QEvent::FileOpen: name = "QFileOpenEvent"; break;
    case QEvent::FocusIn: name = "QFocusEvent"; break;
    case QEvent::FocusOut: name = "QFocusEvent"; break;
    case QEvent::FocusAboutToChange: name = "QFocusEvent"; break;
    case QEvent::Gesture: name = "QGestureEvent"; break;
    case QEvent::GestureOverride: name = "QGestureEvent"; break;
    case QEvent::GrabKeyboard: name = "QGraphicsItem only"; break;
    case QEvent::GrabMouse: name = "QGraphicsItem only"; break;
    case QEvent::GraphicsSceneContextMenu: name = "QGraphicsSceneContextMenuEvent"; break;
    case QEvent::GraphicsSceneDragEnter: name = "QGraphicsSceneDragDropEvent"; break;
    case QEvent::GraphicsSceneDragLeave: name = "QGraphicsSceneDragDropEvent"; break;
    case QEvent::GraphicsSceneDragMove: name = "QGraphicsSceneDragDropEvent"; break;
    case QEvent::GraphicsSceneDrop: name = "QGraphicsSceneDragDropEvent"; break;
    case QEvent::GraphicsSceneHelp: name = "QHelpEvent"; break;
    case QEvent::GraphicsSceneHoverEnter: name = "QGraphicsSceneHoverEvent"; break;
    case QEvent::GraphicsSceneHoverLeave: name = "QGraphicsSceneHoverEvent"; break;
    case QEvent::GraphicsSceneHoverMove: name = "QGraphicsSceneHoverEvent"; break;
    case QEvent::GraphicsSceneMouseDoubleClick: name = "QGraphicsSceneMouseEvent"; break;
    case QEvent::GraphicsSceneMouseMove: name = "QGraphicsSceneMouseEvent"; break;
    case QEvent::GraphicsSceneMousePress: name = "QGraphicsSceneMouseEvent"; break;
    case QEvent::GraphicsSceneMouseRelease: name = "QGraphicsSceneMouseEvent"; break;
    case QEvent::GraphicsSceneMove: name = "QGraphicsSceneMoveEvent"; break;
    case QEvent::GraphicsSceneResize: name = "QGraphicsSceneResizeEvent"; break;
    case QEvent::GraphicsSceneWheel: name = "QGraphicsSceneWheelEvent"; break;
    case QEvent::Hide: name = "QHideEvent"; break;
    case QEvent::HoverEnter: name = "QHoverEvent"; break;
    case QEvent::HoverLeave: name = "QHoverEvent"; break;
    case QEvent::HoverMove: name = "QHoverEvent"; break;
    case QEvent::IconDrag: name = "QIconDragEvent"; break;
    case QEvent::InputMethod: name = "QInputMethodEvent"; break;
    case QEvent::InputMethodQuery: name = "QInputMethodQueryEvent"; break;
    case QEvent::KeyPress: name = "QKeyEvent"; break;
    case QEvent::KeyRelease: name = "QKeyEvent"; break;
    case QEvent::MouseButtonDblClick: name = "QMouseEvent"; break;
    case QEvent::MouseButtonPress: name = "QMouseEvent"; break;
    case QEvent::MouseButtonRelease: name = "QMouseEvent"; break;
    case QEvent::MouseMove: name = "QMouseEvent"; break;
    case QEvent::Move: name = "QMoveEvent"; break;
    case QEvent::NativeGesture: name = "QNativeGestureEvent"; break;
    case QEvent::OrientationChange: name = "QScreenOrientationChangeEvent"; break;
    case QEvent::Paint: name = "QPaintEvent"; break;
    case QEvent::PlatformSurface: name = "QPlatformSurfaceEvent"; break;
    case QEvent::Resize: name = "QResizeEvent"; break;
    case QEvent::ScrollPrepare: name = "QScrollPrepareEvent"; break;
    case QEvent::Scroll: name = "QScrollEvent"; break;
    case QEvent::Shortcut: name = "QShortcutEvent"; break;
    case QEvent::ShortcutOverride: name = "QKeyEvent"; break;
    case QEvent::Show: name = "QShowEvent"; break;
    case QEvent::SockAct: name = "QSocketNotifier"; break;
    case QEvent::StateMachineSignal: name = "QStateMachine::SignalEvent"; break;
    case QEvent::StateMachineWrapped: name = "QStateMachine::WrappedEvent"; break;
    case QEvent::StatusTip: name = "QStatusTipEvent"; break;
    case QEvent::TabletMove: name = "QTabletEvent"; break;
    case QEvent::TabletPress: name = "QTabletEvent"; break;
    case QEvent::TabletRelease: name = "QTabletEvent"; break;
    case QEvent::TabletEnterProximity: name = "QTabletEvent"; break;
    case QEvent::TabletLeaveProximity: name = "QTabletEvent"; break;
    case QEvent::Timer: name = "QTimerEvent"; break;
    case QEvent::ToolTip: name = "QHelpEvent"; break;
    case QEvent::TouchBegin: name = "QTouchEvent"; break;
    case QEvent::TouchCancel: name = "QTouchEvent"; break;
    case QEvent::TouchEnd: name = "QTouchEvent"; break;
    case QEvent::TouchUpdate: name = "QTouchEvent"; break;
    case QEvent::Wheel: name = "QWheelEvent"; break;
    case QEvent::WindowStateChange: name = "QWindowStateChangeEvent"; break;
    default: name = "QEvent";
    }

    PQDBGLPUP(QString("className: %1").arg(name.constData()));
    PQDBG_LVL_DONE();
    return name;
}

void PHPQt5::qevent_cast(PQObjectWrapper *pqobject)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    QByteArray name = plastiqGetQEventClassName(reinterpret_cast<QEvent*>(pqobject->object->plastiq_data()));
    zend_class_entry *ce;

    PQDBGLPUP(QString("cast to: %1").arg(name.constData()));

    if(name != "QEvent"
            && (ce = objectFactory()->getClassEntry(name))) {
        PlastiQMetaObject metaObject = objectFactory()->getMetaObject(name);
        PQDBGLPUP(QString("PlastiQMetaObject className: %1").arg(metaObject.className()));

        PlastiQObject *object = metaObject.createInstanceFromData(pqobject->object->plastiq_data());
        PQDBGLPUP(QString("created object: %1").arg(object->plastiq_metaObject()->className()));

        pqobject->zo.ce = ce; // FIXME: TEST THIS! 0_0
        pqobject->object = object; // FEXME: test for memory leak

        // metaObject.d.static_metacall(pqobject->object, PlastiQMetaObject::DownCast, -1, stack);
    }
#ifdef PQDEBUG
    else if(name != "QEvent") {
        PQDBGLPUP(QString("unknown class `%1`").arg(name.constData()));
    }
    else {
        PQDBGLPUP("`QEvent` does not need cast to `QEvent`");
    }
#endif

    PQDBG_LVL_DONE();
}
