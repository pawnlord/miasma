#include "asm_table.h"
#define OPERATIONS 14

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
        printf("%x\n", ops[i].opcode);
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
