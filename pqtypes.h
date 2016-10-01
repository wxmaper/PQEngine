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

#ifndef PQ_TYPES_H
#define PQ_TYPES_H

#include <QMetaType>

#define PQ_STATIC_PREFIX "static_"
#define PQ_STATIC(M) static_##M
#define PQ_IS_STATIC !__pq_thisPtr

#endif // PQ_TYPES_H
