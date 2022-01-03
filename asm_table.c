#include "asm_table.h"
#define OPERATIONS 14

// EXAMPLE: mov
operation movrmr = {0x88, "mov", {MRM, REG}, 0x88, 0};
operation movrmr32 = {0x89, "mov", {MRM, REG}, 0x89, 1};
operation movrrm = {0x8a, "mov", {REG, MRM}, 0x8a, 0};
operation movrrm32 = {0x8b, "mov", {REG, MRM}, 0x8b, 1};
operation movsrm = {0x8c, "mov", {MRM, SREG}, 0x8c, 1};
operation movalmo = {0xA0, "mov", {AX, MOFF8}, 0xA0, 0};
operation movamo32 = {0xA1, "mov", {AX, MOFF32}, 0xA1, 1};
operation movmoal = {0xA2, "mov", {MOFF8, AX}, 0xA2, 0};
operation movmoa32 = {0xA3, "mov", {MOFF32, AX}, 0xA3, 1};
operation movri = {0xB0, "mov", {REG, IMM8}, 0xB0, 0};
operation movri32 = {0xB8, "mov", {REG, IMM32}, 0xB8, 1};
operation movrmi = {0xC6, "mov", {MRM, IMM8}, 0xC6, 0};
operation movrmi32 = {0xC7, "mov", {MRM, IMM32}, 0xC7, 1};
operation learm32 = {0x8d, "lea", {REG, M}, 0x8d, 1};
operation no_op = {0,"", {0,0}, 0, 0};
// 15
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
                     movsrm, 
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
        return -1;
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
        if(!(mrm->rm == 0b101 && mrm->mod == 0b0)){
            mrm->mrm_register = mrm->rm;
        } else if(mrm->rm == 101 && mrm->mod == 0b0){
            mrm->mrm_register = NONE;    
            mrm->disp = 4;
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


const char* reg_to_string(register reg, int size){
    // 8
    if(size == 1){
        switch(reg){
            case EAX:
                return "AL";
                break;
            case ECX:
                return "CL";
                break;
            case EDX:
                return "DL";
                break;
            case EBX:
                return "BL";
                break;
            case ESP:
                return "AH";
                break;
            case EBP:
                return "CH";
                break;
            case ESI:
                return "DH";
                break;
            case EDI:
                return "BH";
                break;
            default:
                return "NONE";
                break;
        }
    }
    // 16
    if(size == 2){

        switch(reg){
            case EAX:
                return "AX";
                break;
            case ECX:
                return "CX";
                break;
            case EDX:
                return "DX";
                break;
            case EBX:
                return "BX";
                break;
            case ESP:
                return "SP";
                break;
            case EBP:
                return "BP";
                break;
            case ESI:
                return "SI";
                break;
            case EDI:
                return "DI";
                break;
            case BXpSI:
                return "BX + SI";
                break;
            case BXpDI:
                return "BX + DI";
                break;
            case BPpSI:
                return "BP + SI";
                break;
            case BPpDI:
                return "BP + DI";
                break;
            default:
                return "NONE";
                break;
        }
    }
    // 32
    if(size == 4){

        switch(reg){
            case EAX:
                return "EAX";
                break;
            case ECX:
                return "ECX";
                break;
            case EDX:
                return "EDX";
                break;
            case EBX:
                return "EBX";
                break;
            case ESP:
                return "ESP";
                break;
            case EBP:
                return "EBP";
                break;
            case ESI:
                return "ESI";
                break;
            case EDI:
                return "EDI";
                break;
            default:
                return "NONE";
                break;
        }
    }
    
}

const char* sreg_to_string(segment_register sreg){
    switch(sreg){
        case ES:
            return "ES";
            break;
        case CS:
            return "CS";
            break;
        case SS:
            return "SS";
            break;
        case DS:
            return "DS";
            break;
        case FS:
            return "FS";
            break;
        case GS:
            return "GS";
            break;
        default:
            return "NONE";
            break;
    }
}