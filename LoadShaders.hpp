#ifndef SHADER_HPP
#define SHADER_HPP

void shaderInfo(GLuint ProgramID);
GLuint attachShader(GLuint shaderId);
void dettachShader(GLuint programId, GLuint shaderId);

GLuint LoadShader(const char * shader_file_path);
GLuint LoadShaders(const char * vertex_file_path, const char * control_file_path, const char * evaluation_file_path, const char * fragment_file_path);
GLuint LoadShaders(const char * vertex_file_path, const char * geometry_file_path, const char * fragment_file_path);
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

#endif
