out vec4 result;

uniform float alpha;
uniform float rBeta;  // reciprocal beta term

uniform sampler2D pressure;
uniform sampler2D divergence;


void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    // Get the netghboring pressures
    vec4 xL = texelFetchOffset(pressure, coords, 0, ivec2(-1, 0));
    vec4 xR = texelFetchOffset(pressure, coords, 0, ivec2(1, 0));
    vec4 xB = texelFetchOffset(pressure, coords, 0, ivec2(0, -1));
    vec4 xT = texelFetchOffset(pressure, coords, 0, ivec2(0, 1));

    // Get the pressure at our current cell
    vec4 bC = texelFetch(divergence, coords, 0);

    // Calculate the Jacobi approximation for the Poisson-pressure equation
    result = (xL + xR + xB + xT + alpha * bC) * rBeta;
}
