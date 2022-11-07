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
  "uniform int colourTheta;\n"
  "layout(location = 0) in vec3 a_position;\n"
  "layout(location = 1) in vec4 a_offset;\n"
    "float poly(float x, vec4 param){return clamp(x*param.x+pow(x,2.0)*param.y+"
  " pow(x,3.0)*param.z+param.w,0.0,1.0);\n}"
  "vec4 cmap(float t){\n"
  " return vec4( poly(t,vec4(1.2,-8.7,7.6,0.9)), poly(t,vec4(5.6,-13.4,7.9,0.2)), poly(t,vec4(-7.9,16.0,-8.4,1.2)), 1.0 );}"
  "uniform mat4 proj; uniform float scale; uniform float zoom;\n"
  "uniform int tracked;\n"
  "out vec4 o_colour; out float theta;\n"
  "void main(){\n"
  " vec4 pos = proj*vec4(a_offset.xy,0.0,1.0);\n"
  " gl_Position = vec4(a_position.xy+pos.xy,0.0,1.0);\n"
  " gl_PointSize = a_offset.w*scale*zoom;\n"
  "o_colour = vec4(0.5,0.5,1.0,1.0);"
  "if (tracked == gl_InstanceID){o_colour = vec4(1.0,0.0,0.0,1.0);}"
  "else{"
  " if (colourTheta==1) {o_colour = cmap(mod(a_offset.z,2.0*PI)/(2.0*PI));}"
  "}\n"
  "theta = a_offset.z;\n"
  "}";
const char * particleFragmentShader = "#version 330 core\n"
  "in vec4 o_colour; out vec4 colour; in float theta;\n"
  "uniform float zoom;\n"
  "void main(){\n"
  " vec2 c = 2.0*gl_PointCoord-1.0;\n"
  " float d = length(c);\n"
  // bit of simple AA that adapts to zoom
  " float dz = (1.0-smoothstep(1.0,4.0,zoom))*0.1;\n"
  " float alpha = 1.0-smoothstep(0.99-dz,1.01+dz,d);\n"
  " colour = vec4(o_colour.rgb,alpha);\n"
  " float de = distance(c,0.66*vec2(cos(theta),-sin(theta)));\n"
  " if (de < 0.33){"
  // mix for the eye
  "    colour = vec4(1.0,1.0,1.0,1.0);\n"
  " }"
  " if (de > 0.3 && de < 0.375){colour = mix(vec4(o_colour.rgb,alpha),vec4(1.0,1.0,1.0,alpha),1.0-smoothstep(0.3,0.375,de));}\n"
  " if (colour.a == 0.0){discard;}"
  "}";

const char * predatorVertexShader = "#version 330 core\n"
  "#define PI 3.14159265359\n"
  "precision highp float;\n"
  "uniform int colourTheta;\n"
  "layout(location = 0) in vec4 a_position;\n"
  "uniform mat4 proj; uniform float scale; uniform float zoom;\n"
  "out vec4 o_colour; out float theta;\n"
  "void main(){\n"
  " gl_Position = proj*vec4(a_position.xy,0.0,1.0);\n"
  " gl_PointSize = a_position.w*scale*zoom;\n"
  "o_colour = vec4(1.0,0.5,0.5,1.0);"
  "theta = a_position.z;\n"
  "}";
const char * predatorFragmentShader = "#version 330 core\n"
  "in vec4 o_colour; out vec4 colour; in float theta;\n"
  "uniform float zoom;\n"
  "void main(){\n"
  " vec2 c = 2.0*gl_PointCoord-1.0;\n"
  " float d = length(c);\n"
  // bit of simple AA that adapts to zoom
  " float dz = (1.0-smoothstep(1.0,4.0,zoom))*0.05;\n"
  " float alpha = 1.0-smoothstep(0.99-dz,1.01+dz,d);\n"
  " colour = vec4(o_colour.rgb,alpha);\n"
  " float de = distance(c,0.66*vec2(cos(theta),-sin(theta)));\n"
  " if (de < 0.33){"
  // mix for the eye
  "    colour = vec4(1.0,1.0,1.0,1.0);\n"
  " }"
  " if (de > 0.3 && de < 0.375){colour = mix(vec4(o_colour.rgb,alpha),vec4(1.0,1.0,1.0,alpha),1.0-smoothstep(0.3,0.375,de));}\n"
  " if (colour.a == 0.0){discard;}"
  "}";

#endif
