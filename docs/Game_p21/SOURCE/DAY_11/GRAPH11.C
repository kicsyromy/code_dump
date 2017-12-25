
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
#include "graph11.h"

// G L O B A L S /////////////////////////////////////////////////////////////


void (_interrupt _far *Old_Key_Isr)();  // holds old keyboard interrupt handler

int raw_key;  // the global raw keyboard data

// the arrow key state table

int key_table[NUM_KEYS] = {0,0,0,0,0,0,0,0,0,0};


void (_interrupt far *Old_Time_Isr)();  // used to hold old interrupt vector

// multi-tasking stuff

task tasks[MAX_TASKS];             // this is the task list for the system

int num_tasks = 0;                 // tracks number of active tasks

// F U N C T I O N S //////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
