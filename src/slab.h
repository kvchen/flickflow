#include <GL/glew.h>


typedef struct Framebuffer_ {
    GLuint handle;
    GLuint texHandle;
    int numComponents;
} Framebuffer;


/* Since writing to a framebuffer which is bound for reading is undefined, we
 * use the strategy of ping-ponging between two textures, alternating reading
 * and writing from each one.
 */
typedef struct Slab_ {
    Framebuffer ping;
    Framebuffer pong;
} Slab;
