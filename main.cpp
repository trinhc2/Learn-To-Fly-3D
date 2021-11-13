#include <iostream>
#include <algorithm>
#include "mathLib3D.h"
#include "coin.h"
#include "rocket.h"
#include "obstacle.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>

#endif

//States for the game
enum Screen { game, menu };

bool cameraToggle = false;

Screen screen = menu; //screen state

void *font = GLUT_BITMAP_HELVETICA_18; //Font which is used for glutBitMapCharacter

Rocket rocket = Rocket();
CoinSystem coinSystem = CoinSystem();
ObstacleSystem obstacleSystem = ObstacleSystem();

float maxForwardingDistance = 0; // Used to keep track of player score

/**
 * Checks if the rocket is within a range of coordinates
 */
bool inRangeX(int high, int low, Rocket r) {
  return (low <= r.position.mX && r.position.mX <= high);
}

bool inRangeZ(int high, int low, Rocket r) {
  return (low <= r.position.mZ && r.position.mZ <= high);
}

bool inRangeY(int high, int low, Rocket r) {
  return (low <= r.position.mY + r.forwardDistance && r.position.mY + r.forwardDistance <= high);
}

/**
 * Checks if the rocket has collided with the coins
 * 
 * @rocket reference to our rocket object
 * @v reference to our list of coins from CoinSystem
 */
void checkCoinCollision(Rocket &rocket, std::vector<Coin> &v) {
  for (Coin &c: v) {
	//The boundaries are off but intended behaviour works
	if (inRangeY(c.position.mY + 0.9, c.position.mY - 0.9, rocket)
		&& inRangeZ(c.position.mZ + 0.9, c.position.mZ - 0.9, rocket)) {
	  //if collision: set coin as collected (see Coinsystem::update) and increments earned coins
	  c.collected = true;
	  rocket.coins += 100;
	}
  }
}

/**
 * Checks if the rocket has collided with the obstacles
 *
 * @rocket reference to our rocket object
 * @v reference to our list of obstacles from ObstacleSystem
 */
void checkObstacleCollision(Rocket &rocket, std::vector<Obstacle> &v) {
  for (Obstacle &o: v) {
	if (inRangeY(o.position.mY + 0.9, o.position.mY - 0.9, rocket)
		&& inRangeZ(o.position.mZ + 0.9, o.position.mZ - 0.9, rocket)) {
	  o.destroyed = true;
	  rocket.fuel -= rocket.collisionFuelPenalty;
	}
  }
}

/**
 * Draws the rocket to screen
 */
void drawRocket(Rocket rocket) {
  glColor3f(1, 0, 0);
  glPushMatrix();
  //Place the rocket at its position + how much it has traveled
  glTranslatef(rocket.position.mX, rocket.position.mY + rocket.forwardDistance, rocket.position.mZ);
  //Rotate the rocket if it has been turning
  glRotatef(rocket.angle, 0, 1, 0);
  //Scales the rocket size down, scales can be updated in future
  glScalef(0.3, 0.3, 0.3);
  glutSolidCube(1);
  glPopMatrix();
}

/**
 * Draws the coins to the screen
 */
void drawCoins(CoinSystem coinSystem) {
  for (std::size_t i = 0; i < coinSystem.v.size(); i++) {
	glColor3f(1, 1, 0);
	glPushMatrix();
	glTranslatef(coinSystem.v.at(i).position.mX, coinSystem.v.at(i).position.mY, coinSystem.v.at(i).position.mZ);
	glRotatef(coinSystem.rotation, 1, 0, 0);
	glutSolidCube(1);
	glPopMatrix();
  }
}

/**
 * Draws the obstacles to the screen
 */
void drawObstacles(ObstacleSystem obstacleSystem) {
  for (std::size_t i = 0; i < obstacleSystem.v.size(); i++) {
	glColor3f(0, 1, 0);
	glPushMatrix();
	glTranslatef(obstacleSystem.v.at(i).position.mX,
				 obstacleSystem.v.at(i).position.mY,
				 obstacleSystem.v.at(i).position.mZ);
	glutSolidCube(obstacleSystem.v.at(i).size);
	glPopMatrix();
  }
}

/**
 * Draws text to screen
 * 
 * Needs positioning and colour before calling this function 
 */
void drawText(std::string text) {
  for (std::string::iterator i = text.begin(); i != text.end(); ++i) {
	char c = *i;
	glutBitmapCharacter(font, c);
  }
}

