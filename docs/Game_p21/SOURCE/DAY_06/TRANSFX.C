
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

#include "graph3.h"
#include "graph4.h"

// D E F I N E S /////////////////////////////////////////////////////////////

#define NUM_WORMS 320

// S T R U C T U R E S //////////////////////////////////////////////////////

typedef struct worm_typ
        {
        int y;       // current y position of worm
        int color;   // color of worm
        int speed;   // speed of worm
        int counter; // counter

        } worm, *worm_ptr;

// G L O B A L S /////////////////////////////////////////////////////////////

pcx_picture screen_fx; // our test screen

//////////////////////////////////////////////////////////////////////////////

void Fade_Lights(void)
{
// this functions fades the lights by slowly decreasing the color values
// in all color registers

int pal_reg,index;
RGB_color color;

for (index=0; index<30; index++)
    {

    for (pal_reg=1; pal_reg<255; pal_reg++)
        {
        // get the color to fade

        Get_Palette_Register(pal_reg,(RGB_color_ptr)&color);

        if (color.red   > 5) color.red-=3;
        else
           color.red = 0;

        if (color.green > 5) color.green-=3;
        else
           color.green = 0;
        if (color.blue  > 5) color.blue-=3;
        else
           color.blue = 0;

        // set the color to a diminished intensity

        Set_Palette_Register(pal_reg,(RGB_color_ptr)&color);

        } // end for pal_reg

    // wait a bit

    Delay(2);

    } // end fade for

} // end Fade_Lights

//////////////////////////////////////////////////////////////////////////////

void Disolve(void)
{
// disolve screen by ploting zillions of black pixels

unsigned long index;

for (index=0; index<=300000; index++,Plot_Pixel_Fast(rand()%320, rand()%200, 0));

} // end Disolve

//////////////////////////////////////////////////////////////////////////////

void Melt(void)
{

// this function "melts" the screen by moving little worms at different speeds
// down the screen.  These worms change to the color thy are eating

int index,ticks=0;

worm worms[NUM_WORMS]; // the array of worms used to make the screen melt

// initialize the worms

for (index=0; index<160; index++)
    {

    worms[index].color   = Get_Pixel(index,0);
    worms[index].speed   = 3 + rand()%9;
    worms[index].y       = 0;
    worms[index].counter = 0;

    // draw the worm

    Plot_Pixel_Fast((index<<1),0,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1),1,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1),2,(char)worms[index].color);


    Plot_Pixel_Fast((index<<1)+1,0,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1)+1,1,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1)+1,2,(char)worms[index].color);

    } // end index

// do screen melt

while(++ticks<1800)
     {

     // process each worm

     for (index=0; index<320; index++)
         {
         // is it time to move worm

         if (++worms[index].counter == worms[index].speed)
            {
            // reset counter

            worms[index].counter = 0;

            worms[index].color = Get_Pixel(index,worms[index].y+4);

            // has worm hit bottom?

            if (worms[index].y < 193)
               {

               Plot_Pixel_Fast((index<<1),worms[index].y,0);
               Plot_Pixel_Fast((index<<1),worms[index].y+1,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1),worms[index].y+2,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1),worms[index].y+3,(char)worms[index].color);

               Plot_Pixel_Fast((index<<1)+1,worms[index].y,0);
               Plot_Pixel_Fast((index<<1)+1,worms[index].y+1,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1)+1,worms[index].y+2,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1)+1,worms[index].y+3,(char)worms[index].color);

               worms[index].y++;

               } // end if worm isn't at bottom yet

            } // end if time to move worm

         } // end index

     // accelerate melt

     if (!(ticks % 500))
        {

        for (index=0; index<160; index++)
            worms[index].speed--;

        } // end if time to accelerate melt

     } // end while

} // end Melt

//////////////////////////////////////////////////////////////////////////////

void Sheer(void)
{
// this program "sheers" the screen for lack of a better word.

long index;
int x,y;

// select starting point of sheers

x=rand()%320;
y=rand()%200;

// do it a few times to make sure whole screen is destroyed

for (index=0; index<100000; index++)
    {

    // move sheers

    x+=17; // note the use of prime numbers
    y+=13;

    // test if sheers are of boundaries, if so roll them over

    if (x>319)
       x = x - 319;

    if (y>199)
       y = y - 199;

    // plot the pixel in black

    Plot_Pixel_Fast(x,y,0);

    } // end for index

} // end Sheer

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{
int done=0,                       // exit flag
    index=0;                      // current position in instruction string

static char instructions[256]=""; // holds the instructions string

char buffer[41];

// build up instruction string

strcat(instructions,"..................................................");
strcat(instructions,"Press 1 to fade the lights, ");
strcat(instructions,"Press 2 to disolve the screen, ");
strcat(instructions,"Press 3 to melt the screen, ");
strcat(instructions,"Press 4 to sheer the screen.");
strcat(instructions,"..................................................");

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// load in a background picture

PCX_Init((pcx_picture_ptr)&screen_fx);

PCX_Load("screenfx.pcx", (pcx_picture_ptr)&screen_fx,1);

PCX_Show_Buffer((pcx_picture_ptr)&screen_fx);

PCX_Delete((pcx_picture_ptr)&screen_fx);

// main event loop

while(!done)
     {

     // wait for a keyboard hit

     if (kbhit())
        {

        // which special fx did user want to see

        switch(getch())
              {
              case '1':  // dim lights
                      {

                      Fade_Lights();

                      } break;

              case '2': // disolve screen
                      {
                      Disolve();

                      } break;

              case '3': // melt screen
                      {

                      Melt();

                      } break;

              case '4': // sheer screen
                      {

                      Sheer();

                      } break;

              default:break;

              } // end switch

        // set exit flag

        done=1;

        } // end if keyboard hit */

     // extract the sub string to be displayed

     memcpy(buffer,&instructions[index],40);

     // put NULL teminator at end

     buffer[40]=0;

     Blit_String(0,23*8+6,15,buffer,0);

     // increment instruction index
     // roll over if at end of instructions

     if (++index>180)
        index=0;

     // assume that user can only read at 1,000,000 words a second

     Delay(2);

     } // end while

// go back to text mode

Set_Video_Mode(TEXT_MODE);

} // end main



