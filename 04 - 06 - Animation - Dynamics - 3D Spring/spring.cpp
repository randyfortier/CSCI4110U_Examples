#include <time.h>
#include <iostream>

#include "spring.h"

Spring::Spring() {
  // initial state, by default, is unmoving and undisplaced
  this->state.x = 0.0f;
  this->state.v = 0.0f;
}

void Spring::update(float deltaT) {
  // TODO:  RK4
}

void Spring::setRestLength(float restLength) {
  this->restLength = restLength;
}

void Spring::setVertex1Index(unsigned int index) {
  this->vertex1Index = index;
}

void Spring::setVertex2Index(unsigned int index) {
  this->vertex2Index = index;
}

unsigned int Spring::getVertex1Index() {
  return this->vertex1Index;
}

unsigned int Spring::getVertex2Index() {
  return this->vertex2Index;
}

void Spring::setSpringConstant(float springConstant) {
  this->k = springConstant;
}

float Spring::getDisplacement() {
  return this->state.x;
}

float Spring::getVelocity() {
  return this->state.v;
}

void Spring::setVelocity(float v) {
  this->state.v = v;
}

void Spring::setDisplacement(float x) {
  this->state.x = x;
}
