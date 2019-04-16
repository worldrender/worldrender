#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "utils.hpp"

using std::cout;
using std::endl;
using std::fstream;
using std::map;
using std::string;
using std::stringstream;

namespace ComS342 {

    class ShaderProgram {
    private:
        // Class DEBUG flag - if set to false then, errors aside, we'll run completely silently
        static const bool DEBUG = false;

        // We'll use an enum to differentiate between shaders and shader programs when querying the info log
        enum class ObjectType { SHADER, PROGRAM };

        // Shader program and individual shader Ids
        GLuint programId;
        GLuint vertexShaderId;
        GLuint vertexShaderId;
        GLuint fragmentShaderId;
        GLuint fragmentShaderId;

        // How many shaders are attached to the shader program
        GLuint shaderCount;

        // Map of attributes and their binding locations
        map<string,int> attributeMap;

        // Map of uniforms and their binding locations
        map<string,int> uniformMap;

        // Has this shader program been initialised?
        bool initialised;

        // ---------- PRIVATE METHODS ----------

        // Method to compile a shader of a given type
        GLuint compileShader(string shaderSource, GLenum shaderType) {
            string shaderTypeString;
            switch (shaderType) {
                case GL_VERTEX_SHADER:
                    shaderTypeString = "GL_VERTEX_SHADER";
                    break;
                case GL_FRAGMENT_SHADER:
                    shaderTypeString = "GL_FRAGMENT_SHADER";
                    break;
                case GL_GEOMETRY_SHADER:
                    cout << "Geometry shaders are unsupported at this time." << endl;
                    getKeypressThenExit();
                    break;
            }

            // Generate a shader id
            // Note: Shader id will be non-zero if successfully created.
            GLuint shaderId = glCreateShader(shaderType);
            if (shaderId == 0) {
                // Get and display the shader log
                cout << "Could not create shader of type " << shaderTypeString << ": " << getInfoLog(ObjectType::SHADER, shaderId) << endl;
                getKeypressThenExit();
            }

            // Get the source string as a pointer to an array of characters
            const char *shaderSourceChars = shaderSource.c_str();

            // Attach the GLSL source code to the shader
            // Params: GLuint shader, GLsizei count, const GLchar **string, const GLint *length
            // Note: The pointer to an array of source chars will be null terminated, so we don't need to specify the length and can instead use NULL.
            glShaderSource(shaderId, 1, &shaderSourceChars, NULL);

            // Compile the shader
            glCompileShader(shaderId);

            // Check the compilation status and report any errors
            GLint shaderStatus;
            glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shaderStatus);
            if (shaderStatus == GL_FALSE) {
                cout << shaderTypeString << " compilation failed: " << getInfoLog(ObjectType::SHADER, shaderId) << endl;
                getKeypressThenExit();
            } else {
                // All good!
                if (DEBUG) {
                    cout << shaderTypeString << " shader compilation successful." << endl;
                }
            }

            // Assuming everything went well, return the shader id
            return shaderId;
        }

        // Method to compile/attach/link/verify the shaders.
        // Note: Rather than returning a boolean as a success/fail status we'll just consider
        //  a failure here to be an unrecoverable error and abort on failure.
        void initialise(string vertexShaderSource, string fragmentShaderSource) {
            // Compile the shaders and return their id values
            vertexShaderId   = compileShader(vertexShaderSource,   GL_VERTEX_SHADER);
            fragmentShaderId = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

            // Attach the compiled shaders to the shader program
            glAttachShader(programId, vertexShaderId);
            glAttachShader(programId, fragmentShaderId);

            // Link the shader program - details are placed in the program info log
            glLinkProgram(programId);

            // Once the shader program has the shaders attached and linked, the shaders are no longer required.
            // If the linking failed, then we're going to abort anyway so we still detach the shaders.
            glDetachShader(programId, vertexShaderId);
            glDetachShader(programId, fragmentShaderId);

            // Check the link status - zero indicates success.
            GLint programLinkSuccess = GL_FALSE;
            glGetProgramiv(programId, GL_LINK_STATUS, &programLinkSuccess);
            if (programLinkSuccess == GL_TRUE) {
                if (DEBUG) {
                    cout << "ShaderProgram link successful." << endl;
                }
            } else {
                // Get the program info log, display it, and return false to indicate failure
                cout << "ShaderProgram link failed: " << getInfoLog(ObjectType::PROGRAM, programId) << endl;
                getKeypressThenExit();
            }

            // Really, we should validate the shader program here - but it never comes back as 0 for me...
            // ...instead, it's a value like 48155942 that changes every run - I have no idea why =/
            /***
             // Validate the program
             //glValidateProgram(programId);
             // Check the validation status - 0 indicates success.
             GLint programValidatationStatus;
             glGetProgramiv(programId, GL_VALIDATE_STATUS, &programValidatationStatus);
             if (!programValidatationStatus) {
                 if (DEBUG) {
                     cout << "Shader program validation successful." << endl;
                 }
             } else {
                // Get the program info log, display it, and return false to indicate failure
                cout << "Shader program validation failed. Reason: " << getInfoLog(ObjectType::PROGRAM, programId) << endl;
                getKeypressThenExit();
             }
             ***/

            // Finally, the shader program is initialised
            initialised = true;
        }

