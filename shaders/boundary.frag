#version 410

/* A very simple boundary fragment program for calculating edge cases for both
 * the pressure and velocity boundaries at the edge of textures.
 */

out vec4 bounded;

uniform bool isVelocity;
uniform int width;
uniform int height;

uniform sampler2D field; // either the velocity or pressure field

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    int x = coords.x;
    int y = coords.y;

    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) { // At a boundary
        bounded = vec4(0, 0, 0, 0);
    } else {
        bounded = texelFetch(field, coords, 0);
    }
}
