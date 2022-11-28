#pragma once
#include "defines.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct shader_file_info{
        
        FILE * vert_file;
        FILE * frag_file;
        u32 vert_size;
        u32 frag_size;
        char* vert_code;
        char * frag_code;
        char shader_loaded;
        
}shader_file_info;

static shader_file_info shader_data;

PINLINE void  read_shader_file(){
        shader_data.vert_file = NULL;
        shader_data.frag_file = NULL;
        shader_data.vert_file = fopen("/home/petermiller/Desktop/4coder/vulkAttempt/vert.spv", "rb+");
        if(!shader_data.vert_file){
                printf("fail");
                //return false;
        }
        shader_data.frag_file = fopen("/home/petermiller/Desktop/4coder/vulkAttempt/frag.spv", "rb+");
        shader_data.shader_loaded = 1;
        if (shader_data.vert_file == NULL || shader_data.frag_file == NULL) {
                shader_data.shader_loaded = 0;
                printf("could loadnt spv files");
                //return false;
        };
        fseek(shader_data.vert_file, 0, SEEK_END);
        fseek(shader_data.frag_file, 0, SEEK_END);
        
        shader_data.vert_size = ftell(shader_data.vert_file);
        shader_data.frag_size = ftell(shader_data.frag_file);
        
        shader_data.vert_code = malloc(shader_data.vert_size * sizeof(char ));
        shader_data.frag_code = malloc(shader_data.frag_size * sizeof(char));
        
        rewind(shader_data.vert_file);
        rewind(shader_data.frag_file);
        fread(shader_data.vert_code, 1, shader_data.vert_size, shader_data.vert_file);
        fread(shader_data.frag_code, 1, shader_data.frag_size, shader_data.frag_file);
        
        fclose(shader_data.vert_file);
        fclose(shader_data.frag_file);
        //return true;
}

