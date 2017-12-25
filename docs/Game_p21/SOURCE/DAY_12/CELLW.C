
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
#include "graph6.h"

// D E F I N E S /////////////////////////////////////////////////////////////

#define CELL_WIDTH       16   // size of bitmaps in world
#define CELL_HEIGHT      16

#define NUM_ROWS         12   // number of rows and columns in terrain
#define NUM_COLUMNS      20

// G L O B A L S  ////////////////////////////////////////////////////////////

pcx_picture imagery_pcx;      // the background bitmaps

// the sprites used in the demo

sprite objects;

int terrain[NUM_ROWS][NUM_COLUMNS] = {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,3,3,3,3,0,
                                      0,0,0,0,2,0,0,0,0,3,0,0,0,0,0,3,0,0,3,0,
                                      0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,3,0,0,3,0,
                                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,3,0,
                                      0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,3,3,3,3,0,
                                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,
                                      0,0,0,2,2,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,
                                      0,0,2,2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,3,
                                      0,0,2,2,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


// M A I N ////////////////////////////////////////////////////////////////////

void main(void)
{
// this is the main function

int x,y;


// SECTION 1 /////////////////////////////////////////////////////////////////

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// create a double buffer

if (!Create_Double_Buffer(SCREEN_HEIGHT))
   {
   printf("\nNot enough memory to create double buffer.");

   } // end if

// clear the double buffer

Fill_Double_Buffer(0);

// SECTION 2 /////////////////////////////////////////////////////////////////

// load in imagery for the background objects

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("terrain.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize player and extract bitmaps

sprite_width  = 16;
sprite_height = 16;

// initialize the object sprite

Sprite_Init((sprite_ptr)&objects,0,0,0,0,0,0);

// load the bitmaps

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                (sprite_ptr)&objects,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                (sprite_ptr)&objects,1,1,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                (sprite_ptr)&objects,2,2,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                (sprite_ptr)&objects,3,3,0);


objects.curr_frame = 0;
objects.state      = 1;

// SECTION 3 /////////////////////////////////////////////////////////////////

// based on terrain array draw world

// loop through data array

for (y=0; y<NUM_ROWS; y++)
    {

    for (x=0; x<NUM_COLUMNS; x++)
        {

        // draw the proper bitmap at the proper position

        objects.x = x*CELL_WIDTH;
        objects.y = y*CELL_HEIGHT;

        // extract the cell element

        objects.curr_frame  = terrain[y][x];

        Draw_Sprite_DB((sprite_ptr)&objects);

        } // end for x

    } // end for y


// SECTION 4 /////////////////////////////////////////////////////////////////

Blit_String_DB(2,2,10,"Press any key to exit.",1);

// let's see what we drew

Show_Double_Buffer(double_buffer);

// main event loop

while(!kbhit())
     {

     } // end while

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

} // end main


