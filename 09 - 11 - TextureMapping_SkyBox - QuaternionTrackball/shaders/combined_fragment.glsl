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
  vec3 N = normalize(v_Normal);
  vec3 I = normalize(v_Position - u_EyePosition);
  vec3 R = reflect(I, N);

  vec4 materialColour = vec4(1.0, 1.0, 1.0, 1.0);

  float brightness = max(dot(I, N), 0.0) + ambient;
  vec4 baseColour = materialColour * brightness;

	vec4 reflectedColour = texture(u_TextureSampler, v_Reflected);
	vec4 refractedColour = texture(u_TextureSampler, v_Refracted);
	vec4 enviroColour = refractedColour; //mix(reflectedColour, refractedColour, 0.5);

  gl_FragColor = mix(baseColour, enviroColour, 0.8);

}


/*
#version 150

out vec4 out_Colour;

in vec2 pass_textureCoordinates;
in vec3 pass_normal;
in vec3 reflectedVector;
in vec3 refractedVector;

uniform sampler2D modelTexture;
uniform samplerCube enviroMap;

const vec3 lightDirection = normalize(vec3(0.2, -1.0, 0.3));
const float ambient = 0.3;

void main(void){

	float brightness = max(dot(-lightDirection, normalize(pass_normal)), 0.0) + ambient;
	out_Colour = texture(modelTexture, pass_textureCoordinates) * brightness;

	vec4 reflectedColour = texture(enviroMap, reflectedVector);
	vec4 refractedColour = texture(enviroMap, refractedVector);
	vec4 enviroColour = mix(reflectedColour, refractedColour, 0.5);

	out_Colour = mix(out_Colour, enviroColour, 0.8);
}
*/
