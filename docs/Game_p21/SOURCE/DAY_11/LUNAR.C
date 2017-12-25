
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

// D E F I N E S /////////////////////////////////////////////////////////////

#define KEYBOARD_INT    0x09   // the keyboard interrupt number
#define KEY_BUFFER      0x60   // the location of the keyboard buffer
#define KEY_CONTROL     0x61   // the location of the keyboard controller
#define INT_CONTROL     0x20   // the location of the interrupt controller

// make and break codes for the arrow keys (note the make codes are the
// same as the scan codes and the break codes are just the scan codes plus
// 128.  For example the scan code for the UP key is 72 which is the make
// code.  if we add 128 to this then the result is 128+72 = 200.

// arrow keys

#define MAKE_RIGHT      77
#define MAKE_LEFT       75
#define MAKE_UP         72
#define MAKE_DOWN       80

// some useful control keys

#define MAKE_ENTER      28
#define MAKE_TAB        15
#define MAKE_SPACE      57
#define MAKE_CTRL       29
#define MAKE_ALT        56
#define MAKE_ESC        1

// and now the break codes

#define BREAK_RIGHT     205
#define BREAK_LEFT      203
#define BREAK_UP        200
#define BREAK_DOWN      208

#define BREAK_ENTER     156
#define BREAK_TAB       143
#define BREAK_SPACE     185
#define BREAK_CTRL      157
#define BREAK_ALT       184
#define BREAK_ESC       129

// indices into arrow key state table

#define INDEX_UP        0
#define INDEX_DOWN      1
#define INDEX_RIGHT     2
#define INDEX_LEFT      3

#define INDEX_ENTER     4
#define INDEX_TAB       5
#define INDEX_SPACE     6
#define INDEX_CTRL      7
#define INDEX_ALT       8
#define INDEX_ESC       9

#define NUM_KEYS       10  // number of keys in look up table

// G L O B A L S /////////////////////////////////////////////////////////////

void (_interrupt _far *Old_Key_Isr)();  // holds old keyboard interrupt handler

int raw_key;  // the global raw keyboard data

// the arrow key state table

int key_table[NUM_KEYS] = {0,0,0,0,0,0,0,0,0,0};


// globals for the demo

int land_sx = 160,    // starting x of the landing pad
    land_ex = 180,    // ending x of the landing pad
    land_y  = 170;    // the y position of the platform

float lander_xv = 0,    // the initial velocity of the lunar lander
      lander_yv = 0,
      fuel      = 1000; // initial load of fuel

int right_engine = 0,   // these track which engines need to be displayed
    left_engine  = 0,
    up_engine    = 0,
    down_engine  = 0;

pcx_picture imagery_pcx,      // the game imagery
            background_pcx;   // the backdrop

// the sprite used in the game

sprite lander;                // the lunar lander

// F U N C T I O N S ////////////////////////////////////////////////////////

