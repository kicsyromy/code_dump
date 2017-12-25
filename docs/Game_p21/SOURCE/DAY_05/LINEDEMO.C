
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

// D E F I N E S /////////////////////////////////////////////////////////////

// S T R U C T U R E S ///////////////////////////////////////////////////////

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Bline(int xo, int yo, int x1,int y1, unsigned char color);

void Bounce(void);

// G L O B A L S  ////////////////////////////////////////////////////////////

// F U N C T I O N S /////////////////////////////////////////////////////////

void Bounce(void)
{
// this function makes use of the bline function to bounce a line around

int xo,yo,x1,y1,x2,y2,x3,y3;
int dxo,dyo,dx1,dy1,dx2,dy2,dx3,dy3;
long counter=0;
int color=9;

// starting positions of lines

xo=x2=rand()%320;
yo=y2=rand()%200;
x1=x3=rand()%320;
y1=y3=rand()%200;

// velocities of lines

dxo=dx2=2+rand()%5;
dyo=dy2=3+rand()%5;
dx1=dx3=2+rand()%5;
dy1=dy3=2+rand()%5;

// animation loop

while(!kbhit())
     {
     // draw leader

     Bline(xo,yo,x1,y1,color);

     // move line

     if ((xo+=dxo)>=315 || xo<5)
        dxo=-dxo;

     if ((yo+=dyo)>=195 || yo<5)
        dyo=-dyo;

     if ((x1+=dx1)>=315 || x1<5)
        dx1=-dx1;

     if ((y1+=dy1)>=195 || y1<5)
        dy1=-dy1;

     // test if it's time to follow the leader

     if (++counter>50)
        {

        Bline(x2,y2,x3,y3,0);

        // move line

        if ((x2+=dx2)>=315 || x2<5)
           dx2=-dx2;

        if ((y2+=dy2)>=195 || y2<5)
           dy2=-dy2;

        if ((x3+=dx3)>=315 || x3<5)
           dx3=-dx3;

        if ((y3+=dy3)>=195 || y3<5)
           dy3=-dy3;

        } // end if time to follow

     // wait a while so humans can see it

     Delay(1);

     // update color

     if (counter>250)
        {
        if (++color>=16)
           color=1;
        counter = 51;

        } // end if time to change color

     } // end while

} // end Bounce

//////////////////////////////////////////////////////////////////////////////

void Bline(int xo, int yo, int x1,int y1, unsigned char color)
{
// this function uses Bresenham's algorithm IBM (1965) to draw a line from
// (xo,yo) - (x1,y1)

int dx,             // difference in x's
    dy,             // difference in y's
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error=0,        // the discriminant i.e. error i.e. decision variable
    index;          // used for looping



unsigned char far *vb_start = video_buffer; // directly access the video
                                            // buffer for speed


// SECTION 1 /////////////////////////////////////////////////////////////////

// pre-compute first pixel address in video buffer
// use shifts for multiplication


vb_start = vb_start + ((unsigned int)yo<<6) +
                      ((unsigned int)yo<<8) +
                      (unsigned int)xo;

// compute deltas

dx = x1-xo;
dy = y1-yo;

// SECTION 2 /////////////////////////////////////////////////////////////////

// test which direction the line is going in i.e. slope angle

if (dx>=0)
   {
   x_inc = 1;

   } // end if line is moving right
else
   {
   x_inc = -1;
   dx    = -dx;  // need absolute value

   } // end else moving left

// SECTION 3 /////////////////////////////////////////////////////////////////

// test y component of slope

if (dy>=0)
   {
   y_inc = 320; // 320 bytes per line

   } // end if line is moving down
else
   {
   y_inc = -320;
   dy    = -dy;  // need absolute value

   } // end else moving up

// SECTION 4 /////////////////////////////////////////////////////////////////

// now based on which delta is greater we can draw the line

if (dx>dy)
   {

   // draw the line

   for (index=0; index<=dx; index++)
       {
       // set the pixel

       *vb_start = color;

       // adjust the discriminate

       error+=dy;

       // test if error overflowed

       if (error>dx)
          {

          error-=dx;

          // move to next line

          vb_start+=y_inc;

          } // end if error overflowed

       // move to the next pixel

       vb_start+=x_inc;

       } // end for

   } // end if |slope| <= 1
else
   {

// SECTION 5 /////////////////////////////////////////////////////////////////

   // draw the line

   for (index=0; index<=dy; index++)
       {
       // set the pixel

       *vb_start = color;

       // adjust the discriminate

       error+=dx;

       // test if error overflowed

       if (error>0)
          {

          error-=dy;

          // move to next line

          vb_start+=x_inc;

          } // end if error overflowed

       // move to the next pixel

       vb_start+=y_inc;

       } // end for

   } // end else |slope| > 1

} // end Bline

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{
// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// draw a couple lines

while(!kbhit())
     {
     // plot a line between a random start and end point

     Bline(rand()%320,rand()%200,rand()%320,rand()%200,rand()%256);

     } // end while

getch();

// clear the screen

Set_Video_Mode(VGA256);

// show off a little screen saver

Bounce();

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);


} // end main

