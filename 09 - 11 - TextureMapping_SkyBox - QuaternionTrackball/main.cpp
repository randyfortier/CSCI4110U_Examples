#include <string>
#include <iostream>
#include <fstream>
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
#include <glm/gtc/quaternion.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "apis/stb_image.h"

#include "ShaderProgram.h"
#include "ObjMesh.h"

#define SCALE_FACTOR 2.0f

int width = 1024;
int height = 768;

GLuint programId;
GLuint skyboxProgramId;

GLuint vertexBuffer;
GLuint indexBuffer;
GLenum positionBufferId;
GLuint positions_vbo = 0;
GLuint textureCoords_vbo = 0;
GLuint normals_vbo = 0;
GLuint colours_vbo = 0;
GLuint skybox_vbo = 0;

unsigned int skyboxTexture;

unsigned int numVertices;

float angle = 0.0f;
float lightOffsetY = 0.0f;

float theta = -0.4f;
float phi = -1.0f;
float cameraRadius = 100.0f;

bool animateLight = true;
bool rotateObject = true;

glm::vec3 eyePosition;
bool areScaling = false;
float scaleFactor = 25.0f;
float lastX = std::numeric_limits<float>::infinity();
float lastY = std::numeric_limits<float>::infinity();
glm::quat rotation = glm::angleAxis(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

static void createTexture(std::string filename);
static unsigned int createCubemap(std::vector<std::string> filenames);

static void createSkybox(void) {
  float skyboxPositions[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };
  unsigned int numVertices = 36;

  glGenBuffers(1, &skybox_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(float) * 3, skyboxPositions, GL_STATIC_DRAW);
}

static void createGeometry(void) {
  ObjMesh mesh;
  mesh.load("meshes/dragon.obj", true, true);

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
}

static void update(void) {
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);

    // rotate the shape about the y-axis so that we can see the shading
    if (rotateObject) {
      float degrees = (float)milliseconds / 80.0f;
      angle = degrees;
    }

    // move the light position over time along the x-axis, so we can see how it affects the shading
    if (animateLight) {
      float t = milliseconds / 1000.0f;
      lightOffsetY = sinf(t) * 100;
    }

    glutPostRedisplay();
}

glm::mat4 model;

