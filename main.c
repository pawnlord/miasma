#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "asm_table.h"
typedef unsigned char uchar;
typedef unsigned char uint;

typedef struct {
    int16_t machine;
    int16_t no_sections;
    int32_t p_symtab;
    int16_t sz_optional;
    
    int eoheader;
    int eoheaders;
} header;

typedef struct {
    char name[8];
    int32_t virtual_size;
    int32_t virtual_address;
    int32_t size_of_raw_data;
    int32_t p_raw_data;
    int32_t p_relocs;
    int16_t p_linenumber;
    int16_t no_relocs;
    int16_t no_linenumber;
    int32_t characteristics;

    int location;
} section;


int get_number(char* data, int index, int size){
    int result = 0;
    char* raw = data+index;
    for(int i = 0; i < size; i++){
        result += raw[i] * (pow(2,i*8));
    }
    return result;
}

int read_header(char* data, header* h){
    int start = 4;
    char* check = data;
    while(!(*check == 'P' && *(check+1) == 'E' && *(check+2) == '\0' && *(check+3) == '\0')){
        check+=1;
        start+=1;
    }
    h->machine = get_number(data, start, 2);
    h->no_sections = get_number(data, start+2, 2);
    h->p_symtab = get_number(data, start+8, 4);
    h->sz_optional = get_number(data, start+16, 4);
    h->eoheader = start+20;
    h->eoheaders = start+20+h->sz_optional;
}

int read_sections(char* data, section* p_sections, header h){
    int current_index = h.eoheaders;
    printf("ns: %d\n", h.no_sections);
    while(data[current_index] != '.'){
        current_index += 1;
    }
    for(int i = 0; i < h.no_sections; i++, current_index+=40){

        for(int namei = 0; namei < 8; namei++){
            p_sections[i].name[namei] = data[current_index+namei];
        }   
        p_sections[i].virtual_size = get_number(data, current_index+8, 4);
        p_sections[i].virtual_address = get_number(data, current_index+12, 4);
        p_sections[i].size_of_raw_data = get_number(data, current_index+16, 4);
        p_sections[i].p_raw_data = get_number(data, current_index+20, 4);
        p_sections[i].p_relocs = get_number(data, current_index+24, 4);
        p_sections[i].p_linenumber = get_number(data, current_index+28, 4);
        p_sections[i].no_relocs = get_number(data, current_index+32, 2);
        p_sections[i].no_linenumber = get_number(data, current_index+34, 2);
        p_sections[i].characteristics = get_number(data, current_index+36, 4);
        p_sections[i].location = current_index;
    }
}

void debug_section(section s, int i){    
    printf("name %d: %s\n", i, s.name);
    printf("virtual_size %d: %d\n", i, (uint)s.virtual_size);
    printf("virtual_address %d: %x\n", i, (uint)s.virtual_address);
    printf("size_of_raw_data %d: %d\n", i, (uint)s.size_of_raw_data);
    printf("p_raw_data %d: %x\n", i, (uint)s.p_raw_data);
    printf("p_relocs %d: %x\n", i, (uint)s.p_relocs);
    printf("p_linenumber %d: %x\n", i, (uint)s.p_linenumber);
    printf("no_relocs %d: %d\n", i, (uint)s.no_relocs);
    printf("no_linenumber %d: %d\n", i, (uint)s.no_linenumber);
    printf("characteristics %d: %b\n\n", i,  (uint)s.characteristics);
}

int disbin(int dec){
    int bin = 0;
    for(int i = 0; i < sizeof(dec)*8; i++){
        bin+= ((dec>>i) * 1) * pow(10, i);
    }
    return bin;
}

