#ifndef MENU_H
#define MENU_H

class Menu {
public:

  Menu(){}

  void initialiseGL();

  void draw();

private:
  // box
  GLuint boxShader, boxVAO, boxVBO;
};

#endif
