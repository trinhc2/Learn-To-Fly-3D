#include <iostream>
#include <algorithm>
#include <math.h>
#include <limits>
#include "mathLib3D.h"
#include "coin.h"
#include "rocket.h"
#include "obstacle.h"
#include "particle.h"
#include "sceneObject.h"
#include <string>

#define _USE_MATH_DEFINES

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
enum Screen { game, menu, win };

bool cameraToggle = false;

//Detection for coin/obstacles, will display text for a certain amount of time (age value is decremented in FPS)
bool coinGet = false;
bool obstacleHit = false;
float obsHitAge = 0;
float coinGetAge = 0;
// flag for breaking the previous record
bool breakRecord = false;
bool infinite = false; //if inifinite is true then the game goes on forever
bool paused = false;
int recordDisappearTime = 500; // Keeps on screen for 5 secs (300 frames)

// global variables for ray casting & ray picking
double *m_start = new double[3];
double *m_end = new double[3];

float moonLocation = 500; //set to 500 for final.

unsigned int viewportWidth  = 600;
unsigned int viewportHeight = 600;

Screen screen = menu; //screen state

float coinAmbient[4] = {0.24725f, 0.1995f, 0.0745f, 1.0f};
float coinDiffuse[4] = {0.75164f, 0.60648f, 0.22648f, 1.0f};
float coinSpecular[4] = {0.628281f, 0.555802f, 0.366065f, 1.0f};
float coinShine = 51.2f;

//[0] = red, [1] = green , [2] = white
float textAmb[3][4] = { {1, 0, 0, 1},  {0, 1, 0, 1}, {1, 1, 1, 1}};
float textDiff[3][4] = { {1, 0, 0, 1},  {0, 1, 0, 1}, {1, 1, 1, 1}};
float textSpec[3][4] = { {1, 0, 0, 1}, {0, 1, 0, 1}, {1, 1, 1, 1}};

float obstacleAmbient[4] = {0.19225f, 0.19225f, 0.19225f, 1.0f};
float obstacleDiffuse[4] = {0.50754f, 0.50754f, 0.50754f, 1.0f};
float obstacleSpecular[4] = {0.508273f, 0.508273f, 0.508273f, 0.82f};
float obstacleShine = 0.4f;

// White translucent for cloud
float cloudMat[4] = { 1, 1, 1, 0.5 };
// Yellow translucent star
float starMat[4] = { 1, 1, 0, 0.75 };

float ambientDefault[4] = {0.2, 0.2, 0.2, 1.0};
float diffuseDefault[4] = {0.8, 0.8, 0.8, 1.0};
float specularDefault[4] = {0, 0, 0, 1};

void *font = GLUT_BITMAP_HELVETICA_18; //Font which is used for glutBitMapCharacter

Rocket rocket = Rocket();
CoinSystem coinSystem = CoinSystem();
ObstacleSystem obstacleSystem = ObstacleSystem();
ParticleSystem rocketFlame = ParticleSystem();
SceneObjectSystem sceneSystem = SceneObjectSystem();
std::vector<Particle> explosion; //bomb explosion particles

float position[4] = {1, 2 + rocket.forwardDistance, 0, 1};
float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
float diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

float maxForwardingDistance = 0; // Used to keep track of player score
float prevMaxForwardingDistance = 0; // used to keep track max player score from previous plays

// Textures
GLubyte *img_data[5];
GLuint texture_map[5];
int width[5];
int height[5];
int max[5];

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
	  exit(0);
  }
  fscanf(fd, "%c", &c);
    while (c == '#') {
    fscanf(fd, "%[^\n] ", b);
    printf("%s\n", b);
    fscanf(fd, "%c", &c);
  }
  ungetc(c, fd);
  fscanf(fd, "%d %d %d", &n, &m, &k);

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

struct Image {
  int mWidth;
  int mHeight;
	int mMax;
  GLubyte * mImage;

  void load(char * filename) {
    mImage = LoadPPM(filename, &mWidth, &mHeight, &mMax);
  }

