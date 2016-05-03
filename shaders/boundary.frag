#version 410

/* A very simple boundary fragment program for calculating edge cases for both
 * the pressure and velocity boundaries at the edge of textures.
 */

out vec4 boundary;

uniform ivec2 offset;
uniform float scale;
uniform sampler2D bounded; // represents either the velocity or pressure field

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    boundary = scale * texelFetch(bounded, coords + offset, 0);
}
