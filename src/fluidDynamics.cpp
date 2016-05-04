#include "fluidDynamics.h"
#include "shaderUtils.h"

#define SCALE 1.0f
#define TIMESTEP 0.125f
#define DISSIPATION 0.998



Shaders shaders;

void initializeShaders() {
    shaders.advect = loadShaders("../shaders/all.vert", "../shaders/advect.frag");
    shaders.boundary = loadShaders("../shaders/all.vert", "../shaders/boundary.frag");
    shaders.divergence = loadShaders("../shaders/all.vert", "../shaders/divergence.frag");
    shaders.gradient = loadShaders("../shaders/all.vert", "../shaders/gradient.frag");
    shaders.jacobi = loadShaders("../shaders/all.vert", "../shaders/jacobi.frag");
    shaders.splat = loadShaders("../shaders/all.vert", "../shaders/splat.frag");
    shaders.vorticity = loadShaders("../shaders/all.vert", "../shaders/vorticity.frag");
    shaders.vorticityForce = loadShaders("../shaders/all.vert", "../shaders/vorticityForce.frag");
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


void simulate(Slab velocity, Slab density, Slab pressure, Slab temperature, Slab divergence, Slab vorticity,
              int width, int height) {
    advect(velocity.read, velocity.read, velocity.write, width, height, SCALE, TIMESTEP, DISSIPATION);
    swapVectorFields(&velocity);

    // advect(velocity.read, temperature.read, temperature.write, width, height, SCALE, TIMESTEP, DISSIPATION);
    // swapVectorFields(&temperature);

    advect(velocity.read, density.read, density.read, width, height, SCALE, TIMESTEP, DISSIPATION);
    // swapVectorFields(&density);

    // computeVorticity(velocity.read, vorticity.read, SCALE);
    // computeVorticityForce(velocity.read, vorticity.read, velocity.write, SCALE, TIMESTEP, 2.4414e-4, 0.3, 0.3);
    // swapVectorFields(&velocity);

    fillVectorField(pressure.read, 0);
    computeDivergence(velocity.read, divergence.read, SCALE);

    // Project begins here
    for (int i = 0; i < 40; i++) {
        computeJacobi(pressure.read, divergence.read, pressure.write, -1.0, 4.0);
        swapVectorFields(&pressure);
    }

    subtractGradient(velocity.read, pressure.read, velocity.write, SCALE);
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

    glUniform1f(alphaLoc, alpha);
    glUniform1f(rBetaLoc, 1.0 / beta);

    glBindFramebuffer(GL_FRAMEBUFFER, output.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pressure.textureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, divergence.textureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}


void computeVorticity(VectorField velocity, VectorField output,
               float scale) {
    GLuint program = shaders.vorticity;
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

void computeVorticityForce(VectorField velocity, VectorField vorticity, VectorField output,
                           float scale, float timestep, float epsilon, float curlX, float curlY) {
    GLuint program = shaders.vorticityForce;
    glUseProgram(program);

    GLint velocityLoc = glGetUniformLocation(program, "velocity");
    GLint vorticityLoc = glGetUniformLocation(program, "vorticity");

    glUniform1i(velocityLoc, 0);
    glUniform1i(vorticityLoc, 0);

    GLint rHalfScaleLoc = glGetUniformLocation(program, "rHalfScale");
    GLint timestepLoc = glGetUniformLocation(program, "timestep");
    GLint epsilonLoc = glGetUniformLocation(program, "epsilon");
    GLint curlLoc = glGetUniformLocation(program, "curl");

    glUniform1f(rHalfScaleLoc, 0.5 / scale);
    glUniform1f(timestepLoc, timestep);
    glUniform1f(epsilonLoc, epsilon);
    glUniform2f(curlLoc, curlX, curlY);

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
    // glEnable(GL_BLEND);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    resetState();
}
