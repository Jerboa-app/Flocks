#ifndef ORTHOCAM_H
#define ORTHOCAM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
  An orthographic camera.

  Assumes world coordinates: [0,1]x[0,1]
*/
class OrthoCam {
public:
  OrthoCam(int x, int y)
  : resolution(x,y), zoomLevel(1.0f), position(glm::vec2(0.0,0.0)) {
    update();
  }

  OrthoCam(int x, int y, glm::vec2 pos)
  : resolution(x,y), zoomLevel(1.0f), position(pos) {
    update();
  }

  // assumes pos.y is inverted in screen coordinates
  glm::vec4 screenToWorld(float x, float y){
    glm::vec4 nvd(
      2.0*x/resolution.x-1.0,
      2.0*(resolution.y-y)/resolution.y-1.0,
      0.0,
      1.0
    );

    return invProjection*nvd;
  }

  glm::mat4 getVP(){return vp;}
  glm::mat4 getProjection(){return projection;}
  float getZoomLevel(){return zoomLevel;}
  glm::vec2 getPosition(){return position;}

  void setProjection(glm::mat4 newProjection){projection=newProjection; update();}
  void setView(glm::mat4 newView){view=newView; update();}
  void incrementZoom(float dz){
    if (zoomLevel >= 1.0){
      zoomLevel += dz;
      zoomLevel < 1.0 ? zoomLevel = 1.0 : 0;
    }
    else{
      zoomLevel += 1.0/dz;
    }
    update();
  }
  void setPosition(glm::vec2 newPosition){position=newPosition; update();}
  void setPosition(float x, float y){position=glm::vec2(x,y); update();}
  void move(float dx, float dy){position += glm::vec2(dx,dy); update();}

private:

  void update(){

    view = glm::scale(glm::mat4(1.0f),glm::vec3(resolution.x,resolution.y,1.0f)) *
      glm::lookAt(
        glm::vec3(position.x,position.y,1.0),
        glm::vec3(position.x,position.y,0.0),
        glm::vec3(0.0,1.0,0.0)
      );

    glm::vec3 center(position.x+0.5,position.y+0.5, 1.0f);
    view *= glm::translate(glm::mat4(1.0f), center) *
           glm::scale(glm::mat4(1.0f),glm::vec3(zoomLevel,zoomLevel,1.0f))*
           glm::translate(glm::mat4(1.0f), -center);

    projection = glm::ortho(
      0.0,
      double(resolution.x),
      0.0,
      double(resolution.y),
      0.1,
      100.0
    );


    vp = projection*view;
    invProjection = glm::inverse(vp);
  }

  glm::vec2 resolution;
  glm::mat4 view;
  glm::mat4 projection;
  glm::mat4 invProjection;
  glm::mat4 vp;

  float zoomLevel;

  glm::vec2 position;
};

#endif
