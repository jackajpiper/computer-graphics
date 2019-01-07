#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>

#include <stdlib.h>
#include <stddef.h>
#include <math.h>

const float RAD_TO_DEG = 180.0/(atan(1) * 4);

float g_eye_z = 2.0f;
float eyeAcross = 0.0f;
float eyeHeight = 0.0f;
float lookAngleX = 0.0f;
float lookAngleY = 0.0f;
float viewZ = 0.0f;
float viewX = 0.0f;

// The array storing each 2d box representing the bounds of a cuboid
// Used later for collision detection
float boundsArray[1000][4][3];
// Number of cuboids in the world
int cuboidNum = 0;



bool detectCollision() {
  int i;
  for(i = 0; i < cuboidNum; i++) {
    // If the player position is between the left x vertex and x+width, COLLISION
    // If the player position is between the lower Y vertex and y+height, COLLISION
    float lowerX;
    float higherX;
    // Finds the lowest and highest x coordinates
        // THIS MIGHT BE FUCKY, AS THE CUBOID COULD BE ROTATED SLIGHTLY WHICH WOULD MEAN
        // THE HITBOX WOULDN'T BE THE ACTUAL DIMENSIONS OF THE CUBOID
    lowerX = (boundsArray[i][0][0]<boundsArray[i][1][0])?boundsArray[i][0][0]:boundsArray[i][1][0];
    lowerX = (lowerX>boundsArray[i][2][0])?boundsArray[i][2][0]:lowerX;
    lowerX = (lowerX>boundsArray[i][3][0])?boundsArray[i][3][0]:lowerX;
    higherX = (boundsArray[i][0][0]<boundsArray[i][1][0])?boundsArray[i][1][0]:boundsArray[i][0][0];
    higherX = (higherX<boundsArray[i][2][0])?boundsArray[i][2][0]:higherX;
    higherX = (higherX<boundsArray[i][3][0])?boundsArray[i][3][0]:higherX;
    float lowerY;
    float higherY;
    lowerY = (boundsArray[i][0][2]<boundsArray[i][1][2])?boundsArray[i][0][2]:boundsArray[i][1][2];
    lowerY = (lowerY>boundsArray[i][2][2])?boundsArray[i][2][2]:lowerY;
    lowerY = (lowerY>boundsArray[i][3][2])?boundsArray[i][3][2]:lowerY;
    higherY = (boundsArray[i][0][2]<boundsArray[i][1][2])?boundsArray[i][1][2]:boundsArray[i][0][2];
    higherY = (higherY<boundsArray[i][2][2])?boundsArray[i][2][2]:higherY;
    higherY = (higherY<boundsArray[i][3][2])?boundsArray[i][3][2]:higherY;

    // Final check. The 0.1 is to account for the near plane
    if((eyeAcross>lowerX-0.1) & (eyeAcross<higherX+0.1) & (g_eye_z>lowerY-0.1) & (g_eye_z<higherY+0.1)) {
      std::cerr << "\t COLLISION            COLLISION           COLLISION" << std::endl;
      return true;
    }
  }
  std::cerr << "\t No collision. Player: (" << eyeAcross << "," << eyeHeight << "," << g_eye_z <<  ")" << std::endl;
  return false;
}

