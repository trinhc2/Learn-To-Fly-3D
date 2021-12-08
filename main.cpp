#include <iostream>
#include <algorithm>
#include "mathLib3D.h"
#include "coin.h"
#include "rocket.h"
#include "obstacle.h"
#include "particle.h"
#include <string>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#include <GL/glut.h>

#endif

//States for the game
enum Screen { game, menu };

bool cameraToggle = false;

//Detection for coin/obstacles, will display text for a certain amount of time (age value is decremented in FPS)
bool coinGet = false;
bool obstacleHit = false;
float obsHitAge = 0;
float coinGetAge = 0;
// flag for breaking the previous record
bool breakRecord = false;

Screen screen = menu; //screen state

float rocketAmbient[4] = {0.19225f, 0.19225f, 0.19225f, 1.0f};
float rocketDiffuse[4] = {0.50754f, 0.50754f, 0.50754f, 1.0f};
float rocketSpecular[4] = {0.508273f, 0.508273f, 0.508273f, 1.0f};
float rocketShine = 51.2f;

float coinAmbient[4] = {0.24725f, 0.1995f, 0.0745f, 1.0f};
float coinDiffuse[4] = {0.75164f, 0.60648f, 0.22648f, 1.0f};
float coinSpecular[4] = {0.628281f, 0.555802f, 0.366065f, 1.0f};
float coinShine = 51.2f;

float obstacleAmbient[4] = {0.05375f, 0.05f, 0.06625f, 0.82f};
float obstacleDiffuse[4] = {0.18275f, 0.17f, 0.22525f, 0.82f};
float obstacleSpecular[4] = {0.332741f, 0.328634f, 0.346435f, 0.82f};
float obstacleShine = 38.4f;

float particleAmb[3][4] = {{1.0f, 0.1f, 0.0f, 1.0f}, {1.0f, 0.4f, 0.0f, 1.0f}, {1.0f, 0.2f, 0.0f, 1.0f}};
float particleDiff[3][4] = {{1.0f, 0.1f, 0.0f, 1.0f}, {1.0f, 0.4f, 0.0f, 1.0f}, {1.0f, 0.2f, 0.0f, 1.0f}};
float particleSpec[3][4] = {{1.0f, 0.1f, 0.0f, 1.0f}, {1.0f, 0.4f, 0.0f, 1.0f}, {1.0f, 0.2f, 0.0f, 1.0f}};

float ambientDefault[4] = {0.2, 0.2, 0.2, 1.0};
float diffuseDefault[4] = {0.8, 0.8, 0.8, 1.0};
float specularDefault[4] = {0, 0, 0, 1};

void *font = GLUT_BITMAP_HELVETICA_18; //Font which is used for glutBitMapCharacter

Rocket rocket = Rocket();
CoinSystem coinSystem = CoinSystem();
ObstacleSystem obstacleSystem = ObstacleSystem();
ParticleSystem parSys = ParticleSystem();

float position[4] = {1, 5 + rocket.forwardDistance, 0, 1};
float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
float diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

float maxForwardingDistance = 0; // Used to keep track of player score
float prevMaxForwardingDistance = 0; // used to keep track max player score from previous plays

// Textures
GLubyte *img_data[3];
GLuint texture_map[3];
int width[3];
int height[3];
int max[3];

void printWelcomeMessage() {
  std::cout << "Welcome to Learn To Fly 3D!" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "***   INSTRUCTIONS   ***" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "You are learning to fly with your rocket!" << std::endl;
  std::cout << "Your goal is to reach space. Unfortunately, your rocket is too weak right now." << std::endl;
  std::cout
	  << "Collect coins along the path to upgrade your rocket. Avoid the obstacles; if you get hit, your fuel meter will decrease significantly."
	  << std::endl;
  std::cout << "At the menu, press the number on your keyboard to use your Coins to purchase upgrades." << std::endl;
  std::cout
	  << "On the game screen, the amount of fuel you have left is displayed on the bottom-left. Once your fuel reaches 0, you will be redirected to the main menu."
	  << std::endl;
  std::cout
	  << "The number of coins you have is on the top-left. You can use these coins to purchase upgrades, and they do not expire (i.e. you can choose to save your coins after each round instead of purchasing anything)."
	  << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "***     CONTROLS     ***" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "Move: A/D (left/right)" << std::endl;
  std::cout << "Exit: Q/ESC" << std::endl;
  std::cout << "Toggle birds-eye view: v" << std::endl;
  std::cout << "***   IN SHOP/MENU   ***" << std::endl;
  std::cout << "Start next run: Space bar" << std::endl;
  std::cout << "Purchase Upgrades: Number Keys" << std::endl;
  std::cout << "Exit: Q/ESC" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "***   EXTRA DETAILS  ***" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "todo :)" << std::endl;

}

