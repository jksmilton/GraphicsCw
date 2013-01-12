Code for loading texture files is credited to opengl-tutorial.org.

CONTROLS
up: accelerates the player
down: decelerates the player
left: rotates anticlockwise
right: rotates clockwise
page up: increase elevation
page down: decrease elevation
home: look up
end: look down
p: go to screen shot location
q OR esc: quit
t: starts tour

FILES:
main.cpp: contains the code required to compile and run the application
shaded.frag: Fragment shader for the application
shaded.vert: Vertex shader for the application
screenshot.jpg: screenshot of the application with the ship just leaving its silo
mars.obj: Object file that contains all models for the application
~.tga: All .tga files contain textures used in the application
GraphicsCw, Project1, and Project1.vcxproj are ide files for visual studio. GraphicsCw will open the application in the ide

On windows, the easiest way to build the application is by using the GraphicsCw file to open the solution in visual studio and using visual studio to build the application.

The application creates a small martian landscape which the user can explore. By pressing t, the user will be placed on a preplanned tour of the landscape.
