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
    GLFWwindow* window;

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // Make sure we're running the highest version of OpenGL possible
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    // const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    //
    // glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    // glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    // glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    // glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

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
    density = createSlab(viewportWidth, viewportHeight, 3);
    pressure = createSlab(viewportWidth, viewportHeight, 1);
    temperature = createSlab(viewportWidth, viewportHeight, 1);
    divergence = createSlab(viewportWidth, viewportHeight, 3);
    vorticity = createSlab(viewportWidth, viewportHeight, 2);

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
        glBindTexture(GL_TEXTURE_2D, density.read.textureHandle);
        glUniform3f(fillColorLoc, 1, 1, 1);
        glUniform2f(scaleLoc, 1.0f / viewportWidth, 1.0f / viewportHeight);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Disable blending:
        glDisable(GL_BLEND);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Take Leap motion input and create splats at the hand centers
        Frame frame = controller.frame();
        for (Finger finger : frame.fingers()) {
            if (finger.isExtended()) {
                Vector tipPosition = finger.stabilizedTipPosition();
                Vector tipVelocity = finger.tipVelocity();

                int xpos = (int) ((viewportWidth / 2.0) + (tipPosition.x * 4));
                int ypos = (int) (tipPosition.y * 4 - 300);

                switch(finger.type()) {
                    case Finger::TYPE_INDEX:
                        splat(density.read, density.write, xpos, ypos, 400.0f, 1.0f, 1.0f, 1.0f);
                        break;
                    case Finger::TYPE_MIDDLE:
                        splat(density.read, density.write, xpos, ypos, 400.0f, 0.0f, 0.0f, 0.0f);
                        break;
                    default:
                        splat(density.read, density.write, xpos, ypos, 600.0f, 1.0f, 1.0f, 0.33f);
                        break;
                }

                // splat(density.read, density.write, xpos, ypos, 400.0f, 1.0f, 0.0f, 1.0f);
                swapVectorFields(&density);

                splat(velocity.read, velocity.write, xpos, ypos, 400.0f, tipVelocity.x, tipVelocity.y, tipVelocity.z);
                swapVectorFields(&velocity);
            }
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
