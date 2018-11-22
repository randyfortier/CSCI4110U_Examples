#version 130

uniform vec3 u_EyePosition;
uniform vec3 u_LightPos;
uniform samplerCube u_TextureSampler;
uniform float u_Shininess;

varying vec3 v_Normal;
varying vec3 v_Position;
varying vec3 v_Reflected;
varying vec3 v_Refracted;

const float ambient = 0.3;

void main() {
   vec4 materialColour = vec4(1.0, 1.0, 1.0, 1.0);

   vec3 I = normalize(v_Position - u_EyePosition);
   vec3 N = normalize(v_Normal);

   float brightness = max(dot(I, N), 0.0) + ambient;
   vec4 baseColour = materialColour * brightness;

   vec4 reflectedColour = texture(u_TextureSampler, v_Reflected);
   vec4 refractedColour = texture(u_TextureSampler, v_Refracted);
   vec4 enviroColour = refractedColour; //mix(reflectedColour, refractedColour, 0.9);

   gl_FragColor = mix(baseColour, enviroColour, 0.9);
}
