#include <algorithm>
#include "slab.h"


using namespace std;


/* Since writing to a framebuffer which is bound for reading is undefined, we
 * use the strategy of ping-ponging between two textures, alternating reading
 * and writing from each one.
 */
void swapFramebuffers(Slab* slab) {
    std::swap(slab->ping, slab->pong);
}

void fillFramebuffer(Framebuffer fb, float v) {
    glBindFramebuffer(GL_FRAMEBUFFER, fb.handle);
    glClearColor(v, v, v, v);
    glClear(GL_COLOR_BUFFER_BIT);
}
