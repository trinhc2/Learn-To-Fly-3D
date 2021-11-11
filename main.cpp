#include <iostream>
#include "mathLib3D.h"
#include "coin.h"
#include "rocket.h"

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
bool cameraToggle = false;

Rocket rocket = Rocket();
CoinSystem coinSystem = CoinSystem();

/**
 * Checks if the rocket is within a range of coordinates
 */ 
bool inRangeX(int high, int low, Rocket r){
    return (low <= r.position.mX && r.position.mX <= high);
}

bool inRangeZ(int high, int low, Rocket r){
    return (low <= r.position.mZ && r.position.mZ <= high);
}

bool inRangeY(int high, int low, Rocket r){
    return (low <= r.position.mY + r.forwardDistance && r.position.mY + r.forwardDistance <= high);
}

/**
 * Checks if the rocket has collided with the coins
 * 
 * @rocket reference to our rocket object
 * @v reference to our list of coins from CoinSystem
 */
void checkCoinCollision(Rocket& rocket, std::vector<Coin>& v) {
    for (Coin& c: v) {
        //The boundaries are off but intended behaviour works
        if (inRangeY(c.position.mY+0.9, c.position.mY-0.9, rocket) && inRangeZ(c.position.mZ+0.9, c.position.mZ-0.9, rocket))  {
            //if collision: set coin as collected (see Coinsystem::update) and increments earned coins
            c.collected = true;
            rocket.coins += 100;
        }
    }
}

/**
 * Draws the rocket to screen
 */
void drawRocket(Rocket rocket){
    glColor3f(1,0,0);
    glPushMatrix();
    //Place the rocket at its position + how much it has traveled
    glTranslatef(rocket.position.mX, rocket.position.mY + rocket.forwardDistance, rocket.position.mZ);
    //Rotate the rocket if it has been turning
    glRotatef(rocket.angle, 0,1,0);
    //Scales the rocket size down, scales can be updated in future
    glScalef(0.3,0.3,0.3);
	glutSolidCube(1);
	glPopMatrix();
}

/**
 * Draws the coins to the screen
 */
void drawCoins(CoinSystem coinSystem){
    for (std::size_t i=0; i<coinSystem.v.size(); i++) {
        glColor3f(1,1,0);
        glPushMatrix();
        glTranslatef(coinSystem.v.at(i).position.mX, coinSystem.v.at(i).position.mY, coinSystem.v.at(i).position.mZ);
        glRotatef(coinSystem.rotation, 1,0,0);
        glutSolidCube(1);
        glPopMatrix();
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    //For testing
    if (cameraToggle) {
        gluLookAt(7,7,7, rocket.position.mX, rocket.position.mY + rocket.forwardDistance, rocket.position.mZ, 0,1,0);
    }
    else {
        gluLookAt(2, -8 + rocket.forwardDistance, 0, 0, rocket.forwardDistance,0, 1,0,0);
    }

    glColor3f(0,0,1);

    //Draws the ground plane
	glPushMatrix();
	glScalef(0.1, 10, 10);
	glutSolidCube(1);
	glPopMatrix();

    //Draws random cube at the origin
    glColor3f(0,1,0);
    glPushMatrix();
    glTranslatef(0.4,0,0);
	glScalef(1, 1, 1);
	glutSolidCube(1);
	glPopMatrix();

    drawRocket(rocket);
    drawCoins(coinSystem);

    //Display Text
    //https://stackoverflow.com/questions/18847109/displaying-fixed-location-2d-text-in-a-3d-opengl-world-using-glut
    glColor3f(1, 1, 1);
    glDisable(GL_TEXTURE_2D); //added this
    glMatrixMode(GL_PROJECTION);

    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 600, 0.0, 600);
    glMatrixMode(GL_MODELVIEW);

    //Display Fuel
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(10, 10);
    std::string fuelDisplay = "Fuel: " + std::to_string(rocket.fuel);
    void * font = GLUT_BITMAP_HELVETICA_18;
    glColor3f(1, 1, 1);
    for (std::string::iterator i = fuelDisplay.begin(); i != fuelDisplay.end(); ++i)
    {
    char c = *i;
    glutBitmapCharacter(font, c);
    }
    glPushMatrix();

    //Display amount of coins
    glRasterPos2i(10, 580);
    std::string coinDisplay = "Coins: " + std::to_string(rocket.coins);
    glColor3f(1, 1, 1);
    for (std::string::iterator i = coinDisplay.begin(); i != coinDisplay.end(); ++i)
    {
    char c = *i;
    glutBitmapCharacter(font, c);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION); //swapped this with...
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW); //...this
    glPopMatrix();
    //added this
    glEnable(GL_TEXTURE_2D);
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'q':
            exit (0);
            break;
        /*esc*/
        case 27:
            exit (0);
            break;
        case 'a':
        //Turn the rocket left and rotates
            if (rocket.zOffset < 2) {
                rocket.zOffset += 0.1;
                rocket.angle -=1;
            
            }
            break;
        case 'd':
        //Turn the rocket right and rotates
            if (rocket.zOffset > -2) {
                rocket.zOffset -= 0.1;
                rocket.angle +=1;
            
            }
            break;
        case 'v':
            cameraToggle = !cameraToggle;
            break;
    }
    glutPostRedisplay();
}

void FPS(int val) {
    rocket.update();
    coinSystem.update();
    checkCoinCollision(rocket, coinSystem.v);

    glutPostRedisplay();
    glutTimerFunc(17, FPS, 0);
}

void init(void)
{
	glColor3f(1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-1, 1, -1, 1, -20, 20);
	gluPerspective(70, 1, 1, 20);
}

/* main function - program entry point */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(600, 600);

	glutCreateWindow("Learn to Fly");	//creates the window

	glutDisplayFunc(display);	//registers "display" as the display callback function
    glutKeyboardFunc(keyboard);
    glutTimerFunc(17, FPS, 0);

	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

    init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}