#define STB_IMAGE_IMPLEMENTATION

#include "Application.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "ShaderFuncs.h"
#include <chrono>
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

GLuint Application::setupTexture(const std::string& filename)
{
    int width, height, channels;
    unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 4);
    if (img == nullptr)
        return -1;
    GLuint texID = -1;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

    stbi_image_free(img);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

void Application::setupShaders()
{
    // 1. Compile Gouraud Program
    std::string vGouraud = loadTextFile("shaders/GouraudShading.vert");
    std::string fGouraud = loadTextFile("shaders/GouraudShading.frag");
    programs["Gouraud"] = InitializeProgram(vGouraud, fGouraud);

    // 2. Compile Phong Program
    std::string vPhong = loadTextFile("shaders/PhongShading.vert");
    std::string fPhong = loadTextFile("shaders/PhongShading.frag");
    programs["Phong"] = InitializeProgram(vPhong, fPhong);

    // Helper lambda to load uniforms dynamically based on program prefix
    auto loadUniforms = [&](const std::string& prefix, GLuint program) {
        uniforms[prefix + "time"] = glGetUniformLocation(program, "time");
        uniforms[prefix + "camera"] = glGetUniformLocation(program, "camera");
        uniforms[prefix + "modelTrans"] = glGetUniformLocation(program, "modelTrans");
        uniforms[prefix + "projection"] = glGetUniformLocation(program, "projection");
        uniforms[prefix + "eyePos"] = glGetUniformLocation(program, "eyePos");
        uniforms[prefix + "useLighting"] = glGetUniformLocation(program, "useLighting");
        uniforms[prefix + "solidColor"] = glGetUniformLocation(program, "solidColor");

        uniforms[prefix + "myLight.ambient"] = glGetUniformLocation(program, "myLight.ambient");
        uniforms[prefix + "myLight.diffuse"] = glGetUniformLocation(program, "myLight.diffuse");
        uniforms[prefix + "myLight.specular"] = glGetUniformLocation(program, "myLight.specular");
        uniforms[prefix + "myLight.position"] = glGetUniformLocation(program, "myLight.position");

        uniforms[prefix + "myMaterial.ambient"] = glGetUniformLocation(program, "myMaterial.ambient");
        uniforms[prefix + "myMaterial.diffuse"] = glGetUniformLocation(program, "myMaterial.diffuse");
        uniforms[prefix + "myMaterial.specular"] = glGetUniformLocation(program, "myMaterial.specular");
        uniforms[prefix + "myMaterial.shininess"] = glGetUniformLocation(program, "myMaterial.shininess");

        // Textures
        uniforms[prefix + "tex0"] = glGetUniformLocation(program, "tex0");
        uniforms[prefix + "tex1"] = glGetUniformLocation(program, "tex1");
        uniforms[prefix + "tex2"] = glGetUniformLocation(program, "tex2");
        };

    loadUniforms("Gouraud_", programs["Gouraud"]);
    loadUniforms("Phong_", programs["Phong"]);
}

void Application::setup()
{
    // Create Plane (100x100 for the requirement)
    plane.createPlane(100);
    plane.cleanMemory();
    geometry["plane"] = plane.vao;

    // Load and compile shaders
    setupShaders();

    // Load textures
    textures["tex0"] = setupTexture("textures/tex0.jpg");
    textures["tex1"] = setupTexture("textures/tex1.jpg");
    textures["tex2"] = setupTexture("textures/tex2.jpg");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup initial light properties
    myLight.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    myLight.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    myLight.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    myLight.position = glm::vec3(0.0f, 2.5f, 0.0f);

    // Setup initial material properties
    myMaterial.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    myMaterial.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    myMaterial.specular = glm::vec4(0.9f, 0.9f, 1.0f, 1.0f);
    myMaterial.shininess = 64.0f;
}