static void render(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // activate our shader program
	glUseProgram(programId);

  // turn on depth buffering
  glEnable(GL_DEPTH_TEST);

  // projection matrix - perspective projection
  // FOV:           45°
  // Aspect ratio:  4:3 ratio
  // Z range:       between 0.1 and 100.0
  float aspectRatio = (float)width / (float)height;
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

  // projection matrix - orthographic (non-perspective) projection
  // Note:  These are in world coordinates
  // xMin:          -10
  // xMax:          +10
  // yMin:          -10
  // yMax:          +10
  // zMin:           0
  // zMax:          +100
  //glm::mat4 projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f);

  // view matrix - orient everything around our preferred view
  glm::mat4 view = glm::lookAt(
      eyePosition,
      glm::vec3(0,0,0),    // where to look
      glm::vec3(0,1,0)     // up
  );

  model = glm::mat4(1.0f);

  glm::mat4 vMatrix = glm::mat4(1.0f);
  vMatrix = glm::translate(vMatrix, glm::vec3(-eyePosition.x, -eyePosition.y, -eyePosition.z));
  vMatrix = glm::scale(vMatrix, glm::vec3(1.0, 1.0, 1.0));

  // draw the cube map sky box

  // provide the vertex positions to the shaders
  GLint skyboxPositionAttribId = glGetAttribLocation(skyboxProgramId, "position");
  glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  glEnableVertexAttribArray(skyboxPositionAttribId);
  glVertexAttribPointer(skyboxPositionAttribId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // texture sampler - a reference to the texture we've previously created
  GLuint skyboxTextureId  = glGetUniformLocation(skyboxProgramId, "u_TextureSampler");
  glActiveTexture(GL_TEXTURE0);  // texture unit 0
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
  glUniform1i(skyboxTextureId, 0);

	glUseProgram(skyboxProgramId);

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glFrontFace(GL_CCW);
  glDisable(GL_CULL_FACE);

  // set model-view matrix

  GLuint skyboxMVMatrixId = glGetUniformLocation(skyboxProgramId, "u_MVMatrix");
  glUniformMatrix4fv(skyboxMVMatrixId, 1, GL_FALSE, &view[0][0]);

  // set projection matrix
  GLuint skyboxProjMatrixId = glGetUniformLocation(skyboxProgramId, "u_PMatrix");
  glUniformMatrix4fv(skyboxProjMatrixId, 1, GL_FALSE, &projection[0][0]);

  glBindVertexArray(skyboxPositionAttribId);
  glDrawArrays(GL_TRIANGLES, 0, 36);

	// disable the attribute array
  glDisableVertexAttribArray(skyboxPositionAttribId);

  // draw the subject

  glUseProgram(programId);

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  // model matrix: translate, scale, and rotate the model
  //glm::vec3 rotationAxis(0,1,0);
  model = glm::mat4(1.0f);
  model = glm::mat4_cast(rotation);
  //model = glm::translate(model, glm::vec3(-6.0f, -2.0f, 1.0));
  //model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0)); // rotate about the y-axis
  model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

  // model-view-projection matrix
  glm::mat4 mvp = projection * view * model;
  GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
  glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

  // model-view matrix
  //GLuint mvMatrixId = glGetUniformLocation(programId, "u_MVMatrix");
  //glUniformMatrix4fv(mvMatrixId, 1, GL_FALSE, &vMatrix[0][0]);

  // model matrix
  GLuint mMatrixId = glGetUniformLocation(programId, "u_ModelMatrix");
  glUniformMatrix4fv(mMatrixId, 1, GL_FALSE, &model[0][0]);

  // set projection matrix
  //GLuint projMatrixId = glGetUniformLocation(programId, "u_PMatrix");
  //glUniformMatrix4fv(projMatrixId, 1, GL_FALSE, &projection[0][0]);

  GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
  glActiveTexture(GL_TEXTURE0);  // texture unit 0
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
  glUniform1i(textureId, 0);

  // the position of our camera/eye
  GLuint eyePosId = glGetUniformLocation(programId, "u_EyePosition");
  glUniform3f(eyePosId, eyePosition.x, eyePosition.y, eyePosition.z);

  // the position of our light
  GLuint lightPosId = glGetUniformLocation(programId, "u_LightPos");
  glUniform3f(lightPosId, 1, 8 + lightOffsetY, -2);

  // the colour of our object
  GLuint diffuseColourId = glGetUniformLocation(programId, "u_DiffuseColour");
  glUniform4f(diffuseColourId, 0.3, 0.2, 0.8, 1.0);

  // the shininess of the object's surface
  GLuint shininessId = glGetUniformLocation(programId, "u_Shininess");
  glUniform1f(shininessId, 25);

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

  // for testing purposes
  //glutSolidTorus(0.5f, 1.5f, 12, 10);

	// disable the attribute arrays
  glDisableVertexAttribArray(positionAttribId);
  glDisableVertexAttribArray(textureCoordsAttribId);
  glDisableVertexAttribArray(normalAttribId);

	// make the draw buffer to display buffer (i.e. display what we have drawn)
	glutSwapBuffers();
}

static void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    width = w;
    height = h;
}

glm::vec3 get_trackball_vector(int x, int y) {
  glm::vec3 P = glm::vec3(1.0 * x / width * 2 - 1.0,
			                    1.0 * y / height * 2 - 1.0,
			                    0);
  P.y = -P.y;
  float OP_squared = P.x * P.x + P.y * P.y;
  if (OP_squared <= 1 * 1)
    P.z = sqrt(1 * 1 - OP_squared);  // Pythagoras
  else
    P = glm::normalize(P);  // nearest point
  return P;
}

static void drag(int x, int y) {
  if (!isinf(lastX) && !isinf(lastY)) {
    float dx = lastX - (float)x;
    float dy = lastY - (float)y;

    if (areScaling) {
      // scale in/out (right button drag)
      float distance = sqrt(dx * dx + dy * dy);

      /*
      if (dy > 0.0f) {
        scaleFactor = SCALE_FACTOR / distance;
      } else {
        scaleFactor = distance / SCALE_FACTOR;
      }
      */
      scaleFactor = distance / SCALE_FACTOR;

      float factor = 1.0f + (float)(m_zoomEnd.y - m_zoomStart.y) * m_zoomSpeed;

  		if (factor != 1.0f && factor > 0.0f) {
  			m_eye = m_eye * (float)factor;

  			if (m_staticMoving) {
  				m_zoomStart = m_zoomEnd;
  			} else {
  				m_zoomStart.y += (float)(m_zoomEnd.y - m_zoomStart.y) * m_dynamicDampingFactor;
  			}

      }
    } else {
      // trackball rotation (left button drag)
      glm::vec3 a = get_trackball_vector(lastX, lastY);
      glm::vec3 b = get_trackball_vector(x, y);

      float rotateSpeed = 1.0f;
      float angle = (float)acos(glm::dot(a, b) / glm::length(a) / glm::length(b));

		  if (!isnan(angle)) {
  			glm::vec3 axis = glm::normalize(glm::cross(a, b));

  			if (glm::isnan(axis.x) || glm::isnan(axis.y) || glm::isnan(axis.z)) {
          return;
        }

  			angle *= rotateSpeed;
        glm::quat quaternion = glm::angleAxis(-angle, axis);

        glm::mat3 rotationMatrix = glm::mat4_cast(quaternion);
  			eyePosition = eyePosition * quaternion;

        lastX = (float)x;
        lastY = (float)y;
      }
    }
  } else {
    lastX = (float)x;
    lastY = (float)y;
  }
}

static void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    areScaling = false;
  } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    areScaling = true;
  } else if (state == GLUT_UP) {
    lastX = std::numeric_limits<float>::infinity();
    lastY = std::numeric_limits<float>::infinity();
  }
}

static void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'l':
      animateLight = !animateLight;
      break;
    case 'r':
      rotateObject = !rotateObject;
      break;
    }
}

int main(int argc, char** argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(width, height);
   glutCreateWindow("CSCI 4110U - Texture Mapping - SkyBox");
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

   // determine the initial camera position
   eyePosition.x = cameraRadius * sin(theta) * cos(phi);
   eyePosition.y = cameraRadius * sin(theta) * sin(phi);
   eyePosition.z = cameraRadius * cos(theta);

    // load the texture images
    //createTexture("textures/lagoon_rt.tga");

   std::vector<std::string> filenames;
   filenames.push_back("textures/cposx.png");
   filenames.push_back("textures/cnegx.png");
   filenames.push_back("textures/cposy.png");
   filenames.push_back("textures/cnegy.png");
   filenames.push_back("textures/cposz.png");
   filenames.push_back("textures/cnegz.png");
   /*
   filenames.push_back("textures/right.jpg");
   filenames.push_back("textures/left.jpg");
   filenames.push_back("textures/top.jpg");
   filenames.push_back("textures/bottom.jpg");
   filenames.push_back("textures/front.jpg");
   filenames.push_back("textures/back.jpg");
   */

   skyboxTexture = createCubemap(filenames);
   glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

   // create the scene geometry
   createSkybox();
   createGeometry();

   // test out quaternions
   glm::quat rot(1.0f, 0.0f, 0.0f, 0.0f);
   float sqrtHalf = sqrt(0.5f);
   glm::quat rotx(sqrtHalf, sqrtHalf, 0.0f, 0.0f);  // 90 degrees about x
   glm::quat rotz(sqrtHalf, 0.0f, 0.0f, sqrtHalf);  // 90 degrees about z

   rot *= rotx;
   rot *= rotz;


   // load the GLSL shader programs

   ShaderProgram program;
   program.loadShaders("shaders/combined_vertex.glsl", "shaders/combined_fragment.glsl");
   //program.loadShaders("shaders/reflection_vertex.glsl", "shaders/reflection_fragment.glsl");
   //program.loadShaders("shaders/refraction_vertex.glsl", "shaders/refraction_fragment.glsl");
   //program.loadShaders("shaders/copper_vertex.glsl", "shaders/copper_fragment.glsl");
  	programId = program.getProgramId();

   ShaderProgram skyboxProgram;
   skyboxProgram.loadShaders("shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl");
  	skyboxProgramId = skyboxProgram.getProgramId();

   glutMainLoop();

   return 0;
}

static unsigned int createCubemap(std::vector<std::string> filenames) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, numChannels;
    for (unsigned int i = 0; i < filenames.size(); i++) {
        unsigned char *data = stbi_load(filenames[i].c_str(), &width, &height, &numChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << filenames[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE0);

    return textureID;
}

static void createTexture(std::string filename) {
  int width, height, numChannels;
  unsigned char *bitmap = stbi_load(filename.c_str(), &width, &height, &numChannels, 4);

  GLuint textureId;
  glGenTextures(1, &textureId);

  // make this texture active
  glBindTexture(GL_TEXTURE_2D, textureId);

  // specify the functions to use when shrinking/enlarging the texture image
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // send the data to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);

  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);

  // we don't need the bitmap data any longer
  stbi_image_free(bitmap);
}
