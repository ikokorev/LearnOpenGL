#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <experimental/filesystem>


class ShaderProgram
{
public:
    // Constructor generates shader program on the fly
    ShaderProgram(const char* VertexShaderSourceFilePath, const char* FragmentShaderSourceFilePath)
    {
        // 1. retrieve the vertex & fragment shaders source code from files
        std::string vertexShaderSourceCode;
        std::string fragmentShaderSourceCode;
        std::ifstream vertexShaderSourceFile;
        std::ifstream fragmentShaderSourceFile;

        // ensure ifstream objects can throw exceptions:
        vertexShaderSourceFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragmentShaderSourceFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        try 
        {
            vertexShaderSourceFile.open(VertexShaderSourceFilePath);
            fragmentShaderSourceFile.open(FragmentShaderSourceFilePath);

            // read file's buffer contents into streams
            std::stringstream vertexShaderStream, fragmentShaderStream;
            vertexShaderStream << vertexShaderSourceFile.rdbuf();
            fragmentShaderStream << fragmentShaderSourceFile.rdbuf();
            
            // close file handlers
            vertexShaderSourceFile.close();
            fragmentShaderSourceFile.close();
            
            // convert streams into strings
            vertexShaderSourceCode = vertexShaderStream.str();
            fragmentShaderSourceCode = fragmentShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "Failed to properly read shader source code file: " << e.what() << std::endl;
        }
        
        // 2. compile shaders
        unsigned int vertexShader, fragmentShader;
        const char* vertexShaderSourceCodePtr = vertexShaderSourceCode.c_str();
        const char * fragmentShaderSourceCodePtr = fragmentShaderSourceCode.c_str();

        // vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSourceCodePtr, nullptr);
        glCompileShader(vertexShader);
        CheckEntityCompilationErrors(vertexShader, EEntityType::VertexShader);

        // fragment Shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSourceCodePtr, nullptr);
        glCompileShader(fragmentShader);
        CheckEntityCompilationErrors(fragmentShader, EEntityType::FragmentShader);

        // shader Program
        ProgramID = glCreateProgram();
        glAttachShader(ProgramID, vertexShader);
        glAttachShader(ProgramID, fragmentShader);
        glLinkProgram(ProgramID);
        CheckEntityCompilationErrors(ProgramID, EEntityType::ShaderProgram);

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // Activate shader program
    void UseProgram() 
    { 
        glUseProgram(ProgramID); 
    }

// utility uniform functions

    void SetProgramUniform(const std::string& UniformName, bool NewValue) const
    {         
        glUniform1i(glGetUniformLocation(ProgramID, UniformName.c_str()), (int)NewValue); 
    }

    void SetProgramUniform(const std::string& UniformName, int NewValue) const
    { 
        glUniform1i(glGetUniformLocation(ProgramID, UniformName.c_str()), NewValue); 
    }

    void SetProgramUniform(const std::string& UniformName, float NewValue) const
    { 
        glUniform1f(glGetUniformLocation(ProgramID, UniformName.c_str()), NewValue); 
    }

private:

    // utility function for checking shader/shader program compilation/linking errors (respectively)
    enum class EEntityType : unsigned int;
    void CheckEntityCompilationErrors(unsigned int EntityID, EEntityType EntityType) const
    {
        int compiledSuccessfully;
        char compilationInfoLog[1024];

        switch (EntityType)
        {
            case EEntityType::VertexShader: case EEntityType::FragmentShader:
            {
                glGetShaderiv(EntityID, GL_COMPILE_STATUS, &compiledSuccessfully);
                if (!compiledSuccessfully)
                {
                    glGetShaderInfoLog(EntityID, 1024, nullptr, compilationInfoLog);
                    std::cout << EntityTypeToString(EntityType) << " failed to compile successfully.\n" <<
                        compilationInfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
                break;
            }
            
            case EEntityType::ShaderProgram:
            {
                glGetProgramiv(EntityID, GL_LINK_STATUS, &compiledSuccessfully);
                if (!compiledSuccessfully)
                {
                    glGetProgramInfoLog(EntityID, 1024, nullptr, compilationInfoLog);
                    std::cout << EntityTypeToString(EntityType) << " failed to link successfully.\n" <<
                        compilationInfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
                break;
            }
        }
    }

    std::string EntityTypeToString(EEntityType EntityType) const
    {
        switch (EntityType)
        {
            case EEntityType::VertexShader:
            {
                return "VertexShader";
            }
            case EEntityType::FragmentShader:
            {
                return "FragmentShader";
            }
            case EEntityType::ShaderProgram:
            {
                return "ShaderProgram";
            }
        }
        
        return {};
    }

private:
    unsigned int ProgramID;

    enum class EEntityType: unsigned int
    {
        VertexShader,
        FragmentShader,
        ShaderProgram
    };
};
#endif