#version 410

out vec4 result;

uniform float alpha;
uniform float rBeta;  // reciprocal beta term

uniform sampler2D pressure;
uniform sampler2D divergence;


void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    // Get neighboring pressures
    vec4 left = texelFetchOffset(pressure, coords, 0, ivec2(-1, 0));
    vec4 right = texelFetchOffset(pressure, coords, 0, ivec2(1, 0));
    vec4 bottom = texelFetchOffset(pressure, coords, 0, ivec2(0, -1));
    vec4 top = texelFetchOffset(pressure, coords, 0, ivec2(0, 1));

    // Get the pressure at our current cell
    vec4 center = texelFetch(divergence, coords, 0);

    // Calculate the Jacobi approximation for the Poisson-pressure equation
    result = (left + right + bottom + top + alpha * center) * rBeta;
}
