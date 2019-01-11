#include "pqenginecore.h"
#include "plastiqclasses.h"
//#include "plastiqqapplication.h"
//#include "plastiqqguiapplication.h"
//#include "plastiqqobject.h"
//#include "plastiqqwidget.h"
//#include "plastiqqcoreapplication.h"
//#include "plastiqqpaintdevice.h"
#include "plastiqqt.h"
#include "plastiq.h"

int PlastiQClasses::typeId(const QByteArray &typeName) { return plastiqTypes.value(typeName, -1); }
QHash<QByteArray,int> PlastiQClasses::plastiqTypes = {
    { "Qt", 0 },/*%%*/
//    { "QCoreApplication", 1 },
//    { "QGuiApplication", 2 },
//    { "QApplication", 3 },
//    { "QObject", 4 },
//    { "QPaintDevice", 5 },
//    { "QWidget", 6 },
};

PlastiQMetaObjectList PQEngineCore::plastiqClasses() {
    PlastiQMetaObjectList classes;
//    classes << PlastiQQApplication::plastiq_static_metaObject;
//    classes << PlastiQQGuiApplication::plastiq_static_metaObject;
//    classes << PlastiQQObject::plastiq_static_metaObject;
//    classes << PlastiQQWidget::plastiq_static_metaObject;
//    classes << PlastiQQCoreApplication::plastiq_static_metaObject;
//    classes << PlastiQQPaintDevice::plastiq_static_metaObject;
    classes << PlastiQQt::plastiq_static_metaObject;/*%%*/
    return classes;
}

PlastiQUiHash PQEngineCore::plastiqForms() {
    static PlastiQUiHash forms = {
        //{ QByteArrayLiteral("widget.php"), new Ui_Widget() },
        /*%%*/
    };
    return forms;
}
