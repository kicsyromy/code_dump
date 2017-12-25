
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

// global clipping region default value

#define POLY_CLIP_MIN_X   (160-30)
#define POLY_CLIP_MIN_Y   (100-30)

#define POLY_CLIP_MAX_X   (160+30)
#define POLY_CLIP_MAX_Y   (100+30)

#define MAX_VERTICES          16   // maximum numbr of vertices in a polygon

// S T R U C T U R E S ///////////////////////////////////////////////////////

typedef struct vertex_typ
        {

        float x,y;        // the vertex in 2-D

        } vertex, *vertex_ptr;

//////////////////////////////////////////////////////////////////////////////

typedef struct polygon_typ
        {

        int b_color;                    // border color
        int i_color;                    // interior color
        int closed;                     // is the polygon closed
        int filled;                     // is this polygon filled


        int lxo,lyo;                    // local origin of polygon
        int num_vertices;               // number of defined vertices
        vertex vertices[MAX_VERTICES];  // the vertices of the polygon

        } polygon, *polygon_ptr;

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Bline(int xo, int yo, int x1,int y1, unsigned char color);

// G L O B A L S  ////////////////////////////////////////////////////////////


float sin_look[361],   // look up tables for sin and cosine
      cos_look[361];

// the clipping region, set it to default on start up

int poly_clip_min_x = POLY_CLIP_MIN_X,
    poly_clip_min_y = POLY_CLIP_MIN_Y,

    poly_clip_max_x = POLY_CLIP_MAX_X,
    poly_clip_max_y = POLY_CLIP_MAX_Y;


// F U N C T I O N S /////////////////////////////////////////////////////////

void Create_Tables()
{
// this function creates the sin and cosine lookup tables

int index;

// create the tables

for (index=0; index<=360; index++)
    {

    cos_look[index] = (float)cos((double)(index*3.14159/180));
    sin_look[index] = (float)sin((double)(index*3.14159/180));

    } // end for

} // end Create_Tables

//////////////////////////////////////////////////////////////////////////////

void Rotate_Polygon(polygon_ptr poly, int angle)
{

int index;    // loop index

float si,cs,  // values of sin and cosine
      rx,ry;  // roated points

// rotate each point of the poly gon around its local origin
// note that angle is an integer and ranges from -360 to +360

// compute sin and cos of angle to be rotated

if (angle>=0)
   {
   // extract sin and cosine from look up table

   si = sin_look[angle];
   cs = cos_look[angle];


   } // end if positive angle
else
   {
   // angle is negative to convert to positive

   // convert negative angle to positive angle and extract values

   si = sin_look[angle+360];
   cs = cos_look[angle+360];

   } // end else

// using values for sin and cosine rotate the point

for (index=0; index<poly->num_vertices; index++)
    {
    // compute rotated values using rotation eqns.

    rx  = poly->vertices[index].x * cs -  poly->vertices[index].y * si;
    ry  = poly->vertices[index].y * cs +  poly->vertices[index].x * si;

    // store the rotated vertex back into structure

    poly->vertices[index].x = rx;
    poly->vertices[index].y = ry;

    } // end for

} // end Rotate_Polygon

///////////////////////////////////////////////////////////////////////////////

void Scale_Polygon(polygon_ptr poly, float scale)
{

int index;

// scale each vertex of the polygon

for (index=0; index<poly->num_vertices; index++)
    {
    // multiply by the scaling factor

    poly->vertices[index].x*=scale;
    poly->vertices[index].y*=scale;

    } // end for

} // end Scale_Polygon

///////////////////////////////////////////////////////////////////////////////

void Translate_Polygon(polygon_ptr poly, int dx,int dy)
{

// translate the origin of the polygon

poly->lxo+=dx;
poly->lyo+=dy;

// that was easy!

} // end Translate_Polygon

///////////////////////////////////////////////////////////////////////////////

void Draw_Polygon(polygon_ptr poly)
{
// this function draws a polygon on the screen without clipping
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

    Bline(xo+(int)poly->vertices[index].x,yo+(int)poly->vertices[index].y,
          xo+(int)poly->vertices[index+1].x,yo+(int)poly->vertices[index+1].y,
          poly->b_color);

    } // end for index

    // close polygon?

    if (!poly->closed)
       return;

    Bline(xo+(int)poly->vertices[index].x,yo+(int)poly->vertices[index].y,
          xo+(int)poly->vertices[0].x,yo+(int)poly->vertices[0].y,
          poly->b_color);


} // end Draw_Polygon

