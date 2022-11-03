#include <menu.h>

void Menu::initialiseGL(){

  boxShader = glCreateProgram();
  compileShader(boxShader,boxVertexShader,boxFragmentShader);

  glGenVertexArrays(1,&boxVAO);
  glGenBuffers(1,&boxVBO);
  glBindVertexArray(boxVAO);
  glBindBuffer(GL_ARRAY_BUFFER,boxVBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(float)*6*2,NULL,GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);
  glError("Arrays and buffers for box");
}
