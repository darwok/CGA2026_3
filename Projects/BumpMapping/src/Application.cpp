#define STB_IMAGE_IMPLEMENTATION
#include "Application.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "ShaderFuncs.h"
#include <chrono>
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

// Function to load and configure textures
GLuint Application::setupTexture(const std::string& filename)
{
    int width, height, channels;
    unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 4);
    if (img == nullptr)
    {
        std::cout << "Error loading texture: " << filename << std::endl;
        return -1;
    }
    GLuint texID = -1;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

    stbi_image_free(img);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

        // Texture Uniforms
        uniforms[prefix + "tex0"] = glGetUniformLocation(program, "tex0");
        uniforms[prefix + "tex1"] = glGetUniformLocation(program, "tex1");
        };

    // Load uniforms for both Shading techniques
    loadUniforms("Gouraud_", programs["Gouraud"]);
    loadUniforms("Phong_", programs["Phong"]);
}

void Application::setup()
{
    // Create Plane (1x1 requested)
    plane.createPlane(1);
    plane.cleanMemory();
    geometry["plane"] = plane.vao;

    // Load and compile shaders
    setupShaders();

    // Load Diffuse (tex0) and Normal Map (tex1)
    textures["tex0"] = setupTexture("textures/tex0.jpg");
    textures["tex1"] = setupTexture("textures/tex1.jpg");

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
    float targetRoll = -ndx * glm::radians(90.0f);
    float targetPitch = ndy * glm::radians(90.0f);
    float targetYaw = -ndx * glm::radians(90.0f);

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

    // Send Transform Matrices
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

    // Bind Textures to units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures["tex0"]);
    glUniform1i(uniforms[prefix + "tex0"], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures["tex1"]);
    glUniform1i(uniforms[prefix + "tex1"], 1);

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