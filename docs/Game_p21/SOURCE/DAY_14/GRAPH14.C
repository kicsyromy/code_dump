
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
#include "graph5.h"
#include "graph6.h"
#include "graph14.h"

// F U N C T I O N S /////////////////////////////////////////////////////////

void Draw_Polygon_DB(polygon_ptr poly)
{
// this function draws a polygon into the double buffer without clipping
// caller should make sure that vertices are within bounds of clipping
// rectangle, also the polygon will always be unfilled regardless
// of the fill flag

int index,xo,yo;

// extract local origin

xo = poly->lxo;
yo = poly->lyo;

// draw polygon

for (index=0; index<poly->num_vertices-1; index++)
    {

    Bline_DB(xo+(int)poly->vertices[index].x,yo+(int)poly->vertices[index].y,
          xo+(int)poly->vertices[index+1].x,yo+(int)poly->vertices[index+1].y,
          poly->b_color);

    } // end for index

    // close polygon?

    if (!poly->closed)
       return;

    Bline_DB(xo+(int)poly->vertices[index].x,yo+(int)poly->vertices[index].y,
          xo+(int)poly->vertices[0].x,yo+(int)poly->vertices[0].y,
          poly->b_color);


} // end Draw_Polygon_DB

//////////////////////////////////////////////////////////////////////////////

void Draw_Polygon_Clip_DB(polygon_ptr poly)
{

// this function draws a polygon into the double buffer with clipping
// also the polygon will always be unfilled regardless
// of the fill flag in the polygon structure

int index,  // loop index
    xo,yo,  // local origin
    x1,y1,  // end points of current line being processed
    x2,y2;

// extract local origin

xo = poly->lxo;
yo = poly->lyo;

// draw polygon

for (index=0; index<poly->num_vertices-1; index++)
    {

    // extract the line

    x1 = (int)poly->vertices[index].x+xo;
    y1 = (int)poly->vertices[index].y+yo;

    x2 = (int)poly->vertices[index+1].x+xo;
    y2 = (int)poly->vertices[index+1].y+yo;

    // clip line to viewing screen and draw unless line is totally invisible

    if (Clip_Line(&x1,&y1,&x2,&y2))
       {
       // line was clipped and now can be drawn
       Bline_DB(x1,y1,x2,y2,poly->b_color);

       } // end if draw line

    } // end for index

    // close polygon?        // close polygon

    if (!poly->closed)
       return;

    // extract the line

    x1 = (int)poly->vertices[index].x+xo;
    y1 = (int)poly->vertices[index].y+yo;

    x2 = (int)poly->vertices[0].x+xo;
    y2 = (int)poly->vertices[0].y+yo;

    // clip line to viewing screen and draw unless line is totally invisible

    if (Clip_Line(&x1,&y1,&x2,&y2))
       {

       // line was clipped and now can be drawn

       Bline_DB(x1,y1,x2,y2,poly->b_color);

       } // end if draw line

} // end Draw_Polygon_Clip_DB

//////////////////////////////////////////////////////////////////////////////

void Bline_DB(int xo, int yo, int x1,int y1, unsigned char color)
{
// this function uses Bresenham's algorithm IBM (1965) to draw a line from
// (xo,yo) - (x1,y1) into the double buffer

int dx,             // difference in x's
    dy,             // difference in y's
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error=0,        // the discriminant i.e. error i.e. decision variable
    index;          // used for looping



unsigned char far *vb_start = double_buffer; // directly access the double
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

} // end Bline_DB

///////////////////////////////////////////////////////////////////////////////

