#version 410

out vec4 vorticity;

uniform sampler2D velocity;

uniform vec2 size;
uniform float rHalfScale;

void main() {
    vec2 coords = gl_FragCoord.xy / size.xy;

<<<<<<< HEAD
    float left = texelFetchOffset(velocity, coords, 0, ivec2(-1.0, 0.0)).y;
    float right = texelFetchOffset(velocity, coords, 0, ivec2(1.0, 0.0)).y;
    float bottom = texelFetchOffset(velocity, coords, 0, ivec2(0.0, -1.0)).x;
    float top = texelFetchOffset(velocity, coords, 0, ivec2(0.0, 1.0)).x;
=======
    vec2 xOffset = vec2(1.0 / size.x, 0.0);
    vec2 yOffset = vec2(0.0, 1.0 / size.y);

    float left = texture(velocity, coords - xOffset).y;
    float right = texture(velocity, coords + xOffset).y;
    float bottom = texture(velocity, coords - yOffset).x;
    float top = texture(velocity, coords + yOffset).x;
>>>>>>> master

    vorticity = vec4(rHalfScale * (right - left - top + bottom), 0.0, 0.0, 1.0);
}
