#include "include/shader.h"

void compile_shader(const char* shader_source, u32 shader)
{
    glShaderSource(shader, 1, (const GLchar* const*)&shader_source, nullptr);
    glCompileShader(shader);

    s32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        printf("Log: %s", info_log);
    }
}

void Shader::build(const char* vertex_path, const char* fragment_path)
{
    ID = glCreateProgram();

    const char* vertex_source = load_file(vertex_path);
    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(vertex_source, vertex_shader);

    const char* fragment_source = load_file(fragment_path);
    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(fragment_source, fragment_shader);

    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);
    glLinkProgram(ID);

    s32 success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        char info_log[512];
        glGetProgramInfoLog(ID, 512, nullptr, info_log);
        printf("Log: %s", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setInt(const char* name, s32 val) const
{
    glUniform1i(glGetUniformLocation(ID, name), val);
}

void Shader::setFloat(const char* name, float val) const
{
    glUniform1f(glGetUniformLocation(ID, name), val);
}

void Shader::setMat4(const char* name, glm::mat4 val) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setVec3(const char* name, glm::vec3 val) const
{
    glUniform3fv(glGetUniformLocation(ID, name), 1, glm::value_ptr(val));
}