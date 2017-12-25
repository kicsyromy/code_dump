
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

#include "graph3.h" // load in module 3's header
#include "graph4.h" // load in this modules header

// G L O B A L S  ////////////////////////////////////////////////////////////

unsigned int sprite_width  = SPRITE_WIDTH; // default height and width of sprite
unsigned int sprite_height = SPRITE_HEIGHT;

// F U N C T I O N S /////////////////////////////////////////////////////////

unsigned char Get_Pixel(int x,int y)
{

// gets the color value of pixel at (x,y) from the screen and returns it

return video_buffer[((y<<8) + (y<<6)) + x];

} // end Get_Pixel

//////////////////////////////////////////////////////////////////////////////

void PCX_Init(pcx_picture_ptr image)
{
// this function allocates the buffer region needed to load a pcx file

if (!(image->buffer = (char far *)_fmalloc(SCREEN_WIDTH * SCREEN_HEIGHT + 1)))

   printf("\ncouldn't allocate screen buffer");

} // end PCX_Init

//////////////////////////////////////////////////////////////////////////////

void PCX_Load(char *filename, pcx_picture_ptr image,int enable_palette)
{
// this function loads a pcx file into a picture structure, the actual image
// data for the pcx file is decompressed and expanded into a secondary buffer
// within the picture structure, the separate images can be grabbed from this
// buffer later.  also the header and palette are loaded

FILE *fp;
int num_bytes,index;
long count;
unsigned char data;
char far *temp_buffer;

// open the file

fp = fopen(filename,"rb");

// load the header

temp_buffer = (char far *)image;

for (index=0; index<128; index++)
    {
    temp_buffer[index] = (char)getc(fp);
    } // end for index

// load the data and decompress into buffer

count=0;

while(count<=SCREEN_WIDTH * SCREEN_HEIGHT)
     {
     // get the first piece of data

     data = (unsigned char)getc(fp);

     // is this a rle?

     if (data>=192 && data<=255)
        {
        // how many bytes in run?

        num_bytes = data-192;

        // get the actual data for the run

        data  = (unsigned char)getc(fp);

        // replicate data in buffer num_bytes times

        while(num_bytes-->0)
             {
             image->buffer[count++] = data;

             } // end while

        } // end if rle
     else
        {
        // actual data, just copy it into buffer at next location

        image->buffer[count++] = data;

        } // end else not rle

     } // end while

// move to end of file then back up 768 bytes i.e. to begining of palette

fseek(fp,-768L,SEEK_END);

// load the pallete into the palette

for (index=0; index<256; index++)
    {
    // get the red component

    image->palette[index].red   = (unsigned char)(getc(fp) >> 2);

    // get the green component

    image->palette[index].green = (unsigned char)(getc(fp) >> 2);

    // get the blue component

    image->palette[index].blue  = (unsigned char)(getc(fp) >> 2);

    } // end for index

fclose(fp);

// change the palette to newly loaded palette if commanded to do so

if (enable_palette)
   {

   // for each palette register set to the new color values

   for (index=0; index<256; index++)
       {

       Set_Palette_Register(index,(RGB_color_ptr)&image->palette[index]);

       } // end for index

   } // end if change palette

} // end PCX_Load

//////////////////////////////////////////////////////////////////////////////

void PCX_Delete(pcx_picture_ptr image)
{
// this function de-allocates the buffer region used for the pcx file load

_ffree(image->buffer);

} // end PCX_Delete

//////////////////////////////////////////////////////////////////////////////

void PCX_Show_Buffer(pcx_picture_ptr image)
{
// just copy he pcx buffer into the video buffer

char far *data;

data = image->buffer;

_asm
   {
   push ds               ; save the data segment
   les di, video_buffer  ; point es:di to video buffer
   lds si, data          ; point ds:si to data area
   mov cx,320*200/2      ; move 32000 words
   cld                   ; set direction to foward
   rep movsw             ; do the string operation
   pop ds                ; restore the data segment
   }

} // end PCX_Show_Picture

//////////////////////////////////////////////////////////////////////////////

void Sprite_Init(sprite_ptr sprite,int x,int y,int ac,int as,int mc,int ms)
{
// this function initializes a sprite with the sent data

int index;

sprite->x            = x;
sprite->y            = y;
sprite->x_old        = x;
sprite->y_old        = y;
sprite->width        = sprite_width;
sprite->height       = sprite_height;
sprite->anim_clock   = ac;
sprite->anim_speed   = as;
sprite->motion_clock = mc;
sprite->motion_speed = ms;
sprite->curr_frame   = 0;
sprite->state        = SPRITE_DEAD;
sprite->num_frames   = 0;
sprite->background   = (char far *)_fmalloc(sprite_width * sprite_height+1);

// set all bitmap pointers to null

for (index=0; index<MAX_SPRITE_FRAMES; index++)
    sprite->frames[index] = NULL;

} // end Sprite_Init

//////////////////////////////////////////////////////////////////////////////

void Sprite_Delete(sprite_ptr sprite)
{
// this function deletes all the memory associated with a sprire

int index;

_ffree(sprite->background);

// now de-allocate all the animation frames

for (index=0; index<MAX_SPRITE_FRAMES; index++)
    _ffree(sprite->frames[index]);

} // end Sprite_Delete


//////////////////////////////////////////////////////////////////////////////

void PCX_Grab_Bitmap(pcx_picture_ptr image,
                     sprite_ptr sprite,
                     int sprite_frame,
                     int grab_x, int grab_y)

{
// this function will grap a bitmap from the pcx frame buffer. it uses the
// convention that the 320x200 pixel matrix is sub divided into a smaller
// matrix of nxn adjacent squares

int x_off,y_off, x,y;
char far *sprite_data;

// first allocate the memory for the sprite in the sprite structure

sprite->frames[sprite_frame] = (char far *)_fmalloc(sprite_width * sprite_height + 1);

// create an alias to the sprite frame for ease of access

sprite_data = sprite->frames[sprite_frame];

// now load the sprite data into the sprite frame array from the pcx picture

x_off = (sprite_width+1)  * grab_x + 1;
y_off = (sprite_height+1) * grab_y + 1;

// compute starting y address

y_off = y_off * 320;

for (y=0; y<(int)sprite_height; y++)
    {

    for (x=0; x<(int)sprite_width; x++)
        {

        // get the next byte of current row and place into next position in
        // sprite frame data buffer

        sprite_data[y*sprite_width + x] = image->buffer[y_off + x_off + x];

        } // end for x

        // move to next line of picture buffer

        y_off+=320;

    } // end for y

// increment number of frames

sprite->num_frames++;

// done!, let's bail!

} // end PCX_Grab_Bitmap

//////////////////////////////////////////////////////////////////////////////

void Behind_Sprite(sprite_ptr sprite)
{

// this function scans the background behind a sprite so that when the sprite
// is draw, the background isnn'y obliterated

char far *work_back;
int work_offset=0,offset,y;

// alias a pointer to sprite background for ease of access

work_back = sprite->background;

// compute offset of background in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<(int)sprite_height; y++)
    {
    // copy the next row out off screen buffer into sprite background buffer

    _fmemcpy((char far *)&work_back[work_offset],
             (char far *)&video_buffer[offset],
             sprite_width);

    // move to next line in video buffer and in sprite background buffer

    offset      += SCREEN_WIDTH;
    work_offset += sprite_width;

    } // end for y

} // end Behind_Sprite

//////////////////////////////////////////////////////////////////////////////

void Erase_Sprite(sprite_ptr sprite)
{
// replace the background that was behind the sprite

// this function replaces the background that was saved from where a sprite
// was going to be placed

char far *work_back;
int work_offset=0,offset,y;

// alias a pointer to sprite background for ease of access

work_back = sprite->background;

// compute offset of background in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<(int)sprite_height; y++)
    {
    // copy the next row out off screen buffer into sprite background buffer

    _fmemcpy((char far *)&video_buffer[offset],
             (char far *)&work_back[work_offset],
             sprite_width);

    // move to next line in video buffer and in sprite background buffer

    offset      += SCREEN_WIDTH;
    work_offset += sprite_width;

    } // end for y


} // end Erase_Sprite

//////////////////////////////////////////////////////////////////////////////

void Draw_Sprite(sprite_ptr sprite)
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

for (y=0; y<(int)sprite_height; y++)
    {
    // copy the next row into the screen buffer using memcpy for speed

    for (x=0; x<(int)sprite_width; x++)
        {

        // test for transparent pixel i.e. 0, if not transparent then draw

        data=work_sprite[work_offset+x];

        if (data)
             video_buffer[offset+x] = data;

        } // end for x

    // move to next line in video buffer and in sprite bitmap buffer

    offset      += SCREEN_WIDTH;
    work_offset += sprite_width;

    } // end for y

} // end Draw_Sprite

///////////////////////////////////////////////////////////////////////////////

int Sprite_Collide(sprite_ptr sprite_1, sprite_ptr sprite_2)
{
// this function tests the bounding boxes of each sprite to see if a
// collision has occured, if so a 1 is returned else a 0 is returned

int dx,dy;

// compute amount of overlap, if any

dx = abs(sprite_1->x - sprite_2->x);
dy = abs(sprite_1->y - sprite_2->y);

// test x and y extents, note how the width and height are decreased by a
// percentage of the actual width and height. this is to make the bounding
// box a little more realistic since very seldomly will an object be
// rectangula, this helps to insure that there is a solid collision

if (dx<(int)(sprite_width-(sprite_width>>3)) && dy<(int)(sprite_height-(sprite_height>>3)))
   {

   return(1);

   } // end if collision occured
else
   {

   return(0);

   } // end else

} // end Sprite_Collide

