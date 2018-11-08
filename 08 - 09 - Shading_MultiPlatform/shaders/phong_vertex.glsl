uniform mat4 u_MVMatrix;
uniform mat4 u_MVPMatrix;

attribute vec4 position;
attribute vec3 normal;

varying vec3 v_Position;
varying vec3 v_Normal;

void main() {
   // interpolate the position and normal
   v_Position = vec3(u_MVMatrix * position);
   v_Normal = vec3(u_MVMatrix * vec4(normal, 0.0));

   // set the vertex's position
   gl_Position = u_MVPMatrix * position;
}
