#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class ShaderProgram
{
private:
	// static DEBUG flag - if set to false then, errors aside, we'll run completely silent
	static const bool DEBUG = true;

	// We'll use an enum to differentiate between shaders and shader programs when querying the info log
	enum class ObjectType
	{
		SHADER, PROGRAM
	};

	// Shader program and individual shader Ids
	GLuint programId;
	GLuint vertexShaderId;
	GLuint fragmentShaderId;

	// How many shaders are attached to the shader program
	GLuint shaderCount;

	// Map of attributes and their binding locations
	std::map<std::string, int> attributeMap;

	// Map of uniforms and their binding locations
	std::map<std::string, int> uniformMap;

	// Has this shader program been initialised?
	bool initialised;

	// ---------- PRIVATE METHODS ----------

	// Private method to compile a shader of a given type
	GLuint compileShader(std::string shaderSource, GLenum shaderType)
	{
		std::string shaderTypeString;
		switch (shaderType)
		{
			case GL_VERTEX_SHADER:
				shaderTypeString = "GL_VERTEX_SHADER";
				break;
			case GL_FRAGMENT_SHADER:
				shaderTypeString = "GL_FRAGMENT_SHADER";
				break;
			case GL_GEOMETRY_SHADER:
				throw std::runtime_error("Geometry shaders are unsupported at this time.");
				break;
			default:
				throw std::runtime_error("Bad shader type enum in compileShader.");
				break;
		}