  void draw(unsigned int x, unsigned int y) {
    glRasterPos2i(x + mWidth, y);
    glPixelZoom(-0.9, 0.9); //flip image and scale down to 0.9
    glDrawPixels(mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, mImage);
  }
};

struct Handler {
  unsigned int mLeft, mRight, mTop, mBottom;
  void (*mHandlerFunc)();

  bool isInBounds(unsigned int x, unsigned int y) {
    return (mLeft <= x && x <= mRight) && (mBottom <= y && y <= mTop);
  }

  void handleClickAt(unsigned int x, unsigned int y) {
    if (isInBounds(x, y)) {
        mHandlerFunc();
    }
  }

  void drawBoxVertices() {
    glVertex3f(mLeft, mTop, 1);
    glVertex3f(mLeft, mBottom, 1);
    glVertex3f(mRight, mTop, 1);
    glVertex3f(mRight, mBottom, 1);
    glVertex3f(mLeft, mTop, 1);
    glVertex3f(mRight, mTop, 1);
    glVertex3f(mLeft, mBottom, 1);
    glVertex3f(mRight, mBottom, 1);
  }
};

struct InteractionHandler {
  std::vector<Handler *> mHandlers;

  void leftClickDown(int x, int y) {
    for (Handler *handler : mHandlers) {
        handler->handleClickAt(x, y);
    }
  }

  void drawHandlers() {
    glColor3f(1, 1, 1);
    glLineWidth(2);
    glBegin(GL_LINES);
    for (Handler *handler : mHandlers) {
        handler->drawBoxVertices();
    }
    glEnd();
  }

  void addHandler(Handler *handler) {
    mHandlers.push_back(handler);
  }
};

InteractionHandler mouseHandler;
Image gas;
Image speed;
Image turning;

