# PQ_PHP_VERSION=5.4.45 # for Windows XP/Vista
PQ_PHP_VERSION=5.6.11 # for Windows 7 or later
PHP_SRC_PATH="C:/src/php-$${PQ_PHP_VERSION}-src"

# for windows only
DEFINES += ZEND_WIN32
DEFINES += PHP_WIN32
DEFINES += WIN32

# use this define for debug messages
DEFINES += PQDEBUG

INCLUDEPATH += \
    $${PHP_SRC_PATH}\
    $${PHP_SRC_PATH}/main\
    $${PHP_SRC_PATH}/Zend\
    $${PHP_SRC_PATH}/TSRM\
    $${PHP_SRC_PATH}/ext/standard\
    private

# copy php5ts.dll and php5ts.lib into $${PHP_SRC_PATH}/dev
LIBS += "$${PHP_SRC_PATH}/dev/php5ts.dll"
LIBS += -L"$${PHP_SRC_PATH}/dev" -lphp5ts


##########################################
##########################################
##########################################
QT       += core xml

TARGET = pqengine
TEMPLATE = lib
CONFIG +=  dll qt c++11 staticlib
QMAKE_LFLAGS += -static -static-libgcc # required a statically linked Qt

DEFINES += PQEXPORT

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
    pqenginecore.cpp

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
    pqenginecore.h
