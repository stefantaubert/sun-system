# Sunsystem with OpenGL
One of my projects as students was the development of an OpenGL application with C++. I have implemented the sun-system as OpenGL application with textured planets which rotates around the sun and around itself. The sun is shining to all planets as a point light which can be turned on and off.
The planetsize of the diameter and the rotation speed around the sun is true to scale. The own rotation speed is constant for each planet. Also the distance of each planet to the sun is equal to all for a better overview.

<p align="center">
  <img alt="sunsystem" src="/screenshots/sunsystem.png">
</p>

Earth with satellite:

<p align="center">
  <img alt="scenegraph" src="/screenshots/earth.png">
</p>

The animation of the sunsystem can be stopped and resumed and the speed of the planets can be increased or decreased.

You can choose between two camera views:
- view to complete sunsystem 
- view to single planets

The scenegraph looks like:

<p align="center">
  <img alt="scenegraph" src="/res/other/scenegraph.png">
</p>

With a blending the cloud cover is placed onto the earth and rotates with the earth. With Blender I created a satellite which rotates around the earth:

<p align="center">
  <img alt="scenegraph" src="/screenshots/satellite.png">
</p>

## Manual

- f: toggle fps
- h: help
- k: toggle coordinate system 
- w: toggle wireframe
- l: toggle lightning
- c: toggle backfaceculling
- i: initialize camera settings
- a: toggle animation (only by viewing at sun)
- x: toggle view to sun from planet
- left/right: toggle camera
- up/down: adjust rotation speed 

## Acknowledgements

Source of template:
- https://www.tu-chemnitz.de/informatik/HomePages/GDV/lehrmaterial.php

Sources of textures and infos:
- http://www.solarsystemscope.com/textures/
- http://hhh316.deviantart.com/art/Seamless-metal-texture-182943398
- http://henker144.deviantart.com/art/Gold-2-178145570
- https://astrokramkiste.de/planeten-tabelle
- http://ftextures.com/textures/solar-panel-module.jpg