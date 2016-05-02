/* A fragment shader that generates the advection vector field texture. We
 * follow the velocity field "back in time" and trace the particle at the
 * previous position to our current position instead. This is an implementation
 * of the implicit method demonstrated by Stam (1999).
 */

out vec4 advection;  // a color representing the advected quantity

uniform float rdx;
uniform float timestep;
uniform float dissipation;

uniform sampler2D velocity;
uniform sampler2D source;  // the quantity being advected



void main() {
    vec2 coords = gl_FragCoord.xy;

    // We want bilerp here, so we use texture instead of texelFetch
    // vec2 u = texture(velocity, rdx * coords).xy;
    // vec2 pos = rdx * (coords - timestep * u);

    vec2 pos = coords - timestep * rdx * texture(velocity, coords).xy;
    advection = texture(source, pos);
}
