#ifndef UTILS_H
#define UTILS_H

#include <exception>

class IOException: public std::exception {
public:
    IOException(std::string msg)
    : msg(msg)
    {}
private:
    virtual const char * what() const throw(){
        return msg.c_str();
    }
    std::string msg;
};


struct Vertex {

    Vertex(float x, float y)
    : x(x), y(y) {}
    float x;
    float y;

    Vertex operator+(Vertex const & rhs){
        return Vertex(x+rhs.x,y+rhs.y);
    }

    Vertex operator-(Vertex const & rhs){
        return Vertex(x-rhs.x,y-rhs.y);
    }
};

float dot(const Vertex & a, const Vertex & b){
    return a.x*b.x+a.y*b.y;
}

bool pointInCircle(float x, float y, float cx, float cy, float r){
    float rx = (x-cx);
    float ry = (y-cy);
    float d2 = rx*rx+ry*ry;
    if (d2 < r*r){
        return true;
    }
    return false;
}

struct Triangle {

    Triangle(Vertex a, Vertex b, Vertex c)
    : a(a), b(b), c(c) {}
    Vertex a;
    Vertex b;
    Vertex c;
};

bool pointInTriangle(Vertex v, Triangle tri){
    Vertex a = tri.c - tri.a;
    Vertex b = tri.b - tri.a;
    Vertex c = v - tri.a;

    float aDOTa = dot(a,a);
    float aDOTb = dot(a,b);
    float aDOTc = dot(a,c);
    float bDOTb = dot(b,b);
    float bDOTc = dot(b,c);

    float inv = 1.0 / (aDOTa*bDOTb-aDOTb*aDOTb);
    float u = (bDOTb*aDOTc-aDOTb*bDOTc)*inv;

    if (u < 0) {
        return false;
    }

    float t = (aDOTa*bDOTc-aDOTb*aDOTc)*inv;

    if (t >= 0 && (u+t < 1)) {
        return true;
    }

    return false;
}

#endif
