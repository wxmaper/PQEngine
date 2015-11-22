#ifndef PHPQT5_H_H
#define PHPQT5_H_H

#include <QDebug>

#include <QCoreApplication>
#include <QDir>

#include <QTextCodec>
#include <QPoint>
#include <QRect>
#include <QSize>

#include <QEvent>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QHoverEvent>
#include <QFocusEvent>
#include <QEnterEvent>
#include <QMetaObject>
#include <QMetaMethod>

#include "pqengine.h"

/* REMOVE MULTIPLE DEFINITION ERRORS :-( */
#ifdef PHP_WIN32
#include <time.h>
#include <ws2tcpip.h>
#endif

#include <math.h>
#include <io.h>
#include <inttypes.h>
#include <sys/stat.h>
/* end */

/* PHP */
#include <php.h>
#include <SAPI.h>
#include <php_main.h>
#include <php_variables.h>
#include <TSRM.h>
#include <zend_closures.h>
#include <zend_ini.h>
#include <zend_ini_scanner.h>
#include <zend_extensions.h>
#include <zend_highlight.h>
#include <zend_exceptions.h>
#include <zend_interfaces.h>
/* end */

#endif // PHPQT5_H_H

