#include <GL/glew.h>


typedef struct VectorField_ {
    GLuint handle;
    GLuint textureHandle;
    int numComponents;
} VectorField;


typedef struct Slab_ {
    VectorField read;
    VectorField write;
} Slab;


Slab createSlab(GLsizei width, GLsizei height, int numComponents);
VectorField createVectorField(GLsizei width, GLsizei height, int numComponents);

void swapVectorFields(Slab* slab);
void fillVectorField(VectorField vf, float v);
GLuint createQuad();
