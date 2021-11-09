#include <iostream>
#include "mathLib3D.h"
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

float forwardDistance = 0;
bool cameraToggle = false;

class Rocket {
public:
    Point3D position;
    float angle;
    float fuel;
    float zOffset;
    int coins;
    Rocket();
    void update();
};

Rocket::Rocket() {
    position = Point3D(0.5, -5.45, zOffset);
    angle = 0;
    fuel = 100;
    coins = 0;
    zOffset = 0;
}

void Rocket::update() {
    //While the rocket has fuel move forward and decrement fuel gauge
    if (fuel > 0) {
        forwardDistance +=0.01;
        fuel -= 0.1;
    }
    else {
        fuel = 0;
    }
    position.mZ = zOffset;
}

Rocket rocket = Rocket();

void drawRocket(Rocket rocket){
    glColor3f(1,0,0);
    glPushMatrix();
    glTranslatef(rocket.position.mX, rocket.position.mY + forwardDistance, rocket.position.mZ);
    glRotatef(rocket.angle, 0,1,0);
    glScalef(0.3,0.3,0.3);
	glutSolidCube(1);
	glPopMatrix();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    //For testing
    if (cameraToggle) {
        gluLookAt(7,7,7, rocket.position.mX, rocket.position.mY + forwardDistance, rocket.position.mZ, 0,1,0);
    }
    else {
        gluLookAt(2, -8 + forwardDistance, 0, 0,forwardDistance,0, 1,0,0);
    }

    glColor3f(0,0,1);

	glPushMatrix();
	glScalef(0.1, 10, 10);
	glutSolidCube(1);
	glPopMatrix();

    glColor3f(0,1,0);
    glPushMatrix();
    glTranslatef(0.4,0,0);
	glScalef(1, 1, 1);
	glutSolidCube(1);
	glPopMatrix();

    drawRocket(rocket);

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

    //Display Coins
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
            if (rocket.zOffset < 2) {
                rocket.zOffset += 0.1;
                rocket.angle -=1;
            
            }
            break;
        case 'd':
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

    glutPostRedisplay();
    glutTimerFunc(17, FPS, 0);
}

void init(void)
{
	glColor3f(1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-1, 1, -1, 1, -20, 20);
	gluPerspective(60, 1, 1, 20);
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