        void initialise(string vertexShaderSource, string tessellationControlShaderSource, string tessellationEvaluationShaderFilename, string fragmentShaderSource) {
            // Compile the shaders and return their id values
            vertexShaderId                  = compileShader(vertexShaderSource,   GL_VERTEX_SHADER);
            tessellationControlShaderId     = compileShader(tessellationControlShaderSource,   GL_TESS_CONTROL_SHADER);
            tessellationEvaluationShaderId  = compileShader(tessellationEvaluationShaderFilename, GL_TESS_EVALUATION_SHADER);
            fragmentShaderId                = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

            // Attach the compiled shaders to the shader program
            glAttachShader(programId, vertexShaderId);
            glAttachShader(programId, tessellationControlShaderId);
            glAttachShader(programId, tessellationEvaluationShaderId);
            glAttachShader(programId, fragmentShaderId);

            // Link the shader program - details are placed in the program info log
            glLinkProgram(programId);

            // Once the shader program has the shaders attached and linked, the shaders are no longer required.
            // If the linking failed, then we're going to abort anyway so we still detach the shaders.
            glDetachShader(programId, vertexShaderId);
            glDetachShader(programId, tessellationControlShaderId);
            glDetachShader(programId, tessellationEvaluationShaderId);
            glDetachShader(programId, fragmentShaderId);

            // Check the link status - zero indicates success.
            GLint programLinkSuccess = GL_FALSE;
            glGetProgramiv(programId, GL_LINK_STATUS, &programLinkSuccess);
            if (programLinkSuccess == GL_TRUE) {
                if (DEBUG) {
                    cout << "ShaderProgram link successful." << endl;
                }
            } else {
                // Get the program info log, display it, and return false to indicate failure
                cout << "ShaderProgram link failed: " << getInfoLog(ObjectType::PROGRAM, programId) << endl;
                getKeypressThenExit();
            }

            // Really, we should validate the shader program here - but it never comes back as 0 for me...
            // ...instead, it's a value like 48155942 that changes every run - I have no idea why =/
            /***
             // Validate the program
             //glValidateProgram(programId);
             // Check the validation status - 0 indicates success.
             GLint programValidatationStatus;
             glGetProgramiv(programId, GL_VALIDATE_STATUS, &programValidatationStatus);
             if (!programValidatationStatus) {
                 if (DEBUG) {
                     cout << "Shader program validation successful." << endl;
                 }
             } else {
                // Get the program info log, display it, and return false to indicate failure
                cout << "Shader program validation failed. Reason: " << getInfoLog(ObjectType::PROGRAM, programId) << endl;
                getKeypressThenExit();
             }
             ***/

            // Finally, the shader program is initialised
            initialised = true;
        }

        // Method to load the shader source code from a file
        string loadShaderFromFile(const string filename) {
            // Create an input filestream and attempt to open the specified file
            std::ifstream file( filename.c_str() );

            // If we couldn't open the file we'll bail out
            if (!file.good()) {
                cout << "Failed to open file: " << filename << endl;
                getKeypressThenExit();
            }

            // Otherwise, create a string stream...
            stringstream stream;

            // ...and dump the contents of the file into it.
            stream << file.rdbuf();

            // Now that we've read the file we can close it
            file.close();

            // Finally, convert the stringstream into a string and return it
            return stream.str();
        }

        string getInfoLog(ObjectType type, int id) {
            GLint infoLogLength;
            if (type == ObjectType::SHADER) {
                glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
            } else {
                // type must be ObjectType::PROGRAM
                glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
            }

            GLchar *infoLog = new GLchar[infoLogLength + 1];
            if (type == ObjectType::SHADER) {
                glGetShaderInfoLog(id, infoLogLength, NULL, infoLog);
            } else {
                // type will be ObjectType::PROGRAM
                glGetProgramInfoLog(id, infoLogLength, NULL, infoLog);
            }

            // Convert the info log to a string
            string infoLogString(infoLog);

            // Delete the char array version of the log
            delete[] infoLog;

            // Finally, return the string version of the info log
            return infoLogString;
        }

    public:
        // Constructor
        ShaderProgram() {
            // We start in a non-initialised state - calling initFromFiles() or initFromStrings() will
            // initialise us.
            initialised = false;

            // Generate a unique Id / handle for the shader program
            // Note: We MUST have a valid rendering context before generating the programId or we'll segfault!
            programId = glCreateProgram();
            glUseProgram(programId);

            // Initially, we have zero shaders attached to the program
            shaderCount = 0;
        }

        // Destructor
        ~ShaderProgram() {
            // Delete the shader program from the graphics card memory to
            // free all the resources it's been using
            glDeleteProgram(programId);
        }


