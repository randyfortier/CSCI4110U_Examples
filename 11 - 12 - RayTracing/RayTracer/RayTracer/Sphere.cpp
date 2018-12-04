#include <cmath>
#include <iostream>

#include "Sphere.h"
#include "Util.h"

float Sphere::intersectionPoint(Ray ray) {
	Vector3 oc = ray.origin - this->centre;
	float a = ray.direction.dot(ray.direction);
	float b = 2 * ray.direction.dot(oc);
	float c = oc.dot(oc) - (this->radius * this->radius);

	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		// does not hit the sphere
		return NAN;
	}
	else {
		return (-b - sqrt(discriminant)) / (2 * a);
	}
}

Colour Sphere::calculateShading(Vector3 lightPos, Colour lightColour, Ray ray, float t) {
	// determine the intersection point
	Vector3 pointOnSurface = ray.origin + (ray.direction * t);

	// generate surface normal (N)
	Vector3 surfaceNormal = pointOnSurface - this->centre;
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
