#pragma once

struct spring_state {
  float x;
  float v;
};

struct spring_deriv {
  float dx;
  float dv;
};

class Spring {
private:
  unsigned int vertex1Index;
  unsigned int vertex2Index;

  float k;
  float restLength;
  spring_state state;

  float t;

  float acceleration(spring_state state, float t);
  spring_deriv evaluate(spring_state initial, float t, float dt, spring_deriv d);
  spring_state integrate(spring_state state, float t, float dt);

public:
	Spring();

  void update(float dt);

  unsigned int getVertex1Index();
  unsigned int getVertex2Index();
  void setVertex1Index(unsigned int index);
  void setVertex2Index(unsigned int index);

  void setRestLength(float restLength);
  float getDisplacement(); // +ve is toward vertex 1
  void setDisplacement(float x);
  float getVelocity(); // +ve is toward vertex 1
  void setVelocity(float v);

  void setSpringConstant(float springConstant);
};
