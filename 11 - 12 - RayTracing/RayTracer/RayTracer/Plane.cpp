#include <cmath>
#include <iostream>

#include "Plane.h"
#include "Util.h"

float Plane::intersectionPoint(Ray ray) {
	float denominator = ray.direction.dot(this->surfaceNormal);

	if (abs(denominator) > 0.00001f) {
		Vector3 oc = this->origin - ray.origin;
		float numerator = oc.dot(this->surfaceNormal);
		return numerator / denominator;
	}
	else {
		return -1.0f;
	}
}

Colour Plane::calculateShading(Vector3 lightPos, Colour lightColour, Ray ray, float t) {
	// determine the intersection point
	Vector3 pointOnSurface = ray.origin + (ray.direction * t);

	// generate surface normal (N)
	Vector3 surfaceNormal = this->surfaceNormal;
	surfaceNormal.normalize();

	// generate a light path vector (L)
	Vector3 lightPath = lightPos - pointOnSurface;
	lightPath.normalize();

	// generate the eye path vector (V)
	Vector3 eyePath = ray.origin - pointOnSurface;
	eyePath.normalize();

	// diffuse
	float diffuseCoefficient = clamp(surfaceNormal.dot(lightPath));
	Colour diffuseColour = this->material.colour * diffuseCoefficient;

	// reflection
	Vector3 reflection = lightPath + surfaceNormal * -2 * surfaceNormal.dot(lightPath);
	reflection.normalize();

	// specular (Blinn-Phong)
	Vector3 h = lightPath + eyePath;
	h.normalize();
	float z = max(0, h.dot(surfaceNormal));
	float specularPower = (8 + this->material.shininess) / (8 * PI);
	Colour specularColour = lightColour * this->material.specularCoefficient * specularPower * pow(z, this->material.shininess);

	// ambient
	Colour ambientColour = this->material.colour * ambientCoefficient;

	return ambientColour + diffuseColour + specularColour;
}
