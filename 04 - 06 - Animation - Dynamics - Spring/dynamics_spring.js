var anchorPoint = {x: 300, y: 50};

var state = {x: 1.5, v: 0.0};
var springConstant = 5; // N/m

var deltaT = 0.01;
var t = 0.0;

var speed = 10;
var animating = false;

var surface;
var ctx;
var id;

function acceleration(state, t) {
  var b = 0.1;            // damping constant
  var k = springConstant; // to look similar to Hooke's law

  return -k * state.x - b * state.v;
}

function evaluate(initial, t, dt, d) {
  var state = {
    x: initial.x + d.dx * dt,
    v: initial.v + d.dv * dt
  };

  return {
    dx: state.v,
    dv: acceleration(state, t + dt)
  };
}

// RK4
function integrate(state, t, dt) {
  // calculate the k values
  k1 = evaluate(state, t, 0.0, {dx: 0, dv: 0});
  k2 = evaluate(state, t, dt * 0.5, k1);
  k3 = evaluate(state, t, dt * 0.5, k2);
  k4 = evaluate(state, t, dt, k3);

  // calculate the slopes of the predictor lines (T4)
  dxdt = 1.0/6.0 * (k1.dx + 2*k2.dx + 2*k3.dx + k4.dx);
  dvdt = 1.0/6.0 * (k1.dv + 2*k2.dv + 2*k3.dv + k4.dv);

  // calculate the new position and velocity
  return {
    x: state.x + dxdt * dt,
    v: state.v + dvdt * dt
  };
}

function update() {
  // update velocity and position
  state = integrate(state, t, deltaT);
  t += deltaT;

  console.log('pendulumState.x = ', state.x);
  console.log('pendulumState.v = ', state.v);

  redraw();
}

function drawLine(p1, p2, colour) {
  ctx.lineWidth = 4;
  ctx.beginPath();
  ctx.moveTo(p1.x, p1.y);
  ctx.lineTo(p2.x, p2.y);
  ctx.strokeStyle = colour;
  ctx.stroke();
}

function drawPoint(p, colour) {
  var radius = 5;

  ctx.beginPath();
  ctx.arc(p.x, p.y, radius, 0, 2 * Math.PI, false);
  ctx.fillStyle = colour;
  ctx.fill();
}

function clearScreen(colour) {
  ctx.fillStyle = colour;
  ctx.fillRect(0, 0, 600, 600);
}

function lengthToPixels(length) {
  // entire screen (600px) is 10m
  // spring rest length is 2.0m
  return Math.round(((length + 2.0) / 10.0) * 600);
}

function redraw() {
  // clear the screen
  clearScreen('#FFFFFF');

  // calculate the pendulum point
  var pendulumPoint = {x: 300, y: anchorPoint.y + lengthToPixels(state.x)};

  // draw the lines between the control points
  drawLine(anchorPoint, pendulumPoint, '#000000');

  // draw both points
  drawPoint(anchorPoint, '#A05050');
  drawPoint(pendulumPoint, '#A05050');
}

function updateSpringConstant() {
  var springConstantSlider = document.getElementById('spring_constant');
  springConstant = springConstantSlider.value;
}

function stopAnimation() {
  animating = false;
  clearInterval(id);
}

function startAnimation() {
  animating = true;
  clearInterval(id);
  id = setInterval(update, speed);
}

window.onload = function() {
  surface = document.getElementById("surface");
  ctx = surface.getContext("2d");

  var springConstantSlider = document.getElementById('spring_constant');
  springConstantSlider.oninput = function() {
    updateSpringConstant();
  };

  stopAnimation();
  var stopGoButton = document.getElementById('stopgo');
  stopGoButton.onclick = function() {
    if (stopGoButton.textContent === 'Stop') {
      stopAnimation();
      stopGoButton.textContent = 'Go';
    } else {
      startAnimation();
      stopGoButton.textContent = 'Stop';
    }
  };
};
