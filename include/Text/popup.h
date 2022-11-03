#ifndef POPUPTEXT_H
#define POPUPTEXT_H

#include <string>

class FadingText {
public:

  friend class Popup;

  FadingText(
    std::string t,
    float time,
    float x,
    float y,
    glm::vec3 colour,
    std::string tag
  )
  : text(t), fadeTime(time), x(x), y(y), colour(colour), clock(0.0), tag(tag)
  {}

  void increment(float dt){
    clock += dt;
  }

  std::string getTag(){return tag;}

  float alpha(){

    if (clock < fadeTime/2.0){return 1.0;}

    float a = 1.0 - 2.0*(clock-fadeTime/2.0) / (fadeTime);

    if (a < 0){ return 0; }
    else if (a > 1){ return 1; }
    else { return a; }
  }

  bool done(){return clock > fadeTime;}

private:

  std::string text;
  float fadeTime;
  float x;
  float y;
  glm::vec3 colour;
  float clock;
  std::string tag;

};

class Popup {
public:

  Popup(){}

  void draw(TextRenderer & text, Type & type, float dt);
  void post(FadingText f){popups.push_back(f);}
  void clear(){popups.clear();}
  void clear(std::string tag);

private:

  std::vector<FadingText> popups;

};

#endif