void printWelcomeMessage() {
  std::cout << "Welcome to Learn To Fly 3D!" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "***   INSTRUCTIONS   ***" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "You are learning to fly with your rocket." << std::endl;
  std::cout << "Your goal is to reach the moon! Unfortunately, your rocket is too weak right now." << std::endl;
  std::cout
	  << "Collect coins along the path to upgrade your rocket. Avoid the obstacles; Red obstacles knock you back and black obstacles reduce your fuel by 20."
	  << std::endl;
  std::cout << "At the menu, click on the upgrades to use your Coins to purchase upgrades." << std::endl;
  std::cout
	  << "On the game screen, the amount of fuel you have left is displayed on the bottom-left. Once your fuel reaches 0, you will be redirected to the main menu."
	  << std::endl;
  std::cout
	  << "The number of coins you have is on the top-left. You can use these coins to purchase upgrades, and they do not expire (i.e. you can choose to save your coins after each round instead of purchasing anything)."
	  << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "***     CONTROLS     ***" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "Move: W/A/S/D (Up/Left/Down/Right)" << std::endl;
  std::cout << "Exit: Q/ESC" << std::endl;
  std::cout << "Toggle birds-eye view: V" << std::endl;
  std::cout << "Pause: P" << std::endl;
  std::cout << "Destroy Obstacle: Left/Right Click" << std::endl;
  std::cout << "***   IN SHOP/MENU   ***" << std::endl;
  std::cout << "Start next run: Space bar" << std::endl;
  std::cout << "Purchase Upgrades: Mouse" << std::endl;
  std::cout << "Exit: Q/ESC" << std::endl;
  std::cout << "***   IN END SCREEN   ***" << std::endl;
  std::cout << "Activate free play: Space bar" << std::endl;
  std::cout << "Purchase Upgrades: Mouse" << std::endl;
  std::cout << "Exit: Q/ESC" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "***   EXTRA DETAILS  ***" << std::endl;
  std::cout << "************************" << std::endl;
  std::cout << "Clouds and Stars are decoration-only; you can collide into them." << std::endl;
  std::cout << "Your goal is to reach the moon. Good luck & have fun!" << std::endl;

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
void displayObj(std::vector<Point3D> out_vertices, std::vector<Vec3D> out_normals,
               std::vector<Point2D> out_uvs, int size) {
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

    //setting material to default because we are using a texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientDefault);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseDefault);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDefault);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);

    //Place the rocket at its position + how much it has traveled
    glTranslatef(rocket.position.mX, rocket.position.mY + rocket.forwardDistance, rocket.position.mZ);
    //Rotate the rocket if it has been turning
    glRotatef(2.5, 1, 0, 0); //points rocket more to the left so it pointing forwards by default
    glRotatef(-10, 0, 1, 0); //rotates rocket along y so it is not on a diagonal by default
    glRotatef(rocket.angle, -1, 1, 0);
    //Scales the rocket size down, scales can be updated in future
    glScalef(0.3, 0.3, 0.3);
    displayObj(rocket.out_vertices, rocket.out_normals, rocket.out_uvs, rocket.vertexIndices.size());
  glPopMatrix();
  // Reset texture binding after finishing draw
  glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Draws the coins to the screen
 */
void drawCoins(CoinSystem coinSystem) {
	glBindTexture(GL_TEXTURE_2D, 0);
  glColor3f(1, 1, 0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, coinAmbient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, coinDiffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, coinSpecular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, coinShine);
  for (std::size_t i = 0; i < coinSystem.v.size(); i++) {
    glPushMatrix();
      glTranslatef(coinSystem.v.at(i).position.mX, coinSystem.v.at(i).position.mY, coinSystem.v.at(i).position.mZ);
      glRotatef(coinSystem.rotation, 1, 0, 0);
      displayObj(coinSystem.out_vertices, coinSystem.out_normals, coinSystem.out_uvs, coinSystem.vertexIndices.size());
    glPopMatrix();
  }
  // Reset texture binding after obstacle.cppfinishing draw
  glBindTexture(GL_TEXTURE_2D, 0);
}

void drawScenery(SceneObjectSystem sceneSystem) {
  glColor3f(1, 1, 0);
  for (std::size_t i = 0; i < sceneSystem.v.size(); i++) {
    SceneObject sceneObj = sceneSystem.v.at(i);
    Point3D pos = sceneObj.position;
    glPushMatrix();
      glTranslatef(pos.mX, pos.mY, pos.mZ);

      if (sceneObj.type == 0) {
        // Cloud: white translucent (0.5)
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cloudMat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cloudMat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cloudMat);
      } else if (sceneObj.type == 1) {
        // Star: Yellow translucent (0.75)
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, starMat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, starMat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, starMat);
      }
      
      // Scale only affects largeness
      glScalef(sceneObj.size, sceneObj.size, sceneObj.size);
      glRotatef(sceneObj.rotation, 0, 1, 0);
      
      displayObj(sceneSystem.v.at(i).out_vertices, sceneSystem.v.at(i).out_normals, 
                sceneSystem.v.at(i).out_uvs, sceneSystem.v.at(i).vertexIndices.size());
    glPopMatrix();

    // Reset back to default material
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientDefault);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseDefault);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDefault);
  }
}

/**
 * Draws the obstacles to the screen
 */
void drawObstacles(ObstacleSystem obstacleSystem) {
  glColor3f(0, 1, 0);

  for (std::size_t i = 0; i < obstacleSystem.v.size(); i++) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, obstacleAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, obstacleDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, obstacleSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, obstacleShine);

    Obstacle obstacle = obstacleSystem.v.at(i);
    glPushMatrix();
      glTranslatef(obstacle.position.mX,
                   obstacle.position.mY,
                   obstacle.position.mZ);
      if (obstacle.type == 1) {
        glBindTexture(GL_TEXTURE_2D, texture_map[4]);
      } else {
		    glBindTexture(GL_TEXTURE_2D, texture_map[1]);
	    }
      displayObj(obstacleSystem.out_vertices, obstacleSystem.out_normals, obstacleSystem.out_uvs, obstacleSystem.vertexIndices.size());
      // Reset texture binding after finishing draw
      glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
  }
}

/**
 * Draws Rocket trail and explosions
 */
