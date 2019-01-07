#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>

#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <unistd.h>
#include "../cs324-labs/lab-7/png_load.h"

const float RAD_TO_DEG = 180.0/(atan(1) * 4);

float g_eye_z = 2.0f;
float eyeAcross = 0.0f;
float eyeHeight = 0.0f;
float lookAngleX = 0.0f;
float lookAngleY = 0.0f;
float viewZ = 0.0f;
float viewX = 0.0f;
float viewY = 0.0f;
bool helpMode = false;
float savedX = 0.0;
float savedZ = 0.0;
float savedViewX = 0.0;
float savedViewZ = 0.0;

// single texture
unsigned int g_the_tex = 0; // texture handle

enum van_sides_t {FRONT=0, LEFT_SIDE=1, RIGHT_SIDE=2,
                  TOP=3, BACK=4, NUM_SIDES=5};
unsigned int g_road = 0;

// The array storing each 2d box representing the bounds of a cuboid
// Used later for collision detection
float boundsArray[1000][4][3];
// Number of cuboids in the world
int cuboidNum = 0;

unsigned int load_and_bind_texture(const char* filename)
{
    char* image_buffer = NULL; // the image data
	int width = 0;
	int height = 0;

	// read in the PNG image data into image_buffer
	if (png_load(filename, &width, &height, &image_buffer)==0)
    {
        fprintf(stderr, "Failed to read image texture from %s\n", filename);
        exit(1);
    }

	unsigned int tex_handle = 0;

	// request one texture handle
	glGenTextures(1, &tex_handle);

	// create a new texture object and bind it to tex_handle
  	glBindTexture(GL_TEXTURE_2D, tex_handle);

  	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  	glTexImage2D(GL_TEXTURE_2D, 0,
				GL_RGB, width, height, 0,
   		 		GL_RGB, GL_UNSIGNED_BYTE, image_buffer);


	free(image_buffer); // free the image buffer memory

	return tex_handle;
}

void load_and_bind_textures()
{
	// load all textures here
	g_road = load_and_bind_texture("/wood-grain.png");

}

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

    std::cerr << "\t Lower X is " << lowerX << " and higher X is " << higherX << std::endl;
    std::cerr << "\t Lower Y is " << lowerY << " and higher Y is " << higherY << std::endl;

    std::cerr << "\t Player: (" << eyeAcross << "," << eyeHeight << "," << g_eye_z <<  ")" << std::endl;

    // Final check. The 0.1 is to account for the near plane
    if((eyeAcross>lowerX-0.1) & (eyeAcross<higherX+0.1) & (g_eye_z>lowerY-0.1) & (g_eye_z<higherY+0.1)) {
      std::cerr << "\t COLLISION            COLLISION           COLLISION" << eyeAcross << "," << eyeHeight << "," << g_eye_z <<  ")" << std::endl;
      // return true;
    }
  }
  return false;
}