//////////////////////////////////////////////////////////////////////////////

int Clip_Line(int *x1,int *y1,int *x2, int *y2)
{
// this function clips the sent line using the globally defined clipping
// region

int point_1 = 0, point_2 = 0;  // tracks if each end point is visible or invisible

int clip_always = 0;           // used for clipping override

int xi,yi;                     // point of intersection

int right_edge=0,              // which edges are the endpoints beyond
    left_edge=0,
    top_edge=0,
    bottom_edge=0;


int success = 0;               // was there a successfull clipping

float dx,dy;                   // used to holds slope deltas

// SECTION 1 //////////////////////////////////////////////////////////////////

// test if line is completely visible

if ( (*x1>=poly_clip_min_x) && (*x1<=poly_clip_max_x) &&
     (*y1>=poly_clip_min_y) && (*y1<=poly_clip_max_y) )
     point_1 = 1;


if ( (*x2>=poly_clip_min_x) && (*x2<=poly_clip_max_x) &&
     (*y2>=poly_clip_min_y) && (*y2<=poly_clip_max_y) )
     point_2 = 1;

// SECTION 2 /////////////////////////////////////////////////////////////////

// test endpoints

if (point_1==1 && point_2==1)
   return(1);

// SECTION 3 /////////////////////////////////////////////////////////////////

// test if line is completely invisible

if (point_1==0 && point_2==0)
   {

   // must test to see if each endpoint is on the same side of one of
   // the bounding planes created by each clipping region boundary

   if ( ((*x1<poly_clip_min_x) && (*x2<poly_clip_min_x)) || // to the left
        ((*x1>poly_clip_max_x) && (*x2>poly_clip_max_x)) || // to the right

        ((*y1<poly_clip_min_y) && (*y2<poly_clip_min_y)) || // above
        ((*y1>poly_clip_max_y) && (*y2>poly_clip_max_y)) )  // below
        {

        // no need to draw line

        return(0);

        } // end if invisible

   // if we got here we have the special case where the line cuts into and
   // out of the clipping region

   clip_always = 1;

   } // end if test for invisibly

// SECTION 4 /////////////////////////////////////////////////////////////////

// take care of case where either endpoint is in clipping region

if (( point_1==1) || (point_1==0 && point_2==0) )
   {

   // compute deltas

   dx = *x2 - *x1;
   dy = *y2 - *y1;

   // compute what boundary line need to be clipped against

   if (*x2 > poly_clip_max_x)
      {
      // flag right edge

      right_edge = 1;

      // compute intersection with right edge

      if (dx!=0)
         yi = (int)(.5 + (dy/dx) * (poly_clip_max_x - *x1) + *y1);
      else
         yi = -1;  // invalidate intersection

      } // end if to right
   else
   if (*x2 < poly_clip_min_x)
      {
      // flag left edge

      left_edge = 1;

      // compute intersection with left edge

      if (dx!=0)
         yi = (int)(.5 + (dy/dx) * (poly_clip_min_x - *x1) + *y1);
      else
         yi = -1;  // invalidate intersection

      } // end if to left

   // horizontal intersections

   if (*y2 > poly_clip_max_y)
      {
      // flag bottom edge

      bottom_edge = 1;

      // compute intersection with right edge

      if (dy!=0)
         xi = (int)(.5 + (dx/dy) * (poly_clip_max_y - *y1) + *x1);
      else
         xi = -1;  // invalidate inntersection

      } // end if bottom
   else
   if (*y2 < poly_clip_min_y)
      {
      // flag top edge

      top_edge = 1;

      // compute intersection with top edge

      if (dy!=0)
         xi = (int)(.5 + (dx/dy) * (poly_clip_min_y - *y1) + *x1);
      else
         xi = -1;  // invalidate inntersection

      } // end if top

// SECTION 5 /////////////////////////////////////////////////////////////////

   // now we know where the line passed thru
   // compute which edge is the proper intersection

   if (right_edge==1 && (yi>=poly_clip_min_y && yi<=poly_clip_max_y) )
      {

      *x2 = poly_clip_max_x;
      *y2 = yi;

      success = 1;

      } // end if intersected right edge
   else
   if (left_edge==1 && (yi>=poly_clip_min_y && yi<=poly_clip_max_y) )
      {

      *x2 = poly_clip_min_x;
      *y2 = yi;

      success = 1;

      } // end if intersected left edge

   if (bottom_edge==1 && (xi>=poly_clip_min_x && xi<=poly_clip_max_x) )
      {

      *x2 = xi;
      *y2 = poly_clip_max_y;

      success = 1;

      } // end if intersected bottom edge
   else
   if (top_edge==1 && (xi>=poly_clip_min_x && xi<=poly_clip_max_x) )
      {

      *x2 = xi;
      *y2 = poly_clip_min_y;

      success = 1;

      } // end if intersected top edge

   } // end if point_1 is visible

// SECTION 6 /////////////////////////////////////////////////////////////////

// reset edge flags

right_edge = left_edge= top_edge = bottom_edge = 0;

// test second endpoint

if ( (point_2==1) || (point_1==0 && point_2==0))
   {

   // compute deltas

   dx = *x1 - *x2;
   dy = *y1 - *y2;

   // compute what boundary line need to be clipped against

   if (*x1 > poly_clip_max_x)
      {
      // flag right edge

      right_edge = 1;

      // compute intersection with right edge

      if (dx!=0)
         yi = (int)(.5 + (dy/dx) * (poly_clip_max_x - *x2) + *y2);
      else
         yi = -1;  // invalidate inntersection

      } // end if to right
   else
   if (*x1 < poly_clip_min_x)
      {
      // flag left edge

      left_edge = 1;

      // compute intersection with left edge

      if (dx!=0)
         yi = (int)(.5 + (dy/dx) * (poly_clip_min_x - *x2) + *y2);
      else
         yi = -1;  // invalidate intersection

      } // end if to left

   // horizontal intersections

   if (*y1 > poly_clip_max_y)
      {
      // flag bottom edge

      bottom_edge = 1;

      // compute intersection with right edge

      if (dy!=0)
         xi = (int)(.5 + (dx/dy) * (poly_clip_max_y - *y2) + *x2);
      else
         xi = -1;  // invalidate inntersection

      } // end if bottom
   else
   if (*y1 < poly_clip_min_y)
      {
      // flag top edge

      top_edge = 1;

      // compute intersection with top edge

      if (dy!=0)
         xi = (int)(.5 + (dx/dy) * (poly_clip_min_y - *y2) + *x2);
      else
         xi = -1;  // invalidate inntersection

      } // end if top

   // now we know where the line passed thru
   // compute which edge is the proper intersection

   if (right_edge==1 && (yi>=poly_clip_min_y && yi<=poly_clip_max_y) )
      {

      *x1 = poly_clip_max_x;
      *y1 = yi;

      success = 1;

      } // end if intersected right edge
   else
   if (left_edge==1 && (yi>=poly_clip_min_y && yi<=poly_clip_max_y) )
      {

      *x1 = poly_clip_min_x;
      *y1 = yi;

      success = 1;

      } // end if intersected left edge

   if (bottom_edge==1 && (xi>=poly_clip_min_x && xi<=poly_clip_max_x) )
      {

      *x1 = xi;
      *y1 = poly_clip_max_y;

      success = 1;

      } // end if intersected bottom edge
   else
   if (top_edge==1 && (xi>=poly_clip_min_x && xi<=poly_clip_max_x) )
      {

      *x1 = xi;
      *y1 = poly_clip_min_y;

      success = 1;

      } // end if intersected top edge


   } // end if point_2 is visible

// SECTION 7 /////////////////////////////////////////////////////////////////

return(success);

} // end Clip_Line

