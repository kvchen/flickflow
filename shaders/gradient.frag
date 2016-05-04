#version 410

/* Subtracts the gradient. */

out vec2 color;

uniform float rHalfScale;

uniform sampler2D pressure;
uniform sampler2D velocity;


void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    // Get the pressures from our neighboring cells
    vec2 center = texelFetch(velocity, coords, 0).xy;
    float left = texelFetchOffset(pressure, coords, 0, ivec2(-1, 0)).x;
    float right = texelFetchOffset(pressure, coords, 0, ivec2(1, 0)).x;
    float bottom = texelFetchOffset(pressure, coords, 0, ivec2(0, -1)).x;
    float top = texelFetchOffset(pressure, coords, 0, ivec2(0, 1)).x;

    color = center - rHalfScale * vec2(right - left, top - bottom);
}
