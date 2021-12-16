A recreation of the flash game "Learn to Fly" using OpenGL.

# How to Play
You are learning to fly with your rocket. Your goal is to reach the moon! Unfortunately, your rocket is too weak right now. Collect coins along the path to upgrade your rocket. Avoid the obstacles; red obstacles knock you back and black obstacles reduce your fuel by 20.


At the menu, click on the upgrades to use your Coins to purchase upgrades.


On the game screen, the amount of fuel you have left is displayed on the bottom-left. Once your fuel reaches 0, you will be redirected to the main menu. The number of coins you have is on the top-left. You can use these coins to purchase upgrades, and they do not expire (i.e. you can choose to save your coins after each round instead of purchasing anything).

## Controls
### Gameplay:
- Move: W/A/S/D (Up/Left/Down/Right)
- Exit: Q/ESC
- Toggle birds-eye view: V
- Pause: P
- Destroy Obstacle: Left/Right Click
### Shop/Menu:
- Start next run: Space bar
- Purchase Upgrades: Mouse
- Exit: Q/ESC
### Game Over screen:
- Activate free play: Space bar
- Purchase Upgrades: Mouse
- Exit: Q/ESC

# Features

## Obstacles and Coins
- Collision detection with Rocket
- Upon collision updates state of game. Obstacles reduce fuel total and Coins increment currency used for shop
- Bombs explode upon impacting, sending the rocket backwards
- Randomly generated as the game goes on and on each run
- Disappear when rocket has flown past to create room for other obstacles
- Player can use LMB/RMB to shoot an object away [(1)](#misc-notes-to-marker)

## Scenery
- Clouds and stars will be generated depending on how high up you are
- These items do not impact gameplay; you are free to collide into them with no penalty
- All scenery items are translucent (alpha=0.5 or 0.75) to differentiate from obstacles and coins

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

# Resources Referenced
## OBJ loader:
- http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/ 
- https://stackoverflow.com/questions/40107359/obj-file-parser-with-opengl

## Textures
- https://docs.gl/es2/glGenTextures

## Where 
- Coin: Custom-made in blender (simple cylinder object)
- Rocket: https://www.turbosquid.com/3d-models/free-obj-model-rocket/665662
- Cloud: https://www.cgtrader.com/items/644366
- Star: https://www.cgtrader.com/free-3d-models/architectural/decoration/star--4

## Text Display
- https://stackoverflow.com/questions/18847109/displaying-fixed-location-2d-text-in-a-3d-opengl-world-using-glut
- http://www.lighthouse3d.com/tutorials/glut-tutorial/bitmap-fonts-and-orthogonal-projections/

## Blending
- https://learnopengl.com/Advanced-OpenGL/Blending

# Other
- Referenced class notes as well as A4 (for raypicking)

## Misc. Notes to Marker
- Raypicking does not work 100% of the time - it seems like it's slightly off sometimes. Raypicking still works while game is paused if you'd like to test. A ray is also drawn from the object as a visualization.
- The values chosen for upgrades are mainly used for dev/test purposes - in a real game, the upgrades would be less impactful. (Basically, we know that the upgrades are overpowered as they are! - but hopefully we're not being marked for game balance)
- Concepts implemented are:
    - Camera, 3D transformations, etc.
    - Object (.obj file) loader
    - Lighting
    - Textures
    - Raypicking
    - Blending
    - Non-Geometric Primitives (Drawing Text and User Interface)
    - Particle Systems
    
