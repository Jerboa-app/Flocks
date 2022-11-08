#ifndef PREDATOR_H
#define PREDATOR_H

const double MAX_PREDATOR_SPEED = 15.0;

class Predator {
    public:
        Predator(double x, double y, double theta, double r)
        : x(x), y(y), theta(theta),
          xp(x), yp(y), thetap(theta), 
          radius(r), v0(r/0.2), drag(0.01), mass(0.01),
          momentOfInertia(0.0001), rotationalDrag(0.01)
        {
            initialiseGL();
            accelerationMagnitude = r;
            maxSpeed = MAX_PREDATOR_SPEED*r;
            turningRate = 0.05;
            Lx = 1.0;
            Ly = 0.9;
        }

        void setProjection(glm::mat4 p);
        void draw(uint64_t frameId, float zoomLevel, float resX, float resY);
        ~Predator(){
        }

        void step(double dt);

        void setState(double xn, double yn, double thetan){
            x = xn;
            y = yn;
            theta = thetan;

            xp = x;
            yp = y;
            thetap = theta;  
        }

        std::vector<double> getState(){
            return std::vector<double> {x,y,vx,vy};
        }

        void speedIncrement(bool sign){
            double dv = sign ? 1.0 : -1.0;
            v0 += dv*accelerationMagnitude;
            if (v0 < 0){
                v0 = 0.0;
            }
            if (v0 > maxSpeed){
                v0 = maxSpeed;
            }
        }

        void turn(bool sign){
            double dtheta = sign ? 1.0 : -1.0;
            torque += dtheta*turningRate;
        }

        void setBoundary(double lx, double ly){
            Lx = lx; Ly = ly;
        }

        double getRadius(){return radius;}

    private:

    double x;
    double y;
    double theta;

    double xp;
    double yp;
    double thetap;

    double vx;
    double vy;

    double v0;
    double radius;
    double drag;
    double mass;
    double momentOfInertia;
    double rotationalDrag;

    double torque;

    double Lx;
    double Ly;

    double turningRate;
    double accelerationMagnitude;
    double maxSpeed;

    GLuint shader, vertVAO, vertVBO;
    glm::mat4 projection;

    float vertices[4] = {0.0,0.0,0.0,0.0};

    void initialiseGL();

};
#endif