void cuboid2(float transX, float transY, float transZ, float x, float y, float z, float rotation) {

  glPushMatrix();
    glTranslatef(transX, transY, transZ);
    glRotatef(rotation, 0.0, 1.0, 0.0);
    //Multi-colored side - FRONT
    glBegin(GL_POLYGON);
    glColor3f( 1.0, 0.0, 0.0 );     glVertex3f(  0.5*x, -0.5*y, -0.5*z );      // P1 is red
    glColor3f( 0.0, 1.0, 0.0 );     glVertex3f(  0.5*x,  0.5*y, -0.5*z );      // P2 is green
    glColor3f( 0.0, 0.0, 1.0 );     glVertex3f( -0.5*x,  0.5*y, -0.5*z );      // P3 is blue
    glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5*x, -0.5*y, -0.5*z );      // P4 is purple
    glEnd();

    // White side - BACK
    glBegin(GL_POLYGON);
    glColor3f(   1.0,  1.0, 1.0 );
    glVertex3f(  0.5*x, -0.5*y, 0.5*z ); // bottom left
    glVertex3f(  0.5*x,  0.5*y, 0.5*z ); // top left
    glVertex3f( -0.5*x,  0.5*y, 0.5*z ); // top right
    glVertex3f( -0.5*x, -0.5*y, 0.5*z ); // bottom right
    glEnd();

    // Purple side - RIGHT
    glBegin(GL_POLYGON);
    glColor3f(  1.0,  0.0,  1.0 );
    glVertex3f( 0.5*x, -0.5*y, -0.5*z ); // bottom left
    glVertex3f( 0.5*x,  0.5*y, -0.5*z ); // top left
    glVertex3f( 0.5*x,  0.5*y,  0.5*z ); // top right
    glVertex3f( 0.5*x, -0.5*y,  0.5*z ); // bottom right
    glEnd();

    // Green side - LEFT
    glBegin(GL_POLYGON);
    glColor3f(   0.0,  1.0,  0.0 );
    glVertex3f( -0.5*x, -0.5*y,  0.5*z ); // botttom left
    glVertex3f( -0.5*x,  0.5*y,  0.5*z ); // top left
    glVertex3f( -0.5*x,  0.5*y, -0.5*z ); // top right
    glVertex3f( -0.5*x, -0.5*y, -0.5*z ); // bottom right
    glEnd();

    // Blue side - TOP
    glBegin(GL_POLYGON);
    glColor3f(   0.0,  0.0,  1.0 );
    glVertex3f(  0.5*x,  0.5*y,  0.5*z ); // bottom left
    glVertex3f(  0.5*x,  0.5*y, -0.5*z ); // top left
    glVertex3f( -0.5*x,  0.5*y, -0.5*z ); // top right
    glVertex3f( -0.5*x,  0.5*y,  0.5*z ); // bottom right
    glEnd();

    // Red side - BOTTOM
    glBegin(GL_POLYGON);
    glColor3f(   1.0,  0.0,  0.0 );
    glVertex3f(  0.5*x, -0.5*y, -0.5*z ); // bottom left
    glVertex3f(  0.5*x, -0.5*y,  0.5*z ); // top left
    glVertex3f( -0.5*x, -0.5*y,  0.5*z ); // top right
    glVertex3f( -0.5*x, -0.5*y, -0.5*z ); // bottom right
    glEnd();
  glPopMatrix();

  // Adds the verticies to the array of bounds
  // BOTTOM LEFT
  boundsArray[cuboidNum][0][0] = 0.5*x;
  boundsArray[cuboidNum][0][1] = -0.5*y;
  boundsArray[cuboidNum][0][2] = -0.5*z;
  // TOP LEFT
  boundsArray[cuboidNum][1][0] = 0.5*x;
  boundsArray[cuboidNum][1][1] = -0.5*y;
  boundsArray[cuboidNum][1][2] = 0.5*z;
  // TOP RIGHT
  boundsArray[cuboidNum][2][0] = -0.5*x;
  boundsArray[cuboidNum][2][1] = -0.5*y;
  boundsArray[cuboidNum][2][2] = 0.5*z;
  // BOTTOM RIGHT
  boundsArray[cuboidNum][3][0] = -0.5*x;
  boundsArray[cuboidNum][3][1] = -0.5*y;
  boundsArray[cuboidNum][3][2] = -0.5*z;

  cuboidNum++;

}


