#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
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


int main(int argc, char** argv){
    // read header information
    FILE* fp = fopen(argv[1], "rb");
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    uchar* data = malloc(sz);
    char c;
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
    for(int i = 0; i < h.no_sections; i++){
        printf("name %d: %s\n", i, sections[i].name);
        printf("virtual_size %d: %d\n", i, sections[i].virtual_size);
        printf("virtual_address %d: %x\n", i, sections[i].virtual_address);
        printf("size_of_raw_data %d: %d\n", i, sections[i].size_of_raw_data);
        printf("p_raw_data %d: %x\n", i, sections[i].p_raw_data);
        printf("p_relocs %d: %x\n", i, sections[i].p_relocs);
        printf("p_linenumber %d: %x\n", i, sections[i].p_linenumber);
        printf("no_relocs %d: %d\n", i, sections[i].no_relocs);
        printf("no_linenumber %d: %d\n", i, sections[i].no_linenumber);
        printf("characteristics %d: %b\n\n", i, sections[i].characteristics);
    }

    printf("\nbytes: %d\n", bytes);
        
}