/**
 * Checks if the rocket is within a range of coordinates
 */
bool inRangeX(float high, float low, Rocket r) {
  return (low <= r.position.mX && r.position.mX <= high);
}

bool inRangeZ(float high, float low, Rocket r) {
  return (low <= r.position.mZ && r.position.mZ <= high);
}

bool inRangeY(float high, float low, Rocket r) {
  return (low <= r.position.mY + r.forwardDistance && r.position.mY + r.forwardDistance <= high);
}

/*Generate a random float between 0 and x*/
float randomFloat(float x) {
  return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / x));
}

// Display obj
void displayObj(std::string name) {
  std::vector<Point3D> out_vertices;
  std::vector<Vec3D> out_normals;
  std::vector<Point2D> out_uvs;
  int size;

  // Note: vertexIndices, uvIndices, and normalIndices all have the same value
  if (name.compare("rocket") == 0) {
	out_vertices = rocket.out_vertices;
	out_normals = rocket.out_normals;
	out_uvs = rocket.out_uvs;
	size = rocket.vertexIndices.size();
  } else if (name.compare("coin") == 0) {
	out_vertices = coinSystem.out_vertices;
	out_normals = coinSystem.out_normals;
	out_uvs = coinSystem.out_uvs;
	size = coinSystem.vertexIndices.size();
  } else if (name.compare("obstacle") == 0) {
	out_vertices = obstacleSystem.out_vertices;
	out_normals = obstacleSystem.out_normals;
	out_uvs = obstacleSystem.out_uvs;
	size = obstacleSystem.vertexIndices.size();
  }

  // Draw triangles based on the vertices we read from our obj file
  // Each face consists of <vertex1, texture1, normal1, vertex2, texture2, normal2, vertex3, texture3, normal3>
  // That means we can loop through our "out" vectors and generate a bunch of vertices with normal and texture coords
  glPushMatrix();
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < size; i++) {
	//texture:
	Point2D t = out_uvs[i];
	glTexCoord2f(t.mX, t.mY);

	//normal:
	Vec3D v = out_normals[i];
	glNormal3f(v.mX, v.mY, v.mZ);

	//vertex:
	Point3D m = out_vertices[i];
	glVertex3f(m.mX, m.mY, m.mZ);
  }
  glEnd();

  glPopMatrix();
}

/**
 * Draws the rocket to screen
 */
void drawRocket(Rocket rocket) {
  glBindTexture(GL_TEXTURE_2D, texture_map[2]);
  glPushMatrix();

  glColor3f(0.60, 0.61, 0.62);//grey/silver colour

  //setting material
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rocketAmbient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rocketDiffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, rocketSpecular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, rocketShine);

  //Place the rocket at its position + how much it has traveled
  glTranslatef(rocket.position.mX, rocket.position.mY + rocket.forwardDistance, rocket.position.mZ);
  //Rotate the rocket if it has been turning
  glRotatef(2.5, 1, 0, 0); //points rocket more to the left so it pointing forwards by default
  glRotatef(-10, 0, 1, 0); //rotates rocket along y so it is not on a diagonal by default
  glRotatef(rocket.angle, -1, 1, 0);
  //Scales the rocket size down, scales can be updated in future
  glScalef(0.3, 0.3, 0.3);
  displayObj("rocket");
  glPopMatrix();
  // Reset texture binding after finishing draw
  glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Draws the coins to the screen
 */
void drawCoins(CoinSystem coinSystem) {
  glBindTexture(GL_TEXTURE_2D, texture_map[0]);
  glColor3f(1, 1, 0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, coinAmbient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, coinDiffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, coinSpecular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, coinShine);
  for (std::size_t i = 0; i < coinSystem.v.size(); i++) {
	glPushMatrix();
	glTranslatef(coinSystem.v.at(i).position.mX, coinSystem.v.at(i).position.mY, coinSystem.v.at(i).position.mZ);
	glRotatef(coinSystem.rotation, 1, 0, 0);
	displayObj("coin");
	glPopMatrix();
  }
  // Reset texture binding after finishing draw
  glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Draws the obstacles to the screen
 */
void drawObstacles(ObstacleSystem obstacleSystem) {
  glBindTexture(GL_TEXTURE_2D, texture_map[1]);
  glColor3f(0, 1, 0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, obstacleAmbient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, obstacleDiffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, obstacleSpecular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, obstacleShine);

  for (std::size_t i = 0; i < obstacleSystem.v.size(); i++) {
	glPushMatrix();
	glTranslatef(obstacleSystem.v.at(i).position.mX,
				 obstacleSystem.v.at(i).position.mY,
				 obstacleSystem.v.at(i).position.mZ);
	displayObj("obstacle");
	// glutSolidCube(obstacleSystem.v.at(i).size);
	glPopMatrix();
  }
  // Reset texture binding after finishing draw
  glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Draws Rocket trail 
 */
void drawParticles(ParticleSystem parSys) {
  if (parSys.v.size() > 0) {
	for (Particle p: parSys.v) {
	  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, particleAmb[p.material]);
	  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, particleDiff[p.material]);
	  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, particleSpec[p.material]);
	  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32);

	  std::cout << particleAmb[p.material][1] << "\n";
	  glPushMatrix();
	  glColor3f(p.r, p.g, p.b);

	  glTranslatef(p.position.mX, p.position.mY, p.position.mZ);
	  glScalef(0.05, 0.05, 0.05);
	  glutSolidCube(p.size);
	  glPopMatrix();
	}
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

