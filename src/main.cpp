#include <iostream>
#include <sstream>
#include <iomanip>

#if WINDOWS
  #include <glew.c>
#else
  #include <glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <orthoCam.h>

#include <glUtils.h>
#include <utils.h>
#include <shaders.h>

#include <ParticleSystem/particleSystem.cpp>
#include <ParticleSystem/particleSystemRenderer.cpp>
#include <ParticleSystem/trajectory.cpp>
#include <ParticleSystem/predator/predator.cpp>
#include <Text/textRenderer.cpp>
#include <Text/popup.cpp>

#include <Menu/button.cpp>
#include <Menu/slider.cpp>
#include <Menu/sliderGroup.h>

#include <time.h>
#include <random>
#include <iostream>
#include <math.h>
#include <vector>

const int resX = 1000;
const int resY = 1000;

const int subSamples = 1;
const float dt = (1.0 / 60.0) / subSamples;

const int saveFrequency = 1;

const int N = 5000;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];
double physDeltas[60];
double renderDeltas[60];

uint64_t eatenCounter = 0;

float speed = 1.0;

void speedPopUp(Popup & popups){
  popups.clear("speed");
  std::string pos = fixedLengthNumber(std::ceil(100.0*speed),3);
  popups.post(
    FadingText(
      "Speed: "+pos+" %",
      3.0,
      resX-256.,
      resY-64*9,
      glm::vec3(0.0,0.0,0.0),
      "speed"
    )
  );
}