        // Method to initialise a shader program from shaders provided as files
        void initFromFiles(string vertexShaderFilename, string fragmentShaderFilename) {
            string vertexShaderSource   = loadShaderFromFile(vertexShaderFilename);
            string fragmentShaderSource = loadShaderFromFile(fragmentShaderFilename);

            initialise(vertexShaderSource, fragmentShaderSource);
        }

        // Method to initialise a shader program from shaders provided as strings
        void initFromStrings(string vertexShaderSource, string fragmentShaderSource) {
            initialise(vertexShaderSource, fragmentShaderSource);
        }


        void initFromFiles(string vertexShaderFilename, string tessellationControlShaderFilename, string tessellationEvaluationShaderFilename, string fragmentShaderFilename) {
            string vertexShaderSource                       = loadShaderFromFile(vertexShaderFilename);
            string tessellationControlShaderSource          = loadShaderFromFile(tessellationControlShaderFilename);
            string tessellationEvaluationShaderFilename     = loadShaderFromFile(tessellationEvaluationShaderFilename);
            string fragmentShaderSource                     = loadShaderFromFile(fragmentShaderFilename);

            initialise(vertexShaderSource, tessellationControlShaderSource, tessellationEvaluationShaderFilename, fragmentShaderSource);
        }

        // Method to initialise a shader program from shaders provided as strings
        void initFromStrings(string vertexShaderSource, string tessellationControlShaderSource, string tessellationEvaluationShaderSource, string fragmentShaderSource) {
            initialise(vertexShaderSource, tessellationControlShaderSource, tessellationEvaluationShaderSource, fragmentShaderSource);
        }


        // Method to enable the shader program - we'll suggest this for inlining
        inline void use() {
            // Santity check that we're initialised and ready to go...
            if (initialised) {
                glUseProgram(programId);
            } else {
                cout << "Shader program " << programId << " not initialised - bailing!" << endl;
                getKeypressThenExit();
            }
        }

        // Method to disable the shader - we'll also suggest this for inlining
        inline void disable() { glUseProgram(0); }

        // Method to return the bound location of a named attribute, or -1 if the attribute was not found
        GLuint attribute(const string attribute) {
            // You could do this method with the single line:
            //
            //        return attributeMap[attribute/;
            //
            // BUT, if you did, and you asked it for a named attribute which didn't exist
            // like: attributeMap["FakeAttrib!"] then the method would return an invalid
            // value which will likely cause the program to segfault. So we're making sure
            // the attribute asked for exists, and if it doesn't then we alert the user & bail.

            // Create an iterator to look through our attribute map and try to find the named attribute
            static map<string, int>::iterator attributeIter = attributeMap.find(attribute);

            // Not found? Bail.
            if ( attributeIter == attributeMap.end() ) {
                cout << "Could not find attribute in shader program: " << attribute << endl;
                getKeypressThenExit();
            }

            // Otherwise return the attribute location from the attribute map
            return attributeMap[attribute];
        }

        // Method to returns the bound location of a named uniform
        GLuint uniform(const string uniform) {
            // Note: You could do this method with the single line:
            //
            //         return uniformLocList[uniform];
            //
            // But we're not doing that. Explanation in the attribute() method above.

            // Create an iterator to look through our uniform map and try to find the named uniform
            static map<string, int>::iterator uniformIter = uniformMap.find(uniform);

            // Found it? Great - pass it back! Didn't find it? Alert user and halt.
            if (uniformIter == uniformMap.end()) {
                cout << "Could not find uniform in shader program: " << uniform << endl;
                getKeypressThenExit();
            }

            // Otherwise return the attribute location from the uniform map
            return uniformMap[uniform];
        }

        // Method to add an attribute to the shader and return the bound location
        int addAttribute(const string attributeName) {
            // Add the attribute location value for the attributeName key
            attributeMap[attributeName] = glGetAttribLocation( programId, attributeName.c_str() );

            // Check to ensure that the shader contains an attribute with this name
            if (attributeMap[attributeName] == -1) {
                cout << "Could not add attribute: " << attributeName << " - location returned -1!" << endl;
                getKeypressThenExit();
            } else {
                // Valid attribute location? Inform user if we're in debug mode.
                if (DEBUG) {
                    cout << "Attribute " << attributeName << " bound to location: " << attributeMap[attributeName] << endl;
                }
            }

            // Return the attribute location
            return attributeMap[attributeName];
        }

        // Method to add a uniform to the shader and return the bound location
        int addUniform(const string uniformName) {
            // Add the uniform location value for the uniformName key
            uniformMap[uniformName] = glGetUniformLocation( programId, uniformName.c_str() );

            // Check to ensure that the shader contains a uniform with this name
            if (uniformMap[uniformName] == -1) {
                cout << "Could not add uniform: " << uniformName << " - location returned -1!" << endl;
                getKeypressThenExit();
            } else {
                // Valid uniform location? Inform user if we're in debug mode.
                if (DEBUG) {
                    cout << "Uniform " << uniformName << " bound to location: " << uniformMap[uniformName] << endl;
                }
            }

            // Return the uniform location
            return uniformMap[uniformName];
        }

    };

}

#endif /* SHADER_PROGRAM_HPP */
