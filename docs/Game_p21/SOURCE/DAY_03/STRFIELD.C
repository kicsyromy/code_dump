
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

// D E F I N E S /////////////////////////////////////////////////////////////

#define NUM_STARS  75

#define PLANE_1    1
#define PLANE_2    2
#define PLANE_3    3

#define VGA256     0x13
#define TEXT_MODE  0x03

#define ROM_CHAR_SET_SEG 0xF000  // segment of 8x8 ROM character set
#define ROM_CHAR_SET_OFF 0xFA6E  // begining offset of 8x8 ROM character set

#define CHAR_WIDTH        8      // size of characters
#define CHAR_HEIGHT       8

#define SCREEN_WIDTH      (unsigned int)320 // mode 13h screen dimensions
#define SCREEN_HEIGHT     (unsigned int)200

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Blit_Char(int xc,int yc,char c,int color,int trans_flag);

void Blit_String(int x,int y,int color, char *string,int trans_flag);

void Plot_Pixel_Fast(int x,int y,unsigned char color);

void Init_Stars(void);

void Set_Video_Mode(int mode);

void Delay(int clicks);

// S T R U C T U R E S ///////////////////////////////////////////////////////

// data structure for a single star

typedef struct star_typ
        {
        int x,y;    // position of star
        int plane;  // which plane is star in
        int color;  // color of star

        } star, *star_ptr;


// G L O B A L S /////////////////////////////////////////////////////////////

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr

unsigned char far *rom_char_set = (char far *)0xF000FA6EL; // rom characters 8x8

int star_first=1;  // flags first time into star field

star stars[NUM_STARS]; // the star field

int velocity_1=2,      // the speeds of each plane
    velocity_2=4,
    velocity_3=6;


// F U N C T I O N S ////////////////////////////////////////////////////////

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

void Plot_Pixel_Fast(int x,int y,unsigned char color)
{

// plots the pixel in the desired color a little quicker using binary shifting
// to accomplish the multiplications

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

video_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast

//////////////////////////////////////////////////////////////////////////////

void Init_Stars(void)
{

// this function will initialize the star field

int index;

// for each star choose a position, plane and color

for (index=0; index<NUM_STARS; index++)
    {
    // initialize each star to a velocity, position and color

    stars[index].x     = rand()%320;
    stars[index].y     = rand()%180;

    // decide what star plane the star is in

    switch(rand()%3)
          {
          case 0: // plane 1- the farthest star plane
               {
               // set velocity and color

               stars[index].plane = 1;
               stars[index].color = 8;

               } break;

          case 1: // plane 2-The medium distance star plane
               {

               stars[index].plane = 2;
               stars[index].color = 7;

               } break;

          case 2: // plane 3-The nearest star plane
               {

               stars[index].plane = 3;
               stars[index].color = 15;

               } break;

          } // end switch

    } // end for index

} // end Init_Stars

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

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

int done=0, // exit flag
    index;  // loop index

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// initialize the star field data structure

Init_Stars();

// begin main event loop

while(!done)
     {

     // test if user is trying to do something

     if (kbhit())
        {
        // what key was pressed ?

        switch(getch())
              {
              case '-': // slow down star field
                   {
                   // decrease velocity of each plane

                   velocity_1-=1;
                   velocity_2-=2;
                   velocity_3-=3;

                   } break;

              case '=': // speed up star field
                   {
                   // increase velocity of each plane
                   velocity_1+=1;
                   velocity_2+=2;
                   velocity_3+=3;

                   } break;

              case 'q': // user is exiting
                   {
                   done=1;
                   } break;

              default:break;

              } // end switch

        } // end if kbhit

     // move the star fields

     for (index=0; index<NUM_STARS; index++)
         {

         // erase the star

         Plot_Pixel_Fast(stars[index].x,stars[index].y,0);

         // move the star and test for off screen condition

         // each star is in a different plane so test which plane star is
         // in so that proper velocity may be used

         switch(stars[index].plane)
               {
               case PLANE_1: // the slowest plane
                    {
                    stars[index].x+=velocity_1;
                    } break;

               case PLANE_2: // the medium speed plane
                    {
                    stars[index].x+=velocity_2;
                    } break;

               case PLANE_3: // the fastest plane (near)
                    {
                    stars[index].x+=velocity_3;
                    } break;

                } // end switch

         // test if star went off screen

         if (stars[index].x > 319 ) // off right edge?
            stars[index].x=(stars[index].x-320); // wrap around
         else
         if (stars[index].x < 0) // off left edge?
             stars[index].x = (320+stars[index].x); // wrap around

         // draw the star at new position

         Plot_Pixel_Fast(stars[index].x,stars[index].y,stars[index].color);

         } // end for

     // draw the directions again

     Blit_String(0,0,1, "Press '+' or '-' to change speed.",1);

     Blit_String(88,180,2, "Press 'Q' to exit.",1);

     // wait a second so we can see the stars, otherwise it will look like
     // warp speed!

     Delay(1);

     } // end while

// reset back set video mode to 320x200 256 color mode

Set_Video_Mode(TEXT_MODE);

} // end main



