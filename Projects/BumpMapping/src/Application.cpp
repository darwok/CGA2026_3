#include "Application.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "ShaderFuncs.h"
#include <chrono>
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

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
        };

    // Load uniforms for both Shading techniques
    loadUniforms("Gouraud_", programs["Gouraud"]);
    loadUniforms("Phong_", programs["Phong"]);
}

void Application::setup()
{
    // Create Plane
    plane.createPlane(10);
    plane.cleanMemory();
    geometry["plane"] = plane.vao;

    // Load and compile shaders
    setupShaders();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup initial light properties
    myLight.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    myLight.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    myLight.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    myLight.position = glm::vec3(0.0f, 2.5f, 0.0f);

    // Setup initial material properties (Metallic Blue)
    myMaterial.ambient = glm::vec4(0.1f, 0.12f, 0.22f, 1.0f);
    myMaterial.diffuse = glm::vec4(0.15f, 0.45f, 0.9f, 1.0f);
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

    // Time in seconds for wave animation
    time = currentTime;

    if (deltaTime > 0.1f) deltaTime = 0.1f;

    // Get cursor position and window size
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Calculate normalized offsets [-1.0, 1.0]
    float ndx = 0.0f;
    float ndy = 0.0f;
    if (width > 0 && height > 0)
    {
        ndx = static_cast<float>((xpos - (width / 2.0)) / (width / 2.0));
        ndy = static_cast<float>((ypos - (height / 2.0)) / (height / 2.0));
    }
    ndx = glm::clamp(ndx, -1.0f, 1.0f);
    ndy = glm::clamp(ndy, -1.0f, 1.0f);

    // Map to rotation angles
    float targetRoll = -ndx * glm::radians(45.0f);
    float targetPitch = ndy * glm::radians(30.0f);
    float targetYaw = -ndx * glm::radians(35.0f);

    // Interpolation (Inertia)
    float lerpFactor = 5.0f * deltaTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    currentRoll = glm::mix(currentRoll, targetRoll, lerpFactor);
    currentPitch = glm::mix(currentPitch, targetPitch, lerpFactor);
    currentYaw = glm::mix(currentYaw, targetYaw, lerpFactor);

    // Build matrices
    modelTrans = glm::mat4(1.0f);
    modelTrans = glm::rotate(modelTrans, currentYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    modelTrans = glm::rotate(modelTrans, currentPitch, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTrans = glm::rotate(modelTrans, currentRoll, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 eye = glm::vec3(0.0f, 2.5f, 2.5f);
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    camera = glm::lookAt(eye, center, up);

    // Store eye position to send it to the shaders for ADS Calculation
    eyePos = eye;

    float aspect = height > 0 ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void Application::draw()
{
    // Select prefix and program depending on the boolean flag
    std::string prefix = usePhong ? "Phong_" : "Gouraud_";
    GLuint program = usePhong ? programs["Phong"] : programs["Gouraud"];

    glUseProgram(program);

    // Send Transform Matrices & Time
    glUniform1f(uniforms[prefix + "time"], static_cast<float>(time));
    glUniformMatrix4fv(uniforms[prefix + "camera"], 1, GL_FALSE, glm::value_ptr(camera));
    glUniformMatrix4fv(uniforms[prefix + "modelTrans"], 1, GL_FALSE, glm::value_ptr(modelTrans));
    glUniformMatrix4fv(uniforms[prefix + "projection"], 1, GL_FALSE, glm::value_ptr(projection));

    // Send Light data
    glUniform4fv(uniforms[prefix + "myLight.ambient"], 1, glm::value_ptr(myLight.ambient));
    glUniform4fv(uniforms[prefix + "myLight.diffuse"], 1, glm::value_ptr(myLight.diffuse));
    glUniform4fv(uniforms[prefix + "myLight.specular"], 1, glm::value_ptr(myLight.specular));
    glUniform3fv(uniforms[prefix + "myLight.position"], 1, glm::value_ptr(myLight.position));

    // Send Material data and Eye Position
    glUniform4fv(uniforms[prefix + "myMaterial.ambient"], 1, glm::value_ptr(myMaterial.ambient));
    glUniform4fv(uniforms[prefix + "myMaterial.diffuse"], 1, glm::value_ptr(myMaterial.diffuse));
    glUniform4fv(uniforms[prefix + "myMaterial.specular"], 1, glm::value_ptr(myMaterial.specular));
    glUniform1f(uniforms[prefix + "myMaterial.shininess"], myMaterial.shininess);
    glUniform3fv(uniforms[prefix + "eyePos"], 1, glm::value_ptr(eyePos));

    // Select Plane geometry
    glBindVertexArray(geometry["plane"]);

    // 1. Draw the plane filled with ADS Lighting activated
    if (drawMode == 0 || drawMode == 2)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUniform1i(uniforms[prefix + "useLighting"], 1);
        glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());
    }

    // 2. Draw wireframe grid with Fallback solid color (cyan)
    if (drawMode == 1 || drawMode == 2)
    {
        // Apply polygon offset to prevent Z-fighting when drawing both
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