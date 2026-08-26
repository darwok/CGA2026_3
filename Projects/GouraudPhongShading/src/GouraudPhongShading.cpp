#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Application.h"
#include <iostream>

Application app;

void myFirstCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
    }

    // Toggle between Shaders when pressing "S"
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        app.usePhong = !app.usePhong;
        std::cout << "Active Shading: " << (app.usePhong ? "Phong" : "Gouraud") << std::endl;
    }

    // Toggle Draw Mode when pressing "D"
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        app.drawMode = (app.drawMode + 1) % 3;
        std::cout << "Draw Mode: ";
        if (app.drawMode == 0) std::cout << "Fill Only\n";
        else if (app.drawMode == 1) std::cout << "Wireframe Only\n";
        else std::cout << "Fill & Wireframe\n";
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(app.WIDTH, app.HEIGHT, "Gouraud vs Phong Shading", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    glfwSetKeyCallback(window, myFirstCallback);

    app.setup();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        app.update(window);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        app.draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}