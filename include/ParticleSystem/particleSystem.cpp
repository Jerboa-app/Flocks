#include <ParticleSystem/particleSystem.h>
#include <time.h>

void ParticleSystem::resetLists(){
  for (int i = 0; i < Nc*Nc; i++){
    cells[i] = NULL_INDEX;
  }
  for (int i = 0; i < nParticles; i++){
    list[i] = NULL_INDEX;
  }
}

void ParticleSystem::insert(uint64_t next, uint64_t particle){
  uint64_t i = next;
  while (list[i] != NULL_INDEX){
    i = list[i];
  }
  list[i] = particle;
}

void ParticleSystem::populateLists(
){
  for (int i = 0; i < size(); i++){
    uint64_t c = hash(i);
    if (cells[c] == NULL_INDEX){
      cells[c] = uint64_t(i);
    }
    else{
      insert(cells[c],uint64_t(i));
    }
  }
}

void ParticleSystem::handleCollision(uint64_t i, uint64_t j){
  if (i == j){return;}
  double rx,ry,dd,d,ddot,mag,fx,fy,nx,ny,vx,vy,rc;
  rx = state[j*3]-state[i*3];
  ry = state[j*3+1]-state[i*3+1];
  dd = rx*rx+ry*ry;
  rc = parameters[i*2]+parameters[j*2];
  if (dd < rc*rc){
    d = std::sqrt(dd);

    nx = rx / d;
    ny = ry / d;

    d = rc-d;

    vx = velocities[i*2]-velocities[j*2];
    vy = velocities[i*2+1]-velocities[j*2+1];

    ddot = vx*nx+vy*ny;

    mag = -dampingCoefficient*ddot-restorationCoefficient*d;

    // Coefficient of restitution and linear–dashpot model revisited
    //  Thomas Schwager · Thorsten Pösche
    mag = std::min(0.0,mag);

    if (mag == 0){return;}

    fx = mag*nx;
    fy = mag*ny;

    forces[i*2] += fx;
    forces[i*2+1] += fy;

    forces[j*2] -= fx;
    forces[j*2+1] -= fy;
  }
}

void ParticleSystem::cellCollisions(
  uint64_t a1,
  uint64_t b1,
  uint64_t a2,
  uint64_t b2
){
  if (a1 < 0 || a1 >= Nc || b1 < 0 || b1 >= Nc || a2 < 0 || a2 >= Nc || b2 < 0 || b2 >= Nc){
    return;
  }
  uint64_t p1 = cells[a1*Nc+b1];
  uint64_t p2 = cells[a2*Nc+b2];

  if (p1 == NULL_INDEX || p2 == NULL_INDEX){
    return;
  }

  int a2NcPlusb2 = a2*Nc+b2;

  while (p1 != NULL_INDEX){
    p2 = cells[a2NcPlusb2];
    while(p2 != NULL_INDEX){
        handleCollision(p1,p2);
        p2 = list[p2];
    }
    p1 = list[p1];
  }
}

double ParticleSystem::orderParameter(){
  double phi = 0.0;
  double x = 0.0;
  double y = 0.0;
  for (int i = 0; i < size(); i++){
    double theta = state[i*3+2];
    x += std::cos(theta);
    y += std::sin(theta);
  }

  x /= size();
  y /= size();

  return std::sqrt(x*x+y*y);
}

void ParticleSystem::applyForce(double fx, double fy){
  for (int i = 0; i < size(); i++){
    forces[i*2] += fx;
    forces[i*2+1] += fy;
  }
}

void ParticleSystem::setCoeffientOfRestitution(double c){

  if (coefficientOfRestitution == c){
    return;
  }

  coefficientOfRestitution = c;

  dampingCoefficient = damping(mass,mass);
  restorationCoefficient = restoration(mass,mass);

}

void ParticleSystem::newTimeStepStates(double oldDt, double newDt){
  for (int i = 0; i < size(); i++){
    for (int k = 0; k < 3; k++){
      double delta = state[i*3+k]-lastState[i*3+k];
      lastState[i*3+k] = state[i*3+k]-(newDt/oldDt)*delta;
    }
  }
}

