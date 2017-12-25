
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

// G L O B A L S  ////////////////////////////////////////////////////////////

pcx_picture imagery_pcx,      // the game imagery
            background_pcx;   // the backdrop

// the sprites used in the game

sprite thefly;                // the fly

int fly_xv=5,                 // the velocity of the fly
    fly_yv=5,
    fly_clock=10;             // how many cycles of frames fly will
                              // move in current direction

// F U N C T I O N S //////////////////////////////////////////////////////////

void main(void)
{
// this is the main function

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

// load in the background image into the double buffer

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load("flybak.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

Blit_String_DB(8,8,0,"Press any key to exit.",1);

// SECTION 2 /////////////////////////////////////////////////////////////////

// load in imagery for the fly

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("flyimg.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize fly and extract bitmaps

sprite_width  = 36;
sprite_height = 36;

Sprite_Init((sprite_ptr)&thefly,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&thefly,0,0,0);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&thefly,1,1,0);

thefly.x          = 160;
thefly.y          = 100;
thefly.curr_frame = 0;
thefly.state      = 1;

// SECTION 3 /////////////////////////////////////////////////////////////////

// scan behind all objects before entering event loop

Behind_Sprite_DB((sprite_ptr)&thefly);

// main event loop

while(!kbhit())
     {

     // erase all objects

     Erase_Sprite_DB((sprite_ptr)&thefly);

     // move the fly

     thefly.x+=fly_xv;
     thefly.y+=fly_yv;

// SECTION 4 /////////////////////////////////////////////////////////////////

     // check if it's time to change direction

     if (--fly_clock==0)
        {

        // select a new direction

        fly_xv = -10 + rand()%21;  // -10 to +10
        fly_yv = -10 + rand()%21;  // -10 to +10

        // select an amount of time to do it

        fly_clock = 5 + rand()%50;

        } // end if time to change direction

// SECTION 5 /////////////////////////////////////////////////////////////////

     // do boundary collision for thefly, if it hits an edge bounce it back

     if (thefly.x<0 || thefly.x>294)
        {
        // bounce fly back

        fly_xv=-fly_xv;
        thefly.x=thefly.x+2*fly_xv;

        } // end if over x boundary

     if (thefly.y<0 || thefly.y>164)
        {
        // bounce fly back

        fly_yv=-fly_yv;
        thefly.y=thefly.y+2*fly_yv;

        } // end if over y boundary

     // do animation of the fly

     if (++thefly.curr_frame==2)
         thefly.curr_frame = 0;

// SECTION 6 /////////////////////////////////////////////////////////////////

     // scan background under objects

     Behind_Sprite_DB((sprite_ptr)&thefly);

     // draw all the imagery

     Draw_Sprite_DB((sprite_ptr)&thefly);

     // copy the double buffer to the screen

     Show_Double_Buffer(double_buffer);

     // wait a sec

     Delay(1);

     } // end while

// SECTION 7 /////////////////////////////////////////////////////////////////

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

} // end main


