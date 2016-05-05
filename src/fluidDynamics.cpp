#include "fluidDynamics.h"
#include "shaderUtils.h"

#define SCALE 1.0f
#define TIMESTEP 0.125f
#define DISSIPATION 0.996
#define VELOCITY_DISSIPATION 0.97
#define NUM_JACOBI_ITERATIONS 30
#define EPSILON 2.4414e-4
#define CURL 0.3
#define VISCOSITY 0.001f

Shaders shaders;

void initializeShaders() {
    shaders.advect = loadShaders("../shaders/all.vert", "../shaders/advect.frag");
    shaders.boundary = loadShaders("../shaders/all.vert", "../shaders/boundary.frag");
    shaders.divergence = loadShaders("../shaders/all.vert", "../shaders/divergence.frag");
    shaders.gradient = loadShaders("../shaders/all.vert", "../shaders/gradient.frag");
    shaders.jacobi = loadShaders("../shaders/all.vert", "../shaders/jacobi.frag");
    shaders.splat = loadShaders("../shaders/all.vert", "../shaders/splat.frag");
    shaders.gaussianSplat = loadShaders("../shaders/all.vert", "../shaders/gaussianSplat.frag");
    shaders.vorticity = loadShaders("../shaders/all.vert", "../shaders/vorticity.frag");
    shaders.vorticityForce = loadShaders("../shaders/all.vert", "../shaders/vorticityForce.frag");
    shaders.add = loadShaders("../shaders/all.vert", "../shaders/add.frag");
}


static void resetState() {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
}


void simulate(Slab velocity, Slab density, Slab pressure, Slab diffusion, Slab divergence, Slab vorticity,
              int width, int height) {
    // --- Advection ---
    advect(velocity.read, velocity.read, velocity.write, width, height, SCALE, TIMESTEP, VELOCITY_DISSIPATION);
    swapVectorFields(&velocity);

    // --- Advection of the ink or fluid injected ---
    advect(velocity.read, density.read, density.write, width, height, SCALE, TIMESTEP, DISSIPATION);
    swapVectorFields(&density);

    // --- Diffusion ---
    // Make a copy of the velocity field and put it into diffusion field.
    fillVectorField(diffusion.read, 0);
    addFields(diffusion.read, velocity.read, diffusion.write, 1.0, 1.0);
    swapVectorFields(&diffusion);

    // Jacobi iterations to estimate diffusion field
    for (int i = 0; i < NUM_JACOBI_ITERATIONS; i++) {
      computeJacobi(diffusion.read, diffusion.read, diffusion.write, 1.0, 5.0);
      swapVectorFields(&diffusion);
    }

    // Add back into velocity field with VISCOSITY scalar
    addFields(velocity.read, diffusion.read, velocity.write, 1.0, VISCOSITY);
    swapVectorFields(&velocity);

    // --- Vorticity confinement ---
    computeVorticity(velocity.read, vorticity.read, width, height, SCALE);
    computeVorticityForce(velocity.read, vorticity.read, velocity.write, width, height, SCALE, TIMESTEP, EPSILON, CURL, CURL);
    swapVectorFields(&velocity);

    // --- Projection operator (account for change in fluid velocity due to fluid pressure) ---
    // Compute divergence of velocity field
    computeDivergence(velocity.read, divergence.read, SCALE);
    // Jacobi iterations to estimate pressure field
    fillVectorField(pressure.read, 0);
    for (int i = 0; i < NUM_JACOBI_ITERATIONS; i++) {
        computeJacobi(pressure.read, divergence.read, pressure.write, -1.0, 4.0);
        swapVectorFields(&pressure);
    }
    // Enforce boundary conditions on the pressure field
    checkBoundary(pressure.read, pressure.write, width, height, false);
    swapVectorFields(&pressure);
    // Subtract pressure gradient from the velocity field
    subtractGradient(velocity.read, pressure.read, velocity.write, SCALE);
    swapVectorFields(&velocity);

    // --- Finally, enforce boundary conditions on the velocity field ---
    checkBoundary(velocity.read, velocity.write, width, height, true);
    swapVectorFields(&velocity);
}


