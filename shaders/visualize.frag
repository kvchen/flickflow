#version 410

out vec4 color;

uniform vec3 bias;
uniform vec2 scale;
uniform float maxVal;

uniform sampler2D vectorField;


void main() {
    color = vec4(bias, 1.0) + (texture(vectorField, scale * gl_FragCoord.xy) / maxVal);
}
