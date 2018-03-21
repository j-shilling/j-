#ifndef POSIX_THREAD_H
#define POSIX_THREAD_H

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

#if HAVE_PTHREAD_H

#  include <pthread.h>

#  define kava_mutex  pthread_mutex_t
#  define kava_thread pthread_t
#  define kava_cond   pthread_cond_t

#  define kava_thread_create(thread,func,arg)  pthread_create (thread, ((const pthread_attr_t *)0), func, arg)
#  define kava_thread_join(thread,retptr) pthread_join (thread, retptr)
#  define kava_thread_exit(retval) pthread_exit (retval)
#  define kava_mutex_init(mutexptr) pthread_mutex_init (mutexptr, ((const pthread_mutexattr_t *)0))
#  define kava_mutex_lock(mutexptr) pthread_mutex_lock (mutexptr)
#  define kava_mutex_unlock(mutexptr) pthread_mutex_unlock (mutexptr)
#  define kava_cond_init(condptr) pthread_cond_init (condptr, ((const pthread_condattr_t *)0))
#  define kava_cond_signal(condptr) pthread_cond_signal (condptr)
#  define kava_cond_wait(condptr,mutexptr) pthread_cond_wait (condptr, mutexptr)

#else

#  error /* Not yet implemented */

#endif /* HAVE_PTHREAD_H */

#ifdef __cplusplus
}
#endif

#endif /* POSIX_THREAD_H */