// draw a virtual line in the game scene indicating the previous maxForwardingDistance
void drawPreviousMaxScoreIndicatorLine() {
  // only display the indicator line if there are existing records from the previous plays
  if (prevMaxForwardingDistance > 0) {
	glColor3f(255, 255, 255);
	glBegin(GL_LINES);
	glVertex3f(0.1, prevMaxForwardingDistance, -5);
	glVertex3f(0.1, prevMaxForwardingDistance, 5);
	glEnd();
  }
}

void display(void) {
  if (screen == game) { //If state of screen is on game, draw the game
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//defining light properties
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	//For testing
	if (cameraToggle) {
	  gluLookAt(7,
				rocket.position.mY + rocket.forwardDistance,
				rocket.position.mZ,
				rocket.position.mX,
				rocket.position.mY + rocket.forwardDistance,
				rocket.position.mZ,
				0,
				1,
				0);
	  //gluLookAt(4, 4,4, 0,0,0, 0, 1, 0);
	} else {
	  gluLookAt(2, -8 + rocket.forwardDistance, rocket.position.mZ, 0, rocket.forwardDistance, 0, 1, 0, 0);
	}


	//Draws the ground plane
	glPushMatrix();
	// make y arbitrarily high to simulate an infinite long road ahead
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientDefault);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseDefault);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDefault);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
	glColor3f(0, 0, 1);
	glScalef(0.1, 10000, 10);
	glutSolidCube(1);
	glPopMatrix();

	//Visualization of where the lightsource is
	glPushMatrix();
	glTranslatef(position[0], position[1] + rocket.forwardDistance, 0);
	glutSolidSphere(0.1, 10, 10);
	glPopMatrix();

	drawRocket(rocket);
	drawObstacles(obstacleSystem);
	drawCoins(coinSystem);
	drawParticles(parSys);
	drawPreviousMaxScoreIndicatorLine();


	//set materials back to default
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientDefault);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseDefault);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDefault);
	//glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);

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

	//If an obstacle has been hit, display affect to fuel amount
	if (obstacleHit) {
	  glColor3f(1, 0, 0);
	  glRasterPos2i(150, 10);
	  drawText("-20");
	}
	//If a coin has been collected, display affect to coin amount
	if (coinGet) {
	  glColor3f(0, 1, 0);
	  glRasterPos2i(110, 580);
	  drawText("+100");
	}
	// If the previous record has been broken, display the text to the user on the game screen
	if (breakRecord) {
	  glColor3f(1, 0, 0);
	  glRasterPos2i(90, 560);
	  drawText("Congrats! You are setting a new game record!");
	}

	//Display amount of coins
	glColor3f(1, 1, 1);
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
	prevMaxForwardingDistance = maxForwardingDistance;
	drawText(scoreDisplay);

	glRasterPos2i(200, 10);
	drawText("Press Space Bar to play");

	glRasterPos2i(10, 500);
	drawText("(1) 100 Coins: Increase Fuel by 100");

	glRasterPos2i(10, 450);
	drawText("(2) 100 Coins: Increase Forward Speed");

	glRasterPos2i(10, 400);
	drawText("(3) 100 Coins: Increase Turning Speed");

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
		if (rocket.zOffset < 4.5) {
		  rocket.zOffset += rocket.turningSpeed;
		  rocket.angle -= 1;

		}
		break;
	  case 'd':
		//Turn the rocket right and rotates
		if (rocket.zOffset > -4.5) {
		  rocket.zOffset -= rocket.turningSpeed;
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
		//disable lighting for menu
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glClearColor(0.4, 0.79, 1, 1); //change background to sky blue

		//reset rocket
		rocket.forwardDistance = 0;
		rocket.zOffset = 0;
		rocket.fuel = rocket.initialFuel + rocket.fuelUpgrades;
		rocket.angle = 0;
		parSys.origin.mY = -5.40;

		//reset object lists
		coinSystem.v.clear();
		obstacleSystem.v.clear();

		//reset breakRecord flag
		breakRecord = false;

		//reset light
		//position[1] = 3;
		screen = game;
		break;
	  case '1':
		if (rocket.coins >= 100) {
		  rocket.coins -= 100;
		  rocket.fuelUpgrades += 100;
		}
		break;
	  case '2':
		if (rocket.coins >= 100) {
		  rocket.coins -= 100;
		  rocket.forwardSpeed += 0.1;
		}
		break;
	  case '3':
		if (rocket.coins >= 100) {
		  rocket.coins -= 100;
		  rocket.turningSpeed += 0.1;
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
	// detect if the player reaches beyond previous max record
	// this only occurs when there is an existing record
	if ((rocket.forwardDistance > prevMaxForwardingDistance) && !breakRecord && (prevMaxForwardingDistance > 0)) {
	  breakRecord = true;
	}
	coinSystem.update(rocket);
	obstacleSystem.update(rocket);
	parSys.update(rocket);

	if (rocket.fuel <= 0) {
	  screen = menu;
	  glClearColor(0, 0, 0, 1); //change background to black
	  glDisable(GL_LIGHTING); //disable lights
	  glDisable(GL_LIGHT0);
	}
	//If obstacle has been hit, decrement the amount of time the text stays on the screen
	if (obstacleHit) {
	  obsHitAge -= 0.1;
	}
	//If text age is < 0 reset obstacle detection
	if (obsHitAge < 0) {
	  obstacleHit = false;
	}
	//If coin has been hit, decrement the amount of time the text stays on the screen
	if (coinGet) {
	  coinGetAge -= 0.1;
	}
	//If text age is < 0 reset coin detection
	if (coinGetAge < 0) {
	  coinGet = false;
	}

  } else if (screen == menu) {

  }

  glutPostRedisplay();
  glutTimerFunc(17, FPS, 0);
}

