/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   type-flags.h
 * Author: Jake Shilling <shilling.jake@gmail.com>
 *
 * Created on March 14, 2018, 7:01 PM
 */

#ifndef TYPE_FLAGS_H
#define TYPE_FLAGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VOID     0x00
#define BYTE     0x01
#define SHORT    0x02
#define INT      0x03
#define LONG     0x04
#define FLOAT    0x05
#define DOUBLE   0x06
#define REF      0x07
#define ARR      0x08

typedef uint8_t type_flag;

#ifdef __cplusplus
}
#endif

#endif /* TYPE_FLAGS_H */

