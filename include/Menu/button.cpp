#include <Menu/button.h>

bool Button::clicked(float x, float y){
  // assumes axis aligned
  if (xPosition <= x && x <= xPosition+width && yPosition <= y && y <= yPosition+height){
    clickX = x; clickY = y;
    set = true;
    framesSinceClick = 0;
    return true;
  }
  return false;
}

void Button::initialiseGL(){

  buttonShader = glCreateProgram();
  compileShader(buttonShader,buttonVertexShader,buttonFragmentShader);

  float verts[6*2] = {
    xPosition,        yPosition,
    xPosition,        yPosition+height,
    xPosition+width,  yPosition+height,
    xPosition,        yPosition,
    xPosition+width,  yPosition,
    xPosition+width,  yPosition+height
  };

  glGenVertexArrays(1,&buttonVAO);
  glGenBuffers(1,&buttonVBO);
  glBindVertexArray(buttonVAO);
  glBindBuffer(GL_ARRAY_BUFFER,buttonVBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(float)*6*2,&verts[0],GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);
  glError("Arrays and buffers for button");

}

void Button::draw(
  TextRenderer & text,
  Type & type
){

  glUseProgram(buttonShader);

  glUniformMatrix4fv(
    glGetUniformLocation(buttonShader,"proj"),
    1,
    GL_FALSE,
    &projection[0][0]
  );

  glUniform4f(
    glGetUniformLocation(buttonShader,"frameColour"),
    0.0,0.0,0.0,1.0
  );

  glUniform4f(
    glGetUniformLocation(buttonShader,"fillColour"),
    0.0,0.0,1.0,1.0
  );

  glUniform1i(
    glGetUniformLocation(buttonShader,"state"),
    int(set)
  );

  glUniform1f(
    glGetUniformLocation(buttonShader,"alpha"),
    std::max(0.0f,1.0f - framesSinceClick/float(feedback))
  );

  glBindVertexArray(buttonVAO);

  glDrawArrays(GL_TRIANGLES,0,6);

  glBindVertexArray(0);

  text.renderText(
    type,
    label,
    xPosition,
    (yPosition+height*1.5),
    0.5f,
    glm::vec3(0.0f,0.0f,0.0f)
  );

  glError("button draw");

  if (framesSinceClick < feedback){
    framesSinceClick++;
  }
}
