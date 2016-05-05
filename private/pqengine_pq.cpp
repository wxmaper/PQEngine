#include "phpqt5.h"
#include "pqengine_private.h"

extern void pq_register_class(const QMetaObject &metaObject);

void PQEnginePrivate::pq_register_extensions(PQDBG_LVL_D)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    foreach(IPQExtension *extension, m_extensions) {
        pq_register_extension(extension PQDBG_LVL_CC);
    }

    PQDBG_LVL_DONE();
}

void PQEnginePrivate::pq_register_extension(IPQExtension *extension PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
    PQDBGLPUP(extension->entry().fullName);
#endif

    PHPQt5::pq_core_init(PQDBG_LVL_C);

    /* Start extension */
    extension->start();

    /* Register extension classes */
    pq_register_classes(extension->classes() PQDBG_LVL_CC);

    /* Finalize extension */
    extension->finalize();

    PQDBG_LVL_DONE();
}

void PQEnginePrivate::pq_register_classes(QMetaObjectList classes PQDBG_LVL_DC)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    foreach (const QMetaObject &metaObject, classes) {
        PHPQt5::pq_register_class(metaObject PQDBG_LVL_CC);
    }

    PQDBG_LVL_DONE();
}
