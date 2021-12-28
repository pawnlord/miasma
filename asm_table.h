#ifndef ASM_TABLE_H
#define ASM_TABLE_H
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
typedef enum{
    ES = 0, CS, 
    SS, DS,
    FS, GS
} segment_register;

typedef struct {
    int opcode;
    char* name;
    char* arguments;

} operation;


void add_ops(operation* ops, int start, int count, ...);

operation* initialize_optable();

void get_op(int opcode, int byte_mode, operation* ops, operation* dest);
#endif