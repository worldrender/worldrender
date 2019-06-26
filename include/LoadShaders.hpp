#ifndef SHADER_HPP
#define SHADER_HPP

#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

void shaderInfo(GLuint ProgramID);
GLuint attachShader(GLuint shaderId);
void dettachShader(GLuint programId, GLuint shaderId);

GLuint LoadShaders(const char * vertex_file_path, const char * control_file_path, const char * evaluation_file_path, const char * fragment_file_path);
GLuint LoadShaders(const char * vertex_file_path, const char * geometry_file_path, const char * fragment_file_path);
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
GLuint LoadShader(const char * shader_file_path);

#endif