void _interrupt _far New_Key_Int()
{

// read the key from the hardware and then re-enable the keyboard to
// read another key

_asm
   {
   sti                    ; re-enable interrupts
   in al, KEY_BUFFER      ; get the key that was pressed
   xor ah,ah              ; zero out upper 8 bits of AX
   mov raw_key, ax        ; store the key in global variable
   in al, KEY_CONTROL     ; set the control register
   or al, 82h             ; set the proper bits to reset the keyboard flip flop
   out KEY_CONTROL,al     ; send the new data back to the control register
   and al,7fh
   out KEY_CONTROL,al     ; complete the reset
   mov al,20h
   out INT_CONTROL,al     ; re-enable interrupts
                          ; this is not really needed since we are using the
                          ; C _interrupt function type, it does this for us,
                          ; however, it's a good habit to get into and can't
                          ; hurt

   } // end inline assembly

// now for some C to update the arrow state table

// process the key and update the key state table

switch(raw_key)
      {
      case MAKE_UP:    // pressing up
           {
           key_table[INDEX_UP]    = 1;
           } break;

      case MAKE_DOWN:  // pressing down
           {
           key_table[INDEX_DOWN]  = 1;
           } break;

      case MAKE_RIGHT: // pressing right
           {
           key_table[INDEX_RIGHT] = 1;
           } break;

      case MAKE_LEFT:  // pressing left
           {
           key_table[INDEX_LEFT]  = 1;
           } break;

      case MAKE_ENTER:    // pressing enter
           {
           key_table[INDEX_ENTER]    = 1;
           } break;

      case MAKE_TAB :  // pressing tab
           {
           key_table[INDEX_TAB ]  = 1;
           } break;

      case MAKE_SPACE : // pressing space
           {
           key_table[INDEX_SPACE ] = 1;
           } break;

      case MAKE_CTRL :  // pressing control
           {
           key_table[INDEX_CTRL ]  = 1;
           } break;

      case MAKE_ALT  : // pressing alt
           {
           key_table[INDEX_ALT  ] = 1;
           } break;

      case MAKE_ESC  :  // pressing escape
           {
           key_table[INDEX_ESC ]  = 1;
           } break;

      case BREAK_UP:    // releasing up
           {
           key_table[INDEX_UP]    = 0;
           } break;

      case BREAK_DOWN:  // releasing down
           {
           key_table[INDEX_DOWN]  = 0;
           } break;

      case BREAK_RIGHT: // releasing right
           {
           key_table[INDEX_RIGHT] = 0;
           } break;

      case BREAK_LEFT:  // releasing left
           {
           key_table[INDEX_LEFT]  = 0;
           } break;

      case BREAK_ENTER:    // releasing enter
           {
           key_table[INDEX_ENTER]   = 0;
           } break;

      case BREAK_TAB :  // releasing tab
           {
           key_table[INDEX_TAB ]  = 0;
           } break;

      case BREAK_SPACE : // releasing space
           {
           key_table[INDEX_SPACE ] = 0;
           } break;

      case BREAK_CTRL :  // releasing control
           {
           key_table[INDEX_CTRL ]  = 0;
           } break;

      case BREAK_ALT  : // releasing alt
           {
           key_table[INDEX_ALT  ] = 0;
           } break;

      case BREAK_ESC  :  // releasing escape
           {
           key_table[INDEX_ESC ]  = 0;
           } break;

      default: break;

      } // end switch

// note how we don't chain interrupts, we want total control of the keyboard
// however, if you wanted to chain then you would make a call to the old
// keyboard handler right here.


} // end New_Key_Int

///////////////////////////////////////////////////////////////////////////////

void Install_Keyboard(void)
{

Old_Key_Isr = _dos_getvect(KEYBOARD_INT);

_dos_setvect(KEYBOARD_INT, New_Key_Int);

} // end Install_Keyboard

///////////////////////////////////////////////////////////////////////////////

