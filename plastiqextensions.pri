# Path to directory containing the PlastiQ extensions sources
# e.g.: the `D:/pqengine-git/PlastiQExtensions' directory must contain `core', `gui', `widgets' etc.
# no need if PQENGINE_LIBRARY is defined
PLASTIQ_EXTENSIONS_SRC_PATH = D:/pqengine-git/PlastiQExtensions

CONFIG(release, debug|release): \
    PLASTIQ_EXTENSIONS_LIB_PATH = D:/pqengine-git/build-PlastiQExtensions-Desktop_Qt_5_8_0_MSVC2015_32bit-Release

CONFIG(debug, debug|release): \
    PLASTIQ_EXTENSIONS_LIB_PATH = D:/pqengine-git/build-PlastiQExtensions-Desktop_Qt_5_8_0_MSVC2015_32bit-Debug

PLASTIQ_EXTENSIONS_LIBS = core/Qt \
                          #core/QObject \
                         # core/QCoreApplication \
                         # widgets/QApplication \
                         # widgets/QWidget \
                        #  gui/QPaintDevice \
                   #       gui/QGuiApplication
#
