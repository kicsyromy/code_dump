
// I N C L U D E S ////////////////////////////////////////////////////////////

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

#include "graph3.h"  // include graphics our stuff
#include "graph4.h"
#include "graph5.h"
#include "graph6.h"

// D E F I N E S //////////////////////////////////////////////////////////////

// timer defines

#define CONTROL_8253  0x43  // the 8253's control register
#define CONTROL_WORD  0x3C  // the control word to set mode 2,binary least/most
#define COUNTER_0     0x40  // counter 0
#define COUNTER_1     0x41  // counter 1
#define COUNTER_2     0x42  // counter 2

#define TIMER_60HZ    0x4DAE // 60 hz
#define TIMER_50HZ    0x5D37 // 50 hz
#define TIMER_40HZ    0x7486 // 40 hz
#define TIMER_30HZ    0x965C // 30 hz
#define TIMER_20HZ    0xE90B // 20 hz
#define TIMER_18HZ    0xFFFF // 18.2 hz (the standard count and the slowest possible)

// interrupt table defines

#define TIME_KEEPER_INT     0x1C    // the time keeper interrupt

// multi-tasking kernal defines

#define MAX_TASKS      16  // this should be enough to turn your brains to mush
#define TASK_INACTIVE  0   // this is an inactive task
#define TASK_ACTIVE    1   // this is an active task


// defines for demo tasks

#define NUM_ATOMS 30
#define NUM_STARS 50

// M A C R O S ///////////////////////////////////////////////////////////////

#define LOW_BYTE(n) (n & 0x00ff)       // extracts the low-byte of a word
#define HI_BYTE(n)  ((n>>8) & 0x00ff)  // extracts the hi-byte of a word

// S T U C T U R E S /////////////////////////////////////////////////////////

// this is a single task structure

typedef struct task_typ
        {

        int id;             // the id number for this task
        int state;          // the state of this task
        void (far *task)(); // the function pointer to the task itself

        } task, *task_ptr;

// structures for demo tasks

typedef struct particle_typ
        {
        int x,y;              // position of particle
        int xv,yv;            // velocity of particle
        unsigned char color;  // color of particle
        } particle, *particle_ptr;

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Change_Timer(unsigned int new_count);

// multi-tasking stuff

void Initialize_Kernal(void);

void Start_Kernal(void);

void Stop_Kernal(void);

int Add_Task(void (far *function)());

int Delete_Task(int id);

void _interrupt far Multi_Kernal(void);

// G L O B A L S /////////////////////////////////////////////////////////////

void (_interrupt far *Old_Time_Isr)();  // used to hold old interrupt vector

// multi-tasking stuff

task tasks[MAX_TASKS];             // this is the task list for the system

int num_tasks = 0;                 // tracks number of active tasks

// globals for demo tasks

particle atoms[NUM_ATOMS]; // the balls

particle starfield[NUM_STARS]; // the star field

int star_id, mirror_id, ball_id; // used to hold id's so that tasks can be
                                 // terminated later

// F U N C T I O N S //////////////////////////////////////////////////////////

void Initialize_Kernal(void)
{

// this function will set up the task list and prepare for it to be populated

int index; // loop variable

for (index=0; index<MAX_TASKS; index++)
    {
    // set id to current location in list

    tasks[index].id = index;

    // set to inactive

    tasks[index].state = TASK_INACTIVE;

    // set function pointer to NULL;

    tasks[index].task = NULL;

    } // end for index

} // end Initialize_Kernal

///////////////////////////////////////////////////////////////////////////////

void Start_Kernal(void)
{
// install our time keeper ISR while saving old one

Old_Time_Isr = _dos_getvect(TIME_KEEPER_INT);

_dos_setvect(TIME_KEEPER_INT, Multi_Kernal);

} // end Start_Kernal

///////////////////////////////////////////////////////////////////////////////

void Stop_Kernal(void)
{

// replace old time keeper ISR

_dos_setvect(TIME_KEEPER_INT, Old_Time_Isr);

} // end Stop_Kernal

//////////////////////////////////////////////////////////////////////////////

int Add_Task(void (far *function)())
{
// this function will add the task to the task list and return it's id number
// which can be used to delete it.  If the function returns -1 then the
// task list is full and no more tasks can be added

int index;

for (index=0; index<MAX_TASKS; index++)
    {
    // try and find an inactive task

    if (tasks[index].state == TASK_INACTIVE)
       {
       // load new task into this position

       tasks[index].state = TASK_ACTIVE;
       tasks[index].id    = index;
       tasks[index].task  = function;  // assign function pointer

       // adjust global task monitor

       num_tasks++;

       // return id to caller

       return(tasks[index].id);

       } // end if found an inactive task

    } // end for index

// if we got this far then there are no free spots...bummer

return(-1);

} // end Add_Task

///////////////////////////////////////////////////////////////////////////////

int Delete_Task(int id)
{
// this function will try to delete a task from the task list, if the function
// is successful, it will return 1 else it will return 0.

if (tasks[id].state == TASK_ACTIVE)
   {
   // kill task and return success

   tasks[id].task   = NULL;
   tasks[id].state  = TASK_INACTIVE;

   // decrement number of active tasks

   num_tasks--;

   return(1);

   } // end if task can be deleted
else
   {
   // couldn't delete task
   return(0);

   } // end task already dead

} // end Delete_Task

