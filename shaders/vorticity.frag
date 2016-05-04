#version 410

out vec4 vorticity;

uniform sampler2D velocity;

uniform float rHalfScale;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    float left = texelFetchOffset(velocity, coords, 0, ivec2(-1.0, 0.0)).y;
    float right = texelFetchOffset(velocity, coords, 0, ivec2(1.0, 0.0)).y;
    float bottom = texelFetchOffset(velocity, coords, 0, ivec2(0.0, -1.0)).x;
    float top = texelFetchOffset(velocity, coords, 0, ivec2(0.0, 1.0)).x;

    vorticity = vec4(rHalfScale * (right - left - top + bottom), 0.0, 0.0, 1.0);
}
