#ifndef SLIDERGROUP_H
#define SLIDERGROUP_H

#include <Menu/slider.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <map>

class SliderGroup {
public:

    SliderGroup(glm::mat4 p)
    : proj(p)
    {}

    void add(
        std::string name,
        float x, float y, float w, float h,
        std::string label
        ){
        Slider slider(x,y,w,h,label);
        slider.setProjection(proj);
        sliders.insert(std::pair<std::string,Slider>(name, slider));
    }

    void draw(
        TextRenderer & text,
        Type & type,
        float scale = 0.5f
    ){
        for (auto iter = sliders.begin(); iter != sliders.end(); iter++){
            (*iter).second.draw(text,type,scale);
        }
    }

    void clicked(float x, float y){
        for (auto iter = sliders.begin(); iter != sliders.end(); iter++){
            (*iter).second.clicked(x,y);
        }
    }

    void drag(float x, float y){
        for (auto iter = sliders.begin(); iter != sliders.end(); iter++){
            (*iter).second.drag(x,y);
        }
    }

    void mouseUp(){
        for (auto iter = sliders.begin(); iter != sliders.end(); iter++){
            (*iter).second.mouseUp();
        }
    }

    float getPosition(std::string name){
        if (sliders.find(name)!=sliders.end()){
            return sliders[name].getPosition();
        }
    }

    void setPosition(std::string name, float value){
        if (sliders.find(name)!=sliders.end()){
            return sliders[name].setPosition(value);
        }
    }

    void setLabel(std::string name, std::string label){
        if (sliders.find(name)!=sliders.end()){
            return sliders[name].setLabel(label);
        }
    }

    void setEnabled(std::string name, bool enabled){
        if (sliders.find(name)!=sliders.end()){
            return sliders[name].setEnabled(enabled);
        }
    }

    void smoothChangeTo(std::string name, float to, uint64_t p = 60, float r = 0.0){
        if (sliders.find(name)!=sliders.end()){
            sliders[name].smoothChangeTo(to,p,r);
        }
    }

    void setSmoothChange(std::string name, bool b, uint64_t p = 60, float r = 0.0){
        if (sliders.find(name)!=sliders.end()){
            sliders[name].setSmoothChange(b,p,r);
        }
    }

private:
    std::map<std::string,Slider> sliders;  
    glm::mat4 proj;
};

#endif