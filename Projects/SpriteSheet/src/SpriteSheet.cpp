#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Application.h"

Application app;

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(1920, 1024, "SpriteSheet", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    glfwSetWindowUserPointer(window, &app);

    glfwSetKeyCallback(window, Application::keyCallback);

    app.setup();

    while (!glfwWindowShouldClose(window))
    {
        app.update(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        app.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}