void display(void) {
  if (screen == game) { //If state of screen is on game, draw the game
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//For testing
	if (cameraToggle) {
	  gluLookAt(7, 7, 7, rocket.position.mX, rocket.position.mY + rocket.forwardDistance, rocket.position.mZ, 0, 1, 0);
	} else {
	  gluLookAt(2, -8 + rocket.forwardDistance, 0, 0, rocket.forwardDistance, 0, 1, 0, 0);
	}

	glColor3f(0, 0, 1);

	//Draws the ground plane
	glPushMatrix();
	// make y arbitrarily high to simulate an infinite long road ahead
	glScalef(0.1, 10000, 10);
	glutSolidCube(1);
	glPopMatrix();

	drawRocket(rocket);
	drawObstacles(obstacleSystem);
	drawCoins(coinSystem);

	/**
	 * Displays Text
	 * Source: https://stackoverflow.com/questions/18847109/displaying-fixed-location-2d-text-in-a-3d-opengl-world-using-glut
	 * Explanation: http://www.lighthouse3d.com/tutorials/glut-tutorial/bitmap-fonts-and-orthogonal-projections/
	 *
	 */
	//Display Text
	glColor3f(1, 1, 1); //Text is white
	glMatrixMode(GL_PROJECTION); //Setting matrix to projection so we can specify position of text in pixels

	glPushMatrix(); //Pushing matrix on top of stack (saving previous display)
	glLoadIdentity();
	gluOrtho2D(0.0, 600, 0.0, 600);
	glMatrixMode(GL_MODELVIEW);

	//Display Fuel
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1, 1, 1);

	glRasterPos2i(10, 10);
	std::string fuelDisplay = "Fuel: " + std::to_string(rocket.fuel);
	drawText(fuelDisplay);

	//Display amount of coins
	glRasterPos2i(10, 580);
	std::string coinDisplay = "Coins: " + std::to_string(rocket.coins);
	drawText(coinDisplay);

	//Undoing changes to display
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glutSwapBuffers();
  } else if (screen == menu) { //if state of screen is on menu, draw the menu
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 600, 0.0, 600);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glLoadIdentity();

	glColor3f(1, 1, 1);
	glRasterPos2i(10, 580);
	std::string coinDisplay = "Coins: " + std::to_string(rocket.coins);
	drawText(coinDisplay);

	std::string scoreDisplay = "    Max Score: " + std::to_string(maxForwardingDistance);
	drawText(scoreDisplay);

	glRasterPos2i(220, 10);
	drawText("Press Space Bar to play");

	glRasterPos2i(10, 500);
	drawText("(1) 100 Coins: Increase Fuel by 100");

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glutSwapBuffers();
  }
}

void keyboard(unsigned char key, int x, int y) {
  if (screen == game) {
	switch (key) {
	  case 'q':exit(0);
		break;
		/*esc*/
	  case 27:exit(0);
		break;
	  case 'a':
		//Turn the rocket left and rotates
		if (rocket.zOffset < 2) {
		  rocket.zOffset += 0.2;
		  rocket.angle -= 1;

		}
		break;
	  case 'd':
		//Turn the rocket right and rotates
		if (rocket.zOffset > -2) {
		  rocket.zOffset -= 0.2;
		  rocket.angle += 1;

		}
		break;
	  case 'v':cameraToggle = !cameraToggle;
		break;
	}
  } else if (screen == menu) {
	switch (key) {
	  case 'q':exit(0);
		break;

	  case 27: //escape
		exit(0);
		break;
	  case 32: //space bar
		rocket.forwardDistance = 0;
		rocket.fuel += rocket.initialFuel + rocket.fuelUpgrades;
		screen = game;
		break;
	  case '1':
		if (rocket.coins >= 100) {
		  rocket.coins -= 100;
		  rocket.fuel += 100;
		}
		break;
	}
  }
  glutPostRedisplay();
}

void FPS(int val) {
  if (screen == game) {
	rocket.update();
	maxForwardingDistance = std::max(maxForwardingDistance, rocket.forwardDistance);
	coinSystem.update();
	obstacleSystem.update();
	checkCoinCollision(rocket, coinSystem.v);
	checkObstacleCollision(rocket, obstacleSystem.v);
	if (rocket.fuel <= 0) {
	  screen = menu;
	}
  } else if (screen == menu) {

  }

  glutPostRedisplay();
  glutTimerFunc(17, FPS, 0);
}

void init(void) {
  glColor3f(1, 1, 1);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho(-1, 1, -1, 1, -20, 20);
  gluPerspective(70, 1, 1, 20);
}

/* main function - program entry point */
int main(int argc, char **argv) {
  glutInit(&argc, argv);        //starts up GLUT
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(600, 600);

  glutCreateWindow("Learn to Fly");    //creates the window

  glutDisplayFunc(display);    //registers "display" as the display callback function
  glutKeyboardFunc(keyboard);
  glutTimerFunc(17, FPS, 0);

  glEnable(GL_DEPTH_TEST);

  glFrontFace(GL_CW);
  glCullFace(GL_FRONT);
  glEnable(GL_CULL_FACE);

  init();

  glutMainLoop();                //starts the event loop

  return (0);                    //return may not be necessary on all compilers
}