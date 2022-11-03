#ifndef SLIDER_H
#define SLIDER_H

#include <glm/glm.hpp>
#include <string>

class Slider {
public:

  Slider(float x, float y, float w, float h, std::string l)
  : xPosition(x), yPosition(y), width(w), height(h),
  position(0.0), label(l), dragging(false), smoothChange(false), period(60), rate(0.0), smoothChanging(false)
  {
    initialiseGL();
  }

  void initialiseGL();
  void draw(
    TextRenderer & text,
    Type & type
  );

  void setProjection(glm::mat4 p){projection=p;}

  float getPosition(){return position;}

  void setPosition(float p){
    if(p>1){position=1;}
    else if (p<0){position=0;}
    else{position=p;}
  }

  void setLabel(std::string s){label=s;}

  bool clicked(float x, float y);
  void drag(float x, float y);
  void mouseUp(){dragging = false; clickX = xPosition+position*width;}

  void setSmoothChange(bool b, uint64_t p = 60, float r = 0.0){smoothChange = b; period = p; rate = r;}

  void smoothChangeTo(float to, uint64_t p = 60, float r = 0.0){
    nextPos = to; 
    if(nextPos>1){nextPos=1;}
    else if (nextPos<0){nextPos=0;}
    smoothChange = true;
    smoothChanging = true;
    period = p;
    rate = r;
  }

private:

  std::string label;

  float position;

  float xPosition;
  float yPosition;
  float width;
  float height;

  glm::mat4 projection;

  float clickX;
  float clickY;
  bool dragging;

  bool smoothChange;
  bool smoothChanging;
  uint64_t period;
  float delta;
  float nextPos;
  float rate;

  GLuint sliderShader, sliderVAO, sliderVBO;

};

#endif