void drawParticles(std::vector<Particle> v) {
  //bind fire texture
  glBindTexture(GL_TEXTURE_2D, texture_map[0]);

  //setting material to default because we are using texture
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientDefault);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseDefault);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDefault);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);

  if (v.size() > 0) {
    for (Particle p: v) {
      glPushMatrix();
        glColor3f(p.r, p.g, p.b);

        glTranslatef(p.position.mX, p.position.mY, p.position.mZ);

        glScalef(0.05, 0.05, 0.05);
        glutSolidCube(p.size);
      glPopMatrix();
    }
  }

  glBindTexture(GL_TEXTURE_2D, 0);
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
    //calculate background colour based on forward distance * base value
    //base value is determined by 1/500 meaning that at 500 forward distance the background is black (space).
    glClearColor(0.4 - (rocket.forwardDistance * 0.002), 0.79 - (rocket.forwardDistance * 0.002), 1 - (rocket.forwardDistance * 0.002), 1);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

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
    } else {
      gluLookAt(0, -8 + rocket.forwardDistance, rocket.position.mZ, 
                0, rocket.forwardDistance, 0, 
                1, 0, 0);
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientDefault);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseDefault);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDefault);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);

    //Visualization of where the lightsource is
    // glPushMatrix();
    // glTranslatef(position[0], position[1] + rocket.forwardDistance, 0);
    // glutSolidSphere(0.1, 10, 10);
    // glPopMatrix();

    drawRocket(rocket);
    drawObstacles(obstacleSystem);
    drawCoins(coinSystem);
    drawScenery(sceneSystem);

    //https://stackoverflow.com/questions/327043/how-to-apply-texture-to-glutsolidcube
    glEnable(GL_TEXTURE_GEN_S); //this lets us apply texture to glutsolidcube (particles)
    glEnable(GL_TEXTURE_GEN_T);
    drawParticles(rocketFlame.v);
    drawParticles(explosion);

    //Drawing moon at y=moonLocation and only if rocket is near
    if (!infinite && rocket.forwardDistance > moonLocation - 30) {
      glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture_map[3]);
        glTranslatef(0, moonLocation, 0);
        glutSolidSphere(3,10,10);
        glBindTexture(GL_TEXTURE_2D, 0);
      glPopMatrix();
    }

    //Draw ray
    glBegin(GL_LINES);
    glVertex3f(m_start[0], m_start[1], m_start[2]);
    glVertex3f(m_end[0], m_end[1], m_end[2]);
    glEnd();

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    drawPreviousMaxScoreIndicatorLine();


    //set materials back to default
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientDefault);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseDefault);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDefault);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);

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
    gluOrtho2D(0.0, viewportWidth, 0.0, viewportHeight);
    glMatrixMode(GL_MODELVIEW);

    //Display Fuel
    glPushMatrix();
    glLoadIdentity();
    //set colour to white
    glColor3f(1, 1, 1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, textAmb[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, textDiff[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, textSpec[2]);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glRasterPos2i(10, 10);
    std::string fuelDisplay = "Fuel: " + std::to_string(rocket.fuel);
    drawText(fuelDisplay);

    //Display amount of coins
    glColor3f(1, 1, 1);
    glRasterPos2i(10, 580);
    std::string coinDisplay = "Coins: " + std::to_string(rocket.coins);
    drawText(coinDisplay);

    //If an obstacle has been hit, display affect to fuel amount
    if (obstacleHit) {
      glColor3f(1, 0, 0);
      //set colour to red
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, textAmb[0]);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, textDiff[0]);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, textSpec[0]);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
      glRasterPos2i(150, 10);
      drawText("-20");
    }

    //If a coin has been collected, display affect to coin amount
    if (coinGet) {
      glColor3f(0, 1, 0);
      //set colour to green
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, textAmb[1]);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, textDiff[1]);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, textSpec[1]);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
      glRasterPos2i(110, 580);
      drawText("+100");
    }
    // If the previous record has been broken, display the text to the user on the game screen
    if (breakRecord) {
      glColor3f(1, 0, 0);
      glRasterPos2i(90, 560);
      if (recordDisappearTime > 0) {
        drawText("Congrats! You are setting a new game record!");
      }
    }

    //Undoing changes to display
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
  } else if (screen == menu) { //if state of screen is on menu, draw the menu
  	glClearColor(0, 0, 0, 1); //change background to black
    glBindTexture(GL_TEXTURE_2D, 0); // reset texture here so that shop page won't be affected by our other textures
    glDisable(GL_LIGHTING); //disable lights
    glDisable(GL_LIGHT0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, viewportWidth, 0.0, viewportHeight);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();

    //https://stackoverflow.com/questions/15983607/opengl-texture-tilted
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //images would appear slanted without this

    //Drawing UI
    gas.draw(0,300);
    speed.draw(290, 300);
    turning.draw(0,100);

    glColor3f(1, 1, 1);
    glRasterPos2i(10, 580);
    std::string coinDisplay = "Coins: " + std::to_string(rocket.coins);
    drawText(coinDisplay);

    std::string scoreDisplay = "    Max Score: " + std::to_string(maxForwardingDistance);
    prevMaxForwardingDistance = maxForwardingDistance;
    drawText(scoreDisplay);

    glRasterPos2i(200, 10);
    drawText("Press Space Bar to play");


    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
  }
  else if (screen == win) {
    glClearColor(0, 0, 0, 1); //change background to black
    glDisable(GL_LIGHTING); //disable lights
    glDisable(GL_LIGHT0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, viewportWidth, 0.0, viewportHeight);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();

    glColor3f(1, 1, 1);

    glRasterPos2i(260, 340);
    drawText("You Win!");

    glRasterPos2i(150, 300);
    drawText("Press Space Bar to continue playing!");

    glRasterPos2i(210, 260);
    drawText("Press Escape to Quit");

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
      case 'Q':
      case 'q': 
      case 27:
        exit(0);
        break;

      case 'W':
      case 'w': //raises rocket
        if (rocket.position.mX < 1.3) {
          rocket.xOffset += rocket.turningSpeed;
        }
        break;

      case 'S':
      case 's': //lowers rocket
        if (rocket.position.mX > -1.3) {
          rocket.xOffset -= rocket.turningSpeed;
        }
        break;

      case 'A':
      case 'a':
        //Turn the rocket left and rotates
        if (rocket.zOffset < 4.5) {
          rocket.zOffset += rocket.turningSpeed;
          rocket.angle += 0.5;
        }
        break;

      case 'D':
      case 'd':
        //Turn the rocket right and rotates
        if (rocket.zOffset > -4.5) {
          rocket.zOffset -= rocket.turningSpeed;
          rocket.angle -= 0.5;
        }
        break;

      case 'V':
      case 'v':
        cameraToggle = !cameraToggle;
        break;

      case 'P':
      case 'p':
        paused = !paused;
        break;
    }
  } else if (screen == menu) {
    switch (key) {
      case 'Q':
      case 'q':
        exit(0);
        break;
      case 27: //escape
        exit(0);
        break;
      case 32: //space bar
      //reset rocket
      rocket.forwardDistance = 0;
      rocket.zOffset = 0;
      rocket.xOffset = 0;
      rocket.fuel = rocket.initialFuel + rocket.fuelUpgrades;
      rocket.angle = 0;
      rocketFlame.origin.mY = -5.40;

      //reset object lists
      coinSystem.v.clear();
      obstacleSystem.v.clear();
      rocketFlame.v.clear();
      sceneSystem.v.clear();

      //reset breakRecord flag
      breakRecord = false;
      recordDisappearTime = 500;

      screen = game;
      break;
    }
  }
  else if (screen == win) {
	  switch (key) {
      case 'Q':
      case 'q':
        exit(0);
        break;

      case 27: //escape
        exit(0);
        break;
      case 32: //space bar
        //reset rocket
        infinite = true;
        rocket.forwardDistance = 0;
        rocket.zOffset = 0;
        rocket.xOffset = 0;
        rocket.fuel = rocket.initialFuel + rocket.fuelUpgrades;
        rocket.angle = 0;
        rocketFlame.origin.mY = -5.40;

        //reset object lists
        coinSystem.v.clear();
        obstacleSystem.v.clear();
        rocketFlame.v.clear();

        //reset breakRecord flag
        breakRecord = false;
        recordDisappearTime = 500;

        screen = menu;
        break;
    }
  }
  glutPostRedisplay();
}

