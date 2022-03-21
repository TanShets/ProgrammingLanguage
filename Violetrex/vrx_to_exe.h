#include <stdio.h>
#include "symbols.h"
#define LOWER_LIMIT 20
#define UPPER_LIMIT 44

#ifndef _CRT_SYSTEM_DEFINED
#define _CRT_SYSTEM_DEFINED
int __cdecl system(const char* _Command);
#endif

void get_exec_file(char* file_data, char* name_of_exec_file){
    if(name_of_exec_file == NULL || strlen(name_of_exec_file) == 0)
        return;
    FILE* fp1 = fopen("compiler.c", "r");
    FILE* fp2 = fopen("compiler_copy.c", "w");
    int line = 1, hasWritten = 0;
    char c;
    while((c = getc(fp1)) != EOF){
        if(line >= LOWER_LIMIT && line <= UPPER_LIMIT){
            if(!hasWritten){
                fputs("char* line = \"", fp2);
                for(int i = 0; i < strlen(file_data); i++){
                    if(file_data[i] == '\n'){
                        putc('\\', fp2);
                        putc('n', fp2);
                    }
                    else if(file_data[i] == '"'){
                        putc('\\', fp2);
                        putc('"', fp2);
                    }
                    else
                        putc(file_data[i], fp2);
                }
                fputs("\";\n", fp2);
                hasWritten = 1;
            }
        }
        else{
            putc(c, fp2);
        }

        if(c == '\n') line++;
    }
    fclose(fp1);
    fclose(fp2);
    char* cmd = merge_strings("gcc compiler_copy.c -o ", name_of_exec_file);
    system(cmd);
    remove("compiler_copy.c");
}