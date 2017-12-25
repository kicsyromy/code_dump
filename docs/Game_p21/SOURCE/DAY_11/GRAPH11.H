// GRAPH11.H


// D E F I N E S //////////////////////////////////////////////////////////////

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

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void _interrupt _far New_Key_Int();

void Install_Keyboard(void);

void Delete_Keyboard(void);

void Change_Timer(unsigned int new_count);

// multi-tasking stuff

void Initialize_Kernal(void);

void Start_Kernal(void);

void Stop_Kernal(void);

int Add_Task(void (far *function)());

int Delete_Task(int id);

void _interrupt far Multi_Kernal(void);

// E X T E R N A L  G L O B A L S ////////////////////////////////////////////

extern void (_interrupt _far *Old_Key_Isr)();  // holds old keyboard interrupt handler

extern int raw_key;  // the global raw keyboard data

// the arrow key state table

extern int key_table[NUM_KEYS];


extern void (_interrupt far *Old_Time_Isr)();  // used to hold old interrupt vector

// multi-tasking stuff

extern task tasks[MAX_TASKS];             // this is the task list for the system

extern int num_tasks;                 // tracks number of active tasks