void cuboid3(float x, float y, float z) {
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

void cuboid2(float transX, float transY, float transZ, float x, float y, float z, float rotation) {

  float modelView[16];

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

    glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

  glPopMatrix();
  glFlush();

  // Adds the verticies to the array of bounds
  // BOTTOM LEFT



  // vec3 firstPoint = vec3(0.5*x, -0.5*y, -0.5*z);
  //
  // boundsArray[cuboidNum][0][0] = 0.5*x;
  // boundsArray[cuboidNum][0][1] = -0.5*y;
  // boundsArray[cuboidNum][0][2] = -0.5*z;
  // // TOP LEFT
  // boundsArray[cuboidNum][1][0] = 0.5*x;
  // boundsArray[cuboidNum][1][1] = -0.5*y;
  // boundsArray[cuboidNum][1][2] = 0.5*z;
  // // TOP RIGHT
  // boundsArray[cuboidNum][2][0] = -0.5*x;
  // boundsArray[cuboidNum][2][1] = -0.5*y;
  // boundsArray[cuboidNum][2][2] = 0.5*z;
  // // BOTTOM RIGHT
  // boundsArray[cuboidNum][3][0] = -0.5*x;
  // boundsArray[cuboidNum][3][1] = -0.5*y;
  // boundsArray[cuboidNum][3][2] = -0.5*z;
  //
  // cuboidNum++;

}

void cuboid(float x, float y, float z) {
  // glLoadIdentity();
  //Multi-colored side - FRONT

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, g_road);
  glBegin(GL_POLYGON);
  glVertex3f(  0.5*x, -0.5*y, -0.5*z );      // P1 is red
  glVertex3f(  0.5*x,  0.5*y, -0.5*z );      // P2 is green
  glVertex3f( -0.5*x,  0.5*y, -0.5*z );      // P3 is blue
  glVertex3f( -0.5*x, -0.5*y, -0.5*z );      // P4 is purple
  glEnd();

  // White side - BACK
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, g_road);
  glBegin(GL_POLYGON);
  glVertex3f(  0.5*x, -0.5*y, 0.5*z ); // bottom left
  glVertex3f(  0.5*x,  0.5*y, 0.5*z ); // top left
  glVertex3f( -0.5*x,  0.5*y, 0.5*z ); // top right
  glVertex3f( -0.5*x, -0.5*y, 0.5*z ); // bottom right
  glEnd();

  // Purple side - RIGHT
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, g_road);
  glBegin(GL_POLYGON);
  glVertex3f( 0.5*x, -0.5*y, -0.5*z ); // bottom left
  glVertex3f( 0.5*x,  0.5*y, -0.5*z ); // top left
  glVertex3f( 0.5*x,  0.5*y,  0.5*z ); // top right
  glVertex3f( 0.5*x, -0.5*y,  0.5*z ); // bottom right
  glEnd();

  // Green side - LEFT
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, g_road);
  glBegin(GL_POLYGON);
  glVertex3f( -0.5*x, -0.5*y,  0.5*z ); // botttom left
  glVertex3f( -0.5*x,  0.5*y,  0.5*z ); // top left
  glVertex3f( -0.5*x,  0.5*y, -0.5*z ); // top right
  glVertex3f( -0.5*x, -0.5*y, -0.5*z ); // bottom right
  glEnd();

  // Blue side - TOP
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, g_road);
  glBegin(GL_POLYGON);
  glVertex3f(  0.5*x,  0.5*y,  0.5*z ); // bottom left
  glVertex3f(  0.5*x,  0.5*y, -0.5*z ); // top left
  glVertex3f( -0.5*x,  0.5*y, -0.5*z ); // top right
  glVertex3f( -0.5*x,  0.5*y,  0.5*z ); // bottom right
  glEnd();

  // Red side - BOTTOM
  glBegin(GL_POLYGON);
  glVertex3f(  0.5*x, -0.5*y, -0.5*z ); // bottom left
  glVertex3f(  0.5*x, -0.5*y,  0.5*z ); // top left
  glVertex3f( -0.5*x, -0.5*y,  0.5*z ); // top right
  glVertex3f( -0.5*x, -0.5*y, -0.5*z ); // bottom right
  glEnd();
  // glLoadIdentity();

  glDisable(GL_TEXTURE_2D);

  float mv[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, mv);


  // std::cerr << "\t Current model matrix:" << std::endl;
  // std::cerr << "\t " << mv[0] << ", " << mv[1] << ", " << mv[2] << ", " << mv[3] << std::endl;
  // std::cerr << "\t " << mv[4] << ", " << mv[5] << ", " << mv[6] << ", " << mv[7] << std::endl;
  // std::cerr << "\t " << mv[8] << ", " << mv[9] << ", " << mv[10] << ", " << mv[11] << std::endl;
  // std::cerr << "\t " << mv[12] << ", " << mv[13] << ", " << mv[14] << ", " << mv[15] << std::endl;


  // glPushMatrix();
  //   glLoadIdentity();
  //   // Adds the verticies to the array of bounds
  //   // BOTTOM LEFT
  //   float xp = mv[0] * 0.5*x + mv[4] * -0.5*y + mv[8] * -0.5*z;
  //   float yp = mv[1] * 0.5*x + mv[5] * -0.5*y + mv[9] * -0.5*z;
  //   float zp = mv[2] * 0.5*x + mv[6] * -0.5*y + mv[10] * -0.5*z;
  //   float wp = mv[3] * 0.5*x + mv[7] * -0.5*y + mv[11] * -0.5*z;
  //   boundsArray[cuboidNum][0][0] = xp;
  //   boundsArray[cuboidNum][0][1] = yp;
  //   boundsArray[cuboidNum][0][2] = zp;
  //
  //   // glPushMatrix();
  //   //   glLoadIdentity();
  //   //   glTranslatef(xp,yp,zp);
  //   //   cuboid3(0.02,0.02,0.02);
  //   // glPopMatrix();
  //
  //   // TOP LEFT
  //   xp = mv[0] * 0.5*x + mv[4] * -0.5*y + mv[8] * 0.5*z;
  //   yp = mv[1] * 0.5*x + mv[5] * -0.5*y + mv[9] * 0.5*z;
  //   zp = mv[2] * 0.5*x + mv[6] * -0.5*y + mv[10] * 0.5*z;
  //   wp = mv[3] * 0.5*x + mv[7] * -0.5*y + mv[11] * 0.5*z;
  //   boundsArray[cuboidNum][1][0] = xp;
  //   boundsArray[cuboidNum][1][1] = yp;
  //   boundsArray[cuboidNum][1][2] = zp;
  //
  //   // glPushMatrix();
  //   //   glLoadIdentity();
  //   //   glTranslatef(xp,yp,zp);
  //   //   cuboid3(0.02,0.02,0.02);
  //   // glPopMatrix();
  //
  //   // TOP RIGHT
  //   xp = mv[0] * -0.5*x + mv[4] * -0.5*y + mv[8] * 0.5*z;
  //   yp = mv[1] * -0.5*x + mv[5] * -0.5*y + mv[9] * 0.5*z;
  //   zp = mv[2] * -0.5*x + mv[6] * -0.5*y + mv[10] * 0.5*z;
  //   wp = mv[3] * -0.5*x + mv[7] * -0.5*y + mv[11] * 0.5*z;
  //   boundsArray[cuboidNum][2][0] = xp;
  //   boundsArray[cuboidNum][2][1] = yp;
  //   boundsArray[cuboidNum][2][2] = zp;
  //
  //   // glPushMatrix();
  //   //   glLoadIdentity();
  //   //   glTranslatef(xp,yp,zp);
  //   //   cuboid3(0.02,0.02,0.02);
  //   // glPopMatrix();
  //
  //   // BOTTOM RIGHT
  //   xp = mv[0] * -0.5*x + mv[4] * -0.5*y + mv[8] * -0.5*z;
  //   yp = mv[1] * -0.5*x + mv[5] * -0.5*y + mv[9] * -0.5*z;
  //   zp = mv[2] * -0.5*x + mv[6] * -0.5*y + mv[10] * -0.5*z;
  //   wp = mv[3] * -0.5*x + mv[7] * -0.5*y + mv[11] * -0.5*z;
  //
  //   // glPushMatrix();
  //   //   glLoadIdentity();
  //   //   glTranslatef(xp,yp,zp);
  //   //   cuboid3(0.02,0.02,0.02);
  //   // glPopMatrix();
  //
  // glPopMatrix();

  // glLoadIdentity();

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

  glPushMatrix();
    glTranslatef(x, 0.0, y);
    glRotatef(rotate, 0.0, 1.0, 0.0);
    cuboid(length+0.05,0.5,0.1);
    glPushMatrix();
      glTranslatef(-0.45f, 0.0f, -1.0f);
      cuboid(length+0.95,0.5,0.1);
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
			  viewX, viewY, viewZ, // reference point
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

  // glPushMatrix();
  //   glTranslatef(-0.0f, 0.0f, -1.0f);
  //   // glRotatef(90.0, 0.0, 1.0, 0.0);
  //   cuboid(1.0,0.5,0.1);
  // glPopMatrix();

  // rightTurn(2.0,-2.0,1.0,-00.0);

  tJunction(-0.525,0,3,0);

  // Route one
  rightTurn(-1.05,-1.075,0.2,90);
  rightTurn(-0.075,-1.25,2,0);
  leftTurn(1,-2.225,0,-90);
  tJunction(0.725,-3.2,2,0);

  // Route 1a
  rightTurn(-1.025,-3.2,3,0);
  rightTurn(-3.575,-3.1,0.2,180);
  rightTurn(-3.75,-4.075,2,90);
  rightTurn(-2.775,-5.15,2,0);

  // Route 1b
  leftTurn(2.55,-4.625,0.9,-90);
  leftTurn(0.425,-5.15,4.3,0);

  // Route two
  rightTurn(2.05,0.475,1,-90);
  glPushMatrix();
    glTranslatef(2.55, 0.0f, 1.05f);
    cuboid(1.1,0.5,0.1);
  glPopMatrix();

  // Finish
  glPushMatrix();
    glTranslatef(0, 0, -5.5);
    cuboid(0.1,0.1,0.1);
  glPopMatrix();

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

  // if(detectCollision()) {
    // eyeAcross  = eyeAcross - vectorX * speed;
  	// g_eye_z  = g_eye_z - vectorZ * speed;
  	// viewX = viewX - vectorX * speed;
  	// viewZ = viewZ - vectorZ * speed;
  // }

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

