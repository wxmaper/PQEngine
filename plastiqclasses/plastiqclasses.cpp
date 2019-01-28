#include "pqenginecore.h"
#include "plastiqclasses.h"
#include "plastiqqt.h"
#include "plastiqqobject.h"
#include "plastiqqcoreapplication.h"
#include "plastiqqapplication.h"
#include "plastiqqguiapplication.h"
#include "plastiqqwidget.h"
#include "plastiqqpaintdevice.h"
#include "plastiqqthread.h"
#include "plastiqqtimer.h"
#include "plastiqqpushbutton.h"
#include "plastiqqabstractbutton.h"
#include "plastiqqlabel.h"
#include "plastiqqsize.h"
#include "plastiqqrect.h"
#include "plastiqqframe.h"
#include "plastiqqevent.h"
#include "plastiqqcloseevent.h"
#include "plastiqqmoveevent.h"
#include "plastiqqmessagebox.h"
#include "plastiqqdialog.h"
#include "plastiq.h"

PlastiQMetaObjectList PQEngineCore::plastiqClasses() {
    PlastiQMetaObjectList classes;
    classes << PlastiQQApplication::plastiq_static_metaObject;
    classes << PlastiQQGuiApplication::plastiq_static_metaObject;
    classes << PlastiQQObject::plastiq_static_metaObject;
    classes << PlastiQQWidget::plastiq_static_metaObject;
    classes << PlastiQQCoreApplication::plastiq_static_metaObject;
    classes << PlastiQQThread::plastiq_static_metaObject;
    classes << PlastiQQTimer::plastiq_static_metaObject;
    classes << PlastiQQPaintDevice::plastiq_static_metaObject;
    classes << PlastiQQt::plastiq_static_metaObject;
    classes << PlastiQQRect::plastiq_static_metaObject;
    classes << PlastiQQSize::plastiq_static_metaObject;
    classes << PlastiQQPushButton::plastiq_static_metaObject;
    classes << PlastiQQLabel::plastiq_static_metaObject;
    classes << PlastiQQFrame::plastiq_static_metaObject;
    classes << PlastiQQEvent::plastiq_static_metaObject;
    classes << PlastiQQMoveEvent::plastiq_static_metaObject;
    classes << PlastiQQCloseEvent::plastiq_static_metaObject;
    classes << PlastiQQMessageBox::plastiq_static_metaObject;
    classes << PlastiQQDialog::plastiq_static_metaObject;
    classes << PlastiQQAbstractButton::plastiq_static_metaObject;/*%%*/
    return classes;
}

PlastiQUiHash PQEngineCore::plastiqForms() {
    static PlastiQUiHash forms = {
        //{ QByteArrayLiteral("widget.php"), new Ui_Widget() },
        /*%%*/
    };
    return forms;
}
