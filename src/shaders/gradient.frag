/* Subtracts the gradient. */

out vec2 velocity;

uniform float halfrdx;
uniform sampler2D pressure;
uniform sampler2D velocity;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    // Get the pressures from our neighboring cells
    float pL = texelFetchOffset(pressure, coords, 0, ivec2(-1, 0)).r;
    float pR = texelFetchOffset(pressure, coords, 0, ivec2(1, 0)).r;
    float pB = texelFetchOffset(pressure, coords, 0, ivec2(0, -1)).r;
    float pT = texelFetchOffset(pressure, coords, 0, ivec2(0, 1)).r;

    vec2 oldVelocity = texelFetch(velocity, coords, 0).xy;
    velocity = oldVelocity - halfrdx * vec2(pR - pL, pT - pB);
}
