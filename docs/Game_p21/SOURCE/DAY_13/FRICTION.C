
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

#define FRICTION 1   // virtual frictional coefficeint of water

// G L O B A L S  ////////////////////////////////////////////////////////////

pcx_picture imagery_pcx,      // the game imagery
            background_pcx;   // the backdrop


// the sprites used in the game

sprite object;                // the object

// F U N C T I O N S //////////////////////////////////////////////////////////

void main(void)
{

int torpedo_xv=0,  // initial velocities
    torpedo_yv=0,
    fired=0,       // state of torpedo
    done=0;        // exit variable


// SECTION 1 //////////////////////////////////////////////////////////////////

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// create a double buffer

if (!Create_Double_Buffer(SCREEN_HEIGHT))
   {
   printf("\nNot enough memory to create double buffer.");

   } // end if

// clear the double buffer

Fill_Double_Buffer(0);

// SECTION 2 //////////////////////////////////////////////////////////////////

// load in the background image into the double buffer

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load("ship.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

// load in imagery for object

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("torpedo.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// SECTION 3 //////////////////////////////////////////////////////////////////

// initialize player and extract bitmaps

sprite_width  = 8;
sprite_height = 8;

Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&object,0,0,0);

object.x          = 154;
object.y          = 178;
object.curr_frame = 0;
object.state      = 1;

// draw instructions

Blit_String_DB(8,8,15,"Press 'Q' to quit, 'F' to fire.",1);

// scan behind all objects before entering event loop

Behind_Sprite_DB((sprite_ptr)&object);

// SECTION 4 //////////////////////////////////////////////////////////////////

// main event loop

while(!done)
     {

     // erase all objects

     Erase_Sprite_DB((sprite_ptr)&object);

// SECTION 5 //////////////////////////////////////////////////////////////////

     // test if user is trying to fire torpedo

     if (kbhit())
        {

        switch(getch())
              {

              case 'q': // just exit?
                   {
                   done=1;

                   } break;

              case 'f':
              case 'F': // fire the torpedo
                      {
                      // make sure torpedo hasn't been fired

                      if (!fired)
                      {

                      // set flag that torpedo has been fired

                      fired=1;

                      // set initial velocity or torpedo

                      torpedo_xv = 0;   // no movement X direction

                      torpedo_yv = -16; // initial velocity, note it is
                                        // negative since torped is moving
                                        // upward

                      } // end if

                      } break;


              } // end switch

        } // end if kbhit


// SECTION 6 //////////////////////////////////////////////////////////////////

     // test if torpedo has been fired

     if (fired)
        {

        // do translation

        object.x+=torpedo_xv;
        object.y+=torpedo_yv;

        // apply water friction only to vertical component of velocity

        torpedo_yv+=FRICTION;

        // test if velocity has become postive or zero

        if (torpedo_yv>0)
            torpedo_yv = 0;

        } // end if fired

// SECTION 7 //////////////////////////////////////////////////////////////////

     // scan background under objects

     Behind_Sprite_DB((sprite_ptr)&object);

     // draw all the imagery

     Draw_Sprite_DB((sprite_ptr)&object);

     // copy the double buffer to the screen

     Show_Double_Buffer(double_buffer);

     // wait a sec

     Delay(1);

     } // end while

// SECTION 8 //////////////////////////////////////////////////////////////////

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

} // end main


