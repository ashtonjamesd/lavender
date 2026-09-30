#ifndef concurrency_h
#define concurrency_h

#include <pthread.h>

#define with_mutex(mutex) \
    for ( \
        int _lock_once = (pthread_mutex_lock(mutex), 1);  \
        _lock_once != 0; \
        pthread_mutex_unlock(mutex), _lock_once = 0 \
    )

#endif