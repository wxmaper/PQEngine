#ifndef PQ_TYPES_H
#define PQ_TYPES_H

#include <QMetaType>

#define PQ_STATIC_PREFIX "static_"
#define PQ_STATIC(M) static_##M
#define PQ_IS_STATIC !__pq_thisPtr

typedef struct _pq_closure {
    void *closure_zo; // zend_object*
} PQClosure;

Q_DECLARE_METATYPE(PQClosure)
//Q_DECLARE_METATYPE(PQClosure*)

#endif // PQ_TYPES_H
