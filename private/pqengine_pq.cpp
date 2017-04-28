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

void PQEnginePrivate::pq_register_extensions()
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    foreach(IPQExtension *extension, pqExtensions) {
        pq_register_extension(extension);
    }

    PQDBG_LVL_DONE();
}

void PQEnginePrivate::pq_register_extension(IPQExtension *extension)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif

    extension->start();
    pq_register_plastiq_classes(extension->plastiqClasses());
    extension->finalize();

    PQDBG_LVL_DONE();
}


void PQEnginePrivate::pq_register_plastiq_classes(const PlastiQMetaObjectList &classes)
{
#ifdef PQDEBUG
    PQDBG_LVL_START(__FUNCTION__);
#endif
    PHPQt5::pq_register_basic_classes();

    foreach(const PlastiQMetaObject &metaObject, classes) {
        PHPQt5::pq_register_plastiq_class(metaObject);
    }

    PQDBG_LVL_DONE();
}
