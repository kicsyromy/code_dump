
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

#include "graph3.h" // include our graphics library
#include "graph4.h" // include our graphics library

// D E F I N E S /////////////////////////////////////////////////////////////

#define PLANE_START_COLOR_REG 17
#define PLANE_END_COLOR_REG   30

// G L O B A L S /////////////////////////////////////////////////////////////
                                                          // 18.2 clicks/sec

pcx_picture plane;

//////////////////////////////////////////////////////////////////////////////

void Animate_Plane(void)
{
// this function animates a paper plane drawn with 14 different colors by
// illuminating a single color and turning off all the others in a sequence

RGB_color color_1, color_2;
int index;

// clear out each of the color registers used by plane

color_1.red   = 0;
color_1.green = 0;
color_1.blue  = 0;

color_2.red   = 0;
color_2.green = 63;
color_2.blue  = 0;

// clear all the colors out

for (index=PLANE_START_COLOR_REG; index<=PLANE_END_COLOR_REG; index++)
    {

    Set_Palette_Register(index, (RGB_color_ptr)&color_1);

    } // end for index

// make first plane green and then rotate colors

Set_Palette_Register(PLANE_START_COLOR_REG, (RGB_color_ptr)&color_2);

// animate the colors

while(!kbhit())
     {
     // rotate colors

     Get_Palette_Register(PLANE_END_COLOR_REG,(RGB_color_ptr)&color_1);

     for (index=PLANE_END_COLOR_REG-1; index>=PLANE_START_COLOR_REG; index--)
         {

         Get_Palette_Register(index,(RGB_color_ptr)&color_2);
         Set_Palette_Register(index+1,(RGB_color_ptr)&color_2);

         } // end for

         Set_Palette_Register(PLANE_START_COLOR_REG,(RGB_color_ptr)&color_1);

     // wait a while

     Delay(3);

     } // end while

} // end Animate_Plane

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// initialize the pcx file that holds the plane

PCX_Init((pcx_picture_ptr)&plane);

// load the pcx file that holds the cells

PCX_Load("paper.pcx", (pcx_picture_ptr)&plane,1);

PCX_Show_Buffer((pcx_picture_ptr)&plane);

PCX_Delete((pcx_picture_ptr)&plane);

Blit_String(8,8,15,"Hit any key to see animation.",0);

getch();

Blit_String(8,8,15,"Hit any key to exit.         ",0);

Animate_Plane();

// go back to text mode

Set_Video_Mode(TEXT_MODE);

} // end main



