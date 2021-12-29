#include "asm_table.h"
#define OPERATIONS 15

// EXAMPLE: mov
operation movrmr = {0x88, "mov", "rm,r", 0x88};
operation movrmr32 = {0x89, "mov", "rm,r", 0x89};
operation movrrm = {0x8a, "mov", "r,rm", 0x8a};
operation movrrm32 = {0x8b, "mov", "r,rm", 0x8b};
operation movsrm = {0x8c, "mov", "rm,s", 0x8c};
operation movrms = {0x8d, "mov", "s,rm", 0x8d};
operation movalmo = {0xA0, "mov", "al,mo", 0xA0};
operation movamo32 = {0xA1, "mov", "a,mo", 0xA1};
operation movmoal = {0xA2, "mov", "al,mo", 0xA2};
operation movmoa32 = {0xA3, "mov", "a,mo", 0xA3};
operation movri = {0xB0, "mov", "r,i", 0xB0};
operation movri32 = {0xB8, "mov", "r,i", 0xB8};
operation movrmi = {0xC6, "mov", "rm,i", 0xC6};
operation movrmi32 = {0xC7, "mov", "rm,i", 0xC7};
operation learm32 = {0x8d, "lea", "r,m", 0x8d};
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
                     movrmi, movrmi32,
                     learm32); // mov

    return ops;
}

void get_op(int opcode, int address_mode, operation* ops, operation* dest){
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

segment_register get_sreg(int byte){
    switch(byte){
        case 0x2E:
            return CS;
        case 0x36:
            return SS;
        case 0x3E:
            return DS;
        case 0x26:
            return ES;
        case 0x64:
            return FS;
        case 0x65:
            return GS;
        default:
        return 0;
    }
}

void get_modrm(int modrm_byte, modrm_t* mrm, state s){
    mrm->mod = modrm_byte>>6;
    mrm->reg = (modrm_byte>>3) & (0b111);
    mrm->rm = (modrm_byte) & (0b111);
    mrm->is_deref = (mrm->mod!=0b11);
    mrm->disp = 0;
    
    if(mrm->mod == 1 ){
        mrm->disp = 1;
    } else if(mrm->mod == 2){
        mrm->disp = 2 + s.address_mode*2;
    }

    if(s.address_mode == 1){
        if(mrm->rm == 0b100 && mrm->mod != 0b11){
            mrm->needs_sib = 1;
        } else{
            mrm->needs_sib = 0;
        }
        if(mrm->rm != 0b100 && !(mrm->rm == 0b101 && mrm->mod == 0b0)){
            mrm->mrm_register = mrm->rm;
        } else if(mrm->rm == 101 && mrm->mod == 0b0){
            mrm->mrm_register = NONE;    
            mrm->disp = 4;
        } else{
            mrm->mrm_register = NONE;
        }
    } else{
        switch(mrm->rm){
            case 000:
                mrm->mrm_register = BXpSI;
                break; 
            case 001:
                mrm->mrm_register = BXpDI;
                break; 
            case 002:
                mrm->mrm_register = BPpSI;
                break; 
            case 003:
                mrm->mrm_register = BPpDI;
                break;
            case 004:
                mrm->mrm_register = ESI;
                break;  
            case 005:
                mrm->mrm_register = EDI;
                break;  
            case 006:
                if(mrm->mod == 0b0){
                    mrm->mrm_register = NONE;
                    mrm->disp = 2;
                } else{
                    mrm->mrm_register = EBP;
                }
                break;  
            case 007:
                mrm->mrm_register = EBX;
                break;  
        }
    }
    if(mrm->mod == 0b11)
        mrm->mrm_register = mrm->rm;
} 
void get_sib(int sib_byte, sib_t* sib, state s){
    sib->ss = sib_byte>>6;
    sib->index = (sib_byte>>3) & (0b111);
    sib->reg = (sib_byte) & (0b111);    
    
    sib->factor = (1<<sib->ss);
    sib->mrm_register = (sib->index == 0b100)?(NONE):(sib->index);
}