void advect(VectorField velocity, VectorField advected, VectorField output,
            int width, int height, float scale, float timestep, float dissipation) {
    GLuint program = shaders.advect;
    glUseProgram(program);

    GLint velocityLoc = glGetUniformLocation(program, "velocity");
    GLint advectedLoc = glGetUniformLocation(program, "advected");

    glUniform1i(velocityLoc, 0);
    glUniform1i(advectedLoc, 1);

    GLint rSizeLoc = glGetUniformLocation(program, "rSize");
    GLint rScaleLoc = glGetUniformLocation(program, "rScale");
    GLint timestepLoc = glGetUniformLocation(program, "timestep");
    GLint dissipationLoc = glGetUniformLocation(program, "dissipation");

    glUniform2f(rSizeLoc, 1.0 / width, 1.0 / height);
    glUniform1f(rScaleLoc, 1.0 / scale);
    glUniform1f(timestepLoc, timestep);
    glUniform1f(dissipationLoc, dissipation);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.textureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, advected.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}


void computeDivergence(VectorField velocity, VectorField output,
                       float scale) {
    GLuint program = shaders.divergence;
    glUseProgram(program);

    GLint velocityLoc = glGetUniformLocation(program, "velocity");
    glUniform1i(velocityLoc, 0);

    GLint rHalfScaleLoc = glGetUniformLocation(program, "rHalfScale");
    glUniform1f(rHalfScaleLoc, 0.5 / scale);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}


void subtractGradient(VectorField velocity, VectorField pressure, VectorField output,
                      float scale) {
    GLuint program = shaders.gradient;
    glUseProgram(program);

    GLint velocityLoc = glGetUniformLocation(program, "velocity");
    GLint pressureLoc = glGetUniformLocation(program, "pressure");
    glUniform1i(velocityLoc, 0);
    glUniform1i(pressureLoc, 1);

    GLint rHalfScaleLoc = glGetUniformLocation(program, "rHalfScale");
    glUniform1f(rHalfScaleLoc, 0.5 / scale);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.textureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pressure.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}


