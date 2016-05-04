#include <algorithm>
#include "slab.h"


using namespace std;


/* Since writing to a vector field which is bound for reading is undefined, we
 * use the strategy of ping-ponging between two textures, alternating reading
 * and writing from each one.
 */
void swapVectorFields(Slab* slab) {
    std::swap(slab->read, slab->write);
}


void fillVectorField(VectorField vf, float v) {
    glBindFramebuffer(GL_FRAMEBUFFER, vf.handle);
    glClearColor(v, v, v, v);
    glClear(GL_COLOR_BUFFER_BIT);
}


Slab createSlab(GLsizei width, GLsizei height, int numComponents) {
    Slab slab;
    slab.read = createVectorField(width, height, numComponents);
    slab.write = createVectorField(width, height, numComponents);
    return slab;
}


VectorField createVectorField(GLsizei width, GLsizei height, int numComponents) {
    GLuint handle;
    glGenFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);

    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    switch (numComponents) {
        case 1:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, 0);
            break;
        case 2:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, 0);
            break;
        case 3:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
            break;
        case 4:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
            break;
        default:
            break;
    }

    GLuint colorBuffer;
    glGenRenderbuffers(1, &colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHandle, 0);

    VectorField vf = { handle, textureHandle, numComponents };

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return vf;
}


/* Create a quad for us to render all our textures onto. This will be written
 * to whenever we call glDrawArrays. This code is taken from the OpenGL sample
 * code.
 */
GLuint createQuad() {
    short positions[] = {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1,
    };

    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO:
    GLuint vbo;
    GLsizeiptr size = sizeof(positions);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);

    // Set up the vertex layout:
    GLsizeiptr stride = 2 * sizeof(positions[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, stride, 0);

    return vao;
}
