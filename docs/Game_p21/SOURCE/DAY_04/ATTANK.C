
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

#include <graph.h>   // microsoft's stuff if we need it

#include "graph3.h"  // the module from day 3
#include "graph4.h"  // the module from day 4

// D E F I N E S ////////////////////////////////////////////////////////////

#define TANK_SPEED  4
#define PI          (float)3.14159

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{


long index;          // used as a loop index

sprite tank1,  // the player sprite
       tank2;  // the enemy sprite

pcx_picture background_pcx,  // this pcx structure holds background imagery
            objects_pcx;     // this pcx structure holds forground imagery


int tank1_direction=0,  // the tanks current direction, also the current frame
    tank2_direction=0,  // 0 - is straight up North
    done=0;             // system exit flag


float dx,              // motion variables
      dy,
      angle;

// S E C T I O N  1 //////////////////////////////////////////////////////

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// load in background

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load("outpost.pcx", (pcx_picture_ptr)&background_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&background_pcx);

// put up title

Blit_String(90,2,7,"A T T A N K ! ! !",1);

PCX_Delete((pcx_picture_ptr)&background_pcx);

// load the .PCX file with the tank cells

// load in the players imagery

PCX_Init((pcx_picture_ptr)&objects_pcx);

PCX_Load("tanks.pcx", (pcx_picture_ptr)&objects_pcx,0);

// S E C T I O N  2 //////////////////////////////////////////////////////

// initialize sprite size and data structure

sprite_width  = 16;
sprite_height = 16;

// place tank1 (player) in bottom of screen

Sprite_Init((sprite_ptr)&tank1,160,150,0,0,0,0);

// grab all 16 images from the tanks pcx picture

for (index=0; index<16; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&objects_pcx,(sprite_ptr)&tank1,index,index,0);

    } // end for index

// place tank2 (enemy) in top of screen

Sprite_Init((sprite_ptr)&tank2,160,50,0,0,0,0);

// grab all 16 images from the tanks pcx picture

for (index=0; index<16; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&objects_pcx,(sprite_ptr)&tank2,index,index,1);

    } // end for index

// kill the pcx memory and buffers now that were done

PCX_Delete((pcx_picture_ptr)&objects_pcx);

// S E C T I O N  3 //////////////////////////////////////////////////

// point the tanks straight up

tank1.curr_frame = tank1_direction;
tank2.curr_frame = tank2_direction;

// scan the background under tanks on first iteration

Behind_Sprite((sprite_ptr)&tank1); // player
Behind_Sprite((sprite_ptr)&tank2); // enemy

// wait for exit, this is the main event loop

while(!done)
     {

// S E C T I O N  4 //////////////////////////////////////////////////

     // erase the players tank

     Erase_Sprite((sprite_ptr)&tank1);

     // erase the enemy tank

     Erase_Sprite((sprite_ptr)&tank2);

// S E C T I O N  5 //////////////////////////////////////////////////

     // test if user wants to translate or rotate tank

     if (kbhit())
        {

        // reset translation factors

        dx=dy=0;

        // test what key was pressed

        switch(getch())
              {

              case '6': // rotate right
                   {
                   // change direction of tank, make sure to wrap around

                   if (++tank1_direction > 15)
                      tank1_direction=0;

                   } break;

              case '4': // rotate left
                   {
                   // change direction of tank, make sure to wrap around

                   if (--tank1_direction < 0)
                      tank1_direction=15;

                   } break;

              case '8': // move foward
                   {
                   // based on direction variable compute translation factors

                   // compute angle in radians

                   angle =  (90+360-22.5*(float)tank1_direction);

                   // compute factors based on angle and speed

                   dx = TANK_SPEED * cos(PI*angle/180);
                   dy = TANK_SPEED * sin(PI*angle/180);

                   } break;

              case '2': // move backward
                   {
                   // based on direction variable compute translation factors

                   // compute angle in radians

                   angle =  (90+360-22.5*(float)tank1_direction);

                   // compute factors based on angle and speed

                   dx = TANK_SPEED * cos(PI*angle/180);
                   dy = TANK_SPEED * sin(PI*angle/180);

                   } break;

              case 'q': // quit
                   {
                   // set exit flag true

                   done=1;

                   } break;

              default:break;

              } // end switch

// S E C T I O N  6 //////////////////////////////////////////////////

        // do the translation

        tank1.x+=(int)(dx+.5);
        tank1.y-=(int)(dy+.5);

        // test if player bumped into edge, if so push him back

        // set the frame based on new direction

        tank1.curr_frame = tank1_direction;

        } // end if kbhit

// S E C T I O N  7 //////////////////////////////////////////////////

     // now move the enemy tank

     // test if it's time to turn

     if (rand()%10==1)
        {

        // select direction to turn

        switch(rand()%2)
              {

              case 0: // turn right
                   {

                   if (++tank2_direction > 15)
                      tank2_direction=0;

                   } break;

              case 1: // turn left
                   {

                   if (--tank2_direction < 0)
                      tank2_direction=15;

                   } break;

              default:break;

              } // end switch

        // set the frame based on new direction

        tank2.curr_frame = tank2_direction;

        } // end if

// S E C T I O N  8 //////////////////////////////////////////////////

     // compute angle in radians

     angle =  (90+360-22.5*(float)tank2_direction);

     // compute factors based on angle and speed

     dx = (TANK_SPEED+rand()%2) * cos(PI*angle/180);
     dy = (TANK_SPEED+rand()%2) * sin(PI*angle/180);

     // do the translation

     tank2.x+=(int)(dx+.5);
     tank2.y-=(int)(dy+.5);

// S E C T I O N  9 //////////////////////////////////////////////////

     // test if enemy has hit an edge, if so warp to other side

     if (tank2.x > (320-(int)sprite_width) )
        tank2.x = 0;

     else
     if (tank2.x < 0 )
        tank2.x = 319-(int)sprite_width;

     if (tank2.y > (200-(int)sprite_height) )
        tank2.y = 0;

     else
     if (tank2.y < 0 )
        tank2.y = 199-(int)sprite_height;

// S E C T I O N  10 //////////////////////////////////////////////////

     // scan background under players tank

     Behind_Sprite((sprite_ptr)&tank1);

     // scan background under emeny tank

     Behind_Sprite((sprite_ptr)&tank2);

     // draw players tank

     Draw_Sprite((sprite_ptr)&tank1);

     // draw enemy tank

     Draw_Sprite((sprite_ptr)&tank2);

     // test for collision

     if (Sprite_Collide((sprite_ptr)&tank1,(sprite_ptr)&tank2))
        {
        // do something spectacular


        } // end if collision

     // delay main loop for a sec so that user can see a solid image

     Delay(2); // wait 55ms approx. or 1/18.2 sec

     } // end while

// S E C T I O N  11 //////////////////////////////////////////////////

// disolve the screen...in one line I might add!

for (index=0; index<=300000; index++,Plot_Pixel_Fast(rand()%320, rand()%200, 0));

// go back to text mode

Set_Video_Mode(TEXT_MODE);

} // end main
