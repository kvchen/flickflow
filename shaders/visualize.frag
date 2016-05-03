#version 410

out vec4 color;

uniform sampler2D vectorField;
uniform vec3 fillColor;
uniform vec2 scale;

void main() {
    color = vec4(texture(vectorField, gl_FragCoord.xy * scale).xyz, 1.0);
}