void ParticleSystem::step(){
  clock_t tic = clock();
  resetLists();
  populateLists();
  float setup = (clock()-tic)/float(CLOCKS_PER_SEC);
  tic = clock();

  for (int a = 0; a < Nc; a++){
    for (int b = 0; b < Nc; b++){

      // draw it out, we can get away without
      //  checking some cells! Left commented here for
      //  understanding
      cellCollisions(a,b,a,b);
      //cellCollisions(a,b,a-1,b-1);
      //cellCollisions(a,b,a-1,b+1);
      cellCollisions(a,b,a+1,b+1);
      cellCollisions(a,b,a+1,b-1);
      //cellCollisions(a,b,a-1,b);
      cellCollisions(a,b,a+1,b);
      //cellCollisions(a,b,a,b-1);
      cellCollisions(a,b,a,b+1);

    }
  }

  float col = (clock()-tic)/float(CLOCKS_PER_SEC);
  tic = clock();

  double cc = drag*dt/2.0;
  double D = std::sqrt(2.0*rotationalDiffusion/dt);

  for (int i = 0; i < size(); i++){

    double nx = 0.0;
    double ny = 0.0;
    double dtheta = 0.0;
    if (
      (repelDistance > 0 && repelStrength > 0) ||
      (alignDistance > 0 && alignStrength > 0) ||
      (attractDistance > 0 && attractStrength > 0)
      
      ){
      for (int j = i+1; j < size(); j++){
        double rx = state[j*3]-state[i*3];
        double ry = state[j*3+1]-state[i*3+1];
        double d2 = rx*rx+ry*ry;
        if (d2 == 0){continue;}
        if (repelDistance > 0.0 && d2 < repelDistance){
          // repel
          double d = sqrt(d2);
          nx -= repelStrength*rx/d;
          ny -= repelStrength*ry/d;
        } 
        else if (alignDistance > 0.0 && d2 < alignDistance){
          // align
          double vjx = velocities[j*2];
          double vjy = velocities[j*2+1];
          double v = std::sqrt(vjx*vjx+vjy*vjy);
          if (v==0){continue;}
          nx += alignStrength*vjx/v;
          ny += alignStrength*vjy/v;
        }
        else if (attractDistance > 0.0 && d2 < attractDistance){
          //attract
          double d = sqrt(d2);
          nx += attractStrength*rx/d;
          ny += attractStrength*ry/d;
        }
      }

      dtheta = std::atan2(ny,nx);

      if (dtheta < 0.0){
        dtheta = std::abs(dtheta)+M_PI;
      }

    }
    
    noise[i*2+1] = noise[i*2];
    noise[i*2] = normal(generator);

    double ct = cc/parameters[i*2+1];
    double bt = 1.0 / (1.0 + ct);
    double at = (1.0-ct)*bt;

    double cr = (rotationalDrag*dt)/(2.0*momentOfInertia);
    double br = 1.0 / (1.0 + cr);
    double ar = (1.0-cr)*br;

    double x = state[i*3];
    double y = state[i*3+1];
    double theta = state[i*3+2];

    double xp = lastState[i*3];
    double yp = lastState[i*3+1];
    double thetap = lastState[i*3+2];

    double ax = drag*speed*cos(theta)+forces[i*2];
    double ay = drag*speed*sin(theta)+forces[i*2+1];
    double atheta;

    if (nx == 0 && ny == 0){
      atheta = 0.0;
    }
    else{
      double phi = std::fmod(theta,2.0*M_PI);
      if (phi < 0.0){phi += 2.0*M_PI;}
      atheta = rotationalDrag*(dtheta-phi);
    }

    state[i*3] = 2.0*bt*x - at*xp + (bt*dtdt/parameters[i*2+1])*ax;
    state[i*3+1] = 2.0*bt*y - at*yp + (bt*dtdt/parameters[i*2+1])*ay;
    state[i*3+2] = 2.0*br*theta - ar*thetap + (br*dtdt/momentOfInertia)*atheta + (br*dt/(2.0*momentOfInertia))*(noise[i*2]+noise[i*2+1])*dt*rotationalDrag*D;

    lastState[i*3] = x;
    lastState[i*3+1] = y;
    lastState[i*3+2] = theta;

    double vx = state[i*3]-lastState[i*3];
    double vy = state[i*3+1]-lastState[i*3+1];

    velocities[i*2] = vx/dt;
    velocities[i*2+1] = vy/dt;

    double ux = 0.0; double uy = 0.0;
    double newX = state[i*3]; double newY = state[i*3+1];
    double ang = state[i*3+2];
    bool flag = false;

    // kill the particles movement if it's outside the box
    if (state[i*3]-parameters[2*i] < 0 || state[i*3]+parameters[2*i] > Lx){
      ux = -0.9*vx;
      ang = std::atan2(vy,ux);

      if (state[i*3]-parameters[2*i] < 0){
        newX = parameters[2*i];
      }
      else{
        newX = Lx-parameters[2*i];
      }

      flag = true;
    }

    if (state[i*3+1]-parameters[2*i] < 0 || state[i*3+1]+parameters[2*i] > Ly){
      uy = -0.5*vy;
      if (flag){
        ang = std::atan2(uy,ux);
      }
      else{
        ang = std::atan2(uy,vx);
        flag = true;
      }
      if (state[i*3+1]-parameters[2*i] < 0){
        newY = parameters[2*i];
      }
      else{
        newY = Ly-parameters[2*i];
      }
    }

    if (flag){
      state[i*3+2] = ang;
      state[i*3+1] = newY+uy;
      state[i*3] = newX+ux;
      lastState[i*3+2] = state[i*3+2];
    }
  }

  for (int i = 0; i < size(); i++){
    forces[i*2] = 0.0;
    forces[i*2+1] = 0.0;

    for (int k = 0; k < 3; k++){floatState[i*4+k] = float(state[i*3+k]);}
  }

  double updates = (clock()-tic)/double(CLOCKS_PER_SEC);
  tic = clock();
}

