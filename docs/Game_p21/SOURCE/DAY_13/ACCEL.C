
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

// P R O T O T Y P E S //////////////////////////////////////////////////////

// D E F I N E S /////////////////////////////////////////////////////////////


// S T R U C T U R E S ///////////////////////////////////////////////////////


// G L O B A L S  ////////////////////////////////////////////////////////////

pcx_picture imagery_pcx,      // the game imagery
            background_pcx;   // the backdrop


// the sprites used in the game

sprite object;                // the object

// F U N C T I O N S //////////////////////////////////////////////////////////

void main(void)
{
int accel=0,            // acceleration input by user
    velocity=0,         // current velocity of dragster
    peddle_to_metal=0,  // has the user punched it?
    engine=0,           // state of 500CI engine
    done=0;             // system exit variable

char buffer[80];        // used for printing

// this is the main function

// SECTION 1 //////////////////////////////////////////////////////////////////

printf("\nEnter the acceleration of the dragster (1-10) ?");
scanf("%d",&accel);

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

PCX_Load("road.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

Blit_String_DB(8,8,10,"'Q' to quit, 'G' to start.",1);

// load in imagery for object

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("drag.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// SECTION 3 //////////////////////////////////////////////////////////////////

// initialize player and extract bitmaps

sprite_width  = 36;
sprite_height = 8;

Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&object,0,0,0);

object.x          = 0;
object.y          = 170;
object.curr_frame = 0;
object.state      = 1;

// scan behind all objects before entering event loop

Behind_Sprite_DB((sprite_ptr)&object);

// SECTION 4 //////////////////////////////////////////////////////////////////

// main event loop

while(!done)
     {

     // erase all objects

     Erase_Sprite_DB((sprite_ptr)&object);

     // test if user is tryin to go

     if (kbhit())
        {
        // what key?

        switch(getch())
              {
              case 'g': // g for gas, gone, go!
                   {
                   // we can only start dragster if it hasn't already started

                   if (!peddle_to_metal)
                      {
                      peddle_to_metal=engine=1;

                      } // end if

                   } break;

              case 'q': // to quit
                   {

                   done=1;

                   } break;

              default:break;

              } // end switch

        } // end if

// SECTION 5 //////////////////////////////////////////////////////////////////

     // test if it's time to move the dragster

     if (peddle_to_metal && engine)
        {

        // move object with velocity

        object.x+=velocity;

        // apply the horsepower baby (acceleration)

        velocity+=accel;

        // test if dragster has hit end of stip, if so stop it rather abrubtly

        if (object.x > 319-36)
           {
           engine=0; // turn off engine and apply infinite braking force!

           // push dragster back a bit

           object.x = 319-36;

           } // end if

        } // end if

// SECTION 6 //////////////////////////////////////////////////////////////////

     // scan background under objects

     Behind_Sprite_DB((sprite_ptr)&object);

     // draw all the imagery

     Draw_Sprite_DB((sprite_ptr)&object);

     // show some info

     sprintf(buffer,"Speed = %d  ",velocity);
     Blit_String_DB(110,100,10,buffer,0);

     sprintf(buffer,"Acceration = %d  ",accel);
     Blit_String_DB(110,110,10,buffer,0);

     // copy the double buffer to the screen

     Show_Double_Buffer(double_buffer);

     // wait a sec

     Delay(1);

     } // end while

// SECTION 7 //////////////////////////////////////////////////////////////////

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

} // end main


