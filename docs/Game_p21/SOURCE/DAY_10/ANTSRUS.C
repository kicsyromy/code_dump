
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
#include "graph6.h"

// D E F I N E S /////////////////////////////////////////////////////////////

#define CELL_WIDTH       20   // size of bitmaps in world
#define CELL_HEIGHT      20

#define NUM_ROWS         10   // number of rows and columns in terrain
#define NUM_COLUMNS      16

#define NUM_ANTS         20   // this should be enough (has to be multiple of 2)

#define NUM_PATTERNS      3   // there are 3 patterns
#define PATTERN_LENGTH   25   // at most patterns have 25 elements in them

// direction of ant

#define ANT_UP      0
#define ANT_DOWN    2
#define ANT_RIGHT   4
#define ANT_LEFT    6

// state of ants

#define ANT_MARCH   0
#define ANT_RANDOM  1
#define ANT_PATTERN 2
#define ANT_SITTING 3

// S T R U C T U R E S ///////////////////////////////////////////////////////

typedef struct ant_typ
        {
        int dir;      // direction of ant
        int state;    // state of ant
        int count_1;  // counter one
        int count_2;  // counter two
        int max_1;    // maximum count for counter 1
        int max_2;    // maximum count for counter 2
        int index_1;  // general index
        int index_2;  // general index

        } ant, *ant_ptr;

// G L O B A L S  ////////////////////////////////////////////////////////////

pcx_picture imagery_pcx,      // the game imagery
            background_pcx;   // the backdrop

// the sprites used in the game

sprite ants[NUM_ANTS],        // the ant sprites
       rock;                  // the "rock" sprite

ant ant_data[NUM_ANTS];       // the ant data structures


// this array is a probability density of the different states

int ant_personality[10] = {
                          ANT_MARCH,   // 60% of the time march up and down
                          ANT_MARCH,
                          ANT_MARCH,
                          ANT_MARCH,
                          ANT_MARCH,
                          ANT_MARCH,
                          ANT_RANDOM,  // 10% of the time random
                          ANT_PATTERN, // 20% of the time try a pattern
                          ANT_PATTERN,
                          ANT_SITTING  // 10% of the time ant will sit
                          };

