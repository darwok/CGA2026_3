#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Application.h"

Application app;

void myFirstCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
    }
}

int main(void)
{
    GLFWwindow* window;

<<<<<<< Updated upstream
    if (!glfwInit()) return -1;

    window = glfwCreateWindow(1920, 1024, "SpriteSheet_WIP", NULL, NULL);
=======
    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(1920, 1024, "SpriteSheet", NULL, NULL);
>>>>>>> Stashed changes
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    glfwSetKeyCallback(window, myFirstCallback);

    app.setup();

    while (!glfwWindowShouldClose(window))
    {
        app.update(window);
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        app.draw();
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}