void cuboid(float x, float y, float z) {
  //Multi-colored side - FRONT
  glBegin(GL_POLYGON);
  glColor3f( 1.0, 0.0, 0.0 );     glVertex3f(  0.5*x, -0.5*y, -0.5*z );      // P1 is red
  glColor3f( 0.0, 1.0, 0.0 );     glVertex3f(  0.5*x,  0.5*y, -0.5*z );      // P2 is green
  glColor3f( 0.0, 0.0, 1.0 );     glVertex3f( -0.5*x,  0.5*y, -0.5*z );      // P3 is blue
  glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5*x, -0.5*y, -0.5*z );      // P4 is purple
  glEnd();

  // White side - BACK
  glBegin(GL_POLYGON);
  glColor3f(   1.0,  1.0, 1.0 );
  glVertex3f(  0.5*x, -0.5*y, 0.5*z ); // bottom left
  glVertex3f(  0.5*x,  0.5*y, 0.5*z ); // top left
  glVertex3f( -0.5*x,  0.5*y, 0.5*z ); // top right
  glVertex3f( -0.5*x, -0.5*y, 0.5*z ); // bottom right
  glEnd();

  // Purple side - RIGHT
  glBegin(GL_POLYGON);
  glColor3f(  1.0,  0.0,  1.0 );
  glVertex3f( 0.5*x, -0.5*y, -0.5*z ); // bottom left
  glVertex3f( 0.5*x,  0.5*y, -0.5*z ); // top left
  glVertex3f( 0.5*x,  0.5*y,  0.5*z ); // top right
  glVertex3f( 0.5*x, -0.5*y,  0.5*z ); // bottom right
  glEnd();

  // Green side - LEFT
  glBegin(GL_POLYGON);
  glColor3f(   0.0,  1.0,  0.0 );
  glVertex3f( -0.5*x, -0.5*y,  0.5*z ); // botttom left
  glVertex3f( -0.5*x,  0.5*y,  0.5*z ); // top left
  glVertex3f( -0.5*x,  0.5*y, -0.5*z ); // top right
  glVertex3f( -0.5*x, -0.5*y, -0.5*z ); // bottom right
  glEnd();

  // Blue side - TOP
  glBegin(GL_POLYGON);
  glColor3f(   0.0,  0.0,  1.0 );
  glVertex3f(  0.5*x,  0.5*y,  0.5*z ); // bottom left
  glVertex3f(  0.5*x,  0.5*y, -0.5*z ); // top left
  glVertex3f( -0.5*x,  0.5*y, -0.5*z ); // top right
  glVertex3f( -0.5*x,  0.5*y,  0.5*z ); // bottom right
  glEnd();

  // Red side - BOTTOM
  glBegin(GL_POLYGON);
  glColor3f(   1.0,  0.0,  0.0 );
  glVertex3f(  0.5*x, -0.5*y, -0.5*z ); // bottom left
  glVertex3f(  0.5*x, -0.5*y,  0.5*z ); // top left
  glVertex3f( -0.5*x, -0.5*y,  0.5*z ); // top right
  glVertex3f( -0.5*x, -0.5*y, -0.5*z ); // bottom right
  glEnd();

  // Adds the verticies to the array of bounds
  // BOTTOM LEFT
  boundsArray[cuboidNum][0][0] = 0.5*x;
  boundsArray[cuboidNum][0][1] = -0.5*y;
  boundsArray[cuboidNum][0][2] = -0.5*z;
  // TOP LEFT
  boundsArray[cuboidNum][1][0] = 0.5*x;
  boundsArray[cuboidNum][1][1] = -0.5*y;
  boundsArray[cuboidNum][1][2] = 0.5*z;
  // TOP RIGHT
  boundsArray[cuboidNum][2][0] = -0.5*x;
  boundsArray[cuboidNum][2][1] = -0.5*y;
  boundsArray[cuboidNum][2][2] = 0.5*z;
  // BOTTOM RIGHT
  boundsArray[cuboidNum][3][0] = -0.5*x;
  boundsArray[cuboidNum][3][1] = -0.5*y;
  boundsArray[cuboidNum][3][2] = -0.5*z;

  cuboidNum++;

}

