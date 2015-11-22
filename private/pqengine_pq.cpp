#include "phpqt5.h"
#include "pqengine_private.h"

extern void pq_register_class(const QMetaObject &metaObject TSRMLS_DC);

void PQEnginePrivate::pq_register_extensions(TSRMLS_D)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::pq_register_extensions()");
#endif

    foreach (IPQExtension *pqExtension, m_PQExtensions.values()) {
        pq_register_extension(pqExtension TSRMLS_CC);
    }
}

void PQEnginePrivate::pq_register_extension(IPQExtension *pqExtension TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG2("PQEnginePrivate::pq_register_extension()", pqExtension->entry().fullName);
#endif

    PHPQt5::pq_core_init(TSRMLS_C);

    /* Start extension */
    pqExtension->start();

    /* Register extension classes */
    pq_register_classes( pqExtension->classes() TSRMLS_CC );

    /* Finalize extension */
    pqExtension->finalize();
}

void PQEnginePrivate::pq_register_classes(QMetaObjectList classes TSRMLS_DC)
{
#ifdef PQDEBUG
    PQDBG("PQEnginePrivate::pq_register_classes()");
#endif

    foreach (const QMetaObject &metaObject, classes) {
        PHPQt5::pq_register_class(metaObject TSRMLS_CC);
    }
}
