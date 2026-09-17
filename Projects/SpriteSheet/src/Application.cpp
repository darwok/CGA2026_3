#define STB_IMAGE_IMPLEMENTATION
#include "Application.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "ShaderFuncs.h"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

GLuint Application::setupTexture(const std::string& filename)
{
    int width, height, channels;
    unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 4);
<<<<<<< Updated upstream
    if (img == nullptr) return -1;

    GLuint texID = -1;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    stbi_image_free(img);

=======
    if (img == nullptr)
        return -1;
    GLuint texID = -1;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

    stbi_image_free(img);

    // GL_NEAREST mantiene los pixeles nítidos, ideal para Pixel Art / SpriteSheets
>>>>>>> Stashed changes
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

void Application::setupShaders()
{
    std::string vertexShader = loadTextFile("shaders/passthru.vert");
    std::string fragmentShader = loadTextFile("shaders/passthru.frag");
    programs["passthru"] = InitializeProgram(vertexShader, fragmentShader);

    uniforms["camera"] = glGetUniformLocation(programs["passthru"], "camera");
    uniforms["modelTrans"] = glGetUniformLocation(programs["passthru"], "modelTrans");
    uniforms["projection"] = glGetUniformLocation(programs["passthru"], "projection");

<<<<<<< Updated upstream
=======
    // Spritesheet uniform locations
>>>>>>> Stashed changes
    uniforms["tex0"] = glGetUniformLocation(programs["passthru"], "tex0");
    uniforms["col"] = glGetUniformLocation(programs["passthru"], "col");
    uniforms["row"] = glGetUniformLocation(programs["passthru"], "row");
    uniforms["totalCols"] = glGetUniformLocation(programs["passthru"], "totalCols");
    uniforms["totalRows"] = glGetUniformLocation(programs["passthru"], "totalRows");
<<<<<<< Updated upstream
=======
    uniforms["flipX"] = glGetUniformLocation(programs["passthru"], "flipX");
>>>>>>> Stashed changes
}

void Application::setup()
{
    setupShaders();
    stateMachine.setup();

    plane.createPlane(100);
    plane.cleanMemory();
    geometry["plane"] = plane.vao;

<<<<<<< Updated upstream
=======
    // Cargar el spritesheet
>>>>>>> Stashed changes
    textures["tex0"] = setupTexture("textures/tex0.jpg");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::update(GLFWwindow* window)
{
    double currentTime = glfwGetTime();
<<<<<<< Updated upstream
    if (lastTime == 0.0) lastTime = currentTime;
    float deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;

    if (deltaTime > 0.1f) deltaTime = 0.1f;

=======
    if (lastTime == 0.0)
    {
        lastTime = currentTime;
    }
    float deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;

    // Tope de seguridad para evitar saltos de tiempo masivos
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    // 1. Obtener entradas en tiempo real
>>>>>>> Stashed changes
    bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    bool c = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
    bool x = glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS;
    bool f = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    bool click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool v = glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS;

    bool attack = f || click;

<<<<<<< Updated upstream
    stateMachine.updateStateFromInput(w, s, a, d, attack, space, c, x, v);
    stateMachine.update(deltaTime);

    // Plano levantado para verse frente a la cámara (solución al problema previo)
    modelTrans = glm::mat4(1.0f);
    modelTrans = glm::rotate(modelTrans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

=======
    // 2. Actualizar estado y calcular el cuadro (frame) con el delta de tiempo
    stateMachine.updateStateFromInput(w, s, a, d, attack, space, c, x, v);
    stateMachine.update(deltaTime);

    // 3. Matriz de Modelo estática (levantamos el plano 90 grados sobre el eje X)
    modelTrans = glm::mat4(1.0f);
    modelTrans = glm::rotate(modelTrans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // 4. Cámara estática centrada viendo al plano directamente
>>>>>>> Stashed changes
    glm::vec3 eye = glm::vec3(0.0f, 0.0f, 2.5f);
    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera = glm::lookAt(eye, center, up);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float aspect = height > 0 ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void Application::draw()
{
    glUseProgram(programs["passthru"]);

<<<<<<< Updated upstream
=======
    // Matrices
>>>>>>> Stashed changes
    glUniformMatrix4fv(uniforms["camera"], 1, GL_FALSE, glm::value_ptr(camera));
    glUniformMatrix4fv(uniforms["modelTrans"], 1, GL_FALSE, glm::value_ptr(modelTrans));
    glUniformMatrix4fv(uniforms["projection"], 1, GL_FALSE, glm::value_ptr(projection));

<<<<<<< Updated upstream
=======
    // Datos Genéricos de la animación (Máquina de estados -> Shader)
>>>>>>> Stashed changes
    glUniform1f(uniforms["col"], static_cast<float>(stateMachine.getCurrentCol()));
    glUniform1f(uniforms["row"], static_cast<float>(stateMachine.getCurrentRow()));
    glUniform1f(uniforms["totalCols"], stateMachine.totalCols);
    glUniform1f(uniforms["totalRows"], stateMachine.totalRows);
<<<<<<< Updated upstream
=======
    glUniform1i(uniforms["flipX"], stateMachine.isFlipped() ? 1 : 0);
>>>>>>> Stashed changes

    glBindVertexArray(geometry["plane"]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

<<<<<<< Updated upstream
=======
    // Textura
>>>>>>> Stashed changes
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures["tex0"]);
    glUniform1i(uniforms["tex0"], 0);

    glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());
}

Application::~Application()
{
}