void drawGrid() {

	for(float i = -20; i <= 20; i += 0.2)
	{
		glBegin(GL_LINES);
			glColor3ub(150, 190, 150);
			glVertex3f(-20, -0.25, i);
			glVertex3f(20, -0.25, i);

			glVertex3f(i, -0.25, -20);
			glVertex3f(i, -0.25, 20);
		glEnd();
	}
}

void rightTurn(double x, double y, double length, double rotate) {
  // Corridor going to the right

  cuboid2(x, 0.0, y, length+0.05,0.5,0.1, rotate);

  glPushMatrix();
    glTranslatef(x, 0.0, y);
    glRotatef(rotate, 0.0, 1.0, 0.0);
    glPushMatrix();
      cuboid2(-0.45f, 0.0f, -1.0f, length+0.95,0.5,0.1, 0);
    glPopMatrix();

    glPushMatrix();
      glRotatef(90.0, 0.0, 1.0, 0.0);
      glTranslatef(0.5f, 0.0f, -(length/2)-0.975f);
      cuboid(1.1,0.5,0.1);
    glPopMatrix();
  glPopMatrix();

  glFlush();
}

void leftTurn(double x, double y, double length, double rotate) {
    glPushMatrix();
      glTranslatef(x, 0.0, y);
      glRotatef(rotate, 0.0, 1.0, 0.0);
      cuboid(length+0.05,0.5,0.1);

      glPushMatrix();
        glTranslatef(0.45f, 0.0f, -1.0f);
        cuboid(length+0.95,0.5,0.1);
      glPopMatrix();

      glPushMatrix();
        glRotatef(90.0, 0.0, 1.0, 0.0);
        glTranslatef(0.5f, 0.0f, (length/2)+0.975f);
        cuboid(1.1,0.5,0.1);
      glPopMatrix();
    glPopMatrix();

    glFlush();
}

void tJunction(double x, double y, double length, double rotate) {
  // The length of the short walls either side of the entrance
  // and the offset of the longer wall to line it up
  double shortLength = (length/2) - 0.55;
  double offsetLength = (length/4) + 0.275;

  glPushMatrix();
    // Two short walls either side of the entrance
    glTranslatef(x, 0.0, y);
    glRotatef(rotate, 0.0, 1.0, 0.0);
    cuboid(shortLength,0.5,0.1);
    glPushMatrix();
      glTranslatef(shortLength + 1.1, 0.0f, 0.0f);
      cuboid(shortLength,0.5,0.1);
    glPopMatrix();

    // Long wall
    glPushMatrix();
      glTranslatef(offsetLength, 0.0f, -1.0f);
      cuboid(length,0.5,0.1);
    glPopMatrix();

    // Entrance
    glPushMatrix();
      glRotatef(90.0, 0.0, 1.0, 0.0);
      glTranslatef(-0.45f, 0.0f, shortLength/2 + 0.05);
      cuboid(1.0,0.5,0.1);
      glPushMatrix();
        glTranslatef(0.0, 0.0f, 1.0);
        cuboid(1.0,0.5,0.1);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  glFlush();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// position and orient camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeAcross, eyeHeight, g_eye_z, // eye positions
			  viewX, eyeHeight, viewZ, // reference point
			  0, 1, 0  // up vector
		);
	glColor3f(1.0f, 1.0f, 1.0f);

  drawGrid();

  // Two walls at the beginning of the maze
  // glPushMatrix();
  //   glRotatef(90.0, 0.0, 1.0, 0.0);
  //   cuboid(1.0,0.5,0.1);
  //   glPushMatrix();
  //   // To increase the length of one wall to meet the edge of an adjacent wall,
  //   // increase the length by the width of the corridor and then add/subtract
  //   // half of the wall width. Don't forget to shift the wall forwards by half
  //   // the increase length, as the wall will grow at both ends.
  //     glTranslatef(0.0f, 0.0f, 1.0f);
  //     cuboid(1.0,0.5,0.1);
  //   glPopMatrix();
  // glPopMatrix();

  // cuboid(1,0.5,0.1);

  rightTurn(2.0,-1.0,1.0,-90.0);

  // tJunction(-0.525,0,3,0);
  //
  // // Route one
  // rightTurn(-1.05,-1.075,0.2,90);
  // rightTurn(-0.075,-1.25,2,0);
  // leftTurn(1,-2.225,0,-90);
  // tJunction(0.725,-3.2,2,0);
  //
  // // Route 1a
  // rightTurn(-1.025,-3.2,3,0);
  // rightTurn(-3.575,-3.1,0.2,180);
  // rightTurn(-3.75,-4.075,2,90);
  // rightTurn(-2.775,-5.15,2,0);
  //
  // // Route 1b
  // leftTurn(2.55,-4.625,0.9,-90);
  // leftTurn(0.425,-5.15,4.3,0);
  //
  // // Route two
  // rightTurn(2.05,0.475,1,-90);
  // glPushMatrix();
  //   glTranslatef(2.55, 0.0f, 1.05f);
  //   cuboid(1.1,0.5,0.1);
  // glPopMatrix();
  //
  // // Finish
  // glPushMatrix();
  //   glTranslatef(0, 0, -5.5);
  //   cuboid(0.1,0.1,0.1);
  // glPopMatrix();

  glFlush();
	glutSwapBuffers();
}

