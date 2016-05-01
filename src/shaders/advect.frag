/* A fragment shader that generates the advection vector field texture. We
 * follow the velocity field "back in time" and trace the particle at the
 * previous position to our current position instead. This is an implementation
 * of the implicit method demonstrated by Stam (1999).
 */

out vec4 color;  // a color representing the advected quantity

uniform float timestep;
float rdx = 1;

uniform sampler2D velocityField;  // input velocity
uniform sampler2D x;  // quantity to advect by


void main() {

}
