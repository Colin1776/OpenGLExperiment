#ifndef SHADER_H
#define SHADER_H

struct Shader
{
    u32 ID;

    void build(const char*, const char*);
    void use() const;
    void setInt(const char*, s32) const;
    void setFloat(const char*, float) const;
    void setMat4(const char*, glm::mat4) const;
    void setVec3(const char*, glm::vec3) const;
};

#endif