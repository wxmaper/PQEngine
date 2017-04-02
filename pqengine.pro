PQ_PHP_VERSION=7.1.1
PHP_SRC_PATH="D:/src/php-$${PQ_PHP_VERSION}-src"

# remove this define if need to build the standalone app
DEFINES += PQENGINE_LIBRARY

# for Windows only {
DEFINES += ZEND_WIN32
DEFINES += PHP_WIN32
DEFINES += WIN32
# } for Windows only

# for Linux only {
#DEFINES += PTHREADS
# } for Linux only

# use this define for debug messages
DEFINES += PQDEBUG
DEFINES += PQDETAILEDDEBUG

DEFINES += PQSTATIC

QMAKE_CXXFLAGS += -std=gnu++0x -pthread
QMAKE_CFLAGS += -std=gnu++0x -pthread
LIBS += -lpthread

DEFINES += ZEND_ENABLE_STATIC_TSRMLS_CACHE

INCLUDEPATH += \
    $${PHP_SRC_PATH}\
    $${PHP_SRC_PATH}/main\
    $${PHP_SRC_PATH}/Zend\
    $${PHP_SRC_PATH}/TSRM\
    $${PHP_SRC_PATH}/ext/standard\
    private

LIBS += -L"$${PHP_SRC_PATH}/dev/" -lphp7ts
INCLUDEPATH += "$${PHP_SRC_PATH}/dev"
DEPENDPATH += "$${PHP_SRC_PATH}/dev"
INCLUDEPATH += pthreads

##########################################
##########################################
##########################################
QT       += core xml

contains(DEFINES, PQDEBUG) {
    TARGET = pqengine-debug
    QT += network
} else {
    TARGET = pqengine
}

contains(DEFINES, PQENGINE_LIBRARY) {
    TEMPLATE = lib
    CONFIG += c++11 qt staticlib
} else {
    TEMPLATE = app
    CONFIG += c++11 qt
    SOURCES += main.cpp
}


DEFINES += ZTS
DEFINES += "ZEND_DEBUG=0"

MOC_DIR = .moc
OBJECTS_DIR = .obj

INCLUDEPATH += "plastiqclasses/core"
INCLUDEPATH += "plastiqclasses/widgets"
INCLUDEPATH += "plastiqclasses/gui"
INCLUDEPATH += "plastiqclasses"

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
    private/plastiqthreadcreator.h
