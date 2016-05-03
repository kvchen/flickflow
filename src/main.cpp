// Standard includes
#include <stdlib.h>
#include <cstdio>
#include "Leap.h"

// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "fluidDynamics.h"
#include "shaderUtils.h"


#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

// Simulation parameters
#define NUM_JACOBI_ITERATIONS 50


int viewportWidth, viewportHeight;
Slab velocity, density, pressure, temperature, divergence, vorticity;


using namespace Leap;


void render() {

}


void resize(GLFWwindow* window, int width, int height) {
    viewportWidth = width;
    viewportHeight = height;
}


int main(int argc, char** argv) {
    // GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    GLFWwindow* window;

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // Make sure we're running the highest version of OpenGL possible
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "FlickFlow", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwGetWindowSize(window, &viewportWidth, &viewportHeight);

    glewExperimental = GL_TRUE;  // This is necessary because GLEW is shit
    if (glewInit() != GLEW_OK) {
        return -1;
    }

    std::printf("Using GL Version: %s\n", glGetString(GL_VERSION));

    // Set up the Leap motion
    Controller controller;

    // Set up all shaders
    initializeShaders();
    createQuad();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint visualize = loadShaders("../shaders/all.vert", "../shaders/visualize.frag");

    velocity = createSlab(viewportWidth, viewportHeight, 2);
    density = createSlab(viewportWidth, viewportHeight, 1);
    pressure = createSlab(viewportWidth, viewportHeight, 1);
    temperature = createSlab(viewportWidth, viewportHeight, 1);
    divergence = createSlab(viewportWidth, viewportHeight, 3);
    vorticity = createSlab(viewportWidth, viewportHeight, 2);

    // splat(density.read, viewportWidth / 2, viewportHeight / 2, 20.0, 40.0f);

    while (!glfwWindowShouldClose(window)) {
        // Run a step of the simulation
        simulate(velocity, density, pressure, temperature, divergence, vorticity, viewportWidth, viewportHeight);

        // Render the textures to the window framebuffer
        glUseProgram(visualize);
        GLint fillColorLoc = glGetUniformLocation(visualize, "fillColor");
        GLint scaleLoc = glGetUniformLocation(visualize, "scale");

        glEnable(GL_BLEND);

        // Set render target to the backbuffer:
        glViewport(0, 0, viewportWidth, viewportHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw ink:
        glBindTexture(GL_TEXTURE_2D, vorticity.read.textureHandle);
        glUniform3f(fillColorLoc, 1, 1, 1);
        glUniform2f(scaleLoc, 1.0f / viewportWidth, 1.0f / viewportHeight);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Disable blending:
        glDisable(GL_BLEND);

        glfwSwapBuffers(window);
        glfwPollEvents();

        Frame frame = controller.frame();
        for (Hand hand : frame.hands()) {
            Vector handCenter = hand.palmPosition();
            Vector handSpeed = hand.palmVelocity();

            int xpos = (int) ((viewportWidth / 2.0) + (handCenter.x * 4));
            int ypos = (int) (handCenter.y * 4 - 300);

            splat(velocity.read, xpos, ypos, 40.0, handSpeed.x, handSpeed.y, 0.0);
        }
    }

    // controller.removeListener(listener);

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
