#include "slab.h"


typedef struct Shaders_ {
    GLuint add;
    GLuint advect;
    GLuint boundary;
    GLuint divergence;
    GLuint gradient;
    GLuint jacobi;
    GLuint splat;
    GLuint gaussianSplat;
    GLuint vorticity;
    GLuint vorticityForce;
} Shaders;

void initializeShaders();

void simulate(Slab velocity, Slab density, Slab pressure, Slab diffusion, Slab divergence, Slab vorticity,
              int width, int height);
void advect(VectorField velocity, VectorField advected, VectorField output,
            int width, int height, float scale, float timestep, float dissipation);
void computeDivergence(VectorField velocity, VectorField output,
                       float scale);
void subtractGradient(VectorField velocity, VectorField pressure, VectorField output,
                      float scale);
void computeJacobi(VectorField pressure, VectorField divergence, VectorField output,
                   float alpha, float beta);
void computeVorticity(VectorField velocity, VectorField output,
                      int width, int height, float scale);
void computeVorticityForce(VectorField velocity, VectorField vorticity, VectorField output,
                           int width, int height, float scale, float timestep, float epsilon, float curlX, float curlY);
void splat(VectorField source, VectorField output,
           int x, int y, float radius, float fillX, float fillY, float fillZ);
void gaussianSplat(VectorField source, VectorField output,
                   int x, int y, float radius, float fillX, float fillY, float fillZ);
void checkBoundary(VectorField velocity, VectorField output, int width, int height, bool isVelocity);
void addFields(VectorField a, VectorField b, VectorField output, float scaleA, float scaleB);
