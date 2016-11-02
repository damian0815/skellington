# skellington
Implementation of Le, Hodgins 2016 paper *Real-time Skeletal Skinning with Optimized Centers of Rotation* https://www.disneyresearch.com/publication/skinning-with-optimized-cors/

![animated gif of testOptimizedCentersOfRotation](https://media.giphy.com/media/11m3RYQpBj0R5C/giphy.gif)

Requires `glm`, `assimp`, `glfw3`, `fmt`. On MacOSX with homebrew and cmake 3.5+ installed: 
```
brew install glm fmt assimp glfw3
ccmake .
make
./testOptimizedCentersOfRotation
```
