#ifndef BUTTON_H
#define BUTTON_H

class Button {
public:
  Button(float x, float y, float w, float h, std::string l, uint8_t f)
  : xPosition(x), yPosition(y), width(w), height(h),
   label(l), feedback(f)
  {
    framesSinceClick = f;
    initialiseGL();
  }

  void initialiseGL();
  virtual void draw(
    TextRenderer & text,
    Type & type,
    float scale=0.5f
  );

  void setProjection(glm::mat4 p){projection=p;}

  bool getState(){return set;}

  void setState(bool p){
    set = p;
  }

  void setLabel(std::string s){label=s;}

  bool clicked(float x, float y);

protected:

  std::string label;

  bool set;

  float xPosition;
  float yPosition;
  float width;
  float height;

  glm::mat4 projection;

  float clickX;
  float clickY;

  uint8_t feedback;
  uint8_t framesSinceClick;

  GLuint buttonShader, buttonVAO, buttonVBO;

};

class CheckButton : public Button {
  void draw(
    TextRenderer & text,
    Type & type,
    float scale=0.5f
  );
};

#endif
