
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


// G L O B A L S /////////////////////////////////////////////////////////////

unsigned char far *double_buffer = NULL;

unsigned int buffer_height        = SCREEN_HEIGHT;

unsigned int buffer_size = SCREEN_WIDTH*SCREEN_HEIGHT/2;

sprite object;

pcx_picture imagery_pcx;

// F U N C T I O N S //////////////////////////////////////////////////////////

void Show_Double_Buffer(unsigned char far *buffer)
{
// this functions copies the double buffer into the video buffer

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

///////////////////////////////////////////////////////////////////////////////

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

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{
int done=0;        // exit flag
char buffer[128];  // used to build up info string

float scale=32; // initial scale of object

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// set sprite system size so that functions use correct sprite size

sprite_width = sprite_height = 32;

// initialize the pcx file that holds all the animation cells for net-tank

PCX_Init((pcx_picture_ptr)&imagery_pcx);

// load the pcx file that holds the cells

PCX_Load("robopunk.pcx", (pcx_picture_ptr)&imagery_pcx,1);

Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);

// grap the bitmap

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&object,0,3,0);

// create a double buffer

if (!Create_Double_Buffer(SCREEN_HEIGHT))
   {
   printf("\nNot enough memory to create double buffer.");

   } // end if

// initialize the sprite

object.curr_frame = 0;
object.x          = 160-(sprite_width>>1);
object.y          = 100-(sprite_height>>1);

// clear the double buffer

Fill_Double_Buffer(0);

// show the user the scaled texture

Scale_Sprite((sprite_ptr)&object,scale);

Show_Double_Buffer(double_buffer);

Blit_String(0,190,10,"Press 'Q' to Quit,'<' '>' to Scale.",1);

// main loop

while(!done)
     {

     // has user hit a key?

     if (kbhit())
        {
        switch(getch())
              {
              case '.': // scale object larger
                   {
                   if (scale<180)
                      {
                      scale+=4;
                      object.x-=2;
                      object.y-=2;
                      } // end if ok to scale larger

                   } break;

              case ',': // scale object smaller
                   {
                   if (scale>4)
                      {
                      scale-=4;
                      object.x+=2;
                      object.y+=2;
                      } // end if ok to scale smaller

                   } break;

              case 'q': // let's go!
                      {
                      done=1;
                      } break;

              default:break;

              } // end switch


        // create a clean slate

        Fill_Double_Buffer(0);

        // scale the sprite and render into the double buffer

        Scale_Sprite((sprite_ptr)&object,scale);

        // show the double buffer

        Show_Double_Buffer(double_buffer);

        Blit_String(0,190,10,"Press 'Q' to Quit,'<' '>' to Scale.",1);

        sprintf(buffer,"Current scale = %f  ",scale/32);

        Blit_String(0,8,10,buffer,1);


        }// end if

     } // end while

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

// go back to text mode

Set_Video_Mode(TEXT_MODE);

// delete the doubele buffer

Delete_Double_Buffer();


} // end main



