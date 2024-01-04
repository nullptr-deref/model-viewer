#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "wavefront/wavefront.h"

#include <stdio.h>

const char *NAME = "Model Preview";
const size_t WIDTH = 640;
const size_t HEIGHT = 480;

const char *model_filepath = "C:/dev/wavefront-parser/cube.obj";

int main(void) {
    GLFWwindow* window;

    if (!glfwInit()) {
        puts("Could not initialize GLFW.");
        return -1;
    }

    window = glfwCreateWindow(WIDTH, HEIGHT, NAME, NULL, NULL);
    if (!window) {
        puts("Could not initialize window.");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        puts("Could not load glew.");
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
