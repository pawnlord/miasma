#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
typedef enum{
    ES = 0, CS, 
    SS, DS,
    FS, GS
} sreg;

typedef struct {
    int opcode;
    char* name;
    char* arguments;

} operation;

// EXAMPLE: mov
operation movrmr = {0x88, "mov", "rm,r"};
operation movrmr32 = {0x89, "mov", "rm,r"};
operation movrrm = {0x8a, "mov", "r,rm"};
operation movrrm32 = {0x8b, "mov", "r,rm"};
operation movsrm = {0x8c, "mov", "rm,s"};
operation movrms = {0x8d, "mov", "s,rm"};
operation movalmo = {0xA0, "mov", "al,mo"};
operation movamo32 = {0xA1, "mov", "a,mo"};
operation movmoal = {0xA2, "mov", "al,mo"};
operation movmoa32 = {0xA3, "mov", "a,mo"};
operation movri = {0xB0, "mov", "r,i"};
operation movri32 = {0xB8, "mov", "r,i"};
operation movrmi = {0xC6, "mov", "al,mo"};
operation movrmi32 = {0xC7, "mov", "a,mo"};
operation no_op = {0,"",""};
// 14

void add_ops(operation* ops, int start, int count, ...);

operation* initialize_optable();

void get_op(int opcode, int byte_mode, operation* ops, operation* dest);