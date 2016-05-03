#version 410

out vec4 color;

uniform sampler2D source;

uniform vec2 point;
uniform float radius;
uniform vec3 fillColor;


float gauss(vec2 p, float r) {
    return exp(-dot(p, p) / r);
}


void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec4 sourceColor = texelFetch(source, coords, 0);

    vec3 splat = fillColor * gauss(point - coords, radius);
    color = vec4(splat + sourceColor.xyz, 1.0);
}
