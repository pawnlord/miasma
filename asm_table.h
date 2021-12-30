#ifndef ASM_TABLE_H
#define ASM_TABLE_H
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

typedef enum {
    REG = 0,  MRM = 1, SREG = 2, MOFF = 3, IMM = 4,
    M = 5, AX = 6, CX = 7, DX = 8, BX = 9, SP = 10, BP = 11, SI = 12, DI = 13
} arg;

typedef struct {
    arg first;
    arg second;
} args_t;

typedef enum{
    ES = 0, CS, 
    SS, DS,
    FS, GS,
    SREG_NONE
} segment_register;

typedef struct {
    int opcode;
    char* name;
    args_t arguments;
    int readbyte; // for anything other than opcodes
    int is_32;
} operation;

typedef  enum {
    EAX = 0, ECX = 1, EDX = 2, EBX = 3,
    ESP = 4, EBP = 5, ESI = 6, EDI = 7, NONE = 8,
    BXpSI, BXpDI, BPpSI, BPpDI
} asm_register;

typedef struct {
    int mod;
    asm_register reg;
    int rm;
    int needs_sib;
    int disp;
    asm_register mrm_register;
    int is_deref;
    unsigned int disp_number;
} modrm_t;

typedef struct {
    int ss;
    int index;
    asm_register reg;
    asm_register mrm_register;
    int factor;
} sib_t;

typedef struct {
    int address_mode;
    int operand_mode;
    segment_register sreg;
} state;

void add_ops(operation* ops, int start, int count, ...);

operation* initialize_optable();

void get_op(int opcode, int address_mode, operation* ops, operation* dest);

segment_register get_sreg(int byte);

void get_modrm(int modrm_byte, modrm_t* mrm, state s); 
void get_sib(int sib_byte, sib_t* sib, state s); 
#endif