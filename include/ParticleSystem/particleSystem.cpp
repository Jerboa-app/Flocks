#include <ParticleSystem/particleSystem.h>
#include <ParticleSystem/predator/predator.h>
#include <time.h>

const double MAX_PARTICLE_SPEED = MAX_PREDATOR_SPEED*40.0;
const double MAX_INTERACTION_RANGE = 100.0;

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

  if (periodic){
    if (rx > Lx*0.5){
      rx -= Lx;
    }
    else if (rx <= -Lx*0.5){
      rx += Lx;
    }
    if (ry > Ly*0.5){
      ry -= Ly;
    }
    else if (ry <= -Ly*0.5){
      ry += Ly;
    }
  }

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
  if (!periodic){
    if (a1 < 0 || a1 >= Nc || b1 < 0 || b1 >= Nc || a2 < 0 || a2 >= Nc || b2 < 0 || b2 >= Nc){
      return;
    }
  }
  else{
    a1 = a1 % Nc;
    a2 = a2 % Nc;
    b1 = b1 % Nc;
    b2 = b2 % Nc;
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

void ParticleSystem::metricInteractions(uint64_t i, double & nx, double & ny){
    double rd = repelDistance*repelDistance;
    double ra = alignDistance*alignDistance;
    double rat = attractDistance*attractDistance;

    uint32_t nr = 0;
    uint32_t nal = 0;
    uint32_t nat = 0;

    nx = std::cos(state[i*3+2]);
    ny = std::sin(state[i*3+2]);
    
    double normR = 0.0;
    double normAl = 0.0;
    double normAt = 0.0;

    double ctheta = std::cos(state[i*3+2]);
    double stheta = std::sin(state[i*3+2]);
    double cang = M_PI-blindAngle;
    if (
      (repelDistance > 0) ||
      (alignDistance > 0) ||
      (attractDistance > 0)
    ){
      for (int j = 0; j < size(); j++){
        if (j==i){continue;}
        double rx = state[j*3]-state[i*3];
        double ry = state[j*3+1]-state[i*3+1];

        if (periodic){
          if (rx > Lx*0.5){
            rx -= Lx;
          }
          else if (rx <= -Lx*0.5){
            rx += Lx;
          }
          if (ry > Ly*0.5){
            ry -= Ly;
          }
          else if (ry <= -Ly*0.5){
            ry += Ly;
          }
        }

        double d2 = rx*rx+ry*ry;
        if (d2 == 0){continue;}

        if (!vicsek && repelDistance > 0.0 && d2 < rd){
          nr += 1;
          double d = sqrt(d2);
          double mx = rx/d; double my = ry/d;
          // repel
          interactions[i*6] -= mx;
          interactions[i*6+1] -= my;
        } 
        else if (d2 < ra && (vicsek || (!vicsek && nr == 0 && alignDistance > 0.0 && d2 >= rd)) ){
          double d = sqrt(d2);
          double mx = rx/d; double my = ry/d;
          double alpha = std::acos(ctheta*mx+stheta*my);
          if (alpha > cang){
            continue;
          }
          nal += 1;
          // align
          double vjx = velocities[j*2];
          double vjy = velocities[j*2+1];
          double v = std::sqrt(vjx*vjx+vjy*vjy);
          if (v==0){continue;}
          interactions[i*6+2] += vjx/v;
          interactions[i*6+3] += vjy/v;
        }
        else if (!vicsek && nr == 0 && attractDistance > 0.0 && d2 >= rd && d2 >= ra && d2 < rat){
          double d = sqrt(d2);
          double mx = rx/d; double my = ry/d;
          double alpha = std::acos(ctheta*mx+stheta*my);
          if (alpha > cang){
            continue;
          }
          nat += 1;
          //attract
          interactions[i*6+4] += mx;
          interactions[i*6+5] += my;
        }
      }
    }

    double dx = 0.0;
    double dy = 0.0;

    normR = std::sqrt(interactions[i*6]*interactions[i*6]+interactions[i*6+1]*interactions[i*6+1]);
    normAl = std::sqrt(interactions[i*6+2]*interactions[i*6+2]+interactions[i*6+3]*interactions[i*6+3]);
    normAt = std::sqrt(interactions[i*6+4]*interactions[i*6+4]+interactions[i*6+5]*interactions[i*6+5]);

    if (normR > 0){
      dx = interactions[i*6]/normR;
      dy = interactions[i*6+1]/normR;
    }
    else if (normAl > 0 && normAt > 0){
      dx = alignmentPreference*interactions[i*6+2]/normAl+(1.0-alignmentPreference)*interactions[i*6+4]/normAt;
      dy = alignmentPreference*interactions[i*6+3]/normAl+(1.0-alignmentPreference)*interactions[i*6+5]/normAt;
    }
    else if (normAl > 0 && normAt == 0){
      dx = interactions[i*6+2]/normAl;
      dy = interactions[i*6+3]/normAl;
    }
    else if (normAl == 0 && normAt > 0){
      dx = interactions[i*6+4]/normAt;
      dy = interactions[i*6+5]/normAt;
    }

    nx = dx;
    ny = dy;
}

size_t ParticleSystem::step(){
  clock_t tic = clock();
  if (collisions){
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
  }

  float col = (clock()-tic)/float(CLOCKS_PER_SEC);
  tic = clock();

  double cc = drag*dt/2.0;
  double D = std::sqrt(2.0*rotationalDiffusion/dt);

  int cnt = 0;

  std::vector<uint64_t> toRemove;

  for (int i = 0; i < size(); i++){

    interactions[i*6] = 0.0;
    interactions[i*6+1] = 0.0;
    interactions[i*6+2] = 0.0;
    interactions[i*6+3] = 0.0;
    interactions[i*6+4] = 0.0;
    interactions[i*6+5] = 0.0;

    double nx = 0.0;
    double ny = 0.0;
    double dtheta = 0.0;

    metricInteractions(i,nx,ny);

    double speedMultiplier = 1.0;

    if (predatorActive){
      double px = state[i*3]-predX;
      double py = state[i*3+1]-predY;

      if (periodic){
          if (px > Lx*0.5){
            px -= Lx;
          }
          else if (px <= -Lx*0.5){
            px += Lx;
          }
          if (py > Ly*0.5){
            py -= Ly;
          }
          else if (py <= -Ly*0.5){
            py += Ly;
          }
      }

      double d2 = px*px+py*py;
      double vp = (predVx*predVx+predVy*predVy);
      double d = std::sqrt(d2);

      double ctheta = std::cos(state[i*3+2]);
      double stheta = std::sin(state[i*3+2]);
      double cang = M_PI-blindAngle;

      double alpha = std::acos(ctheta*(-1.0*px)/d+stheta*(-1.0*py)/d);
      if (alpha < cang){
        
        double mag = 10.0*(1.0+vp)/(d2/radius);
        nx = (nx + mag*px/d)/(1.0+mag);
        ny = (ny + mag*py/d)/(1.0+mag);
        speedMultiplier = 1.0+(radius+predRadius)/d2;
      
      }

      if (d < radius*2.0){
        // eaten!
        toRemove.push_back(i);
      }
    }


    dtheta = std::cos(state[i*3+2])*ny - std::sin(state[i*3+2])*nx;
    
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

    double s = std::min(MAX_PARTICLE_SPEED*radius,speedMultiplier*speed);

    double ax = drag*s*cos(theta)+forces[i*2];
    double ay = drag*s*sin(theta)+forces[i*2+1];
    double atheta = responseRate*dtheta;

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

    if (periodic){
      if (state[i*3]<0){
        state[i*3] += Lx;
        lastState[i*3] += Lx;
      }
      else if (state[i*3]>Lx){
        state[i*3] -= Lx;
        lastState[i*3] -= Lx;
      }
      if (state[i*3+1]<0){
        state[i*3+1] += Ly;
        lastState[i*3+1] += Ly;
      }
      else if (state[i*3+1] > Ly){
        state[i*3+1] -= Ly;
        lastState[i*3+1] -= Ly;
      }
    }
    else{

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
        //state[i*3+2] = ang;
        state[i*3+1] = newY+uy;
        state[i*3] = newX+ux;
        //lastState[i*3+2] = state[i*3+2];
      }
    }
  }

  for (int i = 0; i < size(); i++){
    forces[i*2] = 0.0;
    forces[i*2+1] = 0.0;

    for (int k = 0; k < 3; k++){floatState[i*4+k] = float(state[i*3+k]);}
  }

  for (int i = 0; i < toRemove.size(); i++){
    removeParticle(toRemove[i]);
  }

  double updates = (clock()-tic)/double(CLOCKS_PER_SEC);
  tic = clock();

  return toRemove.size();
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
  interactions.push_back(0.0);
  interactions.push_back(0.0);
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
      interactions.begin()+2*i+6
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

const float maxResponseRate = M_PI/4.0;
const float maxDiffusion = M_PI;
const float v0 = 20.0;
const float maxInertia = 1.0;

void ParticleSystem::setParameter(Parameter p, double value){
  double dc = MAX_INTERACTION_RANGE*radius;
  switch (p){
    case RepelDistance:
      repelDistance = dc*value;
      break;
    case AlignDistance:
      alignDistance = value*dc;
      break;
    case AttractDistance:
      attractDistance = value*dc;
      break;
    case AlignPreference:
      alignmentPreference = value;
    case Diffusion:
      rotationalDiffusion = value*maxDiffusion;
      break;
    case Speed:
      speed = value*v0*radius;
      break;
    case Inertia:
      momentOfInertia = value*maxInertia+0.001;
      break;
    case ResponseRate:
      responseRate = value*maxResponseRate;
      break;
    case BlindAngle:
      blindAngle = value*M_PI;
  }
}

double ParticleSystem::getParameter(Parameter p){
  switch (p){
    case RepelDistance:
      return repelDistance; 
    case AlignDistance:
      return alignDistance;
    case AttractDistance:
      return attractDistance;
    case AlignPreference:
      return alignmentPreference;
    case Diffusion:
      return rotationalDiffusion;
    case Speed:
      return speed;
    case Inertia:
      return momentOfInertia;
    case ResponseRate:
      return responseRate;
    case BlindAngle:
      return blindAngle;
  }
}