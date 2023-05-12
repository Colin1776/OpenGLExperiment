#ifndef CAMERA_H
#define CAMERA_H

struct Camera
{
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;
    float yaw;
    float pitch;
    float fov;

    void init();
    void move_forward(float);
    void move_right(float);
    void move_up(float);
};

#endif