int terrain[NUM_ROWS][NUM_COLUMNS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                      1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
                                      0,0,0,1,0,0,0,0,0,2,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                      0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,
                                      0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

int ant_patterns[NUM_PATTERNS][PATTERN_LENGTH]=

{ 0,0,0,4,4,4,2,2,2,6,6,6,2,2,2,0,0,4,2,6,0,-1,0,0,0,
  2,2,2,6,6,0,0,4,4,4,4,4,2,6,0,6,2,2,4,0,6,2,4,-1,0,
  4,4,4,4,4,0,6,6,6,6,6,6,6,2,4,0,6,2,2,2,6,0,2,4,-1};

// F U N C T I O N S //////////////////////////////////////////////////////////

void Draw_Ants(void)
{
// this function draws all the ants

int index;

for (index=0; index<NUM_ANTS; index++)
    Draw_Sprite_DB((sprite_ptr)&ants[index]);

} // end Draw_Ants

///////////////////////////////////////////////////////////////////////////////

void Erase_Ants(void)
{
// this function erases all the ants

int index;

// loop through and process all ants

for (index=0; index<NUM_ANTS; index++)
    Erase_Sprite_DB((sprite_ptr)&ants[index]);

} // end Erase_Ants

///////////////////////////////////////////////////////////////////////////////

void Behind_Ants(void)
{
// this function scans behind all the ants

int index;

// loop through and process all ants

for (index=0; index<NUM_ANTS; index++)
    Behind_Sprite_DB((sprite_ptr)&ants[index]);

} // end Behind_Ants

///////////////////////////////////////////////////////////////////////////////

void Init_Ants(void)
{
// this function initializes all the ants and places them in two
// columns on the screen and sets them all to march

int index;

// loop through and process all ants

// first the up ants

for (index=0; index<(NUM_ANTS/2); index++)
    {

    // set up fields in data structure

    ant_data[index].dir      = ANT_UP;
    ant_data[index].state    = ANT_MARCH;
    ant_data[index].count_1  = 0;
    ant_data[index].count_2  = 0;
    ant_data[index].max_1    = 100 + rand()%100;
    ant_data[index].max_2    = 0;
    ant_data[index].index_1  = 0;
    ant_data[index].index_2  = 0;

    // set up fields in sprite structure

    ants[index].curr_frame   = ANT_UP;
    ants[index].x            = 165 + rand()%10;
    ants[index].y            = index*CELL_HEIGHT;

    } // end for index

// now the down

for (index=(NUM_ANTS/2); index<NUM_ANTS; index++)
    {

    // set up fields in data structure

    ant_data[index].dir      = ANT_DOWN;
    ant_data[index].state    = ANT_MARCH;
    ant_data[index].count_1  = 0;
    ant_data[index].count_2  = 0;
    ant_data[index].max_1    = 100 + rand()%100;
    ant_data[index].max_2    = 0;
    ant_data[index].index_1  = 0;
    ant_data[index].index_2  = 0;

    // set up fields in sprite structure

    ants[index].curr_frame   = ANT_DOWN;
    ants[index].x            = 180 + rand()%10;
    ants[index].y            = (index-(NUM_ANTS/2))*CELL_HEIGHT;

    } // end for index


} // end Init_Ants

/////////////////////////////////////////////////////////////////////////////

void Draw_Rocks(void)
{
// based on terrain array place a rock on screen wherever there is a "1"

int x,y;

// loop through draw and draw rocks

for (y=0; y<NUM_ROWS; y++)
    {

    for (x=0; x<NUM_COLUMNS; x++)
        {

        // is there a rock here

        if (terrain[y][x]==1)
           {
           // postion rock sprite

           rock.x = x*CELL_WIDTH;
           rock.y = y*CELL_HEIGHT;

           Draw_Sprite_DB((sprite_ptr)&rock);

           } // end if

        } // end for x

    } // end for y

} // end Draw_Rocks

// M A I N ////////////////////////////////////////////////////////////////////

void main(void)
{
// this is the main function

int done=0,         // exit flag for whole system
    index,          // loop index
    change_state,   // used to flag when a ant should change state
    new_dir,        // local AI variable
    cell_x,cell_y;  // used to compute what cell ant is in for rock collision


// SECTION 1 /////////////////////////////////////////////////////////////////

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// create a double buffer

if (!Create_Double_Buffer(SCREEN_HEIGHT))
   {
   printf("\nNot enough memory to create double buffer.");

   } // end if

// clear the double buffer

Fill_Double_Buffer(0);

// load in the background image into the double buffer

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load("antbak.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

Blit_String_DB(2,2,10,"Press any key to exit.",1);

// SECTION 2 /////////////////////////////////////////////////////////////////

// load in imagery for the ants

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("antimg.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize player and extract bitmaps

sprite_width  = 20;
sprite_height = 20;

// create all the ants...we have a long way to create the Genesis bomb.
// but I'm on the job!

for (index=0; index<NUM_ANTS; index++)
    {
    // initialize the ant sprite

    Sprite_Init((sprite_ptr)&ants[index],0,0,0,0,0,0);

    // load the bitmaps

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&ants[index],0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&ants[index],1,1,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&ants[index],2,2,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&ants[index],3,3,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&ants[index],4,4,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&ants[index],5,5,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&ants[index],6,6,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&ants[index],7,7,0);

    // initialize the ant vars

    ants[index].curr_frame = 0;
    ants[index].state      = 1;

    } // end for index

// load up the rock

Sprite_Init((sprite_ptr)&rock,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                (sprite_ptr)&rock,0,0,1);

rock.curr_frame = 0;
rock.state      = 1;  // alive or dead, it's not doing much!


// SECTION 3 /////////////////////////////////////////////////////////////////

// draw all the rocks

Draw_Rocks();

// initialize the ants

Init_Ants();

// scan background once before loop

Behind_Ants();

// SECTION 4 /////////////////////////////////////////////////////////////////

// main event loop

while(!kbhit())
     {

     // erase all objects

     Erase_Ants();

// SECTION 5 /////////////////////////////////////////////////////////////////

     // BEGIN AI LOGIC  //

     // at this point we want to move all the ants, change stats, etc.

     for (index=0; index<NUM_ANTS; index++)
         {

         // reset state change flag

         change_state = 0;

         // what state is ant in ?

         switch(ant_data[index].state)
               {

               case ANT_MARCH:
                  {

                  // test if it's time to change state

                  if (++ant_data[index].count_1 == ant_data[index].max_1)
                     change_state = 1;

                  } break;

               case ANT_RANDOM:
                  {
                  // select a new direction for ant

                  ant_data[index].dir      = 2*(rand()%4);
                  ant_data[index].state    = ANT_MARCH;
                  ant_data[index].count_1  = 0;
                  ant_data[index].max_1    = 50 + rand()%50;

                  // set up fields in sprite structure

                  ants[index].curr_frame   = ant_data[index].dir;

                  } break;

               case ANT_PATTERN:
                  {
                  // test if it's time to use next pattern element

                  if (++ant_data[index].count_1 == ant_data[index].max_1)
                     {
                     // reset counter

                     ant_data[index].count_1 = 0;

                     // move to next element of pattern

                     ant_data[index].index_2++;

                     new_dir =
                     ant_patterns[ant_data[index].index_1][ant_data[index].index_2];

                     if (new_dir!=-1)
                        {

                        // change direction of ant

                        ant_data[index].dir =  ants[index].curr_frame = new_dir;

                        } // end if not done with pattern
                     else
                        {

                        change_state = 1;

                        } // end else pattern is dead

                     } // end if time to change pattern element

                  } break;

               case ANT_SITTING:
                  {

                  // test if it's time to change state

                  if (++ant_data[index].count_1 == ant_data[index].max_1)
                     change_state = 1;

                  } break;

               default:break;

               } // end switch

// SECTION 6 /////////////////////////////////////////////////////////////////


         // check if there has been a state change

         if (change_state)
            {

            // use personality table to select a new state

            ant_data[index].state = ant_personality[rand()%10];

            // based on new state set up ant appropriately(if needed)

            switch(ant_data[index].state)
                  {

                  case ANT_MARCH:
                     {
                     // select up or down

                     ant_data[index].dir      = 2*(rand()%2);
                     ant_data[index].state    = ANT_MARCH;
                     ant_data[index].count_1  = 0;
                     ant_data[index].max_1    = 100 + rand()%75;

                     // set up current frame

                     ants[index].curr_frame   = ant_data[index].dir;

                     } break;

                  case ANT_PATTERN:
                     {

                     // select the pattern and set a pointer to first element

                     ant_data[index].index_1 = rand()%NUM_PATTERNS;
                     ant_data[index].index_2 = 0;

                     // this time, these two variables will be used to count
                     // how long to play each pattern instruction

                     ant_data[index].count_1  = 0;
                     ant_data[index].max_1    = 2 + rand()%3;

                     // based on the first pattern element set initial direction

                     ant_data[index].dir =
                           ant_patterns[ant_data[index].index_1][0];

                     ants[index].curr_frame = ant_data[index].dir;

                     } break;

                  case ANT_RANDOM:
                     {

                     ant_data[index].state    = ANT_RANDOM;

                     // do nothing, the logic will take care of it above

                     } // break;

                  case ANT_SITTING:
                     {

                     ant_data[index].state    = ANT_SITTING;
                     ant_data[index].count_1  = 0;
                     ant_data[index].max_1    = 10 + rand()%10;

                     } // break;

                  } // end switch state;

            } // end if we need to move to another state

// SECTION 7 /////////////////////////////////////////////////////////////////


         // check if ant has bumped into a rock, if so set state to
         // random

         // obtain cell location of ant using center of ant as reference

         cell_x = (ants[index].x+10)/CELL_WIDTH;
         cell_y = (ants[index].y+10)/CELL_HEIGHT;

         // test if there is a rock there

         if (terrain[cell_y][cell_x]==1)
            {
            // set state of ant to random

            ant_data[index].dir      = 2*(rand()%4);
            ant_data[index].state    = ANT_MARCH;
            ant_data[index].count_1  = 0;
            ant_data[index].max_1    = 50 + rand()%50;
            ants[index].curr_frame   = ant_data[index].dir;

            change_state = 0;

            } // end if ant hit a rock

// SECTION 8 /////////////////////////////////////////////////////////////////

         // now matter what state ant is in we should move it in the direction
         // it is pointing, unless it is sitting

         // don't move ant if it is sitting

         if (ant_data[index].state!=ANT_SITTING)
         {
         // based on direction move the ant

         switch(ant_data[index].dir)
               {

               case ANT_RIGHT:
                  {
                  ants[index].x+=4;
                  } break;

               case ANT_LEFT:
                  {
                  ants[index].x-=4;
                  } break;

               case ANT_UP:
                  {
                  ants[index].y-=4;
                  } break;

               case ANT_DOWN:
                  {
                  ants[index].y+=4;
                  } break;

               default:break;

               } // end switch direction

// SECTION 9 /////////////////////////////////////////////////////////////////


        // change animation frame using a toggle

        if ((ants[index].curr_frame % 2)==0)
            ants[index].curr_frame++;
        else
            ants[index].curr_frame--;

        // boundary detection

        if (ants[index].x > 300)
            ants[index].x = 0;
        else
        if (ants[index].x < 0)
            ants[index].x = 300;

        if (ants[index].y > 180)
            ants[index].y = 0;
        else
        if (ants[index].y < 0)
            ants[index].y = 180;

        } // end if ant wasn't sitting

        } // end for index

     // END AI LOGIC    //

// SECTION 10 ///////////////////////////////////////////////////////////////

     // scan background under objects

     Behind_Ants();

     // draw all the imagery

     Draw_Ants();

     // copy the double buffer to the screen

     Show_Double_Buffer(double_buffer);

     // wait a sec

     Delay(1);

     } // end while

// SECTION 11 ///////////////////////////////////////////////////////////////

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

} // end main


