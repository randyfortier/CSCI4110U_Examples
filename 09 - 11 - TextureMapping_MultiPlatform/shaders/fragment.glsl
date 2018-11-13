#version 130

uniform sampler2D u_TextureSampler;

varying vec2 v_TextureCoords;

void main() {
   vec3 baseColour = texture(u_TextureSampler, v_TextureCoords).rgb;
   gl_FragColor = vec4(baseColour, 1.0);
}
