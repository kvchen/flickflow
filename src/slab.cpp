#include <algorithm>
#include "slab.h"


using namespace std;

void swapFrameBuffers(Slab* slab) {
    std::swap(slab->ping, slab->pong);
}

void fillFrameBuffer(Framebuffer fb, float v) {
    glBindFramebuffer(GL_FRAMEBUFFER, fb.handle);
    glClearColor(v, v, v, v);
    glClear(GL_COLOR_BUFFER_BIT);
}
