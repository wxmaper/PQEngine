#ifndef PQENGINE_GLOBAL_H
#define PQENGINE_GLOBAL_H

#define PQENGINE_MAJOR_VERSION 0
#define PQENGINE_MINOR_VERSION 4
#define PQENGINE_RELEASE_VERSION 0
#define PQENGINE_EXTRA_VERSION ""
#define PQENGINE_VERSION "0.4"
#define PQENGINE_VERSION_ID 40

extern void default_ub_write(const QString &msg, const QString &title);

enum MemoryManager {
    PHPQt5 = 0,
    Zend,
    Hybrid
};

#ifdef PQDEBUG
#include <QDebug>
//#define PQDBG(msg) qDebug() << msg;
//#define PQDBG2(line1, line2) qDebug() << line1 << ":" << line2;
#define PQDBG(msg) default_ub_write(msg, "DEBUG: ");
#define PQDBG2(line1, line2) default_ub_write(QString("%1 %2").arg(QString(line1)).arg(QString(line2)), "DEBUG: ");
#else
#define PQDBG(msg) Q_UNUSED(msg);
#define PQDBG2(line1, line2) Q_UNUSED(line1); Q_UNUSED(line2);
#endif

#define PQAPI __attribute__ ((visibility("default")))

#ifdef PQSTATIC
# define PQDLAPI
#else
# ifdef PQEXPORT
#   ifdef PHP_WIN32
#     define PQDLAPI __declspec(dllexport)
#   else
#     define PQDLAPI __attribute__((visibility("default")))
#   endif
# else
#   define PQDLAPI
# endif
#endif

#endif // PQENGINE_GLOBAL_H
