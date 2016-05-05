PQ_PHP_VERSION=7.0.4
#PHP_SRC_PATH="D:/php-sdk/phpdev/vc14/x86/php-$${PQ_PHP_VERSION}-src"
PHP_SRC_PATH="D:/src/php-$${PQ_PHP_VERSION}-src"

# for Windows only
DEFINES += ZEND_WIN32
DEFINES += PHP_WIN32
DEFINES += WIN32

# for Linux only
#DEFINES += PTHREADS

# use this define for debug messages
#DEFINES += PQDEBUG
#DEFINES += PQDETAILEDDEBUG
DEFINES += PQSTATIC

DEFINES += ZEND_ENABLE_STATIC_TSRMLS_CACHE

INCLUDEPATH += \
    $${PHP_SRC_PATH}\
    $${PHP_SRC_PATH}/main\
    $${PHP_SRC_PATH}/Zend\
    $${PHP_SRC_PATH}/TSRM\
    $${PHP_SRC_PATH}/ext/standard\
    private

#LIBS += "$${PHP_SRC_PATH}/Release_TS/php7ts.dll"
#LIBS += -L"$${PHP_SRC_PATH}/Release_TS/" -lphp7ts
LIBS += -L"$${PHP_SRC_PATH}/dev/" -lphp7ts
INCLUDEPATH += "$${PHP_SRC_PATH}/dev"
DEPENDPATH += "$${PHP_SRC_PATH}/dev"


#INCLUDEPATH += D:/pqengine/extensions/pqengine-widgets
#LIBS += -L"D:/pqengine/extensions/build-pqengine-widgets-Desktop_Qt_5_6_0_MSVC2015_32bit-Release/release" -lpqengine-widgets

##########################################
##########################################
##########################################
QT       += core xml #gui widgets

contains(DEFINES, PQDEBUG) {
TARGET = pqengine-debug
} else {
TARGET = pqengine
}

TEMPLATE = lib
CONFIG += c++11 qt staticlib

DEFINES += ZTS
DEFINES += "ZEND_DEBUG=0" # need for PHP 5.4.45

SOURCES += \
    pqengine.cpp \
    private/pqengine_private.cpp \
    private/pqengine_php.cpp \
    private/pqengine_pq.cpp \
    private/phpqt5.cpp \
    private/phpqt5_zim.cpp \
    private/phpqt5_pq.cpp \
    private/phpqt5_zm.cpp \
    private/simplecrypt.cpp \
    private/phpqt5_conversions.cpp \
    private/phpqt5objectfactory.cpp \
    private/phpqt5connection.cpp \
    private/phpqt5_zif.cpp \
    classes/pqobject.cpp \
    classes/pqtimer.cpp \
    classes/pqcoreapplication.cpp \
    classes/pqregexp.cpp \
    classes/pqsettings.cpp \
    pqenginecore.cpp \
    private/pqobject.cpp \
    classes/pqthread.cpp \
    classes/pqstandardpaths.cpp \
    classes/pqevent.cpp \
    classes/pqlibrary.cpp

HEADERS += \
    pqengine.h\
    pqengine_global.h \
    ipqengineext.h \
    pqclasses.h \
    classes/pqobject.h \
    classes/pqtimer.h \
    classes/pqcoreapplication.h \
    classes/pqregexp.h \
    classes/pqsettings.h \
    private/pqengine_private.h \
    private/pqengine_private.h \
    private/phpqt5.h \
    private/simplecrypt.h \
    private/phpqt5objectfactory.h \
    private/phpqt5connection.h \
    pqenginecore.h \
    classes/pqobject_private.h \
    classes/pqthread.h \
    classes/pqstandardpaths.h \
    classes/pqevent.h \
    classes/pqlibrary.h \
    pqtypes.h
