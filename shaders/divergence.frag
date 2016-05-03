#version 410

/* Calculates the divergence of the intermediate velocity field. This is part
 * of the Jacobi iteration method for approximating the results of the
 * Poisson-pressure equations.
 */

out float divergence;

uniform sampler2D velocity;

uniform float rHalfScale;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    float left = texelFetchOffset(velocity, coords, 0, ivec2(-1, 0)).x;
    float right = texelFetchOffset(velocity, coords, 0, ivec2(1, 0)).x;
    float bottom = texelFetchOffset(velocity, coords, 0, ivec2(0, -1)).y;
    float top = texelFetchOffset(velocity, coords, 0, ivec2(0, 1)).y;

    divergence = rHalfScale * (right - left + top - bottom);
}
