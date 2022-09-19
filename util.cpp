#include "./util.h"

const char* read_file(const char *filename) {
	FILE *fp = fopen(filename, "r");
	if (!fp) {
		throw std::runtime_error("Unable to read file");
	}

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *buf = (char*)calloc(file_size + 1, sizeof(char));

	fread(buf, sizeof(char), file_size, fp);
	fclose(fp);

	return buf;
}


bool did_shader_compile(uint32_t shader) {
	int success = 0;
	char log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, log);
		printf("%s\n", log);
		return false;
	}

	return true;
}

uint32_t create_shader(GLenum shader_type, const char *shader_source) {
	printf("%s\n", shader_source);
	uint32_t shader_id = glCreateShader(shader_type);
	glShaderSource(shader_id, 1, &shader_source, NULL);
	glCompileShader(shader_id);

	if (!did_shader_compile(shader_id)) 
		throw std::runtime_error("Unable to compile shader");

	return shader_id;
}

uint32_t create_shader_program(const char *vert_filename, const char *frag_filename) {
	auto vert_file = read_file(vert_filename);
	auto frag_file = read_file(frag_filename);

	uint32_t vert_shader = create_shader(GL_VERTEX_SHADER, vert_file);
	uint32_t frag_shader = create_shader(GL_FRAGMENT_SHADER, frag_file);

	uint32_t shader_prog = glCreateProgram();
	glAttachShader(shader_prog, vert_shader);
	glAttachShader(shader_prog, frag_shader);
	glLinkProgram(shader_prog);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	free((void*)vert_file);
	free((void*)frag_file);

	return shader_prog;
}
