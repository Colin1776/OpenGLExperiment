#include <iostream>
#include "include/camera.h"

void Camera::init()
{
    pos = glm::vec3(0.0f, 0.0f, 3.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    fov = 45.0f;
}

void Camera::move_forward(float speed)
{
    pos += glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * speed;
}

void Camera::move_right(float speed)
{
    glm::vec3 temp = glm::normalize(glm::cross(front, up)) * speed;
    pos += glm::vec3(temp.x, 0.0f, temp.z);
}

void Camera::move_up(float speed)
{
    pos += glm::vec3(0.0f, speed, 0.0f);
}