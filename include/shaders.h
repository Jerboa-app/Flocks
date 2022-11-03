#ifndef SHADERS_H
#define SHADERS_H

const char * trackVertexShader = "#version 330 core\n"
  "layout(location=0) in vec3 a_position;\n"
  "uniform mat4 proj; uniform float time;\n"
  "out float alpha;\n"
  "void main(){\n"
    "gl_Position = proj*vec4(a_position.xy,0.0,1.0);\n"
    "gl_PointSize = 2.0;\n"
    "alpha = a_position.z/time;"
  "}";

const char * trackFragmentShader = "#version 330 core\n"
  "out vec4 colour; in float alpha;\n"
  "void main(){"
  " vec2 c = 2.0*gl_PointCoord-1.0;\n"
  " float d = length(c);\n"
  // bit of simple AA
  " float a = 1.0-smoothstep(0.99,1.01,d);\n"
  " colour = vec4(0.0,0.0,0.0,alpha);\n"
  " if (colour.a == 0.0){discard;}"
  "}";

const char * shakerVertexShader = "#version 330 core\n"
  "layout(location=0) in vec2 a_position;\n"
  "\n"
  "uniform mat4 proj;\n"
  "uniform float offset;\n"
  "void main(){\n"
  " gl_Position = proj*vec4(a_position.x,a_position.y+offset,0.0,1.0);"
  " \n"
  "}";

const char * shakerFragmentShader = "#version 330 core\n"
  "uniform vec4 u_colour; out vec4 colour;\n"
  "void main(){colour=u_colour;\n}";

const char * boxVertexShader = "#version 330 core\n"
  "layout(location=0) in vec2 a_position;\n"
  "uniform vec3 colour; out vec4 o_colour;\n"
  "uniform mat4 proj;\n"
  "void main(){\n"
  " vec4 pos = vec4(a_position.xy,0.0,1.0);\n"
  " o_colour = vec4(colour,.5);\n"
  " gl_Position = pos;\n"
  "}";

const char * boxFragmentShader = "#version 330 core\n"
  "in vec4 o_colour; out vec4 colour;\n"
  "void main(){colour=o_colour;\n}";

const char * sliderVertexShader = "#version 330 core\n"
  "layout(location=0) in vec2 a_position;\n"
  "uniform mat4 proj;\n"
  "void main(){\n"
  " gl_Position = proj*vec4(a_position.xy,0.0,1.0);\n"
  "}";

const char * sliderFragmentShader = "#version 330 core\n"
  "uniform vec4 frameColour; uniform vec4 fillColour;\n"
  "uniform vec2 state;\n"
  "out vec4 colour;\n"
  "void main(){"
    "if (gl_FragCoord.x > state.x && gl_FragCoord.x < state.y){colour=fillColour;}"
    "else {colour = frameColour;}"
  "}";

const char * buttonVertexShader = "#version 330 core\n"
  "layout(location=0) in vec2 a_position;\n"
  "uniform mat4 proj;\n"
  "void main(){\n"
  " gl_Position = proj*vec4(a_position.xy,0.0,1.0);\n"
  "}";

const char * buttonFragmentShader = "#version 330 core\n"
  "uniform vec4 frameColour; uniform vec4 fillColour;\n"
  "uniform int state; uniform float alpha;\n"
  "out vec4 colour;\n"
  "void main(){"
    "colour=fillColour*alpha+frameColour;\n"
  "}";


// basic particle shader
// cmap(t) defines a periodic RGB colour map for t \in [0,1] using cubic
// interpolation that's hard coded, it's based upon the PHASE4 colour map
// from https://github.com/peterkovesi/PerceptualColourMaps.jl
// which is derived from ColorCET https://colorcet.com/
const char * particleVertexShader = "#version 330 core\n"
  "#define PI 3.14159265359\n"
  "precision highp float;\n"
  "layout(location = 0) in vec3 a_position;\n"
  "layout(location = 1) in vec4 a_offset;\n"
  "uniform mat4 proj; uniform float scale; uniform float zoom;\n"
  "uniform int tracked;\n"
  "out vec4 o_colour;\n"
  "void main(){\n"
  " vec4 pos = proj*vec4(a_offset.xy,0.0,1.0);\n"
  " gl_Position = vec4(a_position.xy+pos.xy,0.0,1.0);\n"
  " gl_PointSize = a_offset.w*scale*zoom;\n"
  "if (tracked == gl_InstanceID){o_colour = vec4(1.0,0.0,0.0,1.0);}"
  "else{ o_colour = vec4(0.5,0.5,1.0,1.0); }\n"
  "}";
const char * particleFragmentShader = "#version 330 core\n"
  "in vec4 o_colour; out vec4 colour;\n"
  "void main(){\n"
  " vec2 c = 2.0*gl_PointCoord-1.0;\n"
  " float d = length(c);\n"
  // bit of simple AA
  " float alpha = 1.0-smoothstep(0.95,1.05,d);\n"
  " colour = vec4(o_colour.rgb,alpha);\n"
  " if (colour.a == 0.0){discard;}"
  "}";

#endif
