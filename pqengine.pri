QT += core xml
CONFIG += c++11 qt

contains(DEFINES, PQDEBUG): QT += network

contains(DEFINES, PQENGINE_LIBRARY) {
    TEMPLATE = lib
    contains(DEFINES, PQSTATIC): CONFIG += staticlib
    !contains(DEFINES, PQSTATIC): DEFINES += PQEXPORT
} else {
    TEMPLATE = app
    #SOURCES += $${PQENGINE_PATH_PREFIX}/main.cpp
    DEFINES += PQEXPORT
}

equals(TEMPLATE, app) || contains(DEFINES, PQEXPORT) {
    #SOURCES += $${PQENGINE_PATH_PREFIX}/plastiqclasses/plastiqclasses.cpp

    for (PLASTIQ_EXTENSIONS_LIB, PLASTIQ_EXTENSIONS_LIBS) {
        INCLUDEPATH += "$${PLASTIQ_EXTENSIONS_SRC_PATH}/$$dirname(PLASTIQ_EXTENSIONS_LIB)"
        INCLUDEPATH += "$${PLASTIQ_EXTENSIONS_SRC_PATH}/$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)"

        !contains(QT, $$dirname(PLASTIQ_EXTENSIONS_LIB)) {
            QT += $$dirname(PLASTIQ_EXTENSIONS_LIB)
            message(Add Qt module: $$dirname(PLASTIQ_EXTENSIONS_LIB))
        }

        CONFIG(release, debug|release) {
            LIBS += -L"$${PLASTIQ_EXTENSIONS_LIB_PATH}/$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)/release" -lPlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)
            message(Add PlastiQ extension: `$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)' in `$${PLASTIQ_EXTENSIONS_LIB_PATH}' [release])
        }

        CONFIG(debug, debug|release) {
            LIBS += -L"$${PLASTIQ_EXTENSIONS_LIB_PATH}/$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)/debug" -lPlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)
            message(Add PlastiQ extension: `$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)' in `$${PLASTIQ_EXTENSIONS_LIB_PATH}' [debug])
        }
    }

    INCLUDEPATH += "$${PLASTIQ_EXTENSIONS_SRC_PATH}"
}

win32 {
    LIBS += -L"$${PHP_LIB_PATH}/" -lphp7ts
    DEFINES += ZEND_WIN32
    DEFINES += PHP_WIN32
    DEFINES += WIN32
}

unix {
    LIBS += -L"$${PHP_LIB_PATH}/" -lphp7
    DEFINES += PTHREADS
}

DEFINES += ZTS
DEFINES += ZEND_ENABLE_STATIC_TSRMLS_CACHE
DEFINES += "ZEND_DEBUG=0"

MOC_DIR = .moc
OBJECTS_DIR = .obj

INCLUDEPATH += \
    $${PHP_LIB_PATH}\
    $${PHP_SRC_PATH}\
    $${PHP_SRC_PATH}/main\
    $${PHP_SRC_PATH}/Zend\
    $${PHP_SRC_PATH}/TSRM\
    $${PHP_SRC_PATH}/ext/standard\
    $${PQENGINE_PATH_PREFIX}/plastiqclasses/core\
    $${PQENGINE_PATH_PREFIX}/plastiqclasses\
    $${PQENGINE_PATH_PREFIX}/private

DEPENDPATH += "$${PHP_LIB_PATH}"

SOURCES += \
    $${PQENGINE_PATH_PREFIX}/pqengine.cpp \
    $${PQENGINE_PATH_PREFIX}/private/pqengine_private.cpp \
    $${PQENGINE_PATH_PREFIX}/private/pqengine_php.cpp \
    $${PQENGINE_PATH_PREFIX}/private/pqengine_pq.cpp \
    $${PQENGINE_PATH_PREFIX}/private/phpqt5.cpp \
    $${PQENGINE_PATH_PREFIX}/private/phpqt5_pq.cpp \
    $${PQENGINE_PATH_PREFIX}/private/simplecrypt.cpp \
    $${PQENGINE_PATH_PREFIX}/private/phpqt5_zif.cpp \
    $${PQENGINE_PATH_PREFIX}/pqenginecore.cpp \
    $${PQENGINE_PATH_PREFIX}/plastiqobject.cpp \
    $${PQENGINE_PATH_PREFIX}/plastiqmethod.cpp \
    $${PQENGINE_PATH_PREFIX}/plastiqmetaobject.cpp \
    $${PQENGINE_PATH_PREFIX}/private/plastiq_zim.cpp \
    $${PQENGINE_PATH_PREFIX}/private/plastiq_zif.cpp \
    $${PQENGINE_PATH_PREFIX}/plastiqproperty.cpp \
    $${PQENGINE_PATH_PREFIX}/private/plastiq.cpp \
    $${PQENGINE_PATH_PREFIX}/private/plastiq_zm.cpp \
    $${PQENGINE_PATH_PREFIX}/private/plastiq_connections.cpp \
    $${PQENGINE_PATH_PREFIX}/private/plastiqthreadcreator.cpp \
    $${PQENGINE_PATH_PREFIX}/private/qevent_cast.cpp \
    $${PQENGINE_PATH_PREFIX}/private/plastiq_debug.cpp \
    $${PQENGINE_PATH_PREFIX}/private/phpqt5objectfactory.cpp

HEADERS += \
    $${PQENGINE_PATH_PREFIX}/pqengine.h\
    $${PQENGINE_PATH_PREFIX}/pqengine_global.h \
    $${PQENGINE_PATH_PREFIX}/ipqengineext.h \
    $${PQENGINE_PATH_PREFIX}/private/pqengine_private.h \
    $${PQENGINE_PATH_PREFIX}/private/pqengine_private.h \
    $${PQENGINE_PATH_PREFIX}/private/phpqt5.h \
    $${PQENGINE_PATH_PREFIX}/private/simplecrypt.h \
    $${PQENGINE_PATH_PREFIX}/private/phpqt5objectfactory.h \
    $${PQENGINE_PATH_PREFIX}/pqenginecore.h \
    $${PQENGINE_PATH_PREFIX}/pqtypes.h \
    $${PQENGINE_PATH_PREFIX}/plastiqobject.h \
    $${PQENGINE_PATH_PREFIX}/plastiqmethod.h \
    $${PQENGINE_PATH_PREFIX}/plastiqmetaobject.h \
    $${PQENGINE_PATH_PREFIX}/plastiq.h \
    $${PQENGINE_PATH_PREFIX}/plastiqclasses/plastiqclasses.h \
    $${PQENGINE_PATH_PREFIX}/plastiqproperty.h \
    $${PQENGINE_PATH_PREFIX}/private/plastiqthreadcreator.h \
    $${PQENGINE_PATH_PREFIX}/private/phpqt5constants.h