///////////////////////////////////////////////////////////////////////////////

void _interrupt far Multi_Kernal(void)
{

// this function will call all of the task in a round robin manner such that
// only one task will be called per interrupt. note: ther must be at least
// one active task in the task list

static int current_task=0;  // current_task to be executed by kernal

// test if there are any tasks at all

if (num_tasks>0)
{

// find an active task

while(tasks[current_task].state!=TASK_ACTIVE)
     {
     // move to next task and round robin if at end of task list

     if (++current_task>=MAX_TASKS)
        current_task=0;

     } // end search for active task

// at this point we have an active task so call it

tasks[current_task].task(); // weird looking huh!

// now we need to move to the next possible task

if (++current_task>=MAX_TASKS)
   current_task=0;

} // end if there are any tasks

// chain to old ISR (play nice with the other children)

Old_Time_Isr();

} // end Multi_Kernal

///////////////////////////////////////////////////////////////////////////////

void Change_Timer(unsigned int new_count)
{

// send the control word, mode 2, binary, least/most load sequence

_outp(CONTROL_8253, CONTROL_WORD);

// now write the least significant byte to the counter register

_outp(COUNTER_0,LOW_BYTE(new_count));

// and now the the most significant byte

_outp(COUNTER_0,HI_BYTE(new_count));

} // end Change_Timer



// D E M O  T A S K S /////////////////////////////////////////////////////////

void Rectangle(int xo,int yo,int x1,int y1,unsigned char color)
{

// draw a rectangle using the Bline function

Bline(xo,yo,x1,yo,color);
Bline(x1,yo,x1,y1,color);
Bline(x1,y1,xo,y1,color);
Bline(xo,y1,xo,yo,color);


} // end Rectangle

///////////////////////////////////////////////////////////////////////////////

void Stars(void)
{
// this function will animate a star field

int index; // loop variable

static int initialized=0;  // this is the local static state variable

if (!initialized)
   {

   // initialize all the stars

   for (index=0; index<NUM_STARS; index++)
       {
       // initialize each star to a velocity, position and color

       starfield[index].x     = 226 + rand()%70;
       starfield[index].y     = 26  + rand()%70;

       // decide what star plane the star is in

       switch(rand()%3)
             {
             case 0: // plane 1- the farthest star plane
                  {
                  // set velocity and color

                  starfield[index].xv = 2;
                  starfield[index].color = 8;

                  } break;

             case 1: // plane 2-The medium distance star plane
                  {

                  starfield[index].xv = 4;
                  starfield[index].color = 7;

                  } break;

             case 2: // plane 3-The nearest star plane
                  {

                  starfield[index].xv = 6;
                  starfield[index].color = 15;

                  } break;

             } // end switch

       } // end for index


   // draw working window

   Rectangle(225,25,225+75,25+75,9);

   // set variable to move to next processing state

   initialized=1;

   } // end if being initialized
else
   { // must be nth time in, so do the usual


   // process each star

   for (index=0; index<NUM_STARS; index++)
       {
       // E R A S E ///////////////////////////////////////////////////////////

       Plot_Pixel_Fast(starfield[index].x,starfield[index].y,0);

       // M O V E /////////////////////////////////////////////////////////////

       if ( (starfield[index].x+=starfield[index].xv)>=225+75 )
          starfield[index].x = 226;

       // D R A W /////////////////////////////////////////////////////////////

       Plot_Pixel_Fast(starfield[index].x,starfield[index].y,
                       starfield[index].color);

       } // end for index

   } // end else

} // end Stars

///////////////////////////////////////////////////////////////////////////////

void Mirror(void)
{
// this function will draw a mirrored pixel image

int x,y;

unsigned char color;

static int initialized=0;  // this is the local static state variable

if (!initialized)
   {

   // draw working window

   Rectangle(125,25,125+75,25+75,9);

   // set variable to move to next processing state

   initialized=1;

   } // end if not intialized
else
   {

   // D R A W /////////////////////////////////////////////////////////////////

   // draw a mirrored image

   x      = rand()%38;
   y      = rand()%38;
   color  = (unsigned char)(rand()%256);

   Plot_Pixel_Fast(x+125,y+25,color);
   Plot_Pixel_Fast((75-1)-x+125,y+25,color);
   Plot_Pixel_Fast(x+125,(75-1)-y+25,color);
   Plot_Pixel_Fast((75-1)-x+125,(75-1)-y+25,color);

   } // end else

} // end Mirror

///////////////////////////////////////////////////////////////////////////////

void Balls(void)
{
// this function will bounce a collection of balls around

int index; // used for looping

static int initialized=0;  // this is the local static state variable

if (!initialized)
   {

   // initialize all structures

   for (index=0; index<NUM_ATOMS; index++)
       {
       // select a random position and trajectory for each atom
       // their background

       atoms[index].x     = 26 + rand()%70;
       atoms[index].y     = 26 + rand()%70;

       atoms[index].xv    = -2 + rand()%4;
       atoms[index].yv    = -2 + rand()%4;

       } // end for index

   // draw working window

   Rectangle(25,25,25+75,25+75,9);

   // set initialized flag so process can switch states

   initialized = 1;

   } // end if need to initialize
else
   { // do normal processing

   // E R A S E /////////////////////////////////////////////////////////////

   // loop through the atoms and erase them

   for (index=0; index<NUM_ATOMS; index++)
       {
       Plot_Pixel_Fast(atoms[index].x, atoms[index].y, 0);
       } // end for index

   // M O V E ////////////////////////////////////////////////////////////////

   // loop through the atom array and move each atom also check collsions
   // with the walls of the container

   for (index=0; index<NUM_ATOMS; index++)
       {

       // move the atoms

       atoms[index].x+=atoms[index].xv;
       atoms[index].y+=atoms[index].yv;

       // did the atom hit a wall, if so reflect the velocity vector

       if (atoms[index].x > 98 || atoms[index].x < 27)
           {
           atoms[index].xv=-atoms[index].xv;
           atoms[index].x+=atoms[index].xv;
           } // end if hit a vertical wall

       if (atoms[index].y > 98 || atoms[index].y < 28)
          {
          atoms[index].yv=-atoms[index].yv;
          atoms[index].y+=atoms[index].yv;
          } // end if hit a horizontal wall

       } // end for index

   // D R A W /////////////////////////////////////////////////////////////////

   // loop through the atoms and draw them

   for (index=0; index<NUM_ATOMS; index++)
       {
       Plot_Pixel_Fast(atoms[index].x, atoms[index].y, 10);
       } // end for index

   } // end else normal processing

} // end Balls

// M A I N ////////////////////////////////////////////////////////////////////

void main(void)
{
int trate=20;           // initial timer rate
int done=0;             // exit flag
char string[80];        // used for printing


// SECTION 1 //////////////////////////////////////////////////////////////////

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// initialize the multi-tasking system

Initialize_Kernal();

// load in some processes and save their id's

star_id   = Add_Task(Stars);

ball_id   = Add_Task(Balls);

mirror_id = Add_Task(Mirror);

// SECTION 2 //////////////////////////////////////////////////////////////////

// set timer rate to 20Hz

Change_Timer(TIMER_20HZ);

// turn on the multi-tasking kernal, each task will be executed every 3
// interrupts since there is a round robin scheduler in place

Start_Kernal();

// SECTION 3 //////////////////////////////////////////////////////////////////

// now do main processing in parallel with other tasks

// draw menu

Blit_String(10,105,10,   "Multi-Tasking Control Menu",1);
Blit_String(10,110+10,2,"Press (2-6) to Change interrupt rate.",1);
Blit_String(10,110+20,2,"Press 'B' to kill ball task.",1);
Blit_String(10,110+30,2,"Press 'S' to kill stars task.",1);
Blit_String(10,110+40,2,"Press 'M' to kill mirror task.",1);
Blit_String(10,110+50,2,"Press 'Q' to exit.",1);

Blit_String(25,10,10,"Balls",0);
Blit_String(125,10,10,"Mirror",0);
Blit_String(225,10,10,"Star Field",0);


// SECTION 4 //////////////////////////////////////////////////////////////////

// main event loop

while(!done)
     {

     // test if key was hit

     if (kbhit())
        {

        // get the character and test it

        switch(getch())
              {

              case '2': // set system timer to 20hz
                      {

                      Change_Timer(TIMER_20HZ);
                      trate = 20;

                      } break;

              case '3': // set system timer to 30hz
                      {
                      Change_Timer(TIMER_30HZ);
                      trate = 30;

                      } break;

              case '4': // set system timer to 40hz
                      {
                      Change_Timer(TIMER_40HZ);
                      trate = 40;

                      } break;

              case '5': // set system timer to 50hz
                      {
                      Change_Timer(TIMER_50HZ);
                      trate = 50;

                      } break;

              case '6': // set system timer to 60hz
                      {
                      Change_Timer(TIMER_60HZ);
                      trate = 60;

                      } break;

// SECTION 5 //////////////////////////////////////////////////////////////////

              case 'b': // kill the ball task
                      {
                      Delete_Task(ball_id);
                      Blit_String(25,10,12,"INACTIVE  ",0);
                      } break;


              case 's': // kill the star field task
                      {
                      Delete_Task(star_id);
                      Blit_String(225,10,12,"INACTIVE  ",0);
                      } break;


              case 'm': // kill the mirror task
                      {
                      Delete_Task(mirror_id);
                      Blit_String(125,10,12,"INACTIVE  ",0);
                      } break;

              case 'q': done=1; break;

              default:break;

              } // end switch

        } // end if kbhit

     // display info

     sprintf(string,"System timer at %dHZ  ",trate);
     Blit_String(10,190,15,string,0);

     } // end while

// SECTION 6 //////////////////////////////////////////////////////////////////

// turn off the multi-tasking kernal

Stop_Kernal();

// reset system timer to 18.2

Change_Timer(TIMER_18HZ);

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

} // end main