void Delete_Keyboard(void)
{

_dos_setvect(KEYBOARD_INT, Old_Key_Isr);

} // end Delete_Keyboard

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{
int done=0,     // system exit flag
    index,      // looping variable
    score;      // used to compute score

char string[80];  // used for printing

// SECTION 1 /////////////////////////////////////////////////////////////////

// install the keyboard driver

Install_Keyboard();

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// create a double buffer

if (!Create_Double_Buffer(SCREEN_HEIGHT))
   {
   printf("\nNot enough memory to create double buffer.");

   } // end if

// clear the double buffer

Fill_Double_Buffer(0);

// SECTION 2 /////////////////////////////////////////////////////////////////

// load in the background image into the double buffer

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load("moon.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

// load in imagery for lunar lander

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("lander.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize player and extract bitmaps

sprite_width  = 16;
sprite_height = 16;

Sprite_Init((sprite_ptr)&lander,0,0,0,0,0,0);

for (index=0; index<5; index++)
    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&lander,index,index,0);

// set position of lander

lander.x          = 100;
lander.y          = 10;
lander.curr_frame = 0;
lander.state      = 1;

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

// scan background under lander

Behind_Sprite_DB((sprite_ptr)&lander);


// SECTION 3 /////////////////////////////////////////////////////////////////

// main event loop

while(!done)
     {

     // erase the lander

     Erase_Sprite_DB((sprite_ptr)&lander);

     // transform the lander, notice how we look at the keyboard state table

     // reset current frame of lander and active engines

     lander.curr_frame = 0;
     right_engine = left_engine = down_engine = up_engine = 0;

     // now we will look into the keyboard table to see what keys are being
     // pressed, note that this table is updated not by the main(), but by
     // the keyboard interrupt, this allows us to track multiple keys
     // simulataneously

// SECTION 4 /////////////////////////////////////////////////////////////////

     // test if user is exiting

     if (key_table[INDEX_ESC]) done=1;

     // test the motion keys

     if (key_table[INDEX_RIGHT])
        {

        // increase x velocity

        lander_xv+=.1;

        // limit velocity

        if (lander_xv>3)
            lander_xv=3;

        // set engine flag

        right_engine = 1;

        // expend fuel

        fuel-=.5;

        } // end if

     if (key_table[INDEX_LEFT])
        {
        // decrease x velocity

        lander_xv-=.1;

        // limit velocity

        if (lander_xv<-3)
            lander_xv=-3;

        // set engine flag

        left_engine = 1;

        // expend fuel

        fuel-=.5;

        } // end if

     if (key_table[INDEX_UP])
        {
        // decrease y velocity

        lander_yv-=.1;

        // limit velocity

        if (lander_yv<-3)
            lander_yv=-3;

        // set engine flag

        up_engine = 1;

        // expend fuel

        fuel-=.5;

        } // end if

     if (key_table[INDEX_DOWN])
        {
        // increase y velocity

        lander_yv+=.1;

        // limit velocity

        if (lander_yv>4)
            lander_yv=4;

        // set engine flag

        down_engine = 1;

        // expend fuel

        fuel-=.5;

        } // end if

// SECTION 5 /////////////////////////////////////////////////////////////////

     // based on current velocity, move lander

     lander.x = lander.x + (int)(lander_xv+.5);
     lander.y = lander.y + (int)(lander_yv+.5);

     // check if lander has moved off screen boundary

     // x tests

     if (lander.x > 320-16)
         lander.x = 0;
     else
     if (lander.x < 0)
         lander.x = 320-16;

     // y tests

     if (lander.y > 190-16)
         lander.y = 190-16;
     else
     if (lander.y < 0)
         lander.y = 0;

// SECTION 6 /////////////////////////////////////////////////////////////////

     // apply gravity

     lander_yv+=.05;

     if (lander_yv>3)
        lander_yv=3;

     // expend fuel

     fuel-=.02;

     // draw the lander

     Behind_Sprite_DB((sprite_ptr)&lander);

// SECTION 7 /////////////////////////////////////////////////////////////////

     // based on the engines that are on, draw the lander

     // always draw the standard lander without engines first

     lander.curr_frame = 0;
     Draw_Sprite_DB((sprite_ptr)&lander);

     // draw any engines that are on

     if (right_engine)
        {
        lander.curr_frame = 2;
        Draw_Sprite_DB((sprite_ptr)&lander);
        } // end if

     if (left_engine)
        {
        lander.curr_frame = 3;
        Draw_Sprite_DB((sprite_ptr)&lander);
        } // end if

     if (up_engine)
        {
        lander.curr_frame = 1;
        Draw_Sprite_DB((sprite_ptr)&lander);
        } // end if

     if (down_engine)
        {
        lander.curr_frame = 4;
        Draw_Sprite_DB((sprite_ptr)&lander);
        } // end if

// SECTION 8 /////////////////////////////////////////////////////////////////

     // draw indicators

     if (fuel<0) fuel=0;

     sprintf(string,"Fuel = %.2f  ",fuel);
     Blit_String_DB(10,2,10,string,0);

     sprintf(string,"XV = %.2f  ",lander_xv);
     Blit_String_DB(10,12,10,string,0);

     sprintf(string,"YV = %.2f  ",lander_yv);
     Blit_String_DB(10,22,10,string,0);

     // show the double buffer

     Show_Double_Buffer(double_buffer);

     // wait a while

     Delay(1);

// SECTION 9 /////////////////////////////////////////////////////////////////

     // test if the lander has landed

     if (lander.x >= 245 && lander.x <= (266-16) && lander.y >= (185-16) &&
         lander_yv < 2.0)
         {
         // print banner

         Blit_String(2,60,15,"T H E  E A G L E  H A S  L A N D E D!",1);

         // compute score based on fuel and velocity

         score = (int)(fuel*10 - lander_yv * 100);

         if (score < 0) score = 0;

         sprintf(string,"Score was %d",score);
         Blit_String(100,110,15,string,1);

         // wait a second

         Delay(100);

         // fade everything

         Fade_Lights();

         // exit system

         done=1;

         } // end if the lander has landed

     } // end while

// SECTION 10 ////////////////////////////////////////////////////////////////

// delete the keyboard driver

Delete_Keyboard();

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

} // end main