void populateRayTracingValues(int x, int y) {
  double matModelView[16], matProjection[16];
  int viewport[4];

  // get matrix and viewport:
  glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
  glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
  glGetIntegerv(GL_VIEWPORT, viewport);

  // window pos of mouse, Y is inverted on Windows
  double winX = (double)x;
  double winY = viewport[3] - (double)y;

  // get point on the 'near' plane (third param is set to 0.0)
  gluUnProject(winX, winY, 0.0, matModelView, matProjection,
			   viewport, &m_start[0], &m_start[1], &m_start[2]);
  gluUnProject(winX, winY, 1.0, matModelView, matProjection,
			   viewport, &m_end[0], &m_end[1], &m_end[2]);
}

float getRayIntersectionTimeSphere(int x, int y, int z, float boundingBoxSize) {
  // reference: Code Lecture on Ray Casting
  double *R0 = new double[3];
  double *Rd = new double[3];
  double xDiff = m_end[0] - m_start[0];
  double yDiff = m_end[1] - m_start[1];
  double zDiff = m_end[2] - m_start[2];
  double mag = sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);

  R0 = m_start;
  Rd[0] = xDiff / mag;
  Rd[1] = yDiff / mag;
  Rd[2] = zDiff / mag;

  double A = Rd[0] * Rd[0] + Rd[1] * Rd[1] + Rd[2] * Rd[2];
  double *R0Pc = new double[3];
  R0Pc[0] = R0[0] - x;
  R0Pc[1] = R0[1] - y;
  R0Pc[2] = R0[2] - z;

  double B = 2 * (R0Pc[0] * Rd[0] + R0Pc[1] * Rd[1] + R0Pc[2] * Rd[2]);
  double C = (R0Pc[0] * R0Pc[0] + R0Pc[1] * R0Pc[1] + R0Pc[2] * R0Pc[2])
	  - (boundingBoxSize * boundingBoxSize);

  double discriminant = B * B - 4 * A * C;

  if (discriminant < 0) {
    return -1;
  }
  else {
    double t0 = (-B + sqrt(discriminant)) / (2 * A);
    double t1 = (-B - sqrt(discriminant)) / (2 * A);
    // return the time for the ray to reach the closest intersection point for comparsion
    return std::min(t0, t1);
  }
}

