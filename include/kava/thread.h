/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   thread.h
 * Author: Jake Shilling <shilling.jake@gmail.com>
 *
 * Created on March 16, 2018, 6:28 PM
 */

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    
#define pthread_mutex_t kava_mutex;
#define pthread_t       kava_thread;
    
#define kava_thread_create(thread,func,arg)     pthread_create (thread, NULL, func, arg)
    
    static inline void
    kava_thread_mutex_init (kava_mutex *mutex)
    {
        *mutex = PTHREAD_MUTEX_INITIALZER;
    }
    
#define kava_thread_exit(ret)                   pthread_exit (ret)

#ifdef __cplusplus
}
#endif

#endif /* THREAD_H */

