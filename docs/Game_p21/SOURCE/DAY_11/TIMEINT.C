
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>

#include "graph3.h"  // include graphics our stuff
#include "graph4.h"
#include "graph6.h"

// D E F I N E S /////////////////////////////////////////////////////////////

#define TIME_KEEPER_INT     0x1C    // the time keeper interrupt

// G L O B A L S /////////////////////////////////////////////////////////////

void (_interrupt far *Old_Time_Isr)();  // used to hold old interrupt vector

long counter=0;                    // global variable to be altered by ISR

// F U N C T I O N S ////////////////////////////////////////////////////////

void _interrupt far Timer(void)
{

// increment value of counter 18.2 times a second

counter++;

// now call old interrupt handler (if there was one?). This is only needed
// if you want to chain interrupts handlers.  In the case that you want
// total control then the next instruction wouldn't be used.

Old_Time_Isr();

} // end Timer

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

char string[128]; // working string

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// draw instructions

Blit_String(2,2,15,"Press any key to exit.",0);

// install our time keeper ISR while saving old one

Old_Time_Isr = _dos_getvect(TIME_KEEPER_INT);

_dos_setvect(TIME_KEEPER_INT, Timer);

// wait for user to hit a key

while(!kbhit())
     {

     // print out current value of "counter", but note how we don't change it!
     // it is being changed by the ISR

     sprintf(string,"The interrupt has been called %ld times",counter);
     Blit_String(2,2+2*8,10,string,0);

     } // end while

// replace old time keeper ISR

_dos_setvect(TIME_KEEPER_INT, Old_Time_Isr);

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

} // end main

