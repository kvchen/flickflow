/* Calculates the divergence of the intermediate velocity field. This is part
 * of the Jacobi iteration method for approximating the results of the
 * Poisson-pressure equations.
 */

out float divergence;

uniform float halfrdx;
uniform sampler2D velocity;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    vec2 wL = texelFetchOffset(velocity, coords, 0, ivec2(-1, 0)).xy;
    vec2 wR = texelFetchOffset(velocity, coords, 0, ivec2(1, 0)).xy;
    vec2 wB = texelFetchOffset(velocity, coords, 0, ivec2(0, -1)).xy;
    vec2 wT = texelFetchOffset(velocity, coords, 0, ivec2(0, 1)).xy;

    divergence = halfrdx * ((wR.x - wL.x) + (wT.y - wB.y));
}
