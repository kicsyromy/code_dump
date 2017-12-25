
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

#include <graph.h>

#include "graph3.h"
#include "graph4.h"

#define SCROLL_WIDTH  (unsigned int)640
#define SCROLL_HEIGHT (unsigned int)100

// G L O B A L S  ////////////////////////////////////////////////////////////

unsigned char far *scroll_buffer = NULL;  // pointer to the scrolling buffer
                                          // which is 640x100 pixels

// F U N C T I O N S /////////////////////////////////////////////////////////

void Show_View_Port(char far *buffer,int pos)
{
// copy a portion of the scrolling buffer to the video buffer

unsigned int y,scroll_off, screen_off;

// there are 100 rows that need to be moved, move the data row by row

for (y=0; y<100; y++)
    {

    // compute starting offset into scroll buffer
    // y * 640 + pos

    scroll_off = ((y<<9) + (y<<7) + pos );

    // compute starting offset in video ram
    // y * 320 + 80

    screen_off = (((y+50)<<8) + ((y+50)<<6) + 80 );

    // move the data

    _fmemmove((char far *)&video_buffer[screen_off],
              (char far *)&buffer[scroll_off],160);

    } // end for y

} // end Show_View_Port



//////////////////////////////////////////////////////////////////////////////

void Plot_Pixel_Scroll(int x,int y,unsigned char color)
{
// plots pixels into the scroll buffer with our new virtual screen size
// of 640x100

// use the fact that 640*y = 512*y + 128*y = y<<9 + y<<7

scroll_buffer[((y<<9) + (y<<7)) + x] = color;

} // end Plot_Pixel_Scroll

/////////////////////////////////////////////////////////////////////////////

void Draw_Terrain(void)
{

// this function draws the terrain into the scroll buffer
// which in this case is 640x100 pixels

int x,y=70,y1,index;

// clear out memory first

_fmemset(scroll_buffer,0,SCROLL_WIDTH*SCROLL_HEIGHT);

// draw a few stars

for (index=0; index<200; index++)
    {
    Plot_Pixel_Scroll(rand()%640,rand()%70,15);
    } // end for index

// draw some moutains

for (x=0; x<640; x++)
    {

    // compute offset

    y+=-1 + rand()%3;

    // make sure terrain stays within resonable boundary

    if (y>90) y=90;
    else
    if (y<40) y=40;

    // plot the dot in the double buffer

    Plot_Pixel_Scroll(x,y,2);

    for (y1=y+1; y1<100; y1++)
         Plot_Pixel_Scroll(x,y1,10);

    } // end for x

} // end Draw_Terrain

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

int done=0, // exit flag
    sx=0;   // scrolling viewport position

// set the videomode to 320x256x256

_setvideomode(_MRES256COLOR);

// put up some information

_settextposition(0,0);

printf("Use < > to move. Press Q to quit.");

// draw a little window

_setcolor(1);

_rectangle(_GBORDER, 80-1,50-1,240+1,150+1);

// allocate memory for scrolling buffer

scroll_buffer = (char far *)_fmalloc(SCROLL_WIDTH * SCROLL_HEIGHT);

// draw the mountains

Draw_Terrain();

// show initial view

Show_View_Port(scroll_buffer,sx);

// main loop

while(!done)
     {

     // has user hit a key

     if (kbhit())
        {

        switch(getch())
              {
              case ',': // move window to left, but don't go too far
                      {
                      sx-=2;

                      if (sx<0)
                          sx=0;

                      } break;

              case '.': // move window to right, but dont go too far
                      {
                      sx+=2;

                      if (sx > 640-160)
                         sx=640-160;

                      } break;

              case 'q': // user trying to bail ?
                      {
                      done=1;

                      } break;

              } // end switch

        // copy view port to screen

        Show_View_Port(scroll_buffer,sx);

        _settextposition(24,0);

        printf("Viewport position = %d  ",sx);

        } // end if

     } // end while

// restore text video mode

_setvideomode(_DEFAULTMODE);

} // end main

