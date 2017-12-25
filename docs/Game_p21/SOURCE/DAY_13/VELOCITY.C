

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

#define VELOCITY 4   // constant speed of ship

// S T R U C T U R E S ///////////////////////////////////////////////////////


// G L O B A L S  ////////////////////////////////////////////////////////////

pcx_picture imagery_pcx,      // the game imagery
            background_pcx;   // the backdrop


// the sprites used in the game

sprite object;                // the object

// F U N C T I O N S //////////////////////////////////////////////////////////

void main(void)
{
// this is the main function

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

PCX_Load("velback.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

// load in imagery for object

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("viper.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// SECTION 3 //////////////////////////////////////////////////////////////////

// initialize player and extract bitmaps

sprite_width  = 50;
sprite_height = 24;

Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&object,0,0,0);

object.x          = 0;
object.y          = 100;
object.curr_frame = 0;
object.state      = 1;

// scan behind all objects before entering event loop

Behind_Sprite_DB((sprite_ptr)&object);

// SECTION 4 //////////////////////////////////////////////////////////////////

// main event loop

while(!kbhit())
     {

     // erase all objects

     Erase_Sprite_DB((sprite_ptr)&object);

// SECTION 5 //////////////////////////////////////////////////////////////////

     // move object with costant velocity

     object.x+=VELOCITY;

     // test if object is beyond edge of screen, if so send back to
     // other edge

     if (object.x>319-50)
         object.x = 0;

// SECTION 6 //////////////////////////////////////////////////////////////////

     // scan background under objects

     Behind_Sprite_DB((sprite_ptr)&object);

     // draw all the imagery

     Draw_Sprite_DB((sprite_ptr)&object);

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