void output_op(operation op, uchar* local_data, int pointer, state s){
    // figure out if we have an mod r/m byte
    int hasMRM = (op.arguments.first == MRM || op.arguments.second == MRM), mrm_byte;
    modrm_t mrm;
    sib_t sib;
    pointer+=1;

    if(hasMRM){
        get_modrm(local_data[pointer], &mrm, s);
        mrm_byte = local_data[pointer];
        pointer+=1;
        if(mrm.needs_sib){
            get_sib(local_data[pointer], &sib, s);
            pointer+=1;
        }
        mrm.disp_number = get_number(local_data, pointer, mrm.disp);
        pointer += mrm.disp;
    }
    char* fullop = malloc(255); // if we have more than 255, we messed up
    for(int i = 0; i < 255; i++){
        fullop[i] = 0;
    }
    strcpy(fullop, op.name);
    arg oargs[2] = {op.arguments.first, op.arguments.second};
    int immediate_value;
    for(int i = 0; i < 2; i++){
        int arg_size = 1;
        if(op.is_32){
            arg_size = 2+2*s.operand_mode;
        }
        switch(oargs[i]){
            case MRM:
                if(mrm.is_deref){
                    if(mrm.disp == 0){
                        sprintf(fullop, "%s [%s]", fullop, reg_to_string(mrm.mrm_register, 2+s.address_mode*2));
                    } else {
                        sprintf(fullop, "%s [%s + 0x%x]", fullop, reg_to_string(mrm.mrm_register, 2+s.address_mode*2), mrm.disp_number);
                    }
                } else {
                    sprintf(fullop, "%s %s", fullop, reg_to_string(mrm.mrm_register, arg_size));
                }
            break;
            case REG:
                sprintf(fullop, "%s %s", fullop, reg_to_string(mrm.mrm_register, arg_size));
            break;
            case IMM8:
                immediate_value = get_number(local_data, pointer, 1);    
                pointer += 1;
                sprintf(fullop, "%s %x", fullop, immediate_value);
            break;
            case IMM32:
                immediate_value = get_number(local_data, pointer, 2+s.operand_mode*2);
                pointer += 2+s.operand_mode*2;
                sprintf(fullop, "%s %x", fullop, immediate_value);
            break;
            case MOFF8:
                immediate_value = get_number(local_data, pointer, 1);    
                pointer += 1;
                sprintf(fullop, "%s [%x]", fullop, immediate_value);
            break;
            case MOFF32:
                immediate_value = get_number(local_data, pointer, 2+s.operand_mode*2);
                pointer += 2+s.operand_mode*2;
                sprintf(fullop, "%s [%x]", fullop, immediate_value);
            break;
            case SREG:
                sprintf(fullop, "%s %s", fullop, sreg_to_string(s.sreg));
            break;
            case M:
            break;
            default:
                sprintf(fullop, "%s %s", fullop, reg_to_string(oargs[i]-AX, arg_size));
            break;
        }
        if(i == 0){
            strcat(fullop, ",");
        }
    }
    printf("%x: %s\n", op.opcode, fullop);
}

void disassemble_sections(uchar* data, section* sections, header h){
    state s = {1, 1, SREG_NONE};
    operation* ops = initialize_optable();
    char* hex = malloc(100);
    for(int i = 0; i < h.no_sections; i++){
        section sec = sections[i];
        char* local_section = data+sec.p_raw_data;
        printf("Disassembly for section %s: (@%x)\n", sec.name, sec.p_raw_data);
        operation op;
        int rep = 0;
        for(int pointer = 0; pointer < sec.virtual_size; pointer++){
            get_op(local_section[pointer] & 0xFF, s.address_mode, ops, &op);
            if(op.opcode != 0){
                output_op(op, local_section, pointer, s);
                s.sreg = SREG_NONE;
            } else{
                switch(op.readbyte){
                    case 0xF3:
                        rep = 1;
                    break;
                    case 0xF2:
                        rep = 2;
                    break;
                    case 0xF0:
                        rep = 3;
                    break;
                    case 0x67:
                        s.address_mode = !s.address_mode;
                    break;
                    case 0x66:
                        s.operand_mode = !s.operand_mode;
                    break;
                    default:;
                        segment_register temp = get_sreg(op.readbyte);
                        if(temp != -1){
                            s.sreg = temp;
                        }
                }
            }
        }
    }
}

int main(int argc, char** argv){
    // read header information
    FILE* fp = fopen(argv[1], "rb");
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    uchar* data = malloc(sz);
    uchar c;
    int bytes = 0;
    while(bytes < sz){
        c = fgetc((FILE*)fp);
        data[bytes] = c;
        bytes+=1;
    }
    header h;
    read_header(data, &h);
    section* sections = malloc((h.no_sections)*sizeof(section));
    read_sections(data, sections, h);
    disassemble_sections(data, sections, h);
    
}