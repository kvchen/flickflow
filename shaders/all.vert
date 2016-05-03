#version 410

/* Since we're doing operations on textures, we don't need to do anything fancy
 * here. We use the same vertex shader for all our fragment shaders.
 */

in vec4 position;

void main() {
    gl_Position = position;
}