void computeJacobi(VectorField pressure, VectorField divergence, VectorField output,
                   float alpha=-1.0, float beta=4.0) {
    GLuint program = shaders.jacobi;
    glUseProgram(program);

    GLint pressureLoc = glGetUniformLocation(program, "pressure");
    GLint divergenceLoc = glGetUniformLocation(program, "divergence");
    glUniform1i(pressureLoc, 0);
    glUniform1i(divergenceLoc, 1);


    GLint alphaLoc = glGetUniformLocation(program, "alpha");
    GLint rBetaLoc = glGetUniformLocation(program, "rBeta");
    // GLint scaleLoc = glGetUniformLocation(program, "scale");
    glUniform1f(alphaLoc, alpha);
    glUniform1f(rBetaLoc, 1.0 / beta);
    // glUniform1i(scaleLoc, scale);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pressure.textureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, divergence.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}


void computeVorticity(VectorField velocity, VectorField output,
                      int width, int height, float scale) {
    GLuint program = shaders.vorticity;
    glUseProgram(program);

    GLint velocityLoc = glGetUniformLocation(program, "velocity");
    glUniform1i(velocityLoc, 0);

    GLint sizeLoc = glGetUniformLocation(program, "size");
    GLint rHalfScaleLoc = glGetUniformLocation(program, "rHalfScale");
    glUniform2f(sizeLoc, width, height);
    glUniform1f(rHalfScaleLoc, 0.5 / scale);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}

void computeVorticityForce(VectorField velocity, VectorField vorticity, VectorField output,
                           int width, int height, float scale, float timestep, float epsilon, float curlX, float curlY) {
    GLuint program = shaders.vorticityForce;
    glUseProgram(program);

    GLint velocityLoc = glGetUniformLocation(program, "velocity");
    GLint vorticityLoc = glGetUniformLocation(program, "vorticity");
    glUniform1i(velocityLoc, 0);
    glUniform1i(vorticityLoc, 1);

    GLint sizeLoc = glGetUniformLocation(program, "size");
    GLint rHalfScaleLoc = glGetUniformLocation(program, "rHalfScale");
    GLint timestepLoc = glGetUniformLocation(program, "timestep");
    GLint epsilonLoc = glGetUniformLocation(program, "epsilon");
    GLint curlLoc = glGetUniformLocation(program, "curl");
    glUniform2f(sizeLoc, width, height);
    glUniform1f(rHalfScaleLoc, 0.5 / scale);
    glUniform1f(timestepLoc, timestep);
    glUniform1f(epsilonLoc, epsilon);
    glUniform2f(curlLoc, curlX * scale, curlY * scale);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.textureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vorticity.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}


void splat(VectorField source, VectorField output,
           int x, int y, float radius, float fillX, float fillY, float fillZ) {
    GLuint program = shaders.splat;
    glUseProgram(program);

    GLint inputLoc = glGetUniformLocation(program, "source");
    glUniform1i(inputLoc, 0);

    GLint pointLoc = glGetUniformLocation(program, "point");
    GLint radiusLoc = glGetUniformLocation(program, "radius");
    GLint fillColorLoc = glGetUniformLocation(program, "fillColor");
    glUniform2f(pointLoc, x, y);
    glUniform1f(radiusLoc, radius);
    glUniform3f(fillColorLoc, fillX, fillY, fillZ);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, source.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}


void gaussianSplat(VectorField source, VectorField output,
                   int x, int y, float radius, float fillX, float fillY, float fillZ) {
    GLuint program = shaders.gaussianSplat;
    glUseProgram(program);

    GLint inputLoc = glGetUniformLocation(program, "source");
    glUniform1i(inputLoc, 0);

    GLint pointLoc = glGetUniformLocation(program, "point");
    GLint radiusLoc = glGetUniformLocation(program, "radius");
    GLint fillColorLoc = glGetUniformLocation(program, "fillColor");
    glUniform2f(pointLoc, x, y);
    glUniform1f(radiusLoc, radius);
    glUniform3f(fillColorLoc, fillX, fillY, fillZ);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, source.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}


void checkBoundary(VectorField field, VectorField output, int width, int height, bool isVelocity) {
    GLuint program = shaders.boundary;
    glUseProgram(program);

    GLint fieldLoc = glGetUniformLocation(program, "field");
    glUniform1i(fieldLoc, 0);

    GLint isVelocityLoc = glGetUniformLocation(program, "isVelocity");
    GLint widthLoc = glGetUniformLocation(program, "width");
    GLint heightLoc = glGetUniformLocation(program, "height");
    glUniform1i(isVelocityLoc, (int)isVelocity);
    glUniform1i(widthLoc, width);
    glUniform1i(heightLoc, height);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, field.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}


void addFields(VectorField a, VectorField b, VectorField output, float scaleA=1.0, float scaleB=1.0) {
  GLuint program = shaders.add;
  glUseProgram(program);

  GLint aLoc = glGetUniformLocation(program, "a");
  GLint bLoc = glGetUniformLocation(program, "b");
  glUniform1i(aLoc, 0);
  glUniform1i(bLoc, 1);

  GLint scaleALoc = glGetUniformLocation(program, "scaleA");
  GLint scaleBLoc = glGetUniformLocation(program, "scaleB");
  glUniform1f(scaleALoc, scaleA);
  glUniform1f(scaleBLoc, scaleB);

  glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, a.textureHandle);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, b.textureHandle);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  resetState();
}
