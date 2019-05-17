#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "include/LoadShaders.hpp"

using namespace std;

void fileNotFound(const char * filename)
{
  std::cout << "Impossible to open " << filename << ". Are you in the right directory?" << std::endl;
  exit(-1);
}

void shaderInfo(GLuint ProgramID)
{
	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cout << &ProgramErrorMessage[0] << std::endl;
	}
}

GLuint attachShader(GLuint shaderId)
{
	// Link the program
	GLuint programID = glCreateProgram();
	glAttachShader(programID, shaderId);
	return programID;
}

void dettachShader(GLuint programID, GLuint shaderId)
{
	//Dettaching and deleting unuseful references
	glDetachShader(programID, shaderId);
	glDeleteShader(shaderId);
}

GLuint LoadShaders(const char * vertex_file_path, const char * control_file_path,
                   const char * evaluation_file_path, const char * fragment_file_path){
//GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint ControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint EvaluationShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
//    GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
   	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		fileNotFound(vertex_file_path);
	}


	std::string TessControlShaderCode;
	std::ifstream TessControlStream(control_file_path, std::ios::in);
	if(TessControlStream.is_open()){
		std::string Line = "";
		while(getline(TessControlStream, Line))
			TessControlShaderCode += "\n" + Line;
		TessControlStream.close();
	}else{
		fileNotFound(control_file_path);
	}


    std::string TessEvaluationShaderCode;
	std::ifstream TessEvaluationStream(evaluation_file_path, std::ios::in);
	if(TessEvaluationStream.is_open()){
		std::string Line = "";
		while(getline(TessEvaluationStream, Line))
			TessEvaluationShaderCode += "\n" + Line;
		TessEvaluationStream.close();
	}else{
		fileNotFound(evaluation_file_path);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}else{
    fileNotFound(fragment_file_path);
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	std::cout << "Compiling shader: " << vertex_file_path << std::endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		std::cout << &VertexShaderErrorMessage[0] << std::endl;
	}



// Compile Tesselation Control Shader
	std::cout << "Compiling shader: " << control_file_path << std::endl;
	char const * TessControlSourcePointer = TessControlShaderCode.c_str();
	glShaderSource(ControlShaderID, 1, &TessControlSourcePointer , NULL);
	glCompileShader(ControlShaderID);

	// Check Vertex Shader
	glGetShaderiv(ControlShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(ControlShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> TessControlErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(ControlShaderID, InfoLogLength, NULL, &TessControlErrorMessage[0]);
		std::cout << &TessControlErrorMessage[0] << std::endl;
	}

// Compile Tesselation Evaluation Shader
	std::cout << "Compiling shader: " << evaluation_file_path << std::endl;
	char const * EvaluationSourcePointer = TessEvaluationShaderCode.c_str();
	glShaderSource(EvaluationShaderID, 1, &EvaluationSourcePointer , NULL);
	glCompileShader(EvaluationShaderID);

	// Check Vertex Shader
	glGetShaderiv(EvaluationShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(EvaluationShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> EvaluationShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(EvaluationShaderID, InfoLogLength, NULL, &EvaluationShaderErrorMessage[0]);
		std::cout << &EvaluationShaderErrorMessage[0] << std::endl;
	}

// Compile Fragment Shader
	std::cout << "Compiling shader: " << fragment_file_path << std::endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		std::cout << &FragmentShaderErrorMessage[0] << std::endl;
	}


	// Link the program
	std::cout << "Linking programs" << std::endl;
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, ControlShaderID);
	glAttachShader(ProgramID, EvaluationShaderID);
    //glAttachShader(ProgramID, GeometryShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cout << &ProgramErrorMessage[0] << std::endl;
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, ControlShaderID);
	glDetachShader(ProgramID, EvaluationShaderID);
    //glDetachShader(ProgramID, GeometryShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
    glDeleteShader(ControlShaderID);
	glDeleteShader(EvaluationShaderID);
    //glDeleteShader(GeometryShaderID);
	glDeleteShader(FragmentShaderID);


	return ProgramID;
}

GLuint LoadShaders(const char * vertex_file_path, const char * geometry_file_path,
                   const char * fragment_file_path){
//GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
//	GLuint ControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
//	GLuint EvaluationShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
    GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
   	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		fileNotFound(vertex_file_path);
	}

//    // Read the Geometry Shader code from the file
	std::string GeometryShaderCode;
	std::ifstream GeometryShaderStream(geometry_file_path, std::ios::in);
	if(GeometryShaderStream.is_open()){
		std::string Line = "";
		while(getline(GeometryShaderStream, Line))
			GeometryShaderCode += "\n" + Line;
		GeometryShaderStream.close();
	}else{
		fileNotFound(geometry_file_path);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}else{
		fileNotFound(fragment_file_path);
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	std::cout << "Compiling shader: " << vertex_file_path << std::endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		std::cout << &VertexShaderErrorMessage[0] << std::endl;
	}
//
// Compile Geometry Shader
	std::cout << "Compiling shader: " << geometry_file_path << std::endl;
	char const * GeometrySourcePointer = GeometryShaderCode.c_str();
	glShaderSource(GeometryShaderID, 1, &GeometrySourcePointer , NULL);
	glCompileShader(GeometryShaderID);

	// Check Geometry Shader
	glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> GeometryShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
		std::cout << &GeometryShaderErrorMessage[0] << std::endl;
	}



// Compile Fragment Shader
	std::cout << "Compiling shader: " << fragment_file_path << std::endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		std::cout << &FragmentShaderErrorMessage[0] << std::endl;
	}


	// Link the program
	std::cout << "Linking programs" << std::endl;
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, GeometryShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cout << &ProgramErrorMessage[0] << std::endl;
	}


	glDetachShader(ProgramID, VertexShaderID);
	//glDetachShader(ProgramID, ControlShaderID);
	//glDetachShader(ProgramID, EvaluationShaderID);
    glDetachShader(ProgramID, GeometryShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
    //glDeleteShader(ControlShaderID);
	//glDeleteShader(EvaluationShaderID);
    glDeleteShader(GeometryShaderID);
	glDeleteShader(FragmentShaderID);


	return ProgramID;
}

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
//GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
	  fileNotFound(vertex_file_path);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}else{
		fileNotFound(fragment_file_path);
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	std::cout << "Compiling shader: " << vertex_file_path << std::endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		std::cout << &VertexShaderErrorMessage[0] << std::endl;
	}


// Compile Fragment Shader
	std::cout << "Compiling shader: " << fragment_file_path << std::endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		std::cout << &FragmentShaderErrorMessage[0] << std::endl;
	}


	// Link the program
	std::cout << "Linking programs" << std::endl;
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cout << &ProgramErrorMessage[0] << std::endl;
	}


	glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);


	return ProgramID;
}

GLuint LoadShader(const char * shader_file_path)
{
  GLuint shaderId = glCreateShader(GL_VERTEX_SHADER);
  std::string shaderCode;

	std::ifstream shaderStream(shader_file_path, std::ios::in);
	if(shaderStream.is_open()){
		std::string Line = "";
		while(getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}else{
	  fileNotFound(shader_file_path);

	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	std::cout << "Compiling shader: " << shader_file_path << std::endl;
	char const * shaderSourcePoint = shaderCode.c_str();
	glShaderSource(shaderId, 1, &shaderSourcePoint , NULL);
	glCompileShader(shaderId);

	// Check Vertex Shader
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> shaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &shaderErrorMessage[0]);
		std::cout << &shaderErrorMessage[0] << std::endl;
	}

	return shaderId;
}



