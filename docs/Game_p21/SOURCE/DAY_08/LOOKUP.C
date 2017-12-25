
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>
#include <memory.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#include "graph3.h"  // include our graphics stuff
#include "graph4.h"

// D E F I N E S /////////////////////////////////////////////////////////////

#define NUM_CIRCLES 1000
#define FULL_CIRCLE 360

// G L O B A L S  ////////////////////////////////////////////////////////////

float sin_table[360], cos_table[360];

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

int index,x,y,xo,yo,radius,ang;

// create look up tables

for (index=0; index<FULL_CIRCLE; index++)
     {

     sin_table[index]= sin(index*3.14159/180);
     cos_table[index] = cos(index*3.14159/180);

     } // end for index

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

printf("\nHit any key to draw circles with internal sin and cosine.");
getch();

// draw circles using built in sin and cos

for (index=0; index<NUM_CIRCLES; index++)
     {

     // get a random circle

     radius = rand()%50;
     xo     = rand()%320;
     yo     = rand()%200;

     for (ang=0; ang<360; ang++)
          {
          x = xo + cos(ang*3.14159/180)*radius;
          y = yo + sin(ang*3.14159/180)*radius;

          // plot the point of the circle with a little image space clipping

          if (x>=0 && x<320 && y>=0 && y<200)
              Plot_Pixel_Fast(x,y,9);

          } // end for ang
     } // end for index

// done, halt the system and wait for user to hit a key

printf("\nHit any key to see circles drawn with look up tables.");
getch();

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// draw circles using look up tables

for (index=0; index<NUM_CIRCLES; index++)
     {

     // get a random circle

     radius = rand()%50;
     xo     = rand()%320;
     yo     = rand()%200;

     for (ang=0; ang<FULL_CIRCLE; ang++)
          {

          x = xo + cos_table[ang]*radius;
          y = yo + sin_table[ang]*radius;

          // plot the point of the circle with a little image space clipping

          if (x>=0 && x<320 && y>=0 && y<200)
              Plot_Pixel_Fast(x,y,12);

          } // end for ang
     } // end for index

// let user hit a key to exit

printf("\nWow! Hit any key to exit.");
getch();

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

} // end main

