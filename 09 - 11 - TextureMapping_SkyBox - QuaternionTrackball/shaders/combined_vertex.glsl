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
  v_Refracted = refract(viewVector, unitNormal, 0.9);

  v_Position = vec3(u_ModelMatrix * vec4(position, 1.0));
  v_Normal = vec3(u_ModelMatrix * vec4(normal, 1.0));

  gl_Position = u_MVPMatrix * vec4(position, 1.0);
}
