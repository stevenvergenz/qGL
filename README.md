# qGL #

An OpenGL/Qt demo application. Loads a model and allows flying through/around. Includes a shadow volume cast by an overhead polygon.

This program is compiled against Qt 4.7 and GLEW 1.5.7 under Linux 64-bit. Both libraries are available for all platforms, but system paths will have to be modified in code to run under Windows. Written for a computer graphics course in 2011.

The provided model is the property of Enrico Dalbosco (arrigosilva.blogspot.com), and was freely available from Google Sketchup as of this authoring.

### Dependencies ###

* Qt 4.7
* GLEW 1.5.7
* glm
* DevIL
* ASSIMP
* GLSL 3.3 shader support

### Controls ###

W/S - move forward/backward
A/D - move left/right
Q/E - move up/down
+/- - change movement speed

Arrow left/right to rotate the light source
Click+Drag to pan the camera view
Spacebar to reset the camera view to default
Escape to quit

