#include <string>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>


const char* read_file(const char *filename);

bool did_shader_compile(uint32_t shader);
uint32_t create_shader(GLenum shader_type, const char* shader_source);
uint32_t create_shader_program(const char *vert_filename, const char *frag_filename);
