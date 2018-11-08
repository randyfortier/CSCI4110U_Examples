uniform vec3 u_EyePosition;
uniform mat4 u_ModelMatrix;
uniform mat4 u_MVPMatrix;

attribute vec3 position;
attribute vec3 normal;

varying vec3 v_Normal;
varying vec3 v_Position;
varying vec3 v_Reflected;
varying vec3 v_Refracted;

void main() {
  vec4 worldPosition = u_ModelMatrix * vec4(position, 1.0);

  vec3 unitNormal = normalize(normal);

  vec3 viewVector = normalize(worldPosition.xyz - u_EyePosition);
  v_Reflected = reflect(viewVector, unitNormal);
  v_Refracted = refract(viewVector, unitNormal, 1.0/1.1);
  v_Normal = normal;
  v_Position = vec3(u_ModelMatrix * vec4(position, 1.0));

  v_Normal = mat3(transpose(inverse(u_ModelMatrix))) * normal;

  gl_Position = u_MVPMatrix * vec4(position, 1.0);
}

/*
#version 150

in vec3 position;
in vec2 textureCoordinates;
in vec3 normal;

out vec3 pass_normal;
out vec2 pass_textureCoordinates;
out vec3 reflectedVector;
out vec3 refractedVector;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 cameraPosition;

void main(void){

	vec4 worldPosition = transformationMatrix * vec4(position, 1.0);
	gl_Position = projectionMatrix * viewMatrix * worldPosition;

	pass_textureCoordinates = textureCoordinates;
	pass_normal = normal;
	vec3 unitNormal = normalize(normal);

	vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
	reflectedVector = reflect(viewVector, unitNormal);
	refractedVector = refract(viewVector, unitNormal, 1.0/1.1);
}
*/
