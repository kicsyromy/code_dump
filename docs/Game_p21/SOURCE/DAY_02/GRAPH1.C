
// GRAPH1.C - This is the first module in the game library

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
#include "graph1.h"

// G L O B A L S //////////////////////////////////////////////////////////////

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr
unsigned int far *video_buffer_w= (int far *)0xA0000000L;  // vram word ptr
unsigned char far *rom_char_set = (char far *)0xF000FA6EL; // rom characters 8x8

// F U N C T I O N S /////////////////////////////////////////////////////////

void Blit_Char(int xc,int yc,char c,int color,int trans_flag)
{
// this function uses the rom 8x8 character set to blit a character on the
// video screen, notice the trick used to extract bits out of each character
// byte that comprises a line

int offset,x,y;
char far *work_char;
unsigned char bit_mask = 0x80;

// compute starting offset in rom character lookup table

work_char = rom_char_set + c * CHAR_HEIGHT;

// compute offset of character in video buffer

offset = (yc << 8) + (yc << 6) + xc;

for (y=0; y<CHAR_HEIGHT; y++)
    {
    // reset bit mask

    bit_mask = 0x80;

    for (x=0; x<CHAR_WIDTH; x++)
        {
        // test for transparent pixel i.e. 0, if not transparent then draw

        if ((*work_char & bit_mask))
             video_buffer[offset+x] = color;

        else if (!trans_flag)  // takes care of transparency
            video_buffer[offset+x] = 0;

        // shift bit mask

        bit_mask = (bit_mask>>1);

        } // end for x

    // move to next line in video buffer and in rom character data area

    offset      += SCREEN_WIDTH;
    work_char++;

    } // end for y

} // end Blit_Char

//////////////////////////////////////////////////////////////////////////////

void Blit_String(int x,int y,int color, char *string,int trans_flag)
{
// this function blits an entire string on the screen with fixed spacing
// between each character.  it calls blit_char.

int index;

for (index=0; string[index]!=0; index++)
     {

     Blit_Char(x+(index<<3),y,string[index],color,trans_flag);

     } /* end while */

} /* end Blit_String */

///////////////////////////////////////////////////////////////////////////////

void Plot_Pixel(int x,int y,unsigned char color)
{

// plots the pixel in the desired color
// each row contains 320 bytes, therefore multiple Y times the row and add x

video_buffer[y*320+x] = color;

} // end Plot_Pixel

///////////////////////////////////////////////////////////////////////////////

void Plot_Pixel_Fast(int x,int y,unsigned char color)
{

// plots the pixel in the desired color a little quicker using binary shifting
// to accomplish the multiplications

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

video_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast

//////////////////////////////////////////////////////////////////////////////

void Set_Video_Mode(int mode)
{

// use the video interrupt 10h to set the video mode to the sent value

union REGS inregs,outregs;

inregs.h.ah = 0;                    // set video mode sub-function
inregs.h.al = (unsigned char)mode;  // video mode to change to

_int86(0x10, &inregs, &outregs);

} // end Set_Video_Mode

/////////////////////////////////////////////////////////////////////////////

void Delay(int clicks)
{
// this function uses the internal time keeper timer i.e. the one that goes
// at 18.2 clicks/sec to to a time delay.  You can find a 32 bit value of
// this timer at 0000:046Ch

unsigned int far *clock = (unsigned int far *)0x0000046CL;

unsigned int now;

// get current time

now = *clock;

// wait till time has gone past current time plus the amount we eanted to
// wait.  Note each click is approx. 55 milliseconds.

while(abs(*clock - now) < clicks){}

} // end Delay

////////////////////////////////////////////////////////////////////////////////

void H_Line(int x1,int x2,int y,unsigned int color)
{
// draw a horizontal line useing the memset function
// note x2 > x1

_fmemset((char far *)(video_buffer + ((y<<8) + (y<<6)) + x1),color,x2-x1+1);

} // end H_Line

//////////////////////////////////////////////////////////////////////////////

void V_Line(int y1,int y2,int x,unsigned int color)
{
// draw a vertical line, note y2 > y1

unsigned int line_offset,
                    index;

// compute starting position

line_offset = ((y1<<8) + (y1<<6)) + x;

for (index=0; index<=y2-y1; index++)
    {
    video_buffer[line_offset] = color;

     line_offset+=320; // move to next line

    } // end for index

} // end V_Line

//////////////////////////////////////////////////////////////////////////////

void H_Line_Fast(int x1,int x2,int y,unsigned int color)
{
// a fast horizontal line renderer uses word writes instead of byte writes
// the only problem is the endpoints of the h line must be taken into account.
// test if the endpoints of the horizontal line are on word boundaries i.e.
// they are envenly divisible by 2
// basically, we must consider the two end points of the line separately
// if we want to write words at a time or in other words two pixels at a time
// note x2 > x1

unsigned int first_word,
            middle_word,
              last_word,
            line_offset,
                   index;


// test the 1's bit of the starting x

if ( (x1 & 0x0001) )
   {

   first_word = (color<<8);

   } // end if starting point is on a word boundary
else
   {
   // replicate color in to both bytes
   first_word = ((color<<8) | color);

   } // end else

// test the 1's bit of the ending x

if ( (x2 & 0x0001) )
   {

   last_word = ((color<<8) | color);

   } // end if ending point is on a word boundary
else
   {
   // place color in high byte of word only

   last_word = color;

   } // end else

// now we can draw the horizontal line two pixels at a time

line_offset = ((y<<7) + (y<<5));  // y*160, since there are 160 words/line

// compute middle color

middle_word = ((color<<8) | color);

// left endpoint

video_buffer_w[line_offset + (x1>>1)] = first_word;

// the middle of the line

for (index=(x1>>1)+1; index<(x2>>1); index++)
    video_buffer_w[line_offset+index] = middle_word;

// right endpoint

video_buffer_w[line_offset + (x2>>1)] = last_word;

} // end H_Line_Fast

//////////////////////////////////////////////////////////////////////////////

void Set_Palette_Register(int index, RGB_color_ptr color)
{

// this function sets a single color look up table value indexed by index
// with the value in the color structure

// tell VGA card we are going to update a pallete register

_outp(PALETTE_MASK,0xff);

// tell vga card which register we will be updating

_outp(PALETTE_REGISTER_WR, index);

// now update the RGB triple, note the same port is used each time

_outp(PALETTE_DATA,color->red);
_outp(PALETTE_DATA,color->green);
_outp(PALETTE_DATA,color->blue);

} // end Set_Palette_Color

///////////////////////////////////////////////////////////////////////////////

void Get_Palette_Register(int index, RGB_color_ptr color)
{

// this function gets the data out of a color lookup regsiter and places it
// into color

// set the palette mask register

_outp(PALETTE_MASK,0xff);

// tell vga card which register we will be reading

_outp(PALETTE_REGISTER_RD, index);

// now extract the data

color->red   = _inp(PALETTE_DATA);
color->green = _inp(PALETTE_DATA);
color->blue  = _inp(PALETTE_DATA);

} // end Get_Palette_Color

///////////////////////////////////////////////////////////////////////////////

