#ifndef PQ_TYPES_H
#define PQ_TYPES_H

#include <QMetaType>

typedef struct _pq_closure {
    void *closure_zo; // zend_object*
} PQClosure;

Q_DECLARE_METATYPE(PQClosure)
//Q_DECLARE_METATYPE(PQClosure*)

#endif // PQ_TYPES_H
