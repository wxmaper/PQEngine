# Path to directory containing the PlastiQ extensions sources
# e.g.: the `D:/pqengine-git/PlastiQExtensions' directory must contain `core', `gui', `widgets' etc.
# no need if PQENGINE_LIBRARY is defined
PLASTIQ_EXTENSIONS_SRC_PATH = D:/pqengine-git/PlastiQExtensions

CONFIG(release, debug|release): \
    PLASTIQ_EXTENSIONS_LIB_PATH = D:/pqengine-git/build-PlastiQExtensions-Desktop_Qt_5_12_0_MSVC2017_32bit-Release

CONFIG(debug, debug|release): \
    PLASTIQ_EXTENSIONS_LIB_PATH = D:/pqengine-git/build-PlastiQExtensions-Desktop_Qt_5_12_0_MSVC2017_32bit-Debug

PLASTIQ_EXTENSIONS_LIBS = core/Qt \
                          core/QObject \
                          core/QCoreApplication \
                          core/QThread \
                          core/QTimer \
                          widgets/QApplication \
                          widgets/QWidget \
                          gui/QPaintDevice \
                          gui/QGuiApplication \
                          widgets/QPushButton \
                          widgets/QAbstractButton \
                          widgets/QLabel \
                          widgets/QFrame \
                          widgets/QMessageBox \
                          widgets/QDialog \
                          core/QEvent \
                          gui/QMoveEvent \
                          gui/QCloseEvent \
                          core/QSize \
                          core/QRect