/* LoadPPM -- loads the specified ppm file, and returns the image data as a GLubyte
 *  (unsigned byte) array. Also returns the width and height of the image, and the
 *  maximum colour value by way of arguments
 *  usage: GLubyte myImg = LoadPPM("myImg.ppm", &width, &height, &max);
 */
GLubyte *LoadPPM(char *file, int *width, int *height, int *max) {
  GLubyte *img;
  FILE *fd;
  int n, m;
  int k, nm;
  char c;
  int i;
  char b[100];
  float s;
  int red, green, blue;

  fd = fopen(file, "r");
  fscanf(fd, "%[^\n] ", b);
  if (b[0] != 'P' || b[1] != '3') {
	printf("%s is not a PPM file!\n", file);
	exit(0);
  }
  printf("%s is a PPM file\n", file);
  fscanf(fd, "%c", &c);
  while (c == '#') {
	fscanf(fd, "%[^\n] ", b);
	printf("%s\n", b);
	fscanf(fd, "%c", &c);
  }
  ungetc(c, fd);
  fscanf(fd, "%d %d %d", &n, &m, &k);

  printf("%d rows  %d columns  max value= %d\n", n, m, k);

  nm = n * m;

  img = (GLubyte *)(malloc(3 * sizeof(GLuint) * nm));

  s = 255.0 / k;

  for (i = 0; i < nm; i++) {
	fscanf(fd, "%d %d %d", &red, &green, &blue);
	img[3 * nm - 3 * i - 3] = red * s;
	img[3 * nm - 3 * i - 2] = green * s;
	img[3 * nm - 3 * i - 1] = blue * s;
  }

  *width = n;
  *height = m;
  *max = k;

  return img;
}
void loadTexture(char *filename, int index) {
  glBindTexture(GL_TEXTURE_2D, texture_map[index]);
  img_data[index] = LoadPPM(filename, &width[index], &height[index], &max[index]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[index], height[index], 0, GL_RGB, GL_UNSIGNED_BYTE, img_data[index]);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void init(void) {
  printWelcomeMessage();
  glColor3f(1, 1, 1);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho(-1, 1, -1, 1, -20, 20);
  gluPerspective(70, 1, 1, 20);
  rocket.loadRocketObj("./assets/rocket/rocket.obj");
  coinSystem.loadCoinObj("./assets/coin/coin.obj");
  obstacleSystem.loadObstacleObj("./assets/obstacle/obstacle.obj");

  glEnable(GL_TEXTURE_2D);
  glGenTextures(3, texture_map);

  // Temporarily using some ppms from lecture, will replace with proper textures later
  loadTexture("lenore.ppm", 0);
  loadTexture("snail_a.ppm", 1);
  loadTexture("./assets/rocket/steel.ppm", 2);

  glMatrixMode(GL_TEXTURE);
  glScalef(1, -1, -1);

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
  glShadeModel(GL_SMOOTH);

  init();

  glutMainLoop();                //starts the event loop

  return (0);                    //return may not be necessary on all compilers
}