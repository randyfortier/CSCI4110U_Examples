#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <cmath>
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "ObjMesh.h"
#include "spring.h"

int windowId = 0;

// projection matrix - perspective projection
glm::mat4 projectionMatrix;

// view matrix - orient everything around our preferred view
glm::mat4 viewMatrix;

GLuint programId;

GLuint vertexBuffer;
GLenum positionBufferId;

GLuint lineVertexBuffer;

GLuint normals_vbo;
GLuint textureCoords_vbo;
GLuint positions_vbo;
GLuint indexBuffer;

// rope geometry
float lineVertexPositions[] = {
  0.0f,  0.0f, 0.0f,
  0.0f, -1.0f, 0.0f
};
int numLineVertices = 2;

// sphere geometry
ObjMesh mesh;
int numVertices;

void drawLine(glm::mat4 modelMatrix, glm::vec4 colour);
void drawSphere(glm::mat4 modelMatrix, glm::vec4 colour);
static GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
static GLuint createShader(const GLenum shaderType, const std::string shaderSource);

Spring pendulumSpring;

float deltaT = 0.01f;

static void createGeometry(void) {
  // sphere geometry
  mesh.load("sphere.obj", true, true);

  numVertices = mesh.getNumIndexedVertices();
  Vector3* vertexPositions = mesh.getIndexedPositions();
  Vector2* vertexTextureCoords = mesh.getIndexedTextureCoords();
  Vector3* vertexNormals = mesh.getIndexedNormals();

  glGenBuffers(1, &positions_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertexPositions, GL_STATIC_DRAW);

  glGenBuffers(1, &textureCoords_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, textureCoords_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), vertexTextureCoords, GL_STATIC_DRAW);

  glGenBuffers(1, &normals_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertexNormals, GL_STATIC_DRAW);

  unsigned int* indexData = mesh.getTriangleIndices();
  int numTriangles = mesh.getNumTriangles();

  glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numTriangles * 3, indexData, GL_STATIC_DRAW);

  // line geometry
  glGenBuffers(1, &lineVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, lineVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, numLineVertices * 3 * sizeof(float), lineVertexPositions, GL_DYNAMIC_DRAW);
}

int previousFrameMillis = -1;
static void update(void) {
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);
    float dt = 0.0f;

    if (previousFrameMillis < 0) {
      dt = (float)milliseconds / 1000.0f;
    } else {
      dt = (float)(milliseconds - previousFrameMillis) / 1000.0f;
      previousFrameMillis = milliseconds;
    }

    // update the animation
    pendulumSpring.update(deltaT);

    glutPostRedisplay();
}

static void render(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // turn on depth buffering
  glEnable(GL_DEPTH_TEST);

  // activate our shader program
  glUseProgram(programId);

  // colours
  glm::vec4 white(1.0, 1.0, 1.0, 1.0);
  glm::vec4 red(1.0, 0.0, 0.0, 1.0);

  // TODO: Draw the weight and the spring

  // make the draw buffer to display buffer (i.e. display what we have drawn)
  glutSwapBuffers();
}

void drawLine(glm::mat4 modelMatrix, glm::vec4 colour) {
  // model-view-projection matrix
  glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
  GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVP");
  glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

  // colour
  GLuint colourId = glGetUniformLocation(programId, "u_colour");
  glUniform4fv(colourId, 1, (GLfloat*)&colour[0]);

  // enable the vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, lineVertexBuffer);

  // configure the attribute array (the layout of the vertex buffer)
  glVertexAttribPointer(positionBufferId, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *)0);
  glEnableVertexAttribArray(positionBufferId);

  glEnable(GL_PROGRAM_POINT_SIZE);

  // draw the vertices
  glDrawArrays(GL_LINES, 0, numLineVertices);

  glDisable(GL_PROGRAM_POINT_SIZE);

  // disable the attribute array
  glDisableVertexAttribArray(positionBufferId);
}

void drawSphere(glm::mat4 modelMatrix, glm::vec4 colour) {
  // model-view-projection matrix
  glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
  GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVP");
  glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

  // colour
  GLuint colourId = glGetUniformLocation(programId, "u_colour");
  glUniform4fv(colourId, 1, (GLfloat*)&colour[0]);

  // find the names (ids) of each vertex attribute
  GLint positionAttribId = glGetAttribLocation(programId, "position");
  GLint textureCoordsAttribId = glGetAttribLocation(programId, "textureCoords");
  GLint normalAttribId = glGetAttribLocation(programId, "normal");

  // provide the vertex positions to the shaders
  glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
  glEnableVertexAttribArray(positionAttribId);
  glVertexAttribPointer(positionAttribId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // provide the vertex texture coordinates to the shaders
  glBindBuffer(GL_ARRAY_BUFFER, textureCoords_vbo);
  glEnableVertexAttribArray(textureCoordsAttribId);
  glVertexAttribPointer(textureCoordsAttribId, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  // provide the vertex normals to the shaders
  glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
  glEnableVertexAttribArray(normalAttribId);
  glVertexAttribPointer(normalAttribId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// draw the triangles
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, (void*)0);

	// disable the attribute arrays
  glDisableVertexAttribArray(positionAttribId);
  glDisableVertexAttribArray(textureCoordsAttribId);
  glDisableVertexAttribArray(normalAttribId);
}

static void reshape(int width, int height) {
    float aspectRatio = (float)width / (float)height;
    projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

    // if using perpsective projection, update projection matrix
    glViewport(0, 0, width, height);
}

static void drag(int x, int y) {
}

static void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    }
}

static void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {
      glutDestroyWindow(windowId);
      exit(0);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(800, 600);
    windowId = glutCreateWindow("Lab 03");
    glutIdleFunc(&update);
    glutDisplayFunc(&render);
    glutReshapeFunc(&reshape);
    glutMotionFunc(&drag);
    glutMouseFunc(&mouse);
    glutKeyboardFunc(&keyboard);

    glewInit();
    if (!GLEW_VERSION_2_0) {
        std::cerr << "OpenGL 2.0 not available" << std::endl;
        return 1;
    }
    std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
		std::cout << "Using OpenGL " << glGetString(GL_VERSION) << std::endl;

    createGeometry();
    pendulumSpring.setDisplacement(2.5f);
    pendulumSpring.setVelocity(0.0f);
    pendulumSpring.setSpringConstant(5.0f);
    programId = createShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");

    // create the view matrix (position and orient the camera)
    viewMatrix = glm::lookAt(
        glm::vec3(0,0,25), // eye/camera location
        glm::vec3(0,0,0),    // where to look
        glm::vec3(0,1,0)     // up
    );

    glutMainLoop();

    return 0;
}

static GLuint createShader(const GLenum shaderType, const std::string shaderFilename) {
  // load the shader source code
  std::ifstream fileIn(shaderFilename.c_str());

	if (!fileIn.is_open()) {
		return -1;
	}

	std::string shaderSource;
	std::string line;
	while (getline(fileIn, line)) {
		shaderSource.append(line);
		shaderSource.append("\n");
	}

	const char* sourceCode = shaderSource.c_str();

	// create a shader with the specified source code
	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &sourceCode, nullptr);

	// compile the shader
	glCompileShader(shaderId);

	// check if there were any compilation errors
	int result;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int errorLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &errorLength);
		char *errorMessage = new char[errorLength];

		glGetShaderInfoLog(shaderId, errorLength, &errorLength, errorMessage);
		std::cout << "Shader compilation failed: " << errorMessage << std::endl;

		delete[] errorMessage;

		glDeleteShader(shaderId);

		return 0;
	}

	return shaderId;
}

static GLuint createShaderProgram(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename) {
	// create and compile a shader for each
	GLuint vShaderId = createShader(GL_VERTEX_SHADER, vertexShaderFilename);
	GLuint fShaderId = createShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);

	// create and link the shaders into a program
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vShaderId);
	glAttachShader(programId, fShaderId);
	glLinkProgram(programId);
	glValidateProgram(programId);

	// delete the shaders
	glDetachShader(programId, vShaderId);
	glDetachShader(programId, fShaderId);
	glDeleteShader(vShaderId);
	glDeleteShader(fShaderId);

	return programId;
}
