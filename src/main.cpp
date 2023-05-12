#include <cstdio>

#include "glad.c"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "include/types.h"
#include "util.cpp"
#include "shader.cpp"
#include "camera.cpp"

u32 SCREEN_WIDTH = 800;
u32 SCREEN_HEIGHT = 600;

bool firstMouse = true;
float delta_time = 0.0f;

float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    auto cam = (struct Camera*)glfwGetWindowUserPointer(window);

    float cam_speed = 2.5f * delta_time;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam->move_forward(cam_speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam->move_forward(-cam_speed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam->move_right(-cam_speed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam->move_right(cam_speed);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cam->move_up(cam_speed);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cam->move_up(-cam_speed);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    auto cam = (struct Camera*)glfwGetWindowUserPointer(window);

    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cam->yaw += xoffset;
    cam->pitch += yoffset;

    if (cam->pitch > 89.0f)
    {
        cam->pitch = 89.0f;
    }
    if (cam->pitch < -89.0f)
    {
        cam->pitch = -89.0f;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(cam->yaw)) *cos(glm::radians(cam->pitch));
    front.y = sin(glm::radians(cam->pitch));
    front.z = sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    cam->front = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto cam = (struct Camera*)glfwGetWindowUserPointer(window);

    cam->fov -= (float)yoffset;
    if (cam->fov < 1.0f)
    {
        cam->fov = 1.0f;
    }
    if (cam->fov > 90.0f)
    {
        cam->fov = 90.0f;
    }
}

u32 load_texture(const char* path)
{
    u32 ID;
    glGenTextures(1, &ID);

    s32 width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data)
    {
        GLenum format;
        switch (nrComponents)
        {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                format = GL_RGB;
                break;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Texture failed to load at path: %s\n", path);
    }

    stbi_image_free(data);
    return ID;
}

int main()
{
    /* opengl setup */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* el camaro */
    struct Camera cam{};
    cam.init();

    glfwSetWindowUserPointer(window, &cam);

    /* shader and vertex stuff lol */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    struct Shader basic{};
    basic.build("../res/shaders/basic.vert", "../res/shaders/basic.frag");

    struct Shader light{};
    light.build("../res/shaders/light.vert", "../res/shaders/light.frag");

    struct Shader outline{};
    outline.build("../res/shaders/basic.vert", "../res/shaders/outline.frag");

    float vertices[] =
    {
        // positions                    // normals                      // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glm::vec3 cube_positions[] =
    {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 light_positions[] =
    {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    u32 VBO, cubeVAO;

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    u32 lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* el texturo */
    u32 diffuse_map = load_texture("../res/textures/container.png");
    u32 specular_map = load_texture("../res/textures/container_specular.png");

    basic.use();
    basic.setInt("material.diffuse", 0);
    basic.setInt("material.specular", 1);

    float last_frame = 0.0f;

    // render loop
    while(!glfwWindowShouldClose(window))
    {
        float current_frame = (float)glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // take input
        process_input(window);

        // render functions called here
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // stuff
        basic.use();
        basic.setVec3("viewPos", cam.pos);
        basic.setFloat("material.shininess", 64.0f);

        // directional light
        basic.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        basic.setVec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        basic.setVec3("dirLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        basic.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        // point light 1
        basic.setVec3("pointLights[0].position", light_positions[0]);
        basic.setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        basic.setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        basic.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        basic.setFloat("pointLights[0].constant", 1.0f);
        basic.setFloat("pointLights[0].linear", 0.09f);
        basic.setFloat("pointLights[0].quadratic", 0.032f);

        // point light 2
        basic.setVec3("pointLights[1].position", light_positions[1]);
        basic.setVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        basic.setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        basic.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        basic.setFloat("pointLights[1].constant", 1.0f);
        basic.setFloat("pointLights[1].linear", 0.09f);
        basic.setFloat("pointLights[1].quadratic", 0.032f);

        // point light 3
        basic.setVec3("pointLights[2].position", light_positions[2]);
        basic.setVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        basic.setVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        basic.setVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        basic.setFloat("pointLights[2].constant", 1.0f);
        basic.setFloat("pointLights[2].linear", 0.09f);
        basic.setFloat("pointLights[2].quadratic", 0.032f);

        // point light 4
        basic.setVec3("pointLights[3].position", light_positions[3]);
        basic.setVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        basic.setVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        basic.setVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        basic.setFloat("pointLights[3].constant", 1.0f);
        basic.setFloat("pointLights[3].linear", 0.09f);
        basic.setFloat("pointLights[3].quadratic", 0.032f);

        // spotlight
        basic.setVec3("spotLight.position", cam.pos);
        basic.setVec3("spotLight.direction", cam.front);
        basic.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        basic.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        basic.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        basic.setFloat("spotLight.constant", 1.0f);
        basic.setFloat("spotLight.linear", 0.09f);
        basic.setFloat("spotLight.quadratic", 0.032f);
        basic.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        basic.setFloat("spotLight.outercutOff", glm::cos(glm::radians(15.0f)));

        glm::mat4 projection = glm::perspective(glm::radians(cam.fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        basic.setMat4("projection", projection);

        glm::mat4 view = glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);
        basic.setMat4("view", view);

        /*glm::mat4 model = glm::mat4(1.0f);
        basic.setMat4("model", model);*/

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);

        // enable le pencil
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        glBindVertexArray(cubeVAO);
        for (u8 i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            basic.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // render cubes again but this time with pencil effect

        outline.use();
        outline.setMat4("projection", projection);
        outline.setMat4("view", view);

        float scale = 1.05f;

        glBindVertexArray(cubeVAO);
        for (u8 i = 0; i < 10; i++)
        {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(scale, scale, scale));
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            outline.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }

        // disable pencil - this causes it to behave weirdly for some reason, maybe if i draw the lights first and draw the outlining last on top of everything it would be fixed? idk
        //glStencilMask(0x00);

        light.use();
        light.setMat4("projection", projection);
        light.setMat4("view", view);

        glBindVertexArray(lightVAO);
        for (u8 i = 0; i < 4; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, light_positions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            light.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // poll from events and swap the buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
