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

// ----------------------------------------------------------
// display() Callback function
// ----------------------------------------------------------
void display(){

  //  Clear screen and Z-buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // Reset transformations
  glLoadIdentity();
  // gluLookAt(0, 0, 1, // eye position
	// 		  0, 0, 0, // reference point
	// 		  0, 1, 0  // up vector
	// 	);

  // Other Transformations
  // glTranslatef( 0.1, 0.0, 0.0 );      // Not included
  // glRotatef( 180, 0.0, 1.0, 0.0 );    // Not included

  // Rotate when user changes rotate_x and rotate_y
  glRotatef( rotate_x, 1.0, 0.0, 0.0 );
  glRotatef( rotate_y, 0.0, 1.0, 0.0 );

  // Other Transformations
  // glScalef( 2.0, 2.0, 0.0 );          // Not included

  cuboid(0.5,0.5,0.5);

  // draw the arm parts and articulate them
  // glPushMatrix();
  //
  //   // place the shoulder at origin (this is the default)
  //   glTranslatef(1.0f, 0.0f, 0.0f);
  //   cuboid(0.5,0.5,1);
  //
  // glPopMatrix();

  glFlush();
  glutSwapBuffers();

}

// ----------------------------------------------------------
// specialKeys() Callback Function
// ----------------------------------------------------------
void specialKeys( int key, int x, int y ) {

  //  Right arrow - increase rotation by 5 degree
  if (key == GLUT_KEY_RIGHT)
    rotate_y += 5;

  //  Left arrow - decrease rotation by 5 degree
  else if (key == GLUT_KEY_LEFT)
    rotate_y -= 5;

  else if (key == GLUT_KEY_UP)
    rotate_x += 5;

  else if (key == GLUT_KEY_DOWN)
    rotate_x -= 5;

  //  Request display update
  glutPostRedisplay();

}

// void reshape(int w, int h)
// {
// 	glViewport(0, 0, w, h);
// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();
// 	// for perspective projection use the GLU helper
// 	// take FOV, ASPECT RATIO, NEAR, FAR
// 	gluPerspective(40.0, 1.0f, 1.0, 5.0);
// }

// void init_lights(const GLenum shade_model=GL_FLAT)
// {
// 	float light_position[] = {1.0, 1.0, 1.0, 0.0};
// 	float light_ambient[] = {0.1, 0.1, 0.1, 1.0};
//
// 	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
// 	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//
//     glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
//
// 	glFrontFace(GL_CW);
// 	glEnable(GL_LIGHTING);
// 	glEnable(GL_LIGHT0);
//
//     glEnable(GL_AUTO_NORMAL);
//     glEnable(GL_NORMALIZE);
// 	glEnable(GL_DEPTH_TEST);
//
//
// 	glShadeModel(shade_model); // GL_FLAT, GL_SMOOTH
//
// 	//glEnable(GL_CULL_FACE);
// 	//glCullFace(GL_BACK);
// }

// void init()
// {
// 	init_lights();
// }


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

  glDepthRange(0,5);
  // Create window
  glutCreateWindow("Awesome Cube");

  //  Enable Z-buffer depth test
  glEnable(GL_DEPTH_TEST);

  // Callback functions
  glutSpecialFunc(specialKeys);
	// glutReshapeFunc(reshape);
  glutDisplayFunc(display);

  // init();

  //  Pass control to GLUT for events
  glutMainLoop();

  //  Return to OS
  return 0;
}
