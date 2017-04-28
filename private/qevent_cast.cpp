#include "phpqt5.h"

QByteArray PHPQt5::plastiqGetQEventClassName(QEvent *event)
{
    switch(event->type()) {
    case QEvent::ActionAdded: return QByteArrayLiteral("QActionEvent");
    case QEvent::ActionChanged: return QByteArrayLiteral("QActionEvent");
    case QEvent::ActionRemoved: return QByteArrayLiteral("QActionEvent");
    case QEvent::ChildAdded: return QByteArrayLiteral("QChildEvent");
    case QEvent::ChildPolished: return QByteArrayLiteral("QChildEvent");
    case QEvent::ChildRemoved: return QByteArrayLiteral("QChildEvent");
    case QEvent::Close: return QByteArrayLiteral("QCloseEvent");
    case QEvent::ContextMenu: return QByteArrayLiteral("QContextMenuEvent");
    case QEvent::DragEnter: return QByteArrayLiteral("QDragEnterEvent");
    case QEvent::DragLeave: return QByteArrayLiteral("QDragLeaveEvent");
    case QEvent::DragMove: return QByteArrayLiteral("QDragMoveEvent");
    case QEvent::Drop: return QByteArrayLiteral("QDropEvent");
    case QEvent::Enter: return QByteArrayLiteral("QEnterEvent");
    case QEvent::FileOpen: return QByteArrayLiteral("QFileOpenEvent");
    case QEvent::FocusIn: return QByteArrayLiteral("QFocusEvent");
    case QEvent::FocusOut: return QByteArrayLiteral("QFocusEvent");
    case QEvent::FocusAboutToChange: return QByteArrayLiteral("QFocusEvent");
    case QEvent::Gesture: return QByteArrayLiteral("QGestureEvent");
    case QEvent::GestureOverride: return QByteArrayLiteral("QGestureEvent");
    case QEvent::GrabKeyboard: return QByteArrayLiteral("QGraphicsItem only");
    case QEvent::GrabMouse: return QByteArrayLiteral("QGraphicsItem only");
    case QEvent::GraphicsSceneContextMenu: return QByteArrayLiteral("QGraphicsSceneContextMenuEvent");
    case QEvent::GraphicsSceneDragEnter: return QByteArrayLiteral("QGraphicsSceneDragDropEvent");
    case QEvent::GraphicsSceneDragLeave: return QByteArrayLiteral("QGraphicsSceneDragDropEvent");
    case QEvent::GraphicsSceneDragMove: return QByteArrayLiteral("QGraphicsSceneDragDropEvent");
    case QEvent::GraphicsSceneDrop: return QByteArrayLiteral("QGraphicsSceneDragDropEvent");
    case QEvent::GraphicsSceneHelp: return QByteArrayLiteral("QHelpEvent");
    case QEvent::GraphicsSceneHoverEnter: return QByteArrayLiteral("QGraphicsSceneHoverEvent");
    case QEvent::GraphicsSceneHoverLeave: return QByteArrayLiteral("QGraphicsSceneHoverEvent");
    case QEvent::GraphicsSceneHoverMove: return QByteArrayLiteral("QGraphicsSceneHoverEvent");
    case QEvent::GraphicsSceneMouseDoubleClick: return QByteArrayLiteral("QGraphicsSceneMouseEvent");
    case QEvent::GraphicsSceneMouseMove: return QByteArrayLiteral("QGraphicsSceneMouseEvent");
    case QEvent::GraphicsSceneMousePress: return QByteArrayLiteral("QGraphicsSceneMouseEvent");
    case QEvent::GraphicsSceneMouseRelease: return QByteArrayLiteral("QGraphicsSceneMouseEvent");
    case QEvent::GraphicsSceneMove: return QByteArrayLiteral("QGraphicsSceneMoveEvent");
    case QEvent::GraphicsSceneResize: return QByteArrayLiteral("QGraphicsSceneResizeEvent");
    case QEvent::GraphicsSceneWheel: return QByteArrayLiteral("QGraphicsSceneWheelEvent");
    case QEvent::Hide: return QByteArrayLiteral("QHideEvent");
    case QEvent::HoverEnter: return QByteArrayLiteral("QHoverEvent");
    case QEvent::HoverLeave: return QByteArrayLiteral("QHoverEvent");
    case QEvent::HoverMove: return QByteArrayLiteral("QHoverEvent");
    case QEvent::IconDrag: return QByteArrayLiteral("QIconDragEvent");
    case QEvent::InputMethod: return QByteArrayLiteral("QInputMethodEvent");
    case QEvent::InputMethodQuery: return QByteArrayLiteral("QInputMethodQueryEvent");
    case QEvent::KeyPress: return QByteArrayLiteral("QKeyEvent");
    case QEvent::KeyRelease: return QByteArrayLiteral("QKeyEvent");
    case QEvent::MouseButtonDblClick: return QByteArrayLiteral("QMouseEvent");
    case QEvent::MouseButtonPress: return QByteArrayLiteral("QMouseEvent");
    case QEvent::MouseButtonRelease: return QByteArrayLiteral("QMouseEvent");
    case QEvent::MouseMove: return QByteArrayLiteral("QMouseEvent");
    case QEvent::Move: return QByteArrayLiteral("QMoveEvent");
    case QEvent::None: return QByteArrayLiteral("None");
    case QEvent::NativeGesture: return QByteArrayLiteral("QNativeGestureEvent");
    case QEvent::OrientationChange: return QByteArrayLiteral("QScreenOrientationChangeEvent");
    case QEvent::Paint: return QByteArrayLiteral("QPaintEvent");
    case QEvent::PlatformSurface: return QByteArrayLiteral("QPlatformSurfaceEvent");
    case QEvent::Resize: return QByteArrayLiteral("QResizeEvent");
    case QEvent::ScrollPrepare: return QByteArrayLiteral("QScrollPrepareEvent");
    case QEvent::Scroll: return QByteArrayLiteral("QScrollEvent");
    case QEvent::Shortcut: return QByteArrayLiteral("QShortcutEvent");
    case QEvent::ShortcutOverride: return QByteArrayLiteral("QKeyEvent");
    case QEvent::Show: return QByteArrayLiteral("QShowEvent");
    case QEvent::SockAct: return QByteArrayLiteral("QSocketNotifier");
    case QEvent::StateMachineSignal: return QByteArrayLiteral("QStateMachine::SignalEvent");
    case QEvent::StateMachineWrapped: return QByteArrayLiteral("QStateMachine::WrappedEvent");
    case QEvent::StatusTip: return QByteArrayLiteral("QStatusTipEvent");
    case QEvent::TabletMove: return QByteArrayLiteral("QTabletEvent");
    case QEvent::TabletPress: return QByteArrayLiteral("QTabletEvent");
    case QEvent::TabletRelease: return QByteArrayLiteral("QTabletEvent");
    case QEvent::TabletEnterProximity: return QByteArrayLiteral("QTabletEvent");
    case QEvent::TabletLeaveProximity: return QByteArrayLiteral("QTabletEvent");
    case QEvent::Timer: return QByteArrayLiteral("QTimerEvent");
    case QEvent::ToolTip: return QByteArrayLiteral("QHelpEvent");
    case QEvent::TouchBegin: return QByteArrayLiteral("QTouchEvent");
    case QEvent::TouchCancel: return QByteArrayLiteral("QTouchEvent");
    case QEvent::TouchEnd: return QByteArrayLiteral("QTouchEvent");
    case QEvent::TouchUpdate: return QByteArrayLiteral("QTouchEvent");
    case QEvent::WhatsThis: return QByteArrayLiteral("QHelpEvent");
    case QEvent::Wheel: return QByteArrayLiteral("QWheelEvent");
    case QEvent::WindowStateChange: return QByteArrayLiteral("QWindowStateChangeEvent");
    default: return QByteArrayLiteral("QEvent");
    }
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
