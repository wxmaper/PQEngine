/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PQEngine.
**
** BEGIN LICENSE: MPL 2.0
**
** This Source Code Form is subject to the terms of the Mozilla Public
** License, v. 2.0. If a copy of the MPL was not distributed with this
** file, You can obtain one at http://mozilla.org/MPL/2.0/.
**
** END LICENSE
**
****************************************************************************/

#include "phpqt5.h"
#include "pqengine_private.h"

extern void pq_register_class(const QMetaObject &metaObject);

void PQEnginePrivate::pq_register_extensions(PQDBG_LVL_D)
{
#ifdef PQDEBUG
    PQDBG_LVL_PROCEED(__FUNCTION__);
#endif

    PHPQt5::pq_core_init(PQDBG_LVL_C);

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

    if(extension->entry().have_instance) {
        if(extension->entry().use_instance) { PQDBGLPUP("have instance; used"); }
        else { PQDBGLPUP("have instance; not used"); }
    }
    else { PQDBGLPUP("not have instance"); }

    if(extension->entry().have_pre) {
        if(extension->entry().use_pre) { PQDBGLPUP("have pre; used"); }
        else { PQDBGLPUP("have pre; not used"); }
    }
    else { PQDBGLPUP("not have pre"); }

    if(extension->entry().have_ub_write) {
        if(extension->entry().use_ub_write) { PQDBGLPUP("have ub_write; used"); }
        else { PQDBGLPUP("have ub_write; not used"); }
    }
    else { PQDBGLPUP("not have ub_write"); }
#endif

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
