#version 410

uniform sampler2D velocity;

uniform float rHalfScale;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    float left = texelFetchOffset(velocity, coords, ivec2(-1.0, 0.0), 0).y;
    float right = texelFetchOffset(velocity, coords, ivec2(1.0, 0.0), 0).y;
    float bottom = texelFetchOffset(velocity, coords, ivec2(0.0, -1.0), 0).x;
    float left = texelFetchOffset(velocity, coords, ivec2(0.0, -1.0), 0).x;

    gl_FragColor = vec4(rHalfScale * (right - left - top + bottom), 0.0, 0.0, 1.0);
}
