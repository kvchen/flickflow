/* A very simple boundary fragment program for calculating edge cases for both
 * the pressure and velocity boundaries at the edge of textures.
 */

out float boundary;

uniform vec2 offset;
uniform float scale;
uniform sampler2D state; // represents either the velocity or pressure field

void main() {
    boundary = scale * texelFetchOffset(state, coords, 0, offset);
}
