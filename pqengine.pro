PQ_PHP_VERSION=7.3.1

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
#DEFINES += PQDEBUG
#DEFINES += PQDETAILEDDEBUG

##########################################
###### Don't change contents below! ######
##########################################
QT += core xml

CONFIG += c++11 qt
QT -= gui

contains(DEFINES, PQDEBUG) {
    QT += network
    TARGET = pqengine-debug
} else {
    TARGET = pqengine
}

contains(DEFINES, PQENGINE_LIBRARY) {
    TEMPLATE = lib

    contains(DEFINES, PQSTATIC): CONFIG += staticlib
    !contains(DEFINES, PQSTATIC): DEFINES += PQEXPORT
} else {
    TEMPLATE = app
    SOURCES += main.cpp
    DEFINES += PQEXPORT
}

equals(TEMPLATE, app) {
    SOURCES += main.cpp
}

equals(TEMPLATE, app) || contains(DEFINES, PQEXPORT) {
    SOURCES += plastiqclasses/plastiqclasses.cpp
    include(plastiqextensions.pri)

    for (PLASTIQ_EXTENSIONS_LIB, PLASTIQ_EXTENSIONS_LIBS) {
        INCLUDEPATH += $${PLASTIQ_EXTENSIONS_SRC_PATH}/$$dirname(PLASTIQ_EXTENSIONS_LIB)
        INCLUDEPATH += $${PLASTIQ_EXTENSIONS_SRC_PATH}/$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)

        !contains(QT, $$dirname(PLASTIQ_EXTENSIONS_LIB)) {
            QT += $$dirname(PLASTIQ_EXTENSIONS_LIB)
            message(Add Qt module: $$dirname(PLASTIQ_EXTENSIONS_LIB))
        }

        CONFIG(release, debug|release) {
            LIBS += -L$${PLASTIQ_EXTENSIONS_LIB_PATH}/$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)/release -lPlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)
            message(Add PlastiQ extension: `$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)' in `$${PLASTIQ_EXTENSIONS_LIB_PATH}' [release])
        }

        CONFIG(debug, debug|release) {
            LIBS += -L$${PLASTIQ_EXTENSIONS_LIB_PATH}/$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)/debug -lPlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)
            message(Add PlastiQ extension: `$$dirname(PLASTIQ_EXTENSIONS_LIB)/PlastiQ$$basename(PLASTIQ_EXTENSIONS_LIB)' in `$${PLASTIQ_EXTENSIONS_LIB_PATH}' [debug])
        }
    }

    INCLUDEPATH += $${PLASTIQ_EXTENSIONS_SRC_PATH}
}

win32 {
    LIBS += -L"$${PHP_LIB_PATH}/" -lphp7ts
    DEFINES += ZEND_WIN32
    DEFINES += PHP_WIN32
    DEFINES += WIN32
    #DEFINES += "\"_MSC_VER=1900\"" // only for MSVC
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
    private/plastiq_zif.cpp \
    plastiqproperty.cpp \
    private/plastiq.cpp \
    private/plastiq_zm.cpp \
    private/plastiq_connections.cpp \
    private/plastiqthreadcreator.cpp \
    private/qevent_cast.cpp \
    private/plastiq_debug.cpp \
    private/phpqt5objectfactory.cpp

HEADERS += \
    pqengine.h\
    pqengine_global.h \
    ipqengineext.h \
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


# installation
#libs.path = D:/PQBuilder-0.6.1/Tools/PHPQt5/pqengine/lib
includes.path = D:/PQBuilder-0.6.1/Tools/PHPQt5/pqengine/include

sources.path = D:/PQBuilder-0.6.1/Tools/PHPQt5/pqengine/source
private_sources.path = D:/PQBuilder-0.6.1/Tools/PHPQt5/pqengine/source/private
plastiq_sources.path = D:/PQBuilder-0.6.1/Tools/PHPQt5/pqengine/source/plastiqclasses

#libs.files = $$OUT_PWD/release/*
sources.files = *.h *.cpp *.pro *.pri *.txt
private_sources.files = private/*.h private/*.cpp private/*.txt
plastiq_sources.files = plastiqclasses/*.h plastiqclasses/*.cpp plastiqclasses/*.txt
INSTALLS += sources private_sources plastiq_sources #libs
