#include "asm_table.h"
#define OPERATIONS 14

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
void add_ops(operation* ops, int start, int count, ...){
    va_list args;
    va_start(args, count);
    for(int i = start; i < count; i++){
        ops[i] = va_arg(args, operation);
    }
    va_end(args);
}

operation* initialize_optable(){
    operation* ops = malloc(OPERATIONS*(sizeof(operation)));
    
    add_ops(ops, 0, OPERATIONS, movrmr, movrmr32,
                     movrrm, movrrm32,
                     movsrm, movrms,
                     movalmo, movamo32,
                     movmoal, movmoa32,
                     movri, movri32,
                     movrmi, movrmi32); // mov

    return ops;
}

void get_op(int opcode, int byte_mode, operation* ops, operation* dest){
    for(int i = 0; i < OPERATIONS; i++){
        if(ops[i].opcode == opcode){
            *dest = ops[i];
            return;
        }
    }
    *dest = no_op;    
}

void debug_op(operation op){
    printf("%x: %s %s\n", op.opcode, op.name, op.arguments);
}
