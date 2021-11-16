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

Screen screen = menu; //screen state

void *font = GLUT_BITMAP_HELVETICA_18; //Font which is used for glutBitMapCharacter

Rocket rocket = Rocket();
CoinSystem coinSystem = CoinSystem();
ObstacleSystem obstacleSystem = ObstacleSystem();
ParticleSystem parSys = ParticleSystem();

float maxForwardingDistance = 0; // Used to keep track of player score

// Textures
GLubyte* img_data[3];
GLuint texture_map[3];
int width[3];
int height[3];
int max[3];

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
    return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/x));
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
  glColor3f(1, 0, 0);
  glBindTexture(GL_TEXTURE_2D, texture_map[2]);
  glPushMatrix();
    //Place the rocket at its position + how much it has traveled
    glTranslatef(rocket.position.mX, rocket.position.mY + rocket.forwardDistance, rocket.position.mZ);
    //Rotate the rocket if it has been turning
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
  for (std::size_t i = 0; i < coinSystem.v.size(); i++) {
	glColor3f(1, 1, 0);
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
  for (std::size_t i = 0; i < obstacleSystem.v.size(); i++) {
    glColor3f(0, 1, 0);
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
        for(Particle p: parSys.v){
            glPushMatrix();
			glTranslatef(p.position.mX, p.position.mY, p.position.mZ);
            glScalef(0.05,0.05,0.05);
            glColor3f(p.r, p.g, p.b);
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

void display(void) {
  if (screen == game) { //If state of screen is on game, draw the game
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //For testing
    if (cameraToggle) {
      gluLookAt(7, rocket.position.mY + rocket.forwardDistance, rocket.position.mZ, rocket.position.mX, rocket.position.mY + rocket.forwardDistance, rocket.position.mZ, 0, 1, 0);
      //gluLookAt(4, 4,4, 0,0,0, 0, 1, 0);
    } else {
      gluLookAt(2, -8 + rocket.forwardDistance, rocket.position.mZ, 0, rocket.forwardDistance, 0, 1, 0, 0);
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
    drawParticles(parSys);

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

    glRasterPos2i(200, 10);
    drawText("Press Space Bar to play");

    glRasterPos2i(10, 500);
    drawText("(1) 100 Coins: Increase Fuel by 100");

    glRasterPos2i(10, 450);
    drawText("(2) 100 Coins: Increase Speed");

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
		if (rocket.zOffset < 3) {
		  rocket.zOffset += 0.2;
		  rocket.angle -= 1;

		}
		break;
	  case 'd':
		//Turn the rocket right and rotates
		if (rocket.zOffset > -3) {
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
	case '2':
		if (rocket.coins >= 100) {
		  rocket.coins -= 100;
		  rocket.forwardSpeed += 0.1;
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
    coinSystem.update(rocket);
    obstacleSystem.update(rocket);
    parSys.update(rocket);
    if (rocket.fuel <= 0) {
      screen = menu;
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
GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
    GLubyte* img;
    FILE *fd;
    int n, m;
    int  k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;
    
    fd = fopen(file, "r");
    fscanf(fd,"%[^\n] ",b);
    if(b[0]!='P'|| b[1] != '3')
    {
        printf("%s is not a PPM file!\n",file);
        exit(0);
    }
    printf("%s is a PPM file\n", file);
    fscanf(fd, "%c",&c);
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);
    
    printf("%d rows  %d columns  max value= %d\n",n,m,k);
    
    nm = n*m;
    
    img = (GLubyte*)(malloc(3*sizeof(GLuint)*nm));
    
    s=255.0/k;
    
    
    for(i=0;i<nm;i++)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        img[3*nm-3*i-3]=red*s;
        img[3*nm-3*i-2]=green*s;
        img[3*nm-3*i-1]=blue*s;
    }
    
    *width = n;
    *height = m;
    *max = k;
    
    return img;
}
void loadTexture(char* filename, int index) {
  glBindTexture(GL_TEXTURE_2D, texture_map[index]);
  img_data[index] = LoadPPM(filename, &width[index], &height[index], &max[index]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[index], height[index], 0, GL_RGB, GL_UNSIGNED_BYTE, img_data[index]);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void init(void) {
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
  loadTexture("marble.ppm", 2);

  glMatrixMode(GL_TEXTURE);
  glScalef(1,-1,-1);

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