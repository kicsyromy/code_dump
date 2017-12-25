

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

// states that the spider (tracker) can be in

#define TRACKER_ATTACK  0   // spider is attacking fly
#define TRACKER_EVADE   1   // spider is evading fly

// S T R U C T U R E S ///////////////////////////////////////////////////////


// G L O B A L S  ////////////////////////////////////////////////////////////

pcx_picture imagery_pcx,      // the game imagery
            background_pcx;   // the backdrop


// the sprites used in the game

sprite player,                // the player
       tracker;               // the spider

// F U N C T I O N S //////////////////////////////////////////////////////////

void main(void)
{
// this is the main function

int done=0;       // exit flag for whole system

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

PCX_Load("trackbak.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

Blit_String_DB(0,0,10,"Press Q to exit, T to toggle mode.",1);

// SECTION 2 /////////////////////////////////////////////////////////////////

// load in imagery for player

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("trackimg.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize player and extract bitmaps

sprite_width  = 24;
sprite_height = 24;

Sprite_Init((sprite_ptr)&player,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,0,0,0);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,1,1,0);

player.x          = 160;
player.y          = 180;
player.curr_frame = 0;
player.state      = 1;

// initialize the tracker and extract bitmaps

Sprite_Init((sprite_ptr)&tracker,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&tracker,0,0,1);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&tracker,1,1,1);

tracker.x          = 20;
tracker.y          = 20;
tracker.curr_frame = TRACKER_ATTACK;
tracker.state      = TRACKER_ATTACK;

// SECTION 3 /////////////////////////////////////////////////////////////////

// scan behind all objects before entering event loop

Behind_Sprite_DB((sprite_ptr)&player);
Behind_Sprite_DB((sprite_ptr)&tracker);

// main event loop

while(!done)
     {

     // erase all objects

     Erase_Sprite_DB((sprite_ptr)&player);
     Erase_Sprite_DB((sprite_ptr)&tracker);

     // do movement of spider based on mode

// SECTION 4 /////////////////////////////////////////////////////////////////

     if (tracker.state==TRACKER_ATTACK)
        {

        // move spider toward player

        // first take care of X components

        if (player.x > tracker.x)
            tracker.x+=2;
        else
        if (player.x < tracker.x)
            tracker.x-=2;

        // now Y components

        if (player.y > tracker.y)
            tracker.y+=2;
        else
        if (player.y < tracker.y)
            tracker.y-=2;

        } // end if spider is attacking

// SECTION 5 /////////////////////////////////////////////////////////////////

     else
        {

        // move spider away from player

        // first take care of X components

        if (player.x > tracker.x)
            tracker.x-=2;
        else
        if (player.x < tracker.x)
            tracker.x+=2;

        // now Y components

        if (player.y > tracker.y)
            tracker.y-=2;
        else
        if (player.y < tracker.y)
            tracker.y+=2;

        } // end else spider evading

// SECTION 6 /////////////////////////////////////////////////////////////////

     // do boundary collision for spider

     if (tracker.x<0)
         tracker.x = 0;
     else
     if (tracker.x>194)
         tracker.x = 194;

     if (tracker.y<0)
         tracker.y = 0;
     else
     if (tracker.y>174)
         tracker.y = 174;

// SECTION 7 /////////////////////////////////////////////////////////////////

     // see if player is trying to move

     if (kbhit())
        {

        // which key ?

        switch(getch())
              {

              // use numeric keypad for movement
              // (note NUMLOCK must be activated)

              case '1':     // move down and left
                 {
                 player.x-=4;
                 player.y+=4;

                 } break;

              case '2':    // move down
                 {
                 player.y+=4;

                 } break;

              case '3':     // move right and down
                 {
                 player.x+=4;
                 player.y+=4;


                 } break;

              case '4':     // move left
                 {
                 player.x-=4;

                 } break;

              case '6':    // move right
                 {
                 player.x+=4;

                 } break;

              case '7':     // move up and left
                 {

                 player.x-=4;
                 player.y-=4;

                 } break;

              case '8':     // move up
                 {

                 player.y-=4;

                 } break;

              case '9':    // move up and right
                 {

                 player.x+=4;
                 player.y-=4;

                 } break;

              case 't': // toggle attack mode
                 {

                 if (tracker.state==TRACKER_ATTACK)
                    tracker.state=tracker.curr_frame = TRACKER_EVADE;
                 else
                    tracker.state=tracker.curr_frame = TRACKER_ATTACK;
                 } break;

              case 'q': // exit the demo
                 {
                 done = 1;
                 } break;


              default:break;

              } // end switch

// SECTION 8 /////////////////////////////////////////////////////////////////

        // do boundary collision for player

        if (player.x<0)
            player.x = 304;
        else
        if (player.x>304)
            player.x = 0;

        if (player.y<0)
            player.y = 184;
        else
        if (player.y>184)
            player.y = 0;

        } // end if kbhit


// SECTION 9 /////////////////////////////////////////////////////////////////

     // do animation

     if (++player.curr_frame==2)
         player.curr_frame = 0;

     // scan background under objects

     Behind_Sprite_DB((sprite_ptr)&player);
     Behind_Sprite_DB((sprite_ptr)&tracker);


     // draw all the imagery

     Draw_Sprite_DB((sprite_ptr)&player);
     Draw_Sprite_DB((sprite_ptr)&tracker);


     // copy the double buffer to the screen

     Show_Double_Buffer(double_buffer);

     // draw the state of spider on top of video buffer

     if (tracker.state == TRACKER_ATTACK)
         Blit_String(8,180,12,"Mode=Attack",1);
     else
         Blit_String(8,180,9,"Mode=Evade",1);

     // wait a sec

     Delay(1);

     } // end while

// SECTION 10 ////////////////////////////////////////////////////////////////

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

} // end main