void ParticleSystem::addParticle(){
  int i = size();

  if (i == nParticles){return;}

  double x = U(generator)*(Lx-2*radius)+radius;
  double y = U(generator)*(Ly-2*radius)+radius;
  double theta = U(generator)*2.0*3.14;

  double r = radius;
  double m = mass;

  addParticle(x,y,theta,r,m);
}

void ParticleSystem::addParticle(
  double x,
  double y,
  double theta,
  double r,
  double m
){

  int i = size();

  state.push_back(x);
  state.push_back(y);
  state.push_back(theta);

  floatState[i*4] = x;
  floatState[i*4+1] = y;
  floatState[i*4+2] = theta;
  floatState[i*4+3] = r;

  lastState.push_back(x);
  lastState.push_back(y);
  lastState.push_back(theta);

  parameters.push_back(r);
  parameters.push_back(m);

  forces.push_back(0.0);
  forces.push_back(0.0);

  interactions.push_back(0.0);
  interactions.push_back(0.0);

  velocities.push_back(0.0);
  velocities.push_back(0.0);

  noise.push_back(0.0);
  noise.push_back(0.0);
}

void ParticleSystem::removeParticle(uint64_t i){
  if (state.size() >= 3*i){
    state.erase(
      state.begin()+3*i,
      state.begin()+3*i+3
    );

    lastState.erase(
      lastState.begin()+3*i,
      lastState.begin()+3*i+3
    );

    parameters.erase(
      parameters.begin()+2*i,
      parameters.begin()+2*i+2
    );

    forces.erase(
      forces.begin()+2*i,
      forces.begin()+2*i+2
    );

    interactions.erase(
      interactions.begin()+2*i,
      interactions.begin()+2*i+2
    );


    velocities.erase(
      velocities.begin()+2*i,
      velocities.begin()+2*i+2
    );

    noise.erase(
      noise.begin()+2*i,
      noise.begin()+2*i+2
    );
  }
}