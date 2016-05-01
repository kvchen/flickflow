#include <GL/glew.h>


typedef struct Framebuffer_ {
    GLuint handle;
    GLuint texHandle;
    int numComponents;
} Framebuffer;


typedef struct Slab_ {
    Framebuffer ping;
    Framebuffer pong;
} Slab;


void swapFrameBuffers(Slab* slab);
void fillFrameBuffer(Framebuffer fb, float v);
