#include <time.h>
#include <iostream>

#include "spring.h"

Spring::Spring() {
  // initial state, by default, is unmoving and undisplaced
  this->state.x = 0.0f;
  this->state.v = 0.0f;

  this->t = 0.0f;
}

float Spring::acceleration(spring_state state, float t) {
   float b = 0.1f;
   return -this->k * state.x - b * state.v;
}

spring_deriv Spring::evaluate(spring_state initial,
                              float t,
                              float dt,
                              spring_deriv d) {
   // calculate new displacement and velocity
   spring_state state;
   state.x = initial.x + d.dx * dt;
   state.v = initial.v + d.dv * dt;

   // calculate the new estimates for acceleration and velocity
   spring_deriv result;
   result.dx = state.v;
   result.dv = acceleration(state, t + dt);

   return result;
}

spring_state Spring::integrate(spring_state state,
                              float t,
                              float dt) {
   // calculate the 4 k values
   spring_deriv init;
   init.dx = 0;
   init.dv = 0;

   spring_deriv k1 = evaluate(state, t, 0.0f, init);
   spring_deriv k2 = evaluate(state, t, dt * 0.5f, k1);
   spring_deriv k3 = evaluate(state, t, dt * 0.5f, k2);
   spring_deriv k4 = evaluate(state, t, dt, k3);

   float dxdt = 1.0f/6.0f * (k1.dx + 2 * k2.dx + 2 * k3.dx + k4.dx);
   float dvdt = 1.0f/6.0f * (k1.dv + 2 * k2.dv + 2 * k3.dv + k4.dv);

   spring_state result;
   result.x = state.x + dxdt * dt;
   result.v = state.v + dvdt * dt;

   return result;
}
void Spring::update(float deltaT) {
  this->state = integrate(this->state, this->t, deltaT);
  this->t += deltaT;
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