int main(){

  for (int i = 0; i < 60; i++){deltas[i] = 0.0;}

  sf::ContextSettings contextSettings;
  contextSettings.depthBits = 24;
  contextSettings.antialiasingLevel = 0;

  sf::RenderWindow window(
    sf::VideoMode(resX,resY),
    "Big Nuts Rise",
    sf::Style::Close|sf::Style::Titlebar,
    contextSettings
  );
  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);
  window.setActive();

  glewInit();

  uint8_t debug = 0;

  // the core simulation
  ParticleSystem particles(N,dt);
  // handles rendering - separation of concerns
  ParticleSystemRenderer pRender(N,60*5.0);
  // player predator
  Predator predator(0.5,0.5,0.0,particles.getRadius());
  bool predatorActive = false;

  sf::Clock clock;
  sf::Clock physClock, renderClock;

  glm::mat4 defaultProj = glm::ortho(0.0,double(resX),0.0,double(resY),0.1,100.0);
  glm::mat4 textProj = glm::ortho(0.0,double(resX),0.0,double(resY));

  // for rendering particles using gl_point instances
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SPRITE);
  glEnable( GL_BLEND );
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  // for freetype rendering
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // must be initialised before so the shader is in use..?
  TextRenderer textRenderer(textProj);

  Type OD("resources/fonts/","OpenDyslexic-Regular.otf",48);

  Popup popups;

  OrthoCam camera(resX,resY,glm::vec2(0.0,0.0));

  glViewport(0,0,resX,resY);

  // sliders are not beautifully handeled, should really have
  //  a widget hierachy system, but won't bother for this
  //  app

  SliderGroup sliders(textProj);

  double x = 128.0*1.0;
  double w = 96.0;

  sliders.add("particles",8.0,resY-32.0,w,8.0,"Particles");
  sliders.add("repDist",8.0*2+x,resY-32.0,w,8.0,"Repel Distance");
  sliders.add("repStr",8.0*2+x*2.0,resY-32.0,w,8.0,"Repel Strength");
  sliders.add("alignDist",8.0*2+x*3.0,resY-32.0,w,8.0,"Align Dist.");
  sliders.add("alignStr",8.0*2+x*4.0,resY-32.0,w,8.0,"Align Str.");
  sliders.add("attrDist",8.0*2+x*5.0,resY-32.0,w,8.0,"Attract Dist.");
  sliders.add("attrStr",8.0*2+x*6.0,resY-32.0,w,8.0,"Attract Str.");
  sliders.add("diff",8.0*2,resY-32.0*2,w,8.0,"Diffusion");
  sliders.add("speed",8.0*2+x*1.0,resY-32.0*2.0,w,8.0,"Speed");
  sliders.add("inertia",8.0*2+x*2.0,resY-32.0*2.0,w,8.0,"Inertia");
  sliders.add("resp",8.0*2+x*3.0,resY-32.0*2.0,w,8.0,"Response Rate");

  std::default_random_engine generator;
  std::uniform_real_distribution<double> U(0.0,1.0);

  sliders.setPosition("particles",0.25);
  sliders.setPosition("repDist",U(generator));
  sliders.setPosition("repStr",U(generator));
  sliders.setPosition("alignDist",U(generator));
  sliders.setPosition("alignStr",U(generator));
  sliders.setPosition("attrDist",U(generator));
  sliders.setPosition("attrStr",U(generator));
  sliders.setPosition("diff",0.1);
  sliders.setPosition("speed",0.5);
  sliders.setPosition("inertia",0.0);
  sliders.setPosition("resp",0.5);
  
  Button newRecording(resX-64.0,resY-48.0,8.0,8.0,"Record",30);
  newRecording.setState(false);
  newRecording.setProjection(textProj);

  CheckButton colours(8.0*2+x*6.0,resY-32.0*2.0,8.0,8.0,"Colours",30);
  colours.setState(true);
  colours.setProjection(textProj);

  double oldMouseX = 0.0;
  double oldMouseY = 0.0;

  double mouseX = resX/2.0;
  double mouseY = resY/2.0;

  bool moving = false;

  bool pause = true;
  bool startUp = true;

  bool isRecording = false;
  Trajectory record;

  while (window.isOpen()){

    sf::Event event;
    while (window.pollEvent(event)){

      if (event.type == sf::Event::Closed){
        if ( isRecording ){record.save();}
        window.close();
      }

      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P){
        predatorActive = !predatorActive;
        if (predatorActive){
          predator.setState(0.5,0.5,0.0);
        }
      }

      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape){
        if ( isRecording ){record.save();}
        window.close();
      }

      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1){
        debug = !debug;
      }

      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space){
        pause = !pause;
        if (startUp){
          startUp = false;
        }
      }


      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::H){
        speed *= 2.0;
        if (speed > 1){
          speed = 1;
        }
        speedPopUp(popups);
      }

      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L){
        speed /= 2.0;
        if (speed < 0.01){
          speed = 0.01;
        }
        speedPopUp(popups);
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        predator.speedIncrement(true);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
        predator.turn(true);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        predator.speedIncrement(false);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
        predator.turn(false);
      }


      if (event.type == sf::Event::MouseWheelScrolled){
        mouseX = event.mouseWheelScroll.x;
        mouseY = event.mouseWheelScroll.y;
        double z = event.mouseWheelScroll.delta;

        camera.incrementZoom(z);
      }

      if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle){
        mouseX = event.mouseButton.x;
        mouseY = event.mouseButton.y;

        glm::vec4 worldPos = camera.screenToWorld(mouseX,mouseY);

        camera.setPosition(worldPos.x,worldPos.y);
      }

      if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
        sf::Vector2i pos = sf::Mouse::getPosition(window);

        // horrible sliders, should have hierachy + drawable class
        //particlesSlider.clicked(pos.x,resY-pos.y);
        sliders.clicked(pos.x,resY-pos.y);
        // buttons
        newRecording.clicked(pos.x,resY-pos.y);
        colours.clicked(pos.x,resY-pos.y);

        // multiply by inverse of current projection
        glm::vec4 worldPos = camera.screenToWorld(pos.x,pos.y);

        pRender.click(particles,worldPos.x,worldPos.y);

        oldMouseX = pos.x;
        oldMouseY = pos.y;
      }

      if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        //particlesSlider.drag(pos.x,resY-pos.y);
        sliders.drag(pos.x,resY-pos.y);
      }

      if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left){
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        //particlesSlider.mouseUp();
        sliders.mouseUp();
      }

    }

    window.clear(sf::Color::White);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    physClock.restart();

    int n = std::floor(sliders.getPosition("particles")*float(N));
    sliders.setLabel("particles","Particles: "+fixedLengthNumber(n,4));
    if (n < particles.size()){
      while (n < particles.size()){
        particles.removeParticle();
      }
    }
    else if (n > particles.size()){
      int i = particles.size()-1;
      while (i < n){
        particles.addParticle();
        i++;
      }
    }

  //     sliders.add("repDist",8.0*2+x,resY-32.0,w,8.0,"Repel Distance");
  // sliders.add("repStr",8.0*2+x*2.0,resY-32.0,w,8.0,"Repel Strength");
  // sliders.add("alignDist",8.0*2+x*3.0,resY-32.0,w,8.0,"Align Dist.");
  // sliders.add("alignStr",8.0*2+x*4.0,resY-32.0,w,8.0,"Align Str.");
  // sliders.add("attrDist",8.0*2+x*5.0,resY-32.0,w,8.0,"Attract Dist.");
  // sliders.add("attrStr",8.0*2+x*6.0,resY-32.0,w,8.0,"Attract Str.");
  // sliders.add("diff",8.0*2,resY-32.0*2,w,8.0,"Diffusion");
  // sliders.add("speed",8.0*2+x*1.0,resY-32.0*2.0,w,8.0,"Speed");
  // sliders.add("inertia",8.0*2+x*2.0,resY-32.0*2.0,w,8.0,"Inertia");
  // sliders.add("resp",8.0*2+x*3.0,resY-32.0*2.0,w,8.0,"Response Rate");

    float value = sliders.getPosition("repDist");
    particles.setParameter(ParticleSystem::Parameter::RepelDistance,value);
    sliders.setLabel("repDist","Repel Distance "+fixedLengthNumber(value*MAX_INTERACTION_RANGE,3));

    value = sliders.getPosition("repStr");
    particles.setParameter(ParticleSystem::Parameter::RepelStrength,value);

    value = sliders.getPosition("alignDist");
    particles.setParameter(ParticleSystem::Parameter::AlignDistance,value);
    sliders.setLabel("alignDist","Align Dist. "+fixedLengthNumber(value*MAX_INTERACTION_RANGE,3));

    value = sliders.getPosition("alignStr");
    particles.setParameter(ParticleSystem::Parameter::AlignStrength,value);

    value = sliders.getPosition("attrDist");
    particles.setParameter(ParticleSystem::Parameter::AttractDistance,value);
    sliders.setLabel("attrDist","Attract Dist. "+fixedLengthNumber(value*MAX_INTERACTION_RANGE,3));

    value = sliders.getPosition("attrStr");
    particles.setParameter(ParticleSystem::Parameter::AttractStrength,value);

    value = sliders.getPosition("diff");
    particles.setParameter(ParticleSystem::Parameter::Diffusion,value);

    value = sliders.getPosition("speed");
    particles.setParameter(ParticleSystem::Parameter::Speed,value);
    sliders.setLabel("speed","Speed "+fixedLengthNumber(value*v0,3));

    value = sliders.getPosition("inertia");
    particles.setParameter(ParticleSystem::Parameter::Inertia,value);

    value = sliders.getPosition("resp");
    particles.setParameter(ParticleSystem::Parameter::ResponseRate,value);
    sliders.setLabel("resp","Response Rate "+fixedLengthNumber(value*maxResponseRate*180.0/M_PI,3)+" (deg)");

    particles.setTimeStep(dt*speed);

    if (!pause){
      for (int s = 0; s < subSamples; s++){

        particles.setPredatorActive(predatorActive);
        if (predatorActive){
            std::vector<double> s = predator.getState();
            particles.predatorState(s[0],s[1],s[2],s[3]);
        }
        size_t eaten = particles.step();
        eatenCounter += eaten;

        pRender.updatedTrack(particles);

        if (predatorActive){
          predator.step(speed*dt/subSamples);
        }

      }
    }

    physDeltas[frameId] = physClock.getElapsedTime().asSeconds();

    renderClock.restart();

    glm::mat4 proj = camera.getVP();

    pRender.setProjection(proj);
    pRender.setColours(colours.getState());
    pRender.draw(
      particles,
      frameId,
      camera.getZoomLevel(),
      resX,
      resY
    );

    if (predatorActive){
      predator.setProjection(proj);
      predator.draw(
        frameId,
        camera.getZoomLevel(),
        resX,
        resY
      );
    }

    if (newRecording.getState()){
      record.newFile();
      isRecording = !isRecording;
      newRecording.setState(false);
    }

    record.setSpeed(speed);

    if (frameId % saveFrequency == 0 && isRecording) {record.takeReading(particles);}

    if (isRecording){
      textRenderer.renderText(
        OD,
        "Recording to file:\n    "+record.fileName(),
        resX-320.0,resY-64.0*1,
        0.25f,
        glm::vec3(0.0f,0.0f,0.0f)
      );
    }

    if (debug){
      double delta = 0.0;
      double renderDelta = 0.0;
      double physDelta = 0.0;
      for (int n = 0; n < 60; n++){
        delta += deltas[n];
        renderDelta += renderDeltas[n];
        physDelta += physDeltas[n];
      }
      delta /= 60.0;
      renderDelta /= 60.0;
      physDelta /= 60.0;
      std::stringstream debugText;

      sf::Vector2i mouse = sf::Mouse::getPosition(window);

      float cameraX = camera.getPosition().x;
      float cameraY = camera.getPosition().y;

      debugText << "Particles: " << n <<
        "\n" <<
        "Delta: " << fixedLengthNumber(delta,6) <<
        " (FPS: " << fixedLengthNumber(1.0/delta,4) << ")" <<
        "\n" <<
        "Render/Physics: " << fixedLengthNumber(renderDelta,6) << "/" << fixedLengthNumber(physDelta,6) <<
        "\n" <<
        "Mouse (" << fixedLengthNumber(mouse.x,4) << "," << fixedLengthNumber(mouse.y,4) << ")" <<
        "\n" <<
        "Camera [world] (" << fixedLengthNumber(cameraX,4) << ", " << fixedLengthNumber(cameraY,4) << ")" <<
        "\n" <<
        "Order: " << fixedLengthNumber(particles.orderParameter(),6) << "\n";
      textRenderer.renderText(
        OD,
        debugText.str(),
        64.0f,resY-128.0f,
        0.5f,
        glm::vec3(0.0f,0.0f,0.0f)
      );
    }
    if (predatorActive){
      std::cout << int(eatenCounter) << "\n";
      textRenderer.renderText(
        OD,
        "Eaten: "+std::to_string(int(eatenCounter)),
        8.0*2+x*7.0,resY-32.0*2.0,
        0.25f,
        glm::vec3(0.0f,0.0f,0.0f)
      );
    }

    // more inelegant slider drawing
    sliders.draw(
      textRenderer,
      OD,
      0.25f
    );

    popups.draw(
      textRenderer,
      OD,
      clock.getElapsedTime().asSeconds()
    );

    newRecording.draw(
      textRenderer,
      OD,
      0.25f
    );

    colours.draw(
      textRenderer,
      OD,
      0.25f
    );

    if(pause && !startUp){
      textRenderer.renderText(
        OD,
        "Space to resume",
        resX/2.,
        resY/2.,
        0.5,
        glm::vec3(0.,0.,0.),
        1.0,
        true
      );
    }
    else if(pause && startUp){
      textRenderer.renderText(
        OD,
        "This app can produce flashing colours.\nClick the \"Colours\" button to suppress them.\nPress space to begin.",
        resX/2.0,
        resY-128.0,
        0.5,
        glm::vec3(0.,0.,0.),
        1.0,
        true
      );
    }

    window.display();

    deltas[frameId] = clock.getElapsedTime().asSeconds();
    renderDeltas[frameId] = renderClock.getElapsedTime().asSeconds();

    clock.restart();

    if (frameId == 60){
      frameId = 0;
    }
    else{
      frameId++;
    }
  }

  return 0;
}
