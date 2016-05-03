#version 410

out vec4 color;

uniform vec2 point;
uniform float radius;
uniform vec3 fillColor;


float gauss(vec2 p, float r) {
    return exp(-dot(p, p) / r);
}


void main() {
    vec2 coords = gl_FragCoord.xy;
    float d = distance(point, coords);

    if (d < radius) {
        float a = (radius - d) * 0.2;
        color = vec4(fillColor, a);
    } else {
        color = vec4(0);
    }
}