void mouse(int button, int state, int x, int y) {
  if (screen == menu) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseHandler.leftClickDown(x, viewportWidth - y);
    }
  } else if (screen == game) {
    if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) {
      if (state == GLUT_DOWN) {
        // populate ray tracing variables each time mouse is clicked
        populateRayTracingValues(x, y);

        // find the nearest object that intersects with the mouse ray
        float closestIntersectionTime = std::numeric_limits<float>::max();
        int closestObstacleIndex = -1;
        for (unsigned int i = 0; i < obstacleSystem.v.size(); i++) {
          Obstacle obstacle = obstacleSystem.v.at(i);
          Point3D pos = obstacle.position;
          float intersectionTime =
              getRayIntersectionTimeSphere(pos.mX, pos.mY, pos.mZ, 0.5); // obstacles have radius 0.5
          if (intersectionTime >= 0 && intersectionTime < closestIntersectionTime) {
            closestIntersectionTime = intersectionTime;
            closestObstacleIndex = i;
          }
        }
        // select the nearest object on mouse left click
        if (button == GLUT_LEFT_BUTTON) {
          if (closestObstacleIndex != -1) {
            // Delete from v
            obstacleSystem.v.erase(obstacleSystem.v.begin() + closestObstacleIndex);
          }
        }
      }
    }
  }
  glutPostRedisplay();
}

void gasIncrease() {
	if (rocket.coins >= 100) {
		rocket.coins -= 100;
		rocket.fuelUpgrades += 100;
    printf("Succesfully purchased fuel upgrade!\n");
	}
	else {
		printf("Not enough coins. \n");
	}
}

