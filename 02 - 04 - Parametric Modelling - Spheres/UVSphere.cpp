#include "UVSphere.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>

void UVSphere::save(const std::string filename) {

  // TODO:  Generate vertex positions, texture coordinates, and surface getNormals

  // TODO:  Save all geometry to the .obj file

}

glm::vec3* UVSphere::getPositions() { return this->positions.data(); }
glm::vec2* UVSphere::getTextureCoords() { return this->textureCoords.data(); }
glm::vec3* UVSphere::getNormals() { return this->normals.data(); }
unsigned int UVSphere::getNumVertices() { return this->numVertices; }
unsigned int UVSphere::getNumTriangles() { return this->numTriangles; }
unsigned int* UVSphere::getTriangleIndices() { return this->triangleIndices; }
float UVSphere::getRadius() { return this->radius; }
unsigned int UVSphere::getNumVerticalSegments() { return this->numVerticalSegments; }
unsigned int UVSphere::getNumHorizontalSegments() { return this->numHorizontalSegments; }
