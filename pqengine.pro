PQ_PHP_VERSION=7.1.1

# Path to PHP sources
PHP_SRC_PATH="D:/src/php-$${PQ_PHP_VERSION}-src"

# Path to directory containing the PHP library
# > php7ts.dll or php7ts.lib for Windows OS
# > libphp.so or libphp7.a for Unix-like OS
PHP_LIB_PATH="D:/src/php-$${PQ_PHP_VERSION}-src/dev"

# Remove this define if need to build the standalone app
DEFINES += PQENGINE_LIBRARY

# Use this define for build a statically library
DEFINES += PQSTATIC

# Use this define for debug messages
DEFINES += PQDEBUG
DEFINES += PQDETAILEDDEBUG

##########################################
###### Don't change contents below! ######
##########################################
QT += core xml

contains(DEFINES, PQDEBUG) {
    QT += network
    TARGET = pqengine-debug
} else {
    TARGET = pqengine
}

contains(DEFINES, PQENGINE_LIBRARY) {
    contains(DEFINES, PQENGINE_LIBRARY) {
        CONFIG += staticlib
    }
    TEMPLATE = lib
    CONFIG += c++11 qt
} else {
    TEMPLATE = app
    CONFIG += c++11 qt
    SOURCES += main.cpp
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
    plastiqclasses/core\
    plastiqclasses\
    private

DEPENDPATH += "$${PHP_LIB_PATH}"

SOURCES += \
    pqengine.cpp \
    private/pqengine_private.cpp \
    private/pqengine_php.cpp \
    private/pqengine_pq.cpp \
    private/phpqt5.cpp \
    private/phpqt5_pq.cpp \
    private/simplecrypt.cpp \
    private/phpqt5_zif.cpp \
    pqenginecore.cpp \
    plastiqobject.cpp \
    plastiqmethod.cpp \
    plastiqmetaobject.cpp \
    private/plastiq_zim.cpp \
    private/plastiqmetaobjectfactory.cpp \
    private/plastiq_zif.cpp \
    plastiqproperty.cpp \
    private/plastiq.cpp \
    private/plastiq_zm.cpp \
    private/plastiq_connections.cpp \
    private/plastiqthreadcreator.cpp \
    private/qevent_cast.cpp \
    private/plastiq_debug.cpp

HEADERS += \
    pqengine.h\
    pqengine_global.h \
    ipqengineext.h \
    private/pqengine_private.h \
    private/pqengine_private.h \
    private/phpqt5.h \
    private/simplecrypt.h \
    private/phpqt5objectfactory.h \
    pqenginecore.h \
    pqtypes.h \
    plastiqobject.h \
    plastiqmethod.h \
    plastiqmetaobject.h \
    plastiq.h \
    plastiqclasses/plastiqclasses.h \
    plastiqproperty.h \
    private/plastiqthreadcreator.h \
    private/phpqt5constants.h
