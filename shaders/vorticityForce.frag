#version 410

out vec4 vorticityForce;

uniform sampler2D velocity;
uniform sampler2D vorticity;

uniform float rHalfScale;

uniform float timestep;
uniform float epsilon;
uniform vec2 curl;


void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    float left = texelFetchOffset(vorticity, coords, 0, ivec2(-1.0, 0.0)).x;
    float right = texelFetchOffset(vorticity, coords, 0, ivec2(1.0, 0.0)).x;
    float bottom = texelFetchOffset(vorticity, coords, 0, ivec2(0.0, -1.0)).x;
    float top = texelFetchOffset(vorticity, coords, 0, ivec2(0.0, 1.0)).x;
    float center = texelFetch(vorticity, coords, 0).x;

    vec2 force = rHalfScale * vec2(abs(top) - abs(bottom), abs(right) - abs(left));
    float lengthSquared = max(epsilon, dot(force, force));

    force *= inversesqrt(lengthSquared) * curl * vc;
    force.y *= -1.0;

    vec2 v = texelFetch(velocity, coords).xy;
    vorticityForce = vec4(v + (timestep * force), 0.0, 1.0);
}
