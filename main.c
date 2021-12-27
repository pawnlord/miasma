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
    int32_t characteristics;
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
    

}

int read_sections(char* data, section** p_sections){

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
    printf("%x\n", h.machine);
    printf("%d\n", h.no_sections);
    printf("%x\n", h.p_symtab);
    printf("%d\n", (uint)h.sz_optional);
    section* sections = malloc((h.no_sections)*sizeof(section));
    read_sections(data, &sections);
    printf("\nbytes: %d\n", bytes);
        
}