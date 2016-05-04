#version 410

out vec4 color;

uniform sampler2D vectorField;

uniform vec3 bias;
uniform vec2 scale;
uniform float maxVal;

void main() {
    color = vec4(bias, 1.0) + (texture(vectorField, scale * gl_FragCoord.xy) / maxVal);
}
