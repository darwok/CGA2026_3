#pragma once
#include <map>
#include <string>
#include "glad/glad.h"
#include "Plane.h"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "StateMachine.h" // FSM

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

    StateMachine stateMachine; // Instancia de nuestra máquina de estados

    Plane plane;

    // Método estático para que GLFW pueda comunicarse con la clase C++
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    void setupShaders();
    GLuint setupTexture(const std::string& filename);

    double lastTime{ 0.0 };
};