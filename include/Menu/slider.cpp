#include <Menu/slider.h>

bool Slider::clicked(float x, float y){
  // assumes axis aligned
  if (smoothChanging){return false;}
  if (!enabled){return false;}
  if (xPosition <= x && x <= xPosition+width && yPosition <= y && y <= yPosition+height){
    clickX = x; clickY = y;
    if (!smoothChange){
      setPosition( (x-xPosition)/width );
    } 
    else {
      nextPos = (x-xPosition)/width; 
      if(nextPos>1){nextPos=1;}
      else if (nextPos<0){nextPos=0;}
      smoothChanging = true;
      return true;
    }
    dragging = true;
    return true;
  }
  return false;
}

void Slider::drag(float x, float y){
  if (smoothChanging){return;}
  if (dragging == false){return;}
  if (!enabled){return;}
  // assumes axis aligned
  if (x < xPosition){
    setPosition(0);
  }
  else if (x > xPosition+width){
    setPosition(1);
  }
  if (xPosition <= x && x <= xPosition+width){
    float p = (x-xPosition)/width;
    setPosition(p);
  }
}

void Slider::initialiseGL(){

  sliderShader = glCreateProgram();
  compileShader(sliderShader,sliderVertexShader,sliderFragmentShader);

  float verts[6*2] = {
    xPosition,        yPosition,
    xPosition,        yPosition+height,
    xPosition+width,  yPosition+height,
    xPosition,        yPosition,
    xPosition+width,  yPosition,
    xPosition+width,  yPosition+height
  };

  glGenVertexArrays(1,&sliderVAO);
  glGenBuffers(1,&sliderVBO);
  glBindVertexArray(sliderVAO);
  glBindBuffer(GL_ARRAY_BUFFER,sliderVBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(float)*6*2,&verts[0],GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);
  glError("Arrays and buffers for slider");

}

void Slider::draw(
  TextRenderer & text,
  Type & type,
  float scale
){

  if (smoothChanging){
    float delta = nextPos-position;
    float sgn = delta < 0 ? -1.0 : 1.0;
    float next = position+sgn*std::exp(std::abs(delta)*rate)/period;
    if ( (position >= nextPos && next <= nextPos) || (position <= nextPos && next >= nextPos)){
      smoothChanging = false;
      position = nextPos;
    }
    else{
      position = next;
    }
  }

  glUseProgram(sliderShader);

  glUniformMatrix4fv(
    glGetUniformLocation(sliderShader,"proj"),
    1,
    GL_FALSE,
    &projection[0][0]
  );

  double alpha = enabled ? 1.0 : 0.33;

  glUniform4f(
    glGetUniformLocation(sliderShader,"frameColour"),
    0.0,0.0,0.0,1.0*alpha
  );

  glUniform4f(
    glGetUniformLocation(sliderShader,"fillColour"),
    0.0,0.0,1.0,0.33*alpha
  );

  float pixelStart = xPosition;
  float pixelFillTo = xPosition + width*position;

  glUniform2f(
    glGetUniformLocation(sliderShader,"state"),
    pixelStart,pixelFillTo
  );

  glBindVertexArray(sliderVAO);

  glDrawArrays(GL_TRIANGLES,0,6);

  glBindVertexArray(0);

  text.renderText(
    type,
    label,
    xPosition,
    (yPosition+height*1.5),
    scale,
    glm::vec3(0.0f,0.0f,0.0f)
  );

  glError("Slider draw");
}
