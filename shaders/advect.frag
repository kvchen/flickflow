#version 410

/* A fragment shader that generates the advection vector field texture. We
 * follow the velocity field "back in time" and trace the particle at the
 * previous position to our current position instead. This is an implementation
 * of the implicit method demonstrated by Stam (1999).
 */

out vec4 advection;

uniform vec2 rSize;
uniform float rScale;
uniform float timestep;
uniform float dissipation;

uniform sampler2D velocity;
uniform sampler2D advected;


void main() {
    vec2 coords = gl_FragCoord.xy;
    vec2 pos = rSize * (coords - timestep * rScale * texture(velocity, rSize * coords).xy);

    // We want bilinear interpolation here, so we use texture instead of texelFetch
    advection = dissipation * texture(advected, pos);
}
