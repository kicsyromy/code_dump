
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

#define VGA_INPUT_STATUS_1   0x3DA // vga status reg 1, bit 3 is the vsync
                                   // when 1 - retrace in progress
                                   // when 0 - no retrace

#define VGA_VSYNC_MASK 0x08        // masks off unwanted bit of status reg



// F U N C T I O N S ////////////////////////////////////////////////////////

void Wait_For_Vsync(void)
{
// this function waits for the start of a vertical retrace, if a vertical
// retrace is in progress then it waits until the next one

while(_inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK)
     {
     // do nothing, vga is in retrace
     } // end while

// now wait for vysnc and exit

while(!(_inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK))
     {
     // do nothing, wait for start of retrace
     } // end while

// at this point a vertical retrace is occuring, so return back to caller

} // end Wait_For_Vsync

// M A I N //////////////////////////////////////////////////////////////////

void main(void)
{
char buffer[128];      // used as temporary string buffer
long number_vsyncs=0;  // tracks number of retrace cycles

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// wait till user hits a key

while(!kbhit())
     {

     // wait for a vsync

     Wait_For_Vsync();

     // do graphics or whatever now that we know electron gun is retracing
     // we only have 1/70 of a second though! Usually, we would copy the
     // double buffer to the video ram

     Plot_Pixel_Fast(rand()%320, rand()%200,rand()%256);

     // tally vsyncs

     number_vsyncs++;

     // print to screen

     sprintf(buffer,"Number of Vsync's = %ld   ",number_vsyncs);

     Blit_String(8,8,9,buffer,0);

     } // end while

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

} // end main