//////////////////////////////////////////////////////////////////////////////

void Draw_Polygon_Clip(polygon_ptr poly)
{

// this function draws a polygon on the screen with clipping
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
       Bline(x1,y1,x2,y2,poly->b_color);

       } // end if draw line

    } // end for index

    // close polygon?

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

       Bline(x1,y1,x2,y2,poly->b_color);

       } // end if draw line

} // end Draw_Polygon_Clip

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

///////////////////////////////////////////////////////////////////////////////

void Draw_Boundary(int color)
{
// draws in the clipping boundary if user is intersted in seeing it

Bline(poly_clip_min_x,poly_clip_min_y,
      poly_clip_max_x,poly_clip_min_y,color);


Bline(poly_clip_max_x,poly_clip_min_y,
      poly_clip_max_x,poly_clip_max_y,color);

Bline(poly_clip_max_x,poly_clip_max_y,
      poly_clip_min_x,poly_clip_max_y,color);

Bline(poly_clip_min_x,poly_clip_max_y,
      poly_clip_min_x,poly_clip_min_y,color);

} // end Draw_Boundary

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

int clip_on = 1;       // tracks if the clipping engine is on

polygon p1;            // the working polygon

int done=0;            // system exit flag

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// draw some instructions

Blit_String(0,0 ,7,"Press Q - To quit.",1);
Blit_String(0,10,7,"Press T - To toggle clipping engine.",1);
Blit_String(0,20,7,"Use Numeric keypad to translate.",1);