void speedIncrease() {
	if (rocket.coins >= 100) {
		rocket.coins -= 100;
		rocket.forwardSpeed += 0.1;
    printf("Succesfully purchased speed upgrade!\n");
	}
	else {
		printf("Not enough coins. \n");
	}
}

void turningIncrease() {
	if (rocket.coins >= 100) {
		rocket.coins -= 100;
		rocket.turningSpeed += 0.1;
    printf("Succesfully purchased handling upgrade!\n");
	}
	else {
		printf("Not enough coins. \n");
	}
}

Handler gasButton = {
  30,
  305,
  510,
  308,
  gasIncrease
};

Handler speedButton = {
  320,
  595,
  510,
  308,
  speedIncrease
};

Handler turningButton = {
  30,
  305,
  310,
  108,
  turningIncrease
};

void FPS(int val) {
  if (screen == game && !paused) {
    rocket.update();
    maxForwardingDistance = std::max(maxForwardingDistance, rocket.forwardDistance);
    // detect if the player reaches beyond previous max record
    // this only occurs when there is an existing record
    if ((rocket.forwardDistance > prevMaxForwardingDistance) && !breakRecord && (prevMaxForwardingDistance > 0)) {
      breakRecord = true;
    }
    coinSystem.update(rocket);
    obstacleSystem.update(rocket, explosion, rocketFlame.v);
    rocketFlame.update(rocket);
    sceneSystem.update(rocket);

    if (rocket.fuel <= 0) {
      screen = menu;
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
    //Decrease amount of time to print the record breaking message
    if (breakRecord && recordDisappearTime > 0) {
      recordDisappearTime -= 1;
    }

    if (!infinite) {
      if (inRangeX(3.15, -3.15, rocket)
        && inRangeY(moonLocation + 3.15, moonLocation - 3.15, rocket)
        && inRangeZ(3.15,-3.15, rocket)) {
          screen = win;
      }
    }
  }
  glutPostRedisplay();
  glutTimerFunc(17, FPS, 0);
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(70, (float)((w+0.0f)/h), 1, 20);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
	viewportWidth = w;
  viewportHeight = h;
}

void init(void) {
  printWelcomeMessage();
  glColor3f(1, 1, 1);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70, 1, 1, 20);

  rocket.loadRocketObj("./assets/rocket/rocket.obj");
  coinSystem.loadCoinObj("./assets/coin/coin.obj");
  obstacleSystem.loadObstacleObj("./assets/obstacle/obstacle.obj");

  glEnable(GL_TEXTURE_2D);
  glGenTextures(5, texture_map);

  loadTexture("./assets/particle/fire.ppm", 0);
  loadTexture("./assets/obstacle/obstacle.ppm", 1);
  loadTexture("./assets/rocket/steel.ppm", 2);
  loadTexture("./assets/moon/moon.ppm", 3);
  loadTexture("./assets/obstacle/bomb.ppm", 4);

	gas.load("./assets/gas.ppm");
	speed.load("./assets/speed.ppm");
	turning.load("./assets/turning.ppm");

	mouseHandler.addHandler(&gasButton);
	mouseHandler.addHandler(&speedButton);
	mouseHandler.addHandler(&turningButton);

  glMatrixMode(GL_TEXTURE);
  glScalef(1, -1, -1);

  // Enable blending for clouds and stars
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/* main function - program entry point */
int main(int argc, char **argv) {
  glutInit(&argc, argv);        //starts up GLUT
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(600, 600);

  setbuf(stdout, NULL); //remove stdout buffer

  glutCreateWindow("Learn to Fly");    //creates the window

  glutDisplayFunc(display);    //registers "display" as the display callback function
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutTimerFunc(17, FPS, 0);
  glutReshapeFunc(reshape);

  glEnable(GL_DEPTH_TEST);

  glFrontFace(GL_CW);
  glCullFace(GL_FRONT);
  glEnable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);

  init();

  glutMainLoop();                //starts the event loop

  return (0);                    //return may not be necessary on all compilers
}