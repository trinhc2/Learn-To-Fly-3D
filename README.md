A recreation of the flash game "Learn to Fly" using OpenGL.

# Features

## Obstacles and Coins
- Collision detection with Rocket
- Upon collision updates state of game. Obstacles reduce fuel total and Coins increment currency used for shop
- Bombs explode upon impacting, sending the rocket backwards
- Randomly generated as the game goes on and on each run
- Disappear when rocket has flown past to create room for other obstacles

## Menu/Shop
- Player is able to purchase upgrades for their rocket at the menu/shop after each run by clicking on the interface
- Players return to shop whenever their run is ended (fuel = 0)
- Furthest distance reached is displayed

## Rocket
- Rocket is able to move using "WASD" keys
- Rotates upon turning to simulate real turning behaviour
- Implemented particle system to simulate firing of rocket

## Camera
- Camera moves when rocket is turning
- Alternate camera perspective provided by pressing "V"
- Camera positioned behind rocket to simulate moving forward although objects are moving upwards

## OBJ and Textures
- OBJ parser implemented to allow the game objects to look like what they are supposed to represent
- Textures implemented to be used in the future for game objects

# What's Left 
- An end goal to the game
- More powerups/upgrades
- Terrain generation (showing user moving from earth and gradually into space)
- Different types of obstacles, some with special features (ex. moving)
- Textures to give the game a nicer look
- Lighting
- Developing game system/mechanics/user interface/etc
- More...

# Resources Referenced
## OBJ loader:
- http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/ 
- https://stackoverflow.com/questions/40107359/obj-file-parser-with-opengl

## Textures
- https://docs.gl/es2/glGenTextures

## Where 
Coin: Custom-made in blender (simple cylinder object)
Rocket: https://www.turbosquid.com/3d-models/free-obj-model-rocket/665662

## Text Display
- https://stackoverflow.com/questions/18847109/displaying-fixed-location-2d-text-in-a-3d-opengl-world-using-glut
- http://www.lighthouse3d.com/tutorials/glut-tutorial/bitmap-fonts-and-orthogonal-projections/
