# Flocks

A demo of a flocking model (Couzin like), **includes a playable predator!**

Uses C++ with SFML and OpenGL

### Controls

| Function     | Keys |
| ----------- | ----------- |
| track a particle | click it |
| start/pause | SPACE |
| spawn predator | P |
| speed up predator | W |
| slow down predator | S | 
| turn (anti-clockwise) predator | A |
| turn (clockwise) ppredator | D |
| slow down time | L |
| speed up time | H |
| toggle debug menu      | F1      |

### Building

#### If building SFML from source (as included)

First ensure you have the SFML dependencies installed in ubuntu this can be achieved via

```console
apt-get install build-essential mesa-common-dev libx11-dev libxrandr-dev libgl1-mesa-dev liblgu1-mesa-dev libfreetype6-dev libopenal-dev libsndfile1-dev libudev-dev
```

Additionally install the glm developement libraries

```console
apt-get install libglm-dev
```

Run 

```console
./dependencies.sh && ./build.sh
```

which will build sfml statically and run cmake to build the game
