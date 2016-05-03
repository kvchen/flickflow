// Standard includes
#include <stdlib.h>

// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "fluidDynamics.h"


#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480



void simulate() {
    // We ping-pong back and forth between two buffers on the GPU
    // u = advect(u);
    // u = diffuse(u);
    // u = addForces(u);
    //
    // p = computePressure(u);
    // u = subtractPressureGradient(u, p);
}


int main(int argc, char** argv) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "FlickFlow", monitor, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        return -1;
    }

    printf("%s", glGetString(GL_VERSION));

    loadShaders();

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
