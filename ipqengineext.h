#ifndef IPQENGINEEXT_H
#define IPQENGINEEXT_H

#include <QCoreApplication>
#include <QMetaObject>
#include <QMetaMethod>

class IPQExtension;
class QMetaObjectList : public QList<QMetaObject> {};

/*
#define PQEXT(classname) classname::getPQExtension()

#define IPQEXT_INIT(extname) \
class extname : public IPQExtension {\
public:\
    static use_instance() { getPQExtension().this_appInstance = true; }\
    static use_ub_write() { getPQExtension().this_ub_write = true; }\
    static use_pre() { getPQExtension().this_pre = true; }\
    static QMetaObjectList classes();\
    static bool start();\
    static bool finalize();\
    \
    static PQExtension getPQExtension() {\
        PQEXT_ENTRY_START_EX(extname)

#define PQEXT_ENTRY_START
#define PQEXT_ENTRY_START_EX(extname)\
    static PQExtension pqExtensionInstance = {\
        extname::classes,\
        extname::start,\
        extname::finalize,\
        #extname,

#define PQEXT_FULLNAME(fullName) fullName,
#define PQEXT_AUTHOR(author) author,
#define PQEXT_VERSION(version) version,
#define PQEXT_DESC(desc) desc,
#define PQEXT_LIBS(libs) libs,
#define PQEXT_QTDEPENDS(qtdepends) qtdepends,
#define PQEXT_INCLUDES(includes) includes,
#define PQEXT_CONFIG(config) config,

#define PQEXT_ENTRY_END \
    }; return pqExtensionInstance; }

#define IPQEXT_END };

#define IPQEXT_NO_INSTANCE \
    QCoreApplication *appInstance() { return 0; }

#define IPQEXT_NO_UB_WRITE \
    static void ub_write(const QString &msg) {}

#define IPQEXT_NO_PRE \
    static void pre(const QString &msg, const QString &title) {}

#define IPQEXT_INSTANCE \
    static QCoreApplication *appInstance(int argc, char** argv);

#define IPQEXT_UB_WRITE \
    static void ub_write(const QString &msg);

#define IPQEXT_PRE \
    static void pre(const QString &msg, const QString &title);

#define PQEXT_NO_INSTANCE 0, false, 0,
#define PQEXT_NO_UB_WRITE 0, false, 0,
#define PQEXT_NO_PRE 0, false, 0,

#define PQEXT_INSTANCE(extname) 1, false, extname::appInstance,
#define PQEXT_UB_WRITE(extname) 1, false, extname::ub_write,
#define PQEXT_PRE(extname) 1, false, extname::pre,

typedef struct ipqengineext {
    QMetaObjectList (*classes)(); // classes
    bool (*start)(); // init *
    bool (*finalize)(); // finalize *

    const char *fullName; // fullName *
    const char *author; // author *
    const char *version; // version *
    const char *description; // descriptin *
    const char *libs; // libs *
    const char *qtdepends; // qtdepends *
    const char *includes; // includes *
    const char *config; // config *

    bool have_appInstance; // have_appInstance *
    bool this_appInstance; // this_appInstance *
    QCoreApplication* (*appInstance)(int argc, char** argv); // appInstance *

    bool have_ub_write; // have_ub_write *
    bool this_ub_write; // this_ub_write *
    void (*ub_write)(const QString &msg); // ub_write *
   // static_cast<void (PQComboBox::*)(int)>(&PQComboBox::currentIndexChanged),

    bool have_pre; // have_pre *
    bool this_pre; // this_pre *
    void (*pre)(const QString &msg, const QString &title); // pre *
} PQExtension;


*/

#define PQEXT_INSTANCE(extname) extname::instance

typedef struct pqext_entry {
    const char *fullName;

    bool                    have_instance;
    bool                    use_instance;
    QCoreApplication *      (*instance)(int argc, char** argv);

    bool                    have_ub_write;
    bool                    use_ub_write;
    void                    (*ub_write)(const QString &msg);

    bool                    have_pre;
    bool                    use_pre;
    void                    (*pre)(const QString &msg, const QString &title);
} PQExtensionEntry;

#define PQEXT_ENTRY_START(extname)\
    PQExtensionEntry entry() { return pqExtEntry; }\
    PQExtensionEntry pqExtEntry = {\
        #extname,

#define PQEXT_NO_INSTANCE false, false, 0,
#define PQEXT_INSTANCE(extname) true, false, extname::instance,

#define PQEXT_NO_UB_WRITE false, false, 0,
#define PQEXT_UB_WRITE(extname) true, false, extname::ub_write,

#define PQEXT_NO_PRE false, false, 0
#define PQEXT_PRE(extname) true, false, extname::pre

#define PQEXT_ENTRY_END\
    };

#define PQEXT_USE(usefn) void use_##usefn() { \
    if(pqExtEntry.have_##usefn) {\
        pqExtEntry.use_##usefn = true; \
    }\
}

class IPQExtension {
public:
    virtual QMetaObjectList classes() = 0;
    virtual bool start() = 0;
    virtual bool finalize() = 0;

    virtual void use_instance() = 0;
    virtual void use_ub_write() = 0;
    virtual void use_pre() = 0;

    virtual PQExtensionEntry entry() = 0;
};

class PQExtensionList : public QList<IPQExtension*> {};
class PQExtensionHash : public QHash<QString, IPQExtension*> {};

#endif // IPQENGINEEXT_H
