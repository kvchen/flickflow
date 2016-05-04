#version 410

out vec4 vorticity;

uniform vec2 size;
uniform float rHalfScale;

uniform sampler2D velocity;


void main() {
    vec2 coords = gl_FragCoord.xy / size.xy;

    vec2 xOffset = vec2(1.0 / size.x, 0.0);
    vec2 yOffset = vec2(0.0, 1.0 / size.y);

    float left = texture(velocity, coords - xOffset).y;
    float right = texture(velocity, coords + xOffset).y;
    float bottom = texture(velocity, coords - yOffset).x;
    float top = texture(velocity, coords + yOffset).x;

    vorticity = vec4(rHalfScale * (right - left - top + bottom), 0.0, 0.0, 1.0);
}
