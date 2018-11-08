// uniform - per draw call, per shape
// attribute - per vertex, come from vertex buffer objects (VBOs)
// varying - from one shader into another, interpolated

varying vec4 v_Colour;

void main() {
   gl_FragColor = v_Colour;
}
