# Big Nuts Rise

A demo simulation of granular materials and a shaker, play with all the parrameters live!

Uses C++ with SFML and OpenGL

### Controls

| Function     | Keys |
| ----------- | ----------- |
| toggle debug menu      | F1      |

![](https://github.com/Jerboa-app/BigNutsRise/blob/main/resources/demo1.gif)
![](https://github.com/Jerboa-app/BigNutsRise/blob/main/resources/demo3.gif)

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
