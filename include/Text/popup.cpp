#include <Text/popup.h>

void Popup::draw(TextRenderer & text, Type & type, float dt){
  for (int i = 0; i < popups.size(); i++){

    text.renderText(
      type,
      popups[i].text,
      popups[i].x,
      popups[i].y,
      0.5,
      popups[i].colour,
      popups[i].alpha()
    );

    popups[i].increment(dt);
  }

  popups.erase(
    std::remove_if(
        popups.begin(),
        popups.end(),
        [](FadingText & f) { return f.done(); }
    ),
    popups.end()
  );

}

void Popup::clear(std::string tag){
  popups.erase(
    std::remove_if(
        popups.begin(),
        popups.end(),
        [tag](FadingText & f) { return f.getTag() == tag; }
    ),
    popups.end()
  );
}
