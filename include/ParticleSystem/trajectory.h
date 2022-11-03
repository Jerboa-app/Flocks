#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <ParticleSystem/particleSystem.h>

#include <fstream>
#include <utils.h>
#include <ctime>
#include <thread>

struct State {
  State (double x, double y, double r)
  : x(x), y(y), radius(r)
  {}
  double x;
  double y;
  double radius;
};

struct Parameters {
  Parameters(double re, double n, double o, double s)
  : restitution(re), nParticles(n),order(o),speed(s)
  {}
  double restitution;
  double nParticles;
  double order;
  double speed;
};

class Trajectory {
public:

const int CACHE_SIZE_BYTES = 1e6;

  Trajectory(){
    newFile();
  }

  void takeReading(ParticleSystem & p);

  void setSpeed(double s){speed = s;}

  void save();

  void clear(){trajectory.clear(); parameters.clear(); size = 0;}

  void newFile(){
    clear();
    ioMode = std::ios_base::trunc;
    std::time_t t = std::time(NULL);
    std::tm tm = *std::localtime(&t);
    std::stringstream ss;
    ss << "trajectory-";
    ss << std::put_time(&tm, "%c %Z");
    file = ss.str();
    size = 0;
  }

  std::string fileName(){return file;}

private:

  // timestep implicitly defined by use of takeReading()
  std::vector<std::vector<State>> trajectory;
  std::vector<Parameters> parameters;
  std::ios_base::openmode ioMode;
  std::string file;
  double speed;

  uint64_t size;

  void threadedSave(std::vector<std::vector<State>> trajectory);
};

#endif
