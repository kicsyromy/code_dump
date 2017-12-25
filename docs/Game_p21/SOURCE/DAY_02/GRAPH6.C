
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
#include "graph6.h"

// G L O B A L S  ////////////////////////////////////////////////////////////

unsigned char far *double_buffer = NULL;

unsigned int buffer_height        = SCREEN_HEIGHT;

unsigned int buffer_size = SCREEN_WIDTH*SCREEN_HEIGHT/2;

// F U N C T I O N S /////////////////////////////////////////////////////////

void Show_Double_Buffer(char far *buffer)
{
// this functions copies the doubele buffer into the video buffer

_asm
   {
   push ds               ; save DS on stack
   mov cx,buffer_size    ; this is the size of buffer in WORDS
   les di,video_buffer   ; es:di is destination of memory move
   lds si,buffer         ; ds:si is source of memory move
   cld                   ; make sure to move in the right direction
   rep movsw             ; move all the words
   pop ds                ; restore the data segment
   } // end asm

} // end Show_Double_Buffer

//////////////////////////////////////////////////////////////////////////////

int Create_Double_Buffer(int num_lines)
{

// allocate enough memory to hold the double buffer

if ((double_buffer = (unsigned char far *)_fmalloc(SCREEN_WIDTH * (num_lines + 1)))==NULL)
   return(0);

// set the height of the buffer and compute it's size

buffer_height = num_lines;

buffer_size = SCREEN_WIDTH * num_lines/2;

// fill the buffer with black

_fmemset(double_buffer, 0, SCREEN_WIDTH * num_lines);

// everything was ok

return(1);

} // end Init_Double_Buffer

///////////////////////////////////////////////////////////////////////////////

void Fill_Double_Buffer(int color)
{
// this function fills in the double buffer with the sent color

_fmemset(double_buffer, color, SCREEN_WIDTH * buffer_height);

} // end Fill_Double_Buffer

//////////////////////////////////////////////////////////////////////////////

void Delete_Double_Buffer(void)
{
// this function free's up the memory allocated by the double buffer
// make sure to use FAR version

if (double_buffer)
  _ffree(double_buffer);

} // end Delete_Double_Buffer

//////////////////////////////////////////////////////////////////////////////

void Plot_Pixel_Fast_DB(int x,int y,unsigned char color)
{

// plots the pixel in the desired color a little quicker using binary shifting
// to accomplish the multiplications

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

double_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast_DB

//////////////////////////////////////////////////////////////////////////////

void Scale_Sprite(sprite_ptr sprite,float scale)
{

// this function scales a sprite by computing the number of source pixels
// needed to satisfy the number of destination pixels
// note: this function works in the double buffer

char far *work_sprite;

int work_offset=0,
    offset,
    x,
    y;

unsigned char data;

float y_scale_index,
      x_scale_step,
      y_scale_step,
      x_scale_index;

// set first source pixel

y_scale_index = 0;

// compute floating point step

y_scale_step = sprite_height/scale;
x_scale_step = sprite_width/scale;

// alias a pointer to sprite for ease of access

work_sprite = sprite->frames[sprite->curr_frame];

// compute offset of sprite in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

// row by row scale object

for (y=0; y<(int)(scale); y++)
    {
    // copy the next row into the screen buffer using memcpy for speed

    x_scale_index=0;

    for (x=0; x<(int)scale; x++)
        {

        // test for transparent pixel i.e. 0, if not transparent then draw

        if ((data=work_sprite[work_offset+(int)x_scale_index]))
             double_buffer[offset+x] = data;

        x_scale_index+=(x_scale_step);

        } // end for x

    // using the floating scale_step, index to next source pixel

    y_scale_index+=y_scale_step;

    // move to next line in video buffer and in sprite bitmap buffer

    offset      += SCREEN_WIDTH;
    work_offset = sprite_width*(int)(y_scale_index);

    } // end for y

} // end Scale_Sprite

////////////////////////////////////////////////////////////////////////////////

void Behind_Sprite_DB(sprite_ptr sprite)
{

// this function scans the background behind a sprite so that when the sprite
// is draw, the background isnn'y obliterated

char far *work_back;
int work_offset=0,offset,y;

// alias a pointer to sprite background for ease of access

work_back = sprite->background;

// compute offset of background in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<sprite_height; y++)
    {
    // copy the next row out off screen buffer into sprite background buffer

    _fmemcpy((char far *)&work_back[work_offset],
             (char far *)&double_buffer[offset],
             sprite_width);

    // move to next line in double buffer and in sprite background buffer

    offset      += SCREEN_WIDTH;
    work_offset += sprite_width;

    } // end for y

} // end Behind_Sprite_DB

//////////////////////////////////////////////////////////////////////////////

void Erase_Sprite_DB(sprite_ptr sprite)
{
// replace the background that was behind the sprite

// this function replaces the background that was saved from where a sprite
// was going to be placed

char far *work_back;
int work_offset=0,offset,y;

// alias a pointer to sprite background for ease of access

work_back = sprite->background;

// compute offset of background in double buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<sprite_height; y++)
    {
    // copy the next row out off screen buffer into sprite background buffer

    _fmemcpy((char far *)&double_buffer[offset],
             (char far *)&work_back[work_offset],
             sprite_width);

    // move to next line in video buffer and in sprite background buffer

    offset      += SCREEN_WIDTH;
    work_offset += sprite_width;

    } // end for y


} // end Erase_Sprite_DB

//////////////////////////////////////////////////////////////////////////////

void Draw_Sprite_DB(sprite_ptr sprite)
{

// this function draws a sprite on the screen row by row very quickly
// note the use of shifting to implement multplication

char far *work_sprite;
int work_offset=0,offset,x,y;
unsigned char data;

// alias a pointer to sprite for ease of access

work_sprite = sprite->frames[sprite->curr_frame];

// compute offset of sprite in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<sprite_height; y++)
    {
    // copy the next row into the double buffer using memcpy for speed

    for (x=0; x<sprite_width; x++)
        {

        // test for transparent pixel i.e. 0, if not transparent then draw

        if ((data=work_sprite[work_offset+x]))
             double_buffer[offset+x] = data;

        } // end for x

    // move to next line in double buffer and in sprite bitmap buffer

    offset      += SCREEN_WIDTH;
    work_offset += sprite_width;

    } // end for y

} // end Draw_Sprite_DB

///////////////////////////////////////////////////////////////////////////////

void Fade_Lights(void)
{
// this functions fades the lights by slowly decreasing the color values
// in all color registers

int pal_reg,index;
RGB_color color;

for (index=0; index<30; index++)
    {

    for (pal_reg=1; pal_reg<255; pal_reg++)
        {
        // get the color to fade

        Get_Palette_Register(pal_reg,(RGB_color_ptr)&color);

        if (color.red   > 5) color.red-=3;
        else
           color.red = 0;

        if (color.green > 5) color.green-=3;
        else
           color.green = 0;
        if (color.blue  > 5) color.blue-=3;
        else
           color.blue = 0;

        // set the color to a diminished intensity

        Set_Palette_Register(pal_reg,(RGB_color_ptr)&color);

        } // end for pal_reg

    // wait a bit

    Delay(2);

    } // end fade for

} // end Fade_Lights

//////////////////////////////////////////////////////////////////////////////

void Disolve(void)
{
// disolve screen by ploting zillions of black pixels

unsigned long index;

for (index=0; index<=300000; index++,Plot_Pixel_Fast(rand()%320, rand()%200, 0));

} // end Disolve

//////////////////////////////////////////////////////////////////////////////

void Melt(void)
{

// this function "melts" the screen by moving little worms at different speeds
// down the screen.  These worms change to the color thy are eating

int index,ticks=0;

worm worms[NUM_WORMS]; // the array of worms used to make the screen melt

// initialize the worms

for (index=0; index<160; index++)
    {

    worms[index].color   = Get_Pixel(index,0);
    worms[index].speed   = 3 + rand()%9;
    worms[index].y       = 0;
    worms[index].counter = 0;

    // draw the worm

    Plot_Pixel_Fast((index<<1),0,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1),1,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1),2,(char)worms[index].color);


    Plot_Pixel_Fast((index<<1)+1,0,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1)+1,1,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1)+1,2,(char)worms[index].color);

    } // end index

// do screen melt

while(++ticks<1800)
     {

     // process each worm

     for (index=0; index<320; index++)
         {
         // is it time to move worm

         if (++worms[index].counter == worms[index].speed)
            {
            // reset counter

            worms[index].counter = 0;

            worms[index].color = Get_Pixel(index,worms[index].y+4);

            // has worm hit bottom?

            if (worms[index].y < 193)
               {

               Plot_Pixel_Fast((index<<1),worms[index].y,0);
               Plot_Pixel_Fast((index<<1),worms[index].y+1,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1),worms[index].y+2,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1),worms[index].y+3,(char)worms[index].color);

               Plot_Pixel_Fast((index<<1)+1,worms[index].y,0);
               Plot_Pixel_Fast((index<<1)+1,worms[index].y+1,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1)+1,worms[index].y+2,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1)+1,worms[index].y+3,(char)worms[index].color);

               worms[index].y++;

               } // end if worm isn't at bottom yet

            } // end if time to move worm

         } // end index

     // accelerate melt

     if (!(ticks % 500))
        {

        for (index=0; index<160; index++)
            worms[index].speed--;

        } // end if time to accelerate melt

     } // end while

} // end Melt

//////////////////////////////////////////////////////////////////////////////

void Sheer(void)
{
// this program "sheers" the screen for lack of a better word.

long index;
int x,y;

// select starting point of sheers

x=rand()%320;
y=rand()%200;

// do it a few times to make sure whole screen is destroyed

for (index=0; index<100000; index++)
    {

    // move sheers

    x+=17; // note the use of prime numbers
    y+=13;

    // test if sheers are of boundaries, if so roll them over

    if (x>319)
       x = x - 319;

    if (y>199)
       y = y - 199;

    // plot the pixel in black

    Plot_Pixel_Fast(x,y,0);

    } // end for index

} // end Sheer

//////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

void Blit_Char_DB(int xc,int yc,char c,int color,int trans_flag)
{
// this function uses the rom 8x8 character set to blit a character into the
// double buffer,notice the trick used to extract bits out of each character
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
             double_buffer[offset+x] = color;

        else if (!trans_flag)  // takes care of transparency
            double_buffer[offset+x] = 0;

        // shift bit mask

        bit_mask = (bit_mask>>1);

        } // end for x

    // move to next line in video buffer and in rom character data area

    offset      += SCREEN_WIDTH;
    work_char++;

    } // end for y

} // end Blit_Char_DB

//////////////////////////////////////////////////////////////////////////////

void Blit_String_DB(int x,int y,int color, char *string,int trans_flag)
{
// this function blits an entire string into the double buffer with fixed
// spacing between each character.  it calls blit_char.

int index;

for (index=0; string[index]!=0; index++)
     {

     Blit_Char_DB(x+(index<<3),y,string[index],color,trans_flag);

     } /* end while */

} /* end Blit_String_DB */

///////////////////////////////////////////////////////////////////////////////
