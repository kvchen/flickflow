#version 410

out vec4 result;

uniform float scaleA;
uniform float scaleB;

uniform sampler2D a;
uniform sampler2D b;


void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    result = scaleA * texelFetch(a, coords, 0) + scaleB * texelFetch(b, coords, 0);
}
