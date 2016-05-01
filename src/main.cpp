// Standard includes
#include <stdlib.h>
#include <vector>

// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>


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

    window = glfwCreateWindow(640, 480, "FlickFlow", monitor, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
