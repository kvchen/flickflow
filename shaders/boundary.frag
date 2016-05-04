#version 410

/* A very simple boundary fragment program for calculating edge cases for both
 * the pressure and velocity boundaries at the edge of textures.
 */

out vec4 bounded;

uniform sampler2D field; // either the velocity or pressure field
uniform bool isVelocity;
uniform int width;
uniform int height;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    int x = coords.x;
    int y = coords.y;

    float scale = 1.0;
    ivec2 offset = ivec2(0, 0);

    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) { // At a boundary
        if (isVelocity) {
            scale = -1.0;
        }

        if (x == 0) {
            offset = ivec2(1, 0);
        } else if (x == width - 1) {
            offset = ivec2(-1, 0);
        } else if (y == 0) {
            offset = ivec2(0, 1);
        } else {
            offset = ivec2(0, -1);
        }
    }

    bounded = scale * texelFetch(field, coords + offset, 0);
}
