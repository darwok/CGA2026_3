#pragma once
#include <map>
#include <string>
#include "glad/glad.h"
#include "Plane.h"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "StateMachine.h" // Se añade la clase FSM

class Application
{
public:
    void setup();
    void update(struct GLFWwindow* window);
    void draw();
    ~Application();

    std::map<std::string, GLuint> geometry;
    std::map<std::string, GLuint> programs;
    std::map<std::string, GLuint> uniforms;
    std::map<std::string, GLuint> textures;

    glm::mat4 camera;
    glm::mat4 modelTrans;
    glm::mat4 projection;

    StateMachine stateMachine;

    Plane plane;

private:
    void setupShaders();
    GLuint setupTexture(const std::string& filename);

    double lastTime{ 0.0 };
};