void idle() {
  usleep(50000);
  if(helpMode) {
    if(eyeHeight < 14) eyeHeight+=0.3;
    if(eyeHeight > 14) eyeHeight-=0.3;
    if(eyeAcross < -1) eyeAcross += 0.1;
    if(eyeAcross > -1) eyeAcross -= 0.1;
    if(viewX < -1) viewX += 0.1;
    if(viewX > -1) viewX -= 0.1;
    if(g_eye_z < -1.5) g_eye_z += 0.1;
    if(g_eye_z > -1.5) g_eye_z -= 0.1;
    if(viewZ < -2.5) viewZ += 0.1;
    if(viewZ > -2.5) viewZ -= 0.1;
    std::cerr << "\t Move up" << std::endl;
  } else {
    if(eyeHeight < 0) eyeHeight+=0.3;
    if(eyeHeight > 0) eyeHeight-=0.3;
    if(eyeAcross < savedX) eyeAcross += 0.1;
    if(eyeAcross > savedX) eyeAcross -= 0.1;
    if(viewX < savedViewX) viewX += 0.1;
    if(viewX > savedViewX) viewX -= 0.1;
    if(g_eye_z < savedZ) g_eye_z += 0.1;
    if(g_eye_z > savedZ) g_eye_z -= 0.1;
    if(viewZ < savedViewZ) viewZ += 0.1;
    if(viewZ > savedViewZ) viewZ -= 0.1;
    std::cerr << "\t Move down" << std::endl;
  }

	glutPostRedisplay();
}

void keyboard(unsigned char key, int, int)
{
	switch (key)
	{
		case 'q': exit(1); break;
		case 'h':
      if(helpMode) {
        eyeAcross = savedX;
        g_eye_z = savedZ;
        viewX = savedViewX;
        viewZ = savedViewZ;
        eyeHeight = 0;
        helpMode = false;
        glutIdleFunc(NULL);
      } else {
        savedX = eyeAcross;
        savedZ = g_eye_z;
        savedViewX = viewX;
        savedViewX = viewZ;
        helpMode = true;
        glutIdleFunc(idle);

      }
      break;
    case 'a': eyeHeight += 0.25; break;
    case 'z': eyeHeight -= 0.25; break;
	}
  cuboidNum = 0;
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

void init()
{
	g_the_tex = load_and_bind_texture("wood-grain.png");
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

  init();

	glutMainLoop();

	return 0;
}