// create lookup tables for polygon engine

Create_Tables();


// build up a little spaceship polygon

p1.vertices[0].x = 3;
p1.vertices[0].y = -19;

p1.vertices[1].x = 12;
p1.vertices[1].y = -1;

p1.vertices[2].x = 17;
p1.vertices[2].y = 2;

p1.vertices[3].x = 17;
p1.vertices[3].y = 9;

p1.vertices[4].x = 8;
p1.vertices[4].y = 14;

p1.vertices[5].x = 5;
p1.vertices[5].y = 8;

p1.vertices[6].x = -5;
p1.vertices[6].y = 8;

p1.vertices[7].x = -8;
p1.vertices[7].y = 14;

p1.vertices[8].x = -17;
p1.vertices[8].y = 9;

p1.vertices[9].x = -17;
p1.vertices[9].y = 2;

p1.vertices[10].x = -12;
p1.vertices[10].y = -1;

p1.vertices[11].x = -3;
p1.vertices[11].y = -19;

p1.vertices[12].x = -3;
p1.vertices[12].y = -8;

p1.vertices[13].x = 3;
p1.vertices[13].y = -8;

// set position of shaceship

p1.lxo = 160;
p1.lyo = 100;

// fill in important fields

p1.num_vertices=14;
p1.b_color = 1;
p1.closed=1;


// main event loop

while(!done)
     {

     // erase polygon

     p1.b_color = 0;

     if (clip_on==1)
        Draw_Polygon_Clip((polygon_ptr)&p1);
     else
        Draw_Polygon((polygon_ptr)&p1);

     // erase the origin of polygon

     Plot_Pixel_Fast(p1.lxo,p1.lyo,0);

     // what is user doing

     if (kbhit())
        {
        switch(getch())
              {
              case '8':   // move ship left
                 {
                 p1.lyo--;

                 } break;

              case '2':  // move ship right
                 {
                 p1.lyo++;

                 } break;

              case '6':   // move ship down
                 {
                 p1.lxo++;

                 } break;

              case '4':   // move ship up
                 {
                 p1.lxo--;

                 } break;

              case 't':   // toggle clipper
                   {
                   clip_on=-clip_on;
                   } break;

              case 'q':   // it's coming now Khan!!!
                 {
                 done=1;
                 } break;

              default:break;

              } // end switch

        } // end if kbhit

     // rotate polygon 5 degrees

     Rotate_Polygon((polygon_ptr)&p1,5);

     // draw polygon

     if (clip_on==1)
        {
        p1.b_color = 1;
        Draw_Polygon_Clip((polygon_ptr)&p1);
        Blit_String(10,100,7,"Clipping on. ",0);
        }
     else
        {
        p1.b_color = 12;
        Draw_Polygon((polygon_ptr)&p1);
        Blit_String(10,100,7,"Clipping off.",0);
        }

     // draw a point art origin of polygon

     Plot_Pixel_Fast(p1.lxo,p1.lyo,15);

     // let user see clipping region

     Draw_Boundary(10);

     // just chill here for 1/18.2 th of a second

     Delay(1);

     } // end while

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

} // end main