void Application::update(GLFWwindow* window)
{
    double currentTime = glfwGetTime();
    if (lastTime == 0.0)
    {
        lastTime = currentTime;
    }
    float deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
    time = currentTime;

    if (deltaTime > 0.1f) deltaTime = 0.1f;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float ndx = 0.0f;
    float ndy = 0.0f;
    if (width > 0 && height > 0)
    {
        ndx = static_cast<float>((xpos - (width / 2.0)) / (width / 2.0));
        ndy = static_cast<float>((ypos - (height / 2.0)) / (height / 2.0));
    }
    ndx = glm::clamp(ndx, -1.0f, 1.0f);
    ndy = glm::clamp(ndy, -1.0f, 1.0f);

    float targetRoll = -ndx * glm::radians(90.0f);
    float targetPitch = ndy * glm::radians(90.0f);
    float targetYaw = -ndx * glm::radians(70.0f);

    float lerpFactor = 5.0f * deltaTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    currentRoll = glm::mix(currentRoll, targetRoll, lerpFactor);
    currentPitch = glm::mix(currentPitch, targetPitch, lerpFactor);
    currentYaw = glm::mix(currentYaw, targetYaw, lerpFactor);

    modelTrans = glm::mat4(1.0f);
    modelTrans = glm::rotate(modelTrans, currentYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    modelTrans = glm::rotate(modelTrans, currentPitch, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTrans = glm::rotate(modelTrans, currentRoll, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 eye = glm::vec3(0.0f, 3.5f, 3.5f);
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    camera = glm::lookAt(eye, center, up);
    eyePos = eye;

    float aspect = height > 0 ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
    projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
}

void Application::draw()
{
    std::string prefix = usePhong ? "Phong_" : "Gouraud_";
    GLuint program = usePhong ? programs["Phong"] : programs["Gouraud"];

    glUseProgram(program);

    glUniform1f(uniforms[prefix + "time"], static_cast<float>(time));
    glUniformMatrix4fv(uniforms[prefix + "camera"], 1, GL_FALSE, glm::value_ptr(camera));
    glUniformMatrix4fv(uniforms[prefix + "modelTrans"], 1, GL_FALSE, glm::value_ptr(modelTrans));
    glUniformMatrix4fv(uniforms[prefix + "projection"], 1, GL_FALSE, glm::value_ptr(projection));

    glUniform4fv(uniforms[prefix + "myLight.ambient"], 1, glm::value_ptr(myLight.ambient));
    glUniform4fv(uniforms[prefix + "myLight.diffuse"], 1, glm::value_ptr(myLight.diffuse));
    glUniform4fv(uniforms[prefix + "myLight.specular"], 1, glm::value_ptr(myLight.specular));
    glUniform3fv(uniforms[prefix + "myLight.position"], 1, glm::value_ptr(myLight.position));

    glUniform4fv(uniforms[prefix + "myMaterial.ambient"], 1, glm::value_ptr(myMaterial.ambient));
    glUniform4fv(uniforms[prefix + "myMaterial.diffuse"], 1, glm::value_ptr(myMaterial.diffuse));
    glUniform4fv(uniforms[prefix + "myMaterial.specular"], 1, glm::value_ptr(myMaterial.specular));
    glUniform1f(uniforms[prefix + "myMaterial.shininess"], myMaterial.shininess);
    glUniform3fv(uniforms[prefix + "eyePos"], 1, glm::value_ptr(eyePos));

    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures["tex0"]);
    glUniform1i(uniforms[prefix + "tex0"], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures["tex1"]);
    glUniform1i(uniforms[prefix + "tex1"], 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures["tex2"]);
    glUniform1i(uniforms[prefix + "tex2"], 2);

    glBindVertexArray(geometry["plane"]);

    if (drawMode == 0 || drawMode == 2)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUniform1i(uniforms[prefix + "useLighting"], 1);
        glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());
    }

    if (drawMode == 1 || drawMode == 2)
    {
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.5f);
        glUniform1i(uniforms[prefix + "useLighting"], 0);
        glUniform4f(uniforms[prefix + "solidColor"], 0.0f, 0.75f, 1.0f, 1.0f);

        glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());

        glDisable(GL_POLYGON_OFFSET_LINE);
    }
}

Application::~Application()
{
}