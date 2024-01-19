# Jurassic_World_Alive_OpenGL

# Subject
The subject of the project consists in the photorealistic presentation of 3D objects using OpenGL library. The user 
directly manipulates by mouse and keyboard inputs the scene of objects and some other functionalities and ways 
to interact with this scene.
The scene that I implemented represents an oasis from the lost world of dinosaurs. The map is concentrated around 
the oasis, where the majority of the dinosaurs are. The rest of the scene is represented by an empty desert. 
# Scene and object description
The central point of my map is the blue oasis, which is surrounded by vegetation such as palm trees and some 
bushes. And all around the body of water you can see dinosaurs such as Brachiosaurus, Stegosaurus, T-rex, Deinonychus, 
the skull of a Triceratops and a Pteranodon flying around.
<p align="center">
  <img src="https://github.com/sergiugaga14/Jurassic_World_Alive_OpenGL/blob/master/dino_1.png" width="700" title="screenshot of the application">
</p>
The map can get very mysterious when the fog effect is activated, its density increases with every press of the F 
key.
<p align="center">
  <img src="https://github.com/sergiugaga14/Jurassic_World_Alive_OpenGL/blob/master/dino_2.png" width="700" title="screenshot of the application">
</p>
Even though the oasis seems to be a calm place, a disaster is about to happen, a meteor of great size is about to 
strike the land and bring an end to the era of dinosaurs.
<p align="center">
  <img src="https://github.com/sergiugaga14/Jurassic_World_Alive_OpenGL/blob/master/dino3.png" width="700" title="screenshot of the application">
</p>

# Functionalities
Right at the beginning the camera moves through the map in a circle around the oasis so that the majority 
of the objects can be seen.
The camera animation can be stopped and then the user can use the keyboard and the mouse to move 
around and see the objects better. The user can also change the view mode from solid to wireframe or point view.
The fog effect can be activated and the density of the fog can be increased/decreased from the keyboard.
I have two types of lights: directional light and spot light. The directional light has a white cube around it 
and the cube can be moved up and down and it can also be rotated around a point on the the map describing a 
circle. The spot light as well as the meteor appear and disappear by pressing a key on the keyboard, this appearance 
comes with the night effect, I changed the color of the directional light so as to seem like it’s night outside, plus 
the spot light can be seen better.
I also have the shadows computed and they can be seen below the dinosaurs and the trees in the pictures 
above.
An animation can also be seen, the pteranodon is moving by itself above the oasis.
