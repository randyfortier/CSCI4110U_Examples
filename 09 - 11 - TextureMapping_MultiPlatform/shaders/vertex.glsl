#version 110

attribute vec3 position;
attribute vec2 textureCoords;
attribute vec3 normal;

uniform mat4 u_MVP;

varying vec2 v_TextureCoords;

void main() {
   v_TextureCoords = textureCoords;

   gl_Position = u_MVP * vec4(position, 1.0);
}
