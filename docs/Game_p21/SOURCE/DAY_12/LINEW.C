
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
#include "graph5.h"  // need polygon stuff
#include "graph6.h"

// D E F I N E S //////////////////////////////////////////////////////////////

#define MAX_LINES 128

// S T R U C T U R E S ////////////////////////////////////////////////////////

typedef struct line_typ
        {

        int color;      // color of the line
        int x1;         // first endpoint of the line
        int y1;
        int x2;         // second endpoint of the line
        int y2;

        } line, *line_ptr;

// G L O B A L S /////////////////////////////////////////////////////////////

// the data structure that holds all the lines

line line_data[MAX_LINES];

// M A I N ////////////////////////////////////////////////////////////////////

void main(void)
{

FILE *fp;

int color,x1,y1,x2,y2,num_lines,lines;  // working variables

// this is the main function

// SECTION 1 /////////////////////////////////////////////////////////////////

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// SECTION 2 /////////////////////////////////////////////////////////////////

// open the data file

fp = fopen("linew.dat","r");

// extract number of lines

fscanf(fp,"%d",&num_lines);

// load each line and draw

for (lines=0; lines<num_lines;lines++)
    {

    // load a line structure

    fscanf(fp,"%d %d %d %d %d",&color,&x1,&y1,&x2,&y2);

    // save line in data structure

    line_data[lines].color = color;
    line_data[lines].x1    = x1;
    line_data[lines].y1    = y1;
    line_data[lines].x2    = x2;
    line_data[lines].y2    = y2;

    // render the line

    Bline(x1,y1,x2,y2,color);

    } // end for lines

// SECTION 3 /////////////////////////////////////////////////////////////////

Blit_String(2,2,10,"Press any key to exit.",1);

// main event loop

while(!kbhit())
     {
     // do nothing !
     } // end while

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

} // end main


