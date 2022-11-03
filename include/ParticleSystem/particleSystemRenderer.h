#ifndef PARTICLESYSTEMRENDERER_H
#define PARTICLESYSTEMRENDERER_H

class ParticleSystemRenderer {
public:
  ParticleSystemRenderer(int sizeHint, int trackLength = 60*100)
  : nParticles(sizeHint)
  {

    trackedParticle = NULL_INDEX;
    track = new float [trackLength*3];
    this->trackLength = trackLength;

    initialiseGL();
  }

  void setProjection(glm::mat4 p);
  void draw(ParticleSystem & p, uint64_t frameId, float zoomLevel, float resX, float resY);

  ~ParticleSystemRenderer(){
    // kill some GL stuff
    glDeleteProgram(particleShader);
    glDeleteProgram(trackShader);

    glDeleteBuffers(1,&offsetVBO);
    glDeleteBuffers(1,&vertVBO);
    glDeleteBuffers(1,&trackVBO);

    glDeleteVertexArrays(1,&vertVAO);
    glDeleteVertexArrays(1,&trackVAO);

    free(track);
  }

  void click(ParticleSystem & p, float x, float y);
  void beginTracking(ParticleSystem & p, uint64_t i);
  void updatedTrack(ParticleSystem & p);

private:
  int nParticles;
  // GL data members
  float * floatState;
  GLuint particleShader, offsetVBO, vertVAO, vertVBO;
  glm::mat4 projection;

  float vertices[3] = {0.0,0.0,0.0};

  float * track;
  int trackLength;
  uint64_t trackedParticle;

  GLuint trackShader, trackVAO, trackVBO;

  void initialiseGL();

};

#endif
