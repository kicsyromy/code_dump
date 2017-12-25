
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

// G L O B A L S  ////////////////////////////////////////////////////////////

unsigned char far *double_buffer = NULL;

unsigned int buffer_height        = SCREEN_HEIGHT;

unsigned int buffer_size = SCREEN_WIDTH*SCREEN_HEIGHT/2;

// F U N C T I O N S /////////////////////////////////////////////////////////

void Show_Double_Buffer(char far *buffer)
{
// this functions copies the doubele buffer into the video buffer

_asm
   {
   push ds               ; save DS on stack
   mov cx,buffer_size    ; this is the size of buffer in WORDS
   les di,video_buffer   ; es:di is destination of memory move
   lds si,buffer         ; ds:si is source of memory move
   cld                   ; make sure to move in the right direction
   rep movsw             ; move all the words
   pop ds                ; restore the data segment
   } // end asm

} // end Show_Double_Buffer

//////////////////////////////////////////////////////////////////////////////

int Create_Double_Buffer(int num_lines)
{

// allocate enough memory to hold the double buffer

if ((double_buffer = (unsigned char far *)_fmalloc(SCREEN_WIDTH * (num_lines + 1)))==NULL)
   return(0);

// set the height of the buffer and compute it's size

buffer_height = num_lines;

buffer_size = SCREEN_WIDTH * num_lines/2;

// fill the buffer with black

_fmemset(double_buffer, 0, SCREEN_WIDTH * num_lines);

// everything was ok

return(1);

} // end Init_Double_Buffer

///////////////////////////////////////////////////////////////////////////////

void Fill_Double_Buffer(int color)
{
// this function fills in the double buffer with the sent color

_fmemset(double_buffer, color, SCREEN_WIDTH * buffer_height);

} // end Fill_Double_Buffer

//////////////////////////////////////////////////////////////////////////////

void Delete_Double_Buffer(void)
{
// this function free's up the memory allocated by the double buffer
// make sure to use FAR version

if (double_buffer)
  _ffree(double_buffer);

} // end Delete_Double_Buffer

//////////////////////////////////////////////////////////////////////////////

void Plot_Pixel_Fast_DB(int x,int y,unsigned char color)
{

// plots the pixel in the desired color a little quicker using binary shifting
// to accomplish the multiplications

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

double_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast_DB

//////////////////////////////////////////////////////////////////////////////

void main(void)
{
// this program creates a kaleidoscope of colors

int x,y,fcolor=1,index;

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// create a double buffer

if (!Create_Double_Buffer(SCREEN_HEIGHT))
   {
   printf("\nNot enough memory to create double buffer.");

   } // end if

// main event loop

while(!kbhit())
     {

     // clear out the double buffer with black

     Fill_Double_Buffer(0);

     // next color

     if (++fcolor>15) fcolor=1;

     // draw something in it

     for (index=0; index<200; index++)
         {
         // make a kaleidoscope of color

         x = rand()%(SCREEN_WIDTH/2);
         y = rand()%(SCREEN_HEIGHT/2);

         Plot_Pixel_Fast_DB(x,y,fcolor);
         Plot_Pixel_Fast_DB((SCREEN_WIDTH-1)-x,y,fcolor);
         Plot_Pixel_Fast_DB(x,(SCREEN_HEIGHT-1)-y,fcolor);
         Plot_Pixel_Fast_DB((SCREEN_WIDTH-1)-x,(SCREEN_HEIGHT-1)-y,fcolor);

         } // end for

     // copy double buffer to video buffer

     Show_Double_Buffer(double_buffer);

     // wait a bit so user can see it

     Delay(2);

     } // end while

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

} // end main

