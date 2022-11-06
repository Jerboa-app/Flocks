#include <ParticleSystem/predator/predator.h>

void Predator::initialiseGL(){
  glGenVertexArrays(1,&vertVAO);
  glGenBuffers(1,&vertVBO);
  glBindVertexArray(vertVAO);
  glBindBuffer(GL_ARRAY_BUFFER,vertVBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);

  glError("initialised buffers");

  shader = glCreateProgram();
  compileShader(shader,predatorVertexShader,predatorFragmentShader);
  glUseProgram(shader);

  glUniformMatrix4fv(
    glGetUniformLocation(shader,"proj"),
    1,
    GL_FALSE,
    &projection[0][0]
  );

  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);

  glError();
  glBufferStatus();

}

void Predator::setProjection(glm::mat4 p){
  projection = p;
  glUseProgram(shader);
  glUniformMatrix4fv(
    glGetUniformLocation(shader,"proj"),
    1,
    GL_FALSE,
    &projection[0][0]
  );

}

void Predator::draw(
  uint64_t frameId,
  float zoomLevel,
  float resX,
  float resY
){
  glUseProgram(shader);

  glUniform1f(
    glGetUniformLocation(shader,"zoom"),
    zoomLevel
  );

  glUniform1f(
    glGetUniformLocation(shader,"scale"),
    resX*2.0
  );

  glBindBuffer(GL_ARRAY_BUFFER,vertVBO);
  glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*4,&vertices[0]);
  glBindBuffer(GL_ARRAY_BUFFER,0);

  glBindVertexArray(vertVAO);
  glDrawArraysInstanced(GL_POINTS,0,1,1);
  glBindVertexArray(0);

  glError("draw predator");

}

void Predator::step(double dt){

    double cc = drag*dt/2.0;
    double dtdt = dt*dt;
    double ct = cc/mass;
    double bt = 1.0 / (1.0 + ct);
    double at = (1.0-ct)*bt;

    double cr = (rotationalDrag*dt)/(2.0*momentOfInertia);
    double br = 1.0 / (1.0 + cr);
    double ar = (1.0-cr)*br;

    double ax = drag*v0*cos(theta);
    double ay = drag*v0*sin(theta);
    double atheta = torque;

    double xn = 2.0*bt*x - at*xp + (bt*dtdt/mass)*ax;
    double yn = 2.0*bt*y - at*yp + (bt*dtdt/mass)*ay;
    double thetan = 2.0*br*theta - ar*thetap + (br*dtdt/momentOfInertia)*atheta;

    vx = x-xp;
    vy = y-yp;

    xp = x;
    yp = y;
    thetap = theta;
    torque = 0.0;

    x = xn;
    y = yn;
    theta = thetan;

    double ux = 0.0; double uy = 0.0;
    double newX = x; double newY = y;
    double ang = theta;
    bool flag = false;

    if (x-radius < 0 || x+radius > Lx){
      ux = -0.9*vx;
      ang = std::atan2(vy,ux);

      if (x-radius < 0){
        newX = radius;
      }
      else{
        newX = Lx-radius;
      }

      flag = true;
    }

    if (y-radius < 0 || y+radius > Ly){
      uy = -0.5*vy;
      if (flag){
        ang = std::atan2(uy,ux);
      }
      else{
        ang = std::atan2(uy,vx);
        flag = true;
      }
      if (y-radius < 0){
        newY = radius;
      }
      else{
        newY = Ly-radius;
      }
    }

    if (flag){
      theta = ang;
      y = newY+uy;
      x = newX+ux;
      thetap = theta;
    }

    vertices[0] = x;
    vertices[1] = y;
    vertices[2] = theta;
    vertices[3] = radius;
}