/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   memory.h
 * Author: Jake Shilling <shilling.jake@gmail.com>
 *
 * Created on March 15, 2018, 9:37 AM
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern void *kava_alloc  (size_t size);
    extern void *kava_alloc0 (size_t size);
    extern void  kava_free   (void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* MEMORY_H */

