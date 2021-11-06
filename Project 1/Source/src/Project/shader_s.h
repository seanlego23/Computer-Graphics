#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:

    static std::map<std::string, Shader*> shaders;

    bool hasGeometryShader;
    unsigned int ID;
    const char* vertexPath;
    const char* geometryPath;
    const char* fragmentPath;
    std::string name;

public:
    char vtext[4096], gtext[4096], ftext[4096];

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------

    Shader() {}

private:

    void init(const char* vPath, const char* gPath, const char* fPath, std::string n) {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string geometryCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream gShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            vShaderFile.open(vertexPath);
            if (hasGeometryShader) 
                gShaderFile.open(geometryPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, gShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            if (hasGeometryShader) 
                gShaderStream << gShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            if (hasGeometryShader) 
                gShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            if (hasGeometryShader) 
                geometryCode = gShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        int myLen = vertexCode.length();

        memcpy(vtext, vertexCode.c_str(), vertexCode.length());
        vtext[vertexCode.length()] = 0;
        if (hasGeometryShader) {
            memcpy(gtext, geometryCode.c_str(), geometryCode.length());
            gtext[geometryCode.length()] = 0;
        }
        memcpy(ftext, fragmentCode.c_str(), fragmentCode.length());
        ftext[fragmentCode.length()] = 0;

        shaders[name] = this;

        reload();
    }

public:

    Shader(const char* vPath, const char* fPath, std::string n) : 
        hasGeometryShader(false), vertexPath(vPath), geometryPath(nullptr), fragmentPath(fPath), name(n) {
        init(vPath, nullptr, fPath, n);
    }

    Shader(const char* vPath, const char* gPath, const char* fPath, std::string n) : 
        hasGeometryShader(true), vertexPath(vPath), geometryPath(gPath), fragmentPath(fPath), name(n) {
        init(vPath, gPath, fPath, n);
    }
    void reload() {
        reload(vtext, gtext, ftext);
    }
    void reload(const char* vShaderCode, const char* gShaderCode, const char* fShaderCode) {
        
        // compile shaders
        unsigned int vertex, fragment, geometry;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // geometry Shader
        if (hasGeometryShader) {
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        if (hasGeometryShader) glAttachShader(ID, geometry);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        if (hasGeometryShader) glDeleteShader(geometry);
        glDeleteShader(fragment);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setDouble(const std::string& name, double value) const {
        glUniform1d(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, float x, float y, float z) {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    void saveShaders() {
        std::ofstream myfile;

        myfile.open(vertexPath);
        myfile << vtext;
        myfile.close();

        //myfile.open(geometryPath);
        //myfile << gtext;
        //myfile.close();

        myfile.open(fragmentPath);
        myfile << ftext;
        myfile.close();
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif