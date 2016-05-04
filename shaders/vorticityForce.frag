#version 410

out vec4 newVelocity;

uniform sampler2D velocity;
uniform sampler2D vorticity;

uniform vec2 size;
uniform float rHalfScale;

uniform float timestep;
uniform float epsilon;
uniform vec2 curl;


void main() {
    vec2 coords = gl_FragCoord.xy / size.xy;

    vec2 xOffset = vec2(1.0 / size.x, 0.0);
    vec2 yOffset = vec2(0.0, 1.0 / size.y);

    float left = texture(vorticity, coords - xOffset).x;
    float right = texture(vorticity, coords + xOffset).x;
    float bottom = texture(vorticity, coords - yOffset).x;
    float top = texture(vorticity, coords + yOffset).x;
    float center = texture(vorticity, coords).x;

    vec2 force = rHalfScale * vec2(abs(top) - abs(bottom), abs(right) - abs(left));
    float lengthSquared = max(epsilon, dot(force, force));

    force *= inversesqrt(lengthSquared) * curl * center;
    force.y *= -1.0;

    vec2 v = texture(velocity, coords).xy;
    newVelocity = vec4(v + (timestep * force), 0.0, 1.0);
}
