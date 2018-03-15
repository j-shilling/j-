/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lexer.h
 * Author: Jake Shilling <shilling.jake@gmail.com>
 *
 * Created on March 15, 2018, 9:28 AM
 */

#ifndef LEXER_H
#define LEXER_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _parameter_list {
        char *type;
        char *identifier;
        
        struct _parameter_list *next;
    } parameter_list;
    
    typedef struct {
        char *return_type;
        char *identifier;
        
        parameter_list *parameters;
    } function_prototype;

#ifdef __cplusplus
}
#endif

#endif /* LEXER_H */

