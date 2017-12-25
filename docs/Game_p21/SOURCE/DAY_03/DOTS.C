
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "graph3.h" // this is all we need to include so that the program
                    // knows all the #defines, structures, prototypes etc.

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

int done=0, // exit flag
    index;  // loop index

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// plot 10000 dots

for (index=0; index<10000; index++)
    Plot_Pixel_Fast(rand()%320, rand()%200,rand()%256);

// wait for user to hit a key

while(!kbhit()){}

// reset back set video mode to 320x200 256 color mode

Set_Video_Mode(TEXT_MODE);

} // end main



