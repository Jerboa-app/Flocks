#include <ParticleSystem/particleSystemRenderer.h>

void ParticleSystemRenderer::setProjection(glm::mat4 p){
  projection = p;
  glUseProgram(particleShader);
  glUniformMatrix4fv(
    glGetUniformLocation(particleShader,"proj"),
    1,
    GL_FALSE,
    &projection[0][0]
  );

  glUseProgram(trackShader);

  glUniformMatrix4fv(
    glGetUniformLocation(trackShader,"proj"),
    1,
    GL_FALSE,
    &projection[0][0]
  );

}

void ParticleSystemRenderer::initialiseGL(){
  // a buffer of particle states
  glGenBuffers(1,&offsetVBO);
  glBindBuffer(GL_ARRAY_BUFFER,offsetVBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(float)*nParticles*4,NULL,GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER,0);

  // setup an array object
  glGenVertexArrays(1,&vertVAO);
  glGenBuffers(1,&vertVBO);
  glBindVertexArray(vertVAO);
  glBindBuffer(GL_ARRAY_BUFFER,vertVBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  // place dummy vertices for instanced particles
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
  // place states
  glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glVertexAttribDivisor(1,1);

  glError("initialised particles");

  particleShader = glCreateProgram();
  compileShader(particleShader,particleVertexShader,particleFragmentShader);
  glUseProgram(particleShader);

  glUniformMatrix4fv(
    glGetUniformLocation(particleShader,"proj"),
    1,
    GL_FALSE,
    &projection[0][0]
  );

  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);

  glError();
  glBufferStatus();

  // track

  glGenVertexArrays(1,&trackVAO);
  glGenBuffers(1,&trackVBO);
  glBindVertexArray(trackVAO);
  glBindBuffer(GL_ARRAY_BUFFER,trackVBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(float)*trackLength*3,track,GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);

  trackShader = glCreateProgram();
  compileShader(trackShader,trackVertexShader,trackFragmentShader);
  glUseProgram(trackShader);

  glUniformMatrix4fv(
    glGetUniformLocation(trackShader,"proj"),
    1,
    GL_FALSE,
    &projection[0][0]
  );

  glUniform1f(
    glGetUniformLocation(trackShader,"time"),
    trackLength
  );

}

void ParticleSystemRenderer::draw(
  ParticleSystem & p,
  uint64_t frameId,
  float zoomLevel,
  float resX,
  float resY
){
  glUseProgram(particleShader);

  glUniform1f(
    glGetUniformLocation(particleShader,"zoom"),
    zoomLevel
  );

  glUniform1f(
    glGetUniformLocation(particleShader,"scale"),
    resX*2.0
  );

  glUniform1i(
    glGetUniformLocation(particleShader,"tracked"),
    trackedParticle != NULL_INDEX ? trackedParticle : -1
  );

  glBindBuffer(GL_ARRAY_BUFFER,offsetVBO);
  glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*nParticles*4,&p.floatState[0]);
  glBindBuffer(GL_ARRAY_BUFFER,0);

  glError("particles buffers");

  glBindVertexArray(vertVAO);
  glDrawArraysInstanced(GL_POINTS,0,1,p.size());
  glBindVertexArray(0);

  glError("draw particles");

  if (trackedParticle != NULL_INDEX){
    glUseProgram(trackShader);

    updatedTrack(p);

    glBindBuffer(GL_ARRAY_BUFFER,trackVBO);
    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*trackLength*3,&track[0]);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindVertexArray(trackVAO);
    glLineWidth(2);
    glDrawArrays(GL_POINTS,0,trackLength);
    glBindVertexArray(0);

  }

}

void ParticleSystemRenderer::updatedTrack(ParticleSystem & p){
  if (trackedParticle != NULL_INDEX){

    float oldx = track[0]; float oldy = track[1];
    for (int t = 1; t < trackLength-1; t++){
      float x = track[t*3]; float y = track[t*3+1];
      track[t*3] = oldx;
      track[t*3+1] = oldy;
      oldx = x; oldy = y;
    }

    track[0] = p.floatState[trackedParticle*4];
    track[1] = p.floatState[trackedParticle*4+1];

  }
}

void ParticleSystemRenderer::beginTracking(ParticleSystem & p, uint64_t i){
  if (i == trackedParticle){return;}

  trackedParticle = i;
  for (int t = 0; t < trackLength; t++){
    track[t*3] = p.floatState[trackedParticle*4];
    track[t*3+1] = p.floatState[trackedParticle*4+1];
    track[t*3+2] = trackLength-t;
  }

}

void ParticleSystemRenderer::click(ParticleSystem & p, float x, float y){

  for (int i = 0; i < p.size(); i++){
    double rx = p.state[i*3]-x;
    double ry = p.state[i*3+1]-y;
    double dd = rx*rx+ry*ry;
    if (dd < p.parameters[i*2]*p.parameters[i*2]){
      return beginTracking(p,i);
    }
  }

  trackedParticle = NULL_INDEX;
}
