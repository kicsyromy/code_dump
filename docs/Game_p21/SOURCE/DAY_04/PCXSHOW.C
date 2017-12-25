
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

#include "graph3.h"  // our graphics functions from day 3

// S T R U C T U R E S ///////////////////////////////////////////////////////

typedef struct pcx_header_typ
        {
        char manufacturer;
        char version;
        char encoding;
        char bits_per_pixel;
        int x,y;
        int width,height;
        int horz_res;
        int vert_res;
        char ega_palette[48];
        char reserved;
        char num_color_planes;
        int bytes_per_line;
        int palette_type;
        char padding[58];

        } pcx_header, *pcx_header_ptr;


typedef struct pcx_picture_typ
        {
        pcx_header header;
        RGB_color palette[256];
        char far *buffer;

        } pcx_picture, *pcx_picture_ptr;

// P R O T O T Y P E S ///////////////////////////////////////////////////////
/*
void PCX_Init(pcx_picture_ptr image);

void PCX_Load(char *filename, pcx_picture_ptr image,int enable_palette);

void PCX_Delete(pcx_picture_ptr image);

void PCX_Show_Buffer(pcx_picture_ptr image);
*/
// F U N C T I O N S /////////////////////////////////////////////////////////

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

} // end PCX_Show_Buffer

// M A I N ///////////////////////////////////////////////////////////////////

void main(int argc, char **argv)
{
long index;                  // loop counter
pcx_picture background_pcx;  // this pcx structure holds background imagery
FILE *fp;                    // used to see if file exists

// make sure there is a file name

if (argc<2)
   {
   printf("\nUsuage: pcxshow filename.PCX");
   return;

   } // end if

// test if the file exist, but not for the .PCX extension

if (!fopen(argv[1],"rb"))
   {
   printf("\nFile:%s - not found!",argv[1]);
   return(1);
   } // end if not found
else
   {
   fclose(fp);
   } // end else found

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// load in background

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load(argv[1], (pcx_picture_ptr)&background_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&background_pcx);

PCX_Delete((pcx_picture_ptr)&background_pcx);

// wait for user to hit keyboard

while(!kbhit()){}

// disolve the screen...in one line I might add!

for (index=0; index<=300000; index++,Plot_Pixel_Fast(rand()%320, rand()%200, 0));

// go back to text mode

Set_Video_Mode(TEXT_MODE);

} // end main
