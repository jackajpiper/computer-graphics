//
// File:        mycube.c
// Author:      Matt Daisley
// Created:     4/25/2012
// Project:     Source code for Make a Cube in OpenGL
// Description: Creates an OpenGL window and draws a 3D cube
//              That the user can rotate using the arrow keys
//
// Controls:    Left Arrow  - Rotate Left
//              Right Arrow - Rotate Right
//              Up Arrow    - Rotate Up
//              Down Arrow  - Rotate Down

// ----------------------------------------------------------
// Includes
// ----------------------------------------------------------

#include <iostream>

#include <stdlib.h>
#include <stddef.h>



#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void display();
void specialKeys();

// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
double rotate_y=0;
double rotate_x=0;
int lookAcross=0;
double eyeUp=0;
double eyeAcross=0;
double viewUp=0;
double viewAcross=0;
int depth=0;
int zoom_amount=0;

double mouseX;
double mouseY;

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
}

void Draw_Grid() {

	for(float i = -500; i <= 500; i += 0.2)
	{
		glBegin(GL_LINES);
			glColor3ub(150, 190, 150);
			glVertex3f(-500, 0, i);
			glVertex3f(500, 0, i);

			glVertex3f(i, 0, -500);
			glVertex3f(i, 0, 500);
		glEnd();
	}
}

// ----------------------------------------------------------
// display() Callback function
// ----------------------------------------------------------
void display(){

  //  Clear screen and Z-buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // Reset transformations
	glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0+eyeAcross, 1+eyeUp, 1, // eye position
			  0+viewAcross, 0+viewUp, 0, // reference point
			  0, 1, 0  // up vector
		);

  // gluLookAt(0+(0.01*width), 1+(0.01*height), 1+(0.01*depth), // eye position
	// 		  0+(0.01*width), 0.2+(0.01*height), 0, // reference point
	// 		  0, 1, 0  // up vector
	// 	);

  // Other Transformations
  // glTranslatef( 0.1, 0.0, 0.0 );      // Not included
  // glRotatef( 180, 0.0, 1.0, 0.0 );    // Not included

  Draw_Grid();
  cuboid(2.5,0.5,0.1);

  // Other Transformations
  // glScalef( 2.0, 2.0, 0.0 );          // Not included


  // glPushMatrix();
  //
  //   glScalef( 1.0+(0.01*zoom_amount), 1.0+(0.01*zoom_amount), 1.0+(0.01*zoom_amount) );
  //   // Rotate when user changes rotate_x and rotate_y
  //   // glRotatef( rotate_x, 1.0, 0.0, 0.0 );
  //   // glRotatef( rotate_y, 0.0, 1.0, 0.0 );
  //   cuboid(2.5,0.5,0.1);
  //
  //   glPushMatrix();
  //     // place the shoulder at origin (this is the default)
  //     glTranslatef(0.0f, 0.0f, 0.5f);
  //     cuboid(2.5,0.5,0.1);
  //   glPopMatrix();
  //
  //   Draw_Grid();
  //
  // glPopMatrix();

  glFlush();
  glutSwapBuffers();

}

// ----------------------------------------------------------
// specialKeys() Callback Function
// ----------------------------------------------------------
void specialKeys( int key, int x, int y ) {

  switch (key) {

    case GLUT_KEY_RIGHT: viewAcross += 0.05;break;

    case GLUT_KEY_LEFT: viewAcross -= 0.05;break;

    case GLUT_KEY_UP: viewUp += 0.05;break;

    case GLUT_KEY_DOWN: viewUp -= 0.05;break;

    // case WM_MOUSEMOVE:
  	// 		// save old mouse coordinates
  	// 		double oldMouseX = mouseX;
  	// 		double oldMouseY = mouseY;
    //
  	// 		// get mouse coordinates from Windows
  	// 		mouseX = LOWORD(lParam);
  	// 		mouseY = HIWORD(lParam);
    //
  	// 		// these lines limit the camera's range
  	// 		if (mouseY < 60)
  	// 			mouseY = 60;
  	// 		if (mouseY > 450)
  	// 			mouseY = 450;
    //
  	// 		if ((mouseX - oldMouseX) > 0)		// mouse moved to the right
  	// 			angle += 3.0f;
  	// 		else if ((mouseX - oldMouseX) < 0)	// mouse moved to the left
  	// 			angle -= 3.0f;
    //
  	// 		return 0;
  	// 		break;
  }


  std::cerr << "\t viewUp  = " << viewUp << std::endl;
  std::cerr << "\t viewAcross  = " << viewAcross << std::endl;

  //  Request display update
  glutPostRedisplay();

}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// for perspective projection use the GLU helper
	// take FOV, ASPECT RATIO, NEAR, FAR
	gluPerspective(90.0, 1.0f, 0.1f,100.0f);
}

void keyboard(unsigned char key, int, int)
{
	switch (key)
	{
		case 'q': exit(1); break;
		case 'w': eyeUp -= 0.02; break;
		case 's': eyeUp += 0.02; break;
		case 'd': eyeAcross -= 0.02; break;
		case 'a': eyeAcross += 0.02; break;
		case 'z': zoom_amount -= 2; break;
		case 'x': zoom_amount += 2; break;
		case '1': depth -= 1; break;
		case '2': depth += 1; break;
	}
  std::cerr << "\t eyeUp  = " << eyeUp << std::endl;
  std::cerr << "\t eyeAcross  = " << eyeAcross << std::endl;
  std::cerr << "\t lookAcross  = " << lookAcross << std::endl;
	glutPostRedisplay();
}

// ----------------------------------------------------------
// main() function
// ----------------------------------------------------------
int main(int argc, char* argv[]){

  //  Initialize GLUT and process user parameters
  glutInit(&argc,argv);

  //  Request double buffered true color window with Z-buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(512, 512);
	glutInitWindowPosition(50, 50);

  // glDepthRange(0,1);
  // Create window
  glutCreateWindow("Awesome Cube");

  //  Enaaaaaable Z-buffer depth test
  glEnable(GL_DEPTH_TEST);


  gluPerspective(40.0, 1.0f, 0.1f,1.0f);

  // Callback functions
  glutSpecialFunc(specialKeys);
  glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
  glutDisplayFunc(display);

  // init();

  //  Pass control to GLUT for events
  glutMainLoop();

  //  Return to OS
  return 0;
}