void moveBackAndForth(bool direction) // forwards is true
{
  float vectorX = viewX - eyeAcross;
  float vectorY = 0.0f;
  float vectorZ = viewZ - g_eye_z;

  float speed = 0.0f;
  if(direction) speed = 0.05f;
  else speed = -0.05f;

  eyeAcross  = eyeAcross  + vectorX * speed;
  g_eye_z  = g_eye_z  + vectorZ * speed;
  viewX = viewX + vectorX * speed;
  viewZ = viewZ + vectorZ * speed;

  if(detectCollision()) {
    eyeAcross  = eyeAcross - vectorX * speed;
  	g_eye_z  = g_eye_z - vectorZ * speed;
  	viewX = viewX - vectorX * speed;
  	viewZ = viewZ - vectorZ * speed;
  }

  cuboidNum = 0;
  glutPostRedisplay();

}

void rotateView(bool direction) // right is true
{
  float speed  = 0.0f;
  float vectorX = 0+viewX - (1+eyeAcross);
  float vectorY = 0.0f;
  float vectorZ = viewZ - g_eye_z;

	if (direction) { // mouse moved to the right
		speed = 0.05f;
  }
	else  {// mouse moved to the left
		speed = -0.05f;
  }

  viewZ = (float)(g_eye_z + sin(speed)*vectorX + cos(speed)*vectorZ);
  viewX = (float)(1+eyeAcross + cos(speed)*vectorX - sin(speed)*vectorZ);

  cuboidNum = 0;
	glutPostRedisplay();
}

void specialKeys( int key, int x, int y )
{
  switch (key) {

    case GLUT_KEY_RIGHT: rotateView(true); break;

    case GLUT_KEY_LEFT: rotateView(false); break;

    case GLUT_KEY_UP: moveBackAndForth(true); break;

    case GLUT_KEY_DOWN: moveBackAndForth(false); break;
  }
}

void keyboard(unsigned char key, int, int)
{
	switch (key)
	{
		case 'q': exit(1); break;
		case 'h':
      gluLookAt(0, 0, 10, // eye positions
            0, 0, 0, // reference point
            0, 1, 0  // up vector
      );
      std::cerr << "\t eye z  = " << g_eye_z << std::endl;
      break;
      case 'a': eyeHeight += 0.25; break;
      case 'z': eyeHeight -= 0.25; break;
	}
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// for perspective projection use the GLU helper
	// take FOV, ASPECT RATIO, NEAR, FAR
	gluPerspective(40.0, 1.0f, 0.1f, 50.0);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);

	glutInitWindowSize(750, 750);
	glutInitWindowPosition(0, 10);

	glutCreateWindow("Perspective Viewing");

  glEnable(GL_DEPTH_TEST);

  glutSpecialFunc(specialKeys);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	glutMainLoop();

	return 0;
}
