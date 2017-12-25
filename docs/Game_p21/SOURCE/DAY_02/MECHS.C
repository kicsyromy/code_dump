
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
#include "graph9.h"  // sound library


// P R O T O T Y P E S //////////////////////////////////////////////////////

void Start_PDeath(void);

void Erase_PDeath(void);

void Animate_PDeath(void);

void Behind_PDeath(void);

void Draw_PDeath(void);

void Draw_Sprite_DBM(sprite_ptr sprite);

unsigned char Get_Pixel_DB(int x,int y);

void Blit_Char_G(int xc,int yc,char c,int color,int trans_flag);

void Blit_String_G(int x,int y,int color, char *string,int trans_flag);

void Blink_Lights(void);

void Energize(void);

void Start_Wave(void);

void Init_Mechs(void);

void Delete_Mechs(void);

void Move_Mechs(void);

void Erase_Mechs(void);

void Draw_Mechs(void);

void Behind_Mechs(void);

void Control_Mother(void);

void Move_Mother(void);

void Erase_Mother(void);

void Behind_Mother(void);

void Draw_Mother(void);

void Init_Stars(void);

void Move_Stars(void);

void Behind_Stars(void);

void Draw_Stars(void);

void Erase_Stars(void);

void Start_Missile(sprite_ptr who,
                   int x,
                   int y,
                   int xv,
                   int yv,
                   int color,
                   int tag);

void Erase_Missiles(void);

void Behind_Missiles(void);

void Draw_Missiles(void);

void Move_Missiles(void);

void Init_Missiles(void);

void Start_Explosion(int x,int y,int speed);

void Behind_Explosions(void);

void Erase_Explosions(void);

void Draw_Explosions(void);

void Animate_Explosions(void);

void Display_Instruments(void);

void Erase_Instruments(void);

void Play_Sound(int sound);

void _interrupt _far New_Key_Int(void);

// D E F I N E S /////////////////////////////////////////////////////////////

// begin keyboard stuff

#define KEYBOARD_INT    0x09        // the keyboard interrupt number
#define KEY_BUFFER      0x60        // the buffer port
#define KEY_CONTROL     0x61        // the controller port
#define INT_CONTROL     0x20        // the interrupt controller

// make and break codes for the arrow keys, space and Q

#define MAKE_RIGHT       77
#define MAKE_LEFT        75

#define MAKE_Q           16
#define MAKE_SPACE       57


#define BREAK_RIGHT      205
#define BREAK_LEFT       203

#define BREAK_Q          144
#define BREAK_SPACE      185

// indices into arrow key state table

#define INDEX_SPACE     0
#define INDEX_Q         1
#define INDEX_RIGHT     2
#define INDEX_LEFT      3

// end keyboard


#define PLAYER_DEATH_TIME    120    // how long it takes player to die
#define NUM_DEATH_PARTICLES  30     // number of explosion particles in death



#define NUM_PATTERNS          4     // number of patterns mechs have
#define MAX_PATTERN_ELEMENTS  60    // number of elements in a pattern

#define NUM_DIRECTIONS        9     // number of directions a mech can go

#define RED_BASE 32                 // start of the reds in default palette

#define BARRIER_START_COLOR 176     // the barrier color range
#define BARRIER_END_COLOR   176+16


// defines for starfield

#define NUM_STARS  50               // number of stars in the star field

#define PLANE_1    1
#define PLANE_2    2
#define PLANE_3    3

// constants for player and enemy

#define ENEMY_MISSILE  0
#define PLAYER_MISSILE 1
#define MISS_ALIVE     1
#define MISS_DEAD      0
#define NUM_MISSILES  30

// velocity of player

#define PLAYER_X_MOVE 6
#define PLAYER_Y_MOVE 0

// player states

#define PLAYER_NOT_FIRING 0
#define PLAYER_FIRING     1

#define PLAYER_DEAD       0
#define PLAYER_ALIVE      1
#define PLAYER_DYING      2

// general explosions

#define NUM_EXPLOSIONS    5         // numbre of explosions that can run at once

#define EXPLOSION_DEAD    0
#define EXPLOSION_ALIVE   1


// defines for mothership

#define MOTHER_DEAD       0
#define MOTHER_ALIVE      1

#define MOTHER_RIGHT      1
#define MOTHER_LEFT       0


// defines for initial attack pattern

#define PATTERN_X_SIZE    7          // dimensions of pattern matrix
#define PATTERN_Y_SIZE    5
#define PATTERN_XO        48         // origin of pattern formation
#define PATTERN_YO        16

#define NUM_ROBOT_FRAMES  10         // number of animation frames a mech has

// mech types

#define MECH_1            1
#define MECH_2            2
#define MECH_3            3

// states of mechs

#define MECH_DEAD         0 // dead
#define MECH_ALIVE        1 // alive
#define MECH_DYING        2 // dying

#define MECH_ATTACK       2 // action of attacking
#define MECH_PATTERN      3 // action of pattern
#define MECH_RETREAT      4 // looking for a place to stop
#define MECH_FLOCK        5 // moving with others
#define MECH_ROTATING     6 // mech is just spining
#define MECH_ENERGIZING   7 // mech is energizing


#define MECH_RIGHT        0 // mech moving to the right
#define MECH_LEFT         1 // mech moving to the left
#define MECH_UP           2 // mech moving up
#define MECH_DOWN         3 // mech moving down

#define MAX_NUMBER_MECHS  20 // maximum number of mechs in game

#define NUMBER_WAVES      15

// sound stuff

#define NUM_SOUNDS 8

#define SOUND_MISSILE  0
#define SOUND_EXPL1    1
#define SOUND_EXPL2    2
#define SOUND_EXPL3    3
#define SOUND_KILL     4
#define SOUND_ENERGY   5
#define SOUND_READY    6
#define SOUND_END      7

#define SOUND_DEFAULT_PORT  0x220  // default sound port for sound blaster
#define SOUND_DEFAULT_INT   7   // default interrupt

// S T R U C T U R E S ///////////////////////////////////////////////////////

// typedef for a explosion particle and for a missile

typedef struct particle_typ
        {
        int x;                   // x position
        int y;                   // y position
        int xv;                  // x velocity
        int yv;                  // y velocity
        unsigned char color;     // the color of the particle
        unsigned char back;      // the color behind the particle
        int state;               // the state of the particle
        int tag;                 // if the particle is a missile then who
                                 // does it belong to?
        int counter;             // use for counting
        int threshold;           // the counters threshold

        int counter_2;
        int threshold_2;

        } particle, *particle_ptr;


// data structure for a single star

typedef struct star_typ
        {
        int x,y;              // position of star
        int plane;            // which plane is star in
        unsigned char color;  // color of star
        unsigned char back;   // under star

        } star, *star_ptr;


// data structure for mech

typedef struct mech_typ
        {
        int type;                 // type of mech 1,2,3
        int x;                    // position of mech
        int y;
        int xv;                   // velocity of mech
        int yv;
        int state_1;              // state variables
        int state_2;
        int counter_1;            // counters
        int counter_2;
        int threshold_1;          // thresholds for counters
        int threshold_2;
        int aux_1;                // aux variables
        int aux_2;
        int new_state;            // the next state
        int direction;            // direction of motion when flocking
        int curr_frame;           // current animation frame
        char far *background;     // background pointer

        } mech, *mech_ptr;


// G L O B A L S  ////////////////////////////////////////////////////////////

// begin keyboard

void (_interrupt _far *Old_Isr)();  // holds old keyboard interrupt handler

int raw_key;                        // the global raw keyboard data

int key_table[4] = {0,0,0,0};      // the arrow key state table


// end keyboard


int cos_look[320];            // lookup table for cosines used for moving mother

long game_clock=0,            // how many ticks has current wave been running for
     attack_time=500;         // threshold to start attack

star stars[NUM_STARS];        // the star field

particle pdeath[NUM_DEATH_PARTICLES]; // the particles used for player's death

// star field velocities

int velocity_1=2,             // the speeds of each plane
    velocity_2=4,
    velocity_3=6;

// pcx imagery

pcx_picture intro_pcx,        // the introduction and instructions
            imagery_pcx,      // the game imagery
            background_pcx;   // the backdrop

// the sprites used in the game

sprite player,                // the player
       fire,                  // and explosion
       mother,                // the mothership
       robot_1,               // each robot type
       robot_2,
       robot_3;


// variables pertaining to the player

long player_ships     = 3,    // number of ships the player has
     player_energy    = 100,  // the initial energy of player's weapon
     player_score     = 0,    // his score
     player_gun_state = PLAYER_NOT_FIRING; // state of cannons

particle missiles[NUM_MISSILES];   // the array of missiles in the world

sprite explosions[NUM_EXPLOSIONS]; // the array of explosions

mech mech_array[MAX_NUMBER_MECHS]; // the mechs themselves

int energize_state = 0;            // state of wave, are the mech's energizing

int num_mechs      = 0;            // number of mechs for the current wave
int wave_number    = 0;            // the wave number
int mechs_killed   = 0;            // mechs killed thus far in current wave

int *current_wave;                 // pointer to current wave data



// these are the wave tables, there are used to place the mechs


int wave_0[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {0,0,1,1,1,0,0,
                                             0,0,2,2,2,0,0,
                                             0,0,3,3,3,0,0,
                                             0,0,0,0,0,0,0,
                                             0,0,0,0,0,0,0};

int wave_1[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {0,1,0,0,0,1,0,
                                             0,0,1,2,1,0,0,
                                             0,3,3,3,3,3,0,
                                             0,0,0,0,0,0,0,
                                             0,0,0,0,0,0,0};




int wave_2[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {2,0,2,3,2,0,2,
                                             0,2,0,3,0,2,0,
                                             0,0,3,3,3,0,0,
                                             0,0,0,3,0,0,0,
                                             0,0,0,0,0,0,0};

int wave_3[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {1,1,0,1,0,1,1,
                                             1,1,0,0,0,1,1,
                                             0,0,2,2,2,0,0,
                                             0,3,3,3,3,3,0,
                                             0,0,0,0,0,0,0};



int wave_4[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {0,0,3,3,3,0,0,
                                             1,0,1,0,1,0,1,
                                             2,1,2,1,2,1,2,
                                             0,0,3,3,3,0,0,
                                             0,0,0,3,0,0,0};



int wave_5[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {0,1,0,1,0,1,0,
                                             0,0,3,3,3,0,0,
                                             0,3,3,1,3,3,0,
                                             0,3,2,2,2,3,0,
                                             0,0,0,0,0,0,0};



int wave_6[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {1,2,3,3,3,2,1,
                                             0,1,2,3,2,1,0,
                                             0,0,1,2,1,0,0,
                                             0,0,0,1,0,0,0,
                                             0,0,0,0,0,0,0};



int wave_7[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {1,2,1,0,1,2,1,
                                             0,1,0,0,0,1,0,
                                             0,0,0,2,0,0,0,
                                             0,0,3,3,3,0,0,
                                             0,0,0,3,0,0,0};


int wave_8[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {0,0,0,0,0,0,0,
                                             2,2,2,2,2,2,2,
                                             0,3,0,3,0,3,0,
                                             0,0,0,0,0,0,0,
                                             0,0,1,0,1,0,0};




int wave_9[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {1,3,1,3,1,3,1,
                                             1,0,1,0,1,0,1,
                                             1,0,1,0,1,0,1,
                                             0,2,0,2,0,2,0,
                                             0,0,0,0,0,0,0};


int wave_10[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {0,0,1,1,1,0,0,
                                              0,0,2,2,2,0,0,
                                              0,2,3,3,3,2,0,
                                              2,3,0,0,0,3,2,
                                              3,0,0,0,0,0,3};




int wave_11[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {0,0,0,1,0,0,0,
                                              0,0,1,2,1,0,0,
                                              0,1,2,3,2,1,0,
                                              0,0,1,2,1,0,0,
                                              0,0,0,1,0,0,0};





int wave_12[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {1,2,1,0,1,2,1,
                                              0,0,2,0,2,0,0,
                                              0,0,1,3,1,0,0,
                                              0,0,0,3,0,0,0,
                                              0,0,0,3,0,0,0};

int wave_13[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {1,0,0,3,0,0,1,
                                              0,0,2,0,2,0,0,
                                              0,3,0,0,0,3,0,
                                              0,0,2,0,2,0,0,
                                              1,0,0,3,0,0,1};




int wave_14[PATTERN_X_SIZE*PATTERN_Y_SIZE] = {1,1,1,0,3,0,0,
                                              1,0,1,0,3,0,0,
                                              1,1,1,2,3,0,0,
                                              1,0,1,0,3,0,0,
                                              1,0,1,0,3,3,3};

// this is an array that is used to point to all the wave tables

int *waves[NUMBER_WAVES];


// this is a data structure that holds the instructions for the patterns
// that the mechs take when in pattern mode

int patterns[NUM_PATTERNS][MAX_PATTERN_ELEMENTS]

    = {1,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5,6,6,7,7,7,7,8,8,8,8,7,7,7,7,7,7,6,6,7,5,4,4,3,3,2,2,1,1,0,0,0,-1,
       1,1,1,1,1,1,1,1,1,1,2,2,3,3,4,4,5,5,5,5,5,5,5,5,5,5,7,7,7,7,7,7,7,7,8,8,8,8,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,-1,
       1,1,8,8,7,7,8,8,7,7,7,7,7,7,7,7,6,6,5,5,4,4,3,3,3,3,3,3,3,3,4,4,4,4,5,5,6,6,7,7,7,7,7,7,7,7,6,6,5,5,4,4,3,3,2,2,1,1,0,-1,
       1,1,2,2,3,3,3,3,3,3,3,3,4,4,5,5,5,5,5,5,6,6,7,7,7,7,7,7,7,7,7,8,8,1,1,1,1,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,1,1,0,0,0,0,-1};

// these are the velocity vectors used to move the mechs via a pattern instruction

int dirs_x[NUM_DIRECTIONS] = {0, 0, 4, 4, 4, 0,-4,-4,-4};
int dirs_y[NUM_DIRECTIONS] = {0,-4,-4, 0, 4, 4, 4, 0,-4};

// sound stuff

char far *sound_fx[NUM_SOUNDS];
unsigned char sound_lengths[NUM_SOUNDS];

int sound_available = 1;

int sound_port = SOUND_DEFAULT_PORT;  // default sound port
int sound_int  = SOUND_DEFAULT_INT;   // default sound interrupt
                                      // both read from mechs.cfg

// F U N C T I O N S //////////////////////////////////////////////////////////

void _interrupt _far New_Key_Int(void)
{

// I'm in the mood for some inline!

_asm
   {
   sti                    ; re-enable interrupts
   in al, KEY_BUFFER      ; get the key that was pressed
   xor ah,ah              ; zero out upper 8 bits of AX
   mov raw_key, ax        ; store the key in global
   in al, KEY_CONTROL     ; set the control register
   or al, 82h             ; set the proper bits to reset the FF
   out KEY_CONTROL,al     ; send the new data back to the control register
   and al,7fh
   out KEY_CONTROL,al     ; complete the reset
   mov al,20h
   out INT_CONTROL,al     ; re-enable interrupts
                          ; when this baby hits 88 mph, your gonna see
                          ; some serious @#@#$%

   } // end inline assembly

// now for some C to update the arrow state table

// process the key and update the table

switch(raw_key)
      {

      case MAKE_RIGHT: // pressing right
           {
           key_table[INDEX_RIGHT] = 1;
           } break;

      case MAKE_LEFT:  // pressing left
           {
           key_table[INDEX_LEFT]  = 1;
           } break;

      case MAKE_SPACE: // pressing space
           {
           key_table[INDEX_SPACE] = 1;
           } break;

      case MAKE_Q:  // pressing Q
           {
           key_table[INDEX_Q]  = 1;
           } break;

      case BREAK_RIGHT: // releasing right
           {
           key_table[INDEX_RIGHT] = 0;
           } break;

      case BREAK_LEFT:  // releasing left
           {
           key_table[INDEX_LEFT]  = 0;
           } break;

      case BREAK_SPACE: // releasing space
           {
           key_table[INDEX_SPACE] = 0;
           } break;

      case BREAK_Q:  // releasing Q
           {
           key_table[INDEX_Q]  = 0;
           } break;

      default: break;

      } // end switch

} // end New_Key_Int

///////////////////////////////////////////////////////////////////////////////

int Initialize_Sound_System(void)
{

FILE *fp;

// test if driver is on disk

if ( (fp=fopen("ct-voice.drv","rb") )==NULL)
   {
   return(0);
   } // end if not file

fclose(fp);

// load up sound configuration file

if ( (fp=fopen("mechs.cfg","r"))==NULL )
   {
   printf("\nSound configuration file not found...");
   printf("\nUsing default values of port 220h and interrupt 5.");

   } // end if open sound configuration file
else
   {

   fscanf(fp,"%d %d",&sound_port, &sound_int);
   printf("\nSetting sound system to port %d decimal with interrupt %d.",
                                                    sound_port, sound_int);

   } // end else

// start up the whole sound system and load everything

Voc_Load_Driver();

Voc_Set_Port(sound_port);
Voc_Set_IRQ(sound_int);
Voc_Init_Driver();
Voc_Get_Version();
Voc_Set_Status_Addr((char __far *)&ct_voice_status);

// load in sounds

sound_fx[SOUND_MISSILE ] = Voc_Load_Sound("missile.voc", &sound_lengths[SOUND_MISSILE ]);
sound_fx[SOUND_EXPL1   ] = Voc_Load_Sound("expl1.voc",   &sound_lengths[SOUND_EXPL1   ]);
sound_fx[SOUND_EXPL2   ] = Voc_Load_Sound("expl2.voc",   &sound_lengths[SOUND_EXPL2   ]);
sound_fx[SOUND_EXPL3   ] = Voc_Load_Sound("expl3.voc",   &sound_lengths[SOUND_EXPL3   ]);
sound_fx[SOUND_KILL    ] = Voc_Load_Sound("kill.voc",    &sound_lengths[SOUND_KILL    ]);
sound_fx[SOUND_ENERGY  ] = Voc_Load_Sound("energy.voc",  &sound_lengths[SOUND_ENERGY  ]);
sound_fx[SOUND_READY   ] = Voc_Load_Sound("ready.voc",   &sound_lengths[SOUND_READY   ]);
sound_fx[SOUND_END     ] = Voc_Load_Sound("end.voc",     &sound_lengths[SOUND_END]);

Voc_Set_Speaker(1);

return(1);

} // end Initialize_Sound_System

/////////////////////////////////////////////////////////////////////////////

void Close_Sound_System(void)
{
// make sure there is sound

if (sound_available)
   {

   Voc_Set_Speaker(0);

   // unload sounds

   Voc_Unload_Sound(sound_fx[SOUND_MISSILE ]);
   Voc_Unload_Sound(sound_fx[SOUND_EXPL1   ]);
   Voc_Unload_Sound(sound_fx[SOUND_EXPL2   ]);
   Voc_Unload_Sound(sound_fx[SOUND_EXPL3   ]);
   Voc_Unload_Sound(sound_fx[SOUND_KILL    ]);
   Voc_Unload_Sound(sound_fx[SOUND_ENERGY  ]);
   Voc_Unload_Sound(sound_fx[SOUND_READY   ]);
   Voc_Unload_Sound(sound_fx[SOUND_END     ]);

   Voc_Terminate_Driver();

   } // end if sound

} // end Close_Sound_System

/////////////////////////////////////////////////////////////////////////////

void Play_Sound(int sound)
{

if (sound_available)
   {
   Voc_Stop_Sound();
   Voc_Play_Sound(sound_fx[sound] , sound_lengths[sound]);
   } // end if sound available

} // end Play_Sound

///////////////////////////////////////////////////////////////////////////////

void Start_PDeath(void)
{

// this function begins the death of a player

int index;

// make sure the player is dying otherwise return

if (player.state!=PLAYER_DYING) return;

// loop thru all particles and initialize them to different upward velocities

for (index=0; index<NUM_DEATH_PARTICLES; index++)
    {

    pdeath[index].x           =  9+player.x - 4 + rand()%9;
    pdeath[index].y           =  9+player.y - 4 + rand()%9;
    pdeath[index].xv          =  -5 + rand()%11;
    pdeath[index].yv          =  -5-(rand()%7);
    pdeath[index].color       =  137;
    pdeath[index].back        =  0;
    pdeath[index].state       =  0;
    pdeath[index].tag         =  0;

    pdeath[index].counter     =  0;
    pdeath[index].threshold   =  5;

    pdeath[index].counter_2   =  0;
    pdeath[index].threshold_2 =  5;


    } // end for index

Play_Sound(SOUND_EXPL1);

} // end Start_PDeath

///////////////////////////////////////////////////////////////////////////////

void Erase_PDeath(void)
{
// this function is used to erase all the particles in the players death

int index;

// make sure the player is dying otherwise return

if (player.state!=PLAYER_DYING) return;

// loop thru all particles and erase them i.e. replace the background

for (index=0; index<NUM_DEATH_PARTICLES; index++)
    {

    Plot_Pixel_Fast_DB(pdeath[index].x,pdeath[index].y,pdeath[index].back);

    } // end for index

} // end Erase_PDeath

///////////////////////////////////////////////////////////////////////////////

void Animate_PDeath(void)
{

// this is the workhorse of the death animation sequence, it moves the particles
// applies gravity to them and changes their colors, also it bounds them to the
// screen

int index;

// should we be doing this?

if (player.state!=PLAYER_DYING) return;

// process each particle

for (index=0; index<NUM_DEATH_PARTICLES; index++)
    {

    // translation

    pdeath[index].x+=pdeath[index].xv;
    pdeath[index].y+=pdeath[index].yv;

    // boundary tests

    // xtests

    if (pdeath[index].x > 319)
       {
       pdeath[index].x = 319;
       }
    else
    if (pdeath[index].x < 0)
       {
       pdeath[index].x = 0;
       }

    // ytests

    if (pdeath[index].y > 199)
        {
        pdeath[index].y     = 199;
        pdeath[index].xv    = 0;
        pdeath[index].color = 0;
        }
    else
    if (pdeath[index].y < 1)
        {
        pdeath[index].y = 1;
        }

    // gravity

    if (++pdeath[index].counter == pdeath[index].threshold)
       {
       // apply gravity field

       pdeath[index].yv++;

       // reset counter

       pdeath[index].counter = 0;

       } // end if time to apply gravity

    // color

    if (pdeath[index].y<199)
       if (++pdeath[index].counter_2 == pdeath[index].threshold_2)
          {
          // change color

          pdeath[index].color++;

          // reset counter

          pdeath[index].counter_2 = 0;

          } // end if time to change color

    // end of sequence

    } // end for

// update death time

if (++player.anim_clock == PLAYER_DEATH_TIME)
   {

   // reset players position

   player.x          = 160;
   player.y          = 168;
   player.curr_frame = 0;
   player.state      = PLAYER_ALIVE;

   // clear all the missiles

   Init_Missiles();

   // reset the wave

   Start_Wave();

   } // end if player done dying

} // end Animate_PDeath

///////////////////////////////////////////////////////////////////////////////

void Behind_PDeath(void)
{

int index;

// check if we should do this

if (player.state!=PLAYER_DYING) return;

// loop thru all particles and scan their backgrounds

for (index=0; index<NUM_DEATH_PARTICLES; index++)
    {

    pdeath[index].back = Get_Pixel_DB(pdeath[index].x,pdeath[index].y);

    } // end for index

} // end Behind_PDeath

///////////////////////////////////////////////////////////////////////////////

void Draw_PDeath(void)
{

int index;

// check if we should do this

if (player.state!=PLAYER_DYING) return;

// loop thru all particles and draw them

for (index=0; index<NUM_DEATH_PARTICLES; index++)
    {

    Plot_Pixel_Fast_DB(pdeath[index].x,pdeath[index].y,pdeath[index].color);

    } // end for index

} // end Draw_PDeath

//////////////////////////////////////////////////////////////////////////////

void Draw_Sprite_DBM(sprite_ptr sprite)
{

// this function draws a sprite on the screen row by row very quickly
// note the use of shifting to implement multplication
// also it is used as a special effect, the sprite drawn is melted by
// randomly selecting red pixels


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

        if ((work_sprite[work_offset+x]))
             double_buffer[offset+x] = RED_BASE+rand()%32;

        } // end for x

    // move to next line in double buffer and in sprite bitmap buffer

    offset      += SCREEN_WIDTH;
    work_offset += sprite_width;

    } // end for y

} // end Draw_Sprite_DBM

///////////////////////////////////////////////////////////////////////////////

unsigned char Get_Pixel_DB(int x,int y)
{

// gets the color value of pixel at (x,y) from the double buffer

return double_buffer[((y<<8) + (y<<6)) + x];

} // end Get_Pixel_DB

///////////////////////////////////////////////////////////////////////////////

void Blit_Char_G(int xc,int yc,char c,int color,int trans_flag)
{
// this function uses the rom 8x8 character set to blit a character to the
// double buffer, also it blits the character in two colors

int offset,x,y;
unsigned char data;
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

    // test if it's time to change colors

    if (y==(CHAR_HEIGHT/2))
       color-=8; // change to lower intensity

    for (x=0; x<CHAR_WIDTH; x++)
        {
        // test for transparent pixel i.e. 0, if not transparent then draw

        if ((*work_char & bit_mask))
             video_buffer[offset+x] = color;
        else
        if (!trans_flag)  // takes care of transparency
            video_buffer[offset+x] = 0;


        // shift bit mask

        bit_mask = (bit_mask>>1);

        } // end for x

    // move to next line in video buffer and in rom character data area

    offset      += SCREEN_WIDTH;
    work_char++;

    } // end for y

} // end Blit_Char_G

//////////////////////////////////////////////////////////////////////////////

void Blit_String_G(int x,int y,int color, char *string,int trans_flag)
{
// this function blits an entire string to the double buffer
// It calls blit_char_g which is the gradient version of the character blitter

int index;

for (index=0; string[index]!=0; index++)
     {

     Blit_Char_G(x+(index<<3),y,string[index],color,trans_flag);

     } /* end while */

} /* end Blit_String_G */

//////////////////////////////////////////////////////////////////////////////

void Blink_Lights(void)
{
// this function blinks the lights on the barriers

static int clock=0,
           entered_yet=0;// used for timing, note: they are static!

static RGB_color color;

// this function blinks the running lights on the walkway

if (!entered_yet)
   {

   // reset the palette register 243 to light green
   // black, black

   color.red   = 0;
   color.green = 10;
   color.blue  = 0;

   Set_Palette_Register(243,(RGB_color_ptr)&color);

   // system has initialized, so flag it

   entered_yet=1;

   } // end if first time into function

// try and blick the lights, is it time

   ++clock;

   if (clock==4)
      {
      // turn the lights on

      color.green = 255;

      Set_Palette_Register(243,(RGB_color_ptr)&color);

      } // end if time to rotate
   else
   if (clock==8)
      {

      // turn the lights off

      color.green = 0;

      Set_Palette_Register(243,(RGB_color_ptr)&color);

      clock=0;

      } // end if time to blink

} // end Blink_Lights

///////////////////////////////////////////////////////////////////////////////

void Energize(void)
{

// this function is used to slowly materialize the mechs, it does this by
// drwing them in separate bitmaps that are of a single color that can
// be controlled

static RGB_color color_grey,
                 color_red,
                 color_green;


// test if this is start of sequence

if (energize_state==0)
   {

   // the grey robot

   color_grey.red    = 0;
   color_grey.green  = 0;
   color_grey.blue   = 0;

   // the red robot

   color_red.red    = 0;
   color_red.green  = 0;
   color_red.blue   = 0;


   // the grey robot

   color_green.red    = 0;
   color_green.green  = 0;
   color_green.blue   = 0;


   // energizing has begun

   energize_state  = 1;

   Set_Palette_Register(240,(RGB_color_ptr)&color_grey);
   Set_Palette_Register(241,(RGB_color_ptr)&color_red);
   Set_Palette_Register(242,(RGB_color_ptr)&color_green);

   } // end if starting
else
if (rand()%2==1)
   {
   // update grey mechs

   ++color_grey.red;
   ++color_grey.green;
   ++color_grey.blue;

   // update red mechs

   ++color_red.red;

   // update green mechs

   ++color_green.green;

   // update the palette registers

   Set_Palette_Register(240,(RGB_color_ptr)&color_grey);

   Set_Palette_Register(241,(RGB_color_ptr)&color_red);

   Set_Palette_Register(242,(RGB_color_ptr)&color_green);

   } // end if time to increment color intensity

} // end Energize

///////////////////////////////////////////////////////////////////////////////

void Start_Wave(void)
{

// this function is used to start a wave off

int x,y,mech_index=0,element, local_wave;

// reset number of mechs on this level

num_mechs=0;

// reset number of mechs killed

mechs_killed=0;

// select level

local_wave = wave_number;

// test to see if level is too high, if so choose randomly

if (local_wave>14)
   {
   // select wave randomly

   local_wave = rand()%10 + 5;

   // decrease the overall attack time of game now

   attack_time=-10;

   // test if we overflowed attack time

   if (attack_time<100)
      attack_time=100;

   } // end if did the first 15 waves

// alias data structure for current wave

current_wave = waves[local_wave];

// loop and create mechs

for (x=0; x<PATTERN_X_SIZE; x++)
    {

    for (y=0; y<PATTERN_Y_SIZE; y++)
        {

        // extract element out of database

        element = current_wave[PATTERN_X_SIZE*y + x];

        // test if this is a live mech

        if (element!=0)
           {

           // set fixed fields

           mech_array[num_mechs].x            = x * 32 + PATTERN_XO;
           mech_array[num_mechs].y            = y * 22 + PATTERN_YO;

           mech_array[num_mechs].xv           = 0;
           mech_array[num_mechs].yv           = 0;
           mech_array[num_mechs].state_1      = MECH_ALIVE;
           mech_array[num_mechs].state_2      = MECH_ENERGIZING;
           mech_array[num_mechs].counter_1    = 0;
           mech_array[num_mechs].counter_2    = 0;
           mech_array[num_mechs].aux_1        = 0;
           mech_array[num_mechs].aux_2        = 0;
           mech_array[num_mechs].threshold_1  = 0;
           mech_array[num_mechs].threshold_2  = 64;
           mech_array[num_mechs].direction    = 0;
           mech_array[num_mechs].curr_frame   = 9;

           // set type field

           mech_array[num_mechs].type         = element;

           // there is one more mech now

           num_mechs++;

           } // end if a live mech

        } // end for y

    } // end for x

// reset energizer colors

energize_state = 0;

// reset game clock

game_clock = 0;

// start things up

Energize();

// let's here some noise

if (player_ships>0)
    Play_Sound(SOUND_READY);


} // end Start_Wave

///////////////////////////////////////////////////////////////////////////////

void Init_Mechs(void)
{

// this function is used to clear all the mechs and get them ready

int index;

for (index=0; index<MAX_NUMBER_MECHS; index++)
    {
    // zero out all the fields and allocate the memory

    mech_array[index].type        = 0;
    mech_array[index].x           = 0;
    mech_array[index].y           = 0;
    mech_array[index].xv          = 0;
    mech_array[index].yv          = 0;
    mech_array[index].state_1     = 0;
    mech_array[index].state_2     = 0;
    mech_array[index].aux_1       = 0;
    mech_array[index].aux_2       = 0;
    mech_array[index].new_state   = 0;
    mech_array[index].counter_1   = 0;
    mech_array[index].counter_2   = 0;
    mech_array[index].threshold_1 = 0;
    mech_array[index].threshold_2 = 0;
    mech_array[index].direction   = 0;
    mech_array[index].curr_frame  = 0;
    mech_array[index].background  =
                      (char far *)_fmalloc(sprite_width * sprite_height+1);

    } // end index

} // end Init_Mechs

///////////////////////////////////////////////////////////////////////////////

void Delete_Mechs(void)
{

int index;

for (index=0; index<MAX_NUMBER_MECHS; index++)
    {
    _ffree(mech_array[index].background);
    } // end for index

} // end Delete_Mechs

///////////////////////////////////////////////////////////////////////////////

void Move_Mechs(void)
{

// this is an extremely complex function is controls the mechs movement
// and their state machine transitions.  currently only pattern mode and
// flock mode are implemented

int index,                 // loop variable
    flock_switch=0,        // has the flock switched direction
    mech_killed_switch=0,  // has there been a death
    mdx=0,                 // the delta x
    curr_direction;        // current pattern direction

static int global_flock=MECH_RIGHT; // used to track the flock's direction

mech_ptr worker;                    // used as an alias to current mech

// loop process each mech

for (index=0; index<num_mechs; index++)
    {

    // alias current mech

    worker = (mech_ptr)&mech_array[index];

    // test state of mech

    if (worker->state_1==MECH_ALIVE)
       {

       // this is the hard part, based on state perform proper logic

       switch(worker->state_2)
             {

             case MECH_PATTERN: // process pattern mode
                  {

                  // test for start of state

                  if (worker->new_state)
                     {
                     // reset new state

                     worker->new_state=0;

                     // select a pattern and reset all vars

                     worker->aux_1     = rand()%NUM_PATTERNS;

                     // use counter 1 as index into pattern table

                     worker->counter_1 = 0;
                     worker->counter_2   = 0;
                     worker->threshold_2 = 2+rand()%3;

                     } // end if need to initialize state

                  // else must be continuing state

                  // access current direction

                  curr_direction = patterns[worker->aux_1][worker->counter_1];

                  // test if we are at end of sequence

                  if (curr_direction==-1)
                     {
                     worker->state_2    = MECH_FLOCK;
                     worker->new_state  = 1;
                     break;

                     } // end if at end

                  // extract current frame of animation

                  worker->curr_frame = curr_direction;

                  // using current direction, compute velocity vector

                  worker->x+=dirs_x[curr_direction];
                  worker->y+=dirs_y[curr_direction];

                  // test if we went too far

                  if (worker->x > 300 )
                     {
                     worker->x = 0;
                     }
                  else
                  if (worker->x < 0)
                     {
                     worker->x = 300;
                     }

                  if (worker->y >= 120)
                     worker->y = 120;
                  else
                  if (worker->y < 0)
                     worker->y = 0;

                  // move to next element in pattern

                  if (++worker->counter_2 == worker->threshold_2)
                     {
                     worker->counter_2=0;
                     worker->counter_1++;

                     } // end if time to switch pattern element


                  // test if we want to fire a missile

                  if ( (worker->x > (player.x - 60)) &&
                       (worker->x < (player.x + 80)) &&
                       (curr_direction>=4)   &&
                       (curr_direction<=6)   &&
                       (rand()%10==1) )
                     {


                     // start missile with current trajectory


                     Start_Missile((sprite_ptr)worker,
                                   worker->x+10,
                                   worker->y+sprite_height,
                                   dirs_x[curr_direction]*2,
                                   dirs_y[curr_direction]*2,
                                   9,
                                   ENEMY_MISSILE);

                     } // end if fire missile


                  } break;

             case MECH_ATTACK:  // mech attack mode (not implemented)
                  {

                  // test for start of state

                  if (worker->new_state)
                     {




                     } // end if need to initialize state

                  // else must be continuing state

                  } break;

             case MECH_RETREAT:  // mech retreat mode (not implemented)
                  {

                  // test for start of state

                  if (worker->new_state)
                     {


                     } // end if need to initialize state


                  // else must be continuing state

                  } break;


             case MECH_FLOCK:   // mech flock mode
                  {

                  // test for start of state

                  if (worker->new_state)
                     {
                     // reset new state

                     worker->new_state=0;

                     // select a pattern and reset all vars

                     worker->counter_2    = 0;
                     worker->threshold_2  = 4;
                     worker->direction    = global_flock;

                     } // end if need to initialize state


                  // motion

                  if (worker->direction==MECH_RIGHT)
                     {

                     // do translation

                     worker->x+=(worker->xv);

                     // test right boundary

                     if (worker->x > SCREEN_WIDTH-32)
                        flock_switch=1;

                     } // end if moving right
                  else
                  if (worker->direction==MECH_LEFT)
                     {
                     // do translation

                     worker->x-=(worker->xv);

                     // test left boundary

                     if (worker->x < 32-18)
                        flock_switch=1;

                     } // end if moving left


                  // animation

                  if (++worker->counter_2 == worker->threshold_2)
                     {
                     // reset counter

                     worker->counter_2 = 0;

                     if (++worker->curr_frame > 1)
                        worker->curr_frame=0;

                     } // end if time to change frames


                  // weapons

                  if ( (worker->x > (player.x - 50)) &&
                       (worker->x < (player.x + 70)) &&
                       (rand()%50==1) )
                     {


                     // compute trajectory

                     if (worker->x < player.x - 10)
                        mdx=+3;
                     else
                     if (worker->x > player.x + 30)
                        mdx=-3;
                     else
                        mdx=0;

                     // the the missile with computed trajectory velocity

                     Start_Missile((sprite_ptr)worker,
                                   worker->x+10,
                                   worker->y+sprite_height,
                                   mdx,
                                   6,
                                   9,
                                   ENEMY_MISSILE);

                     } // end if time to fire

                  // test if it's time to blow this coup!

                  if (game_clock>attack_time && rand()%100==1)
                     {

                     // switch state to pattern

                     worker->state_2    = MECH_PATTERN;
                     worker->new_state  = 1;

                     if (rand()%5==1)
                        Play_Sound(SOUND_KILL);

                     } // end if time to switch to pattern state

                  } break;


             case MECH_ROTATING:  // not implemented
                  {

                  } break;


             case MECH_ENERGIZING:  // the initial start up state of mechs
                  {

                  // increment energizer time

                  worker->counter_2++;

                  // test if we are done energizing

                  if (worker->counter_2 < worker->threshold_2)
                     {
                     // continue energizing


                     } // end if still energizing
                  else
                     {
                     // need to move to flock state

                     worker->state_2      = MECH_FLOCK;
                     worker->xv           = 2;
                     worker->counter_2    = 0;
                     worker->threshold_2  = 4;
                     worker->direction    = MECH_RIGHT;
                     worker->curr_frame   = 0;

                     // move out of energizing state

                     ++energize_state;

                     } // end else move to flock state

                  } break;

             default:break;

             } // end switch state_2

       } // end if alive
    else
    if (mech_array[index].state_1==MECH_DYING)  // test if mech is dying
       {
       // one more frame of death

       if (++worker->counter_1 > worker->threshold_1)
          {

          worker->state_1 = MECH_DEAD;
          mechs_killed++;
          mech_killed_switch=1;

          // were getting killed, let's get more agressive!

          game_clock+=25;

          } // end if done dying

       } // end if alive

    } // end for index

// processed all mechs now do any global updates

// test if we need to flip the whole crew around

if (flock_switch==1)
{

// loop thru all mechs

for (index=0; index<num_mechs; index++)
    {

    // alias pointer to current mech

    worker = (mech_ptr)&mech_array[index];

    // test if mech is alive and flocking

    if (worker->state_1 == MECH_ALIVE &&
        worker->state_2 == MECH_FLOCK )
       {
       // switch directions

       if (worker->direction==MECH_RIGHT)
           {
           global_flock = MECH_LEFT;
           worker->direction=MECH_LEFT;
           }
       else // else must be right
           {
           global_flock = MECH_RIGHT;
           worker->direction=MECH_RIGHT;
           }

       } // end if alive

    } // end for index global

} // end if global change

// test if someone has been killed

if (mech_killed_switch==1)
{

// loop thru them all

for (index=0; index<num_mechs; index++)
    {

    // alias for speed

    worker = (mech_ptr)&mech_array[index];

    // if it was a flocker then crank up velocity and move down a little

    if (worker->state_1 == MECH_ALIVE &&
        worker->state_2 == MECH_FLOCK )
       {

       // increase acceleration

       if (++worker->xv > 6)
          worker->xv=6;

       // move them down a little

       worker->y+=2;

       } // end if alive

    } // end for index global

} // end if global change


// test if mechs are energizing, if so call energizing function to
// perform special fX

if (energize_state==1)
   Energize();

} // end Move_Mechs

///////////////////////////////////////////////////////////////////////////////

void Erase_Mechs(void)
{

// this function erases all the mechs

int index;

// loop thru all mechs

for (index=0; index<num_mechs; index++)
    {

    // based on type of mech use proper animation frames

    // test if mech is alive

    if (mech_array[index].state_1 != MECH_DEAD)
       {

       // need to know which mech type so correct bitmaps can be used

       switch(mech_array[index].type)
             {

             case MECH_1:   // type one mech
                  {
                  robot_1.x          = mech_array[index].x;
                  robot_1.y          = mech_array[index].y;
                  robot_1.background = mech_array[index].background;

                  Erase_Sprite_DB((sprite_ptr)&robot_1);

                  } break;

             case MECH_2:   // type two mech
                  {
                  robot_2.x          = mech_array[index].x;
                  robot_2.y          = mech_array[index].y;
                  robot_2.background = mech_array[index].background;

                  Erase_Sprite_DB((sprite_ptr)&robot_2);

                  } break;

             case MECH_3:   // type three mech
                  {
                  robot_3.x          = mech_array[index].x;
                  robot_3.y          = mech_array[index].y;
                  robot_3.background = mech_array[index].background;

                  Erase_Sprite_DB((sprite_ptr)&robot_3);

                  } break;

             default:break;

             } // end switch

       } // end if mech dead

    } // end for index

} // end Erase_Mechs

///////////////////////////////////////////////////////////////////////////////

void Draw_Mechs(void)
{

// this function draws the mechs

int index;

// process each mech

for (index=0; index<num_mechs; index++)
    {

    // test if mech is alive

    if (mech_array[index].state_1 != MECH_DEAD)
       {

       switch(mech_array[index].type)
             {

             case MECH_1:   // type one mech
                  {
                  robot_1.x          = mech_array[index].x;
                  robot_1.y          = mech_array[index].y;
                  robot_1.curr_frame = mech_array[index].curr_frame;

                  // test if we should use dying blitter

                  if (mech_array[index].state_1==MECH_ALIVE)
                     Draw_Sprite_DB((sprite_ptr)&robot_1);
                  else // use melter draw
                     Draw_Sprite_DBM((sprite_ptr)&robot_1);

                  } break;

             case MECH_2: // type two mech
                  {
                  robot_2.x          = mech_array[index].x;
                  robot_2.y          = mech_array[index].y;
                  robot_2.curr_frame = mech_array[index].curr_frame;

                  // test if we should use dying blitter

                  if (mech_array[index].state_1==MECH_ALIVE)
                     Draw_Sprite_DB((sprite_ptr)&robot_2);
                  else // use melter draw
                     Draw_Sprite_DBM((sprite_ptr)&robot_2);

                  } break;

             case MECH_3:    // type three mech
                  {
                  robot_3.x          = mech_array[index].x;
                  robot_3.y          = mech_array[index].y;
                  robot_3.curr_frame = mech_array[index].curr_frame;

                  // test if we should use dying blitter

                  if (mech_array[index].state_1==MECH_ALIVE)
                     Draw_Sprite_DB((sprite_ptr)&robot_3);
                  else // use melter draw
                     Draw_Sprite_DBM((sprite_ptr)&robot_3);

                  } break;

             default:break;

             } // end switch

       } // end if mech dead

    } // end for index

} // end Draw_Mechs

//////////////////////////////////////////////////////////////////////////////

void Behind_Mechs(void)
{
// this function scans the background under the mechs

int index;

// loop and process all mechs

for (index=0; index<num_mechs; index++)
    {

    // test if mech is alive

    if (mech_array[index].state_1 != MECH_DEAD)
       {

       switch(mech_array[index].type)
             {

             case MECH_1:   // type one mech
                  {
                  robot_1.x          = mech_array[index].x;
                  robot_1.y          = mech_array[index].y;
                  robot_1.background = mech_array[index].background;

                  Behind_Sprite_DB((sprite_ptr)&robot_1);

                  } break;

             case MECH_2:   // type two mech
                  {
                  robot_2.x          = mech_array[index].x;
                  robot_2.y          = mech_array[index].y;
                  robot_2.background = mech_array[index].background;

                  Behind_Sprite_DB((sprite_ptr)&robot_2);

                  } break;

             case MECH_3:   // type three mech
                  {
                  robot_3.x          = mech_array[index].x;
                  robot_3.y          = mech_array[index].y;
                  robot_3.background = mech_array[index].background;

                  Behind_Sprite_DB((sprite_ptr)&robot_3);

                  } break;

             default:break;

             } // end switch

       } // end if mech dead

    } // end for index

} // end Behind_Mechs

//////////////////////////////////////////////////////////////////////////////

void Control_Mother(void)
{

// this function controls the mother ship

if (mother.state == MOTHER_DEAD && rand()%500==1)
   {

   // turn on the mother ship

   mother.state = MOTHER_ALIVE;

   // select a random direction

   switch(rand()%2)
         {

         case 0:  // right
               {

               mother.curr_frame   = MOTHER_RIGHT;
               mother.motion_speed = 4+rand()%2;
               mother.x            = 0;
               mother.y            = 12+rand()%16;

               } break;

         case 1: // left
               {

               mother.curr_frame   = MOTHER_LEFT;
               mother.motion_speed = -(4+rand()%2);
               mother.x            = SCREEN_WIDTH-20;
               mother.y            = 12+rand()%16;


               } break;


         default:break;

         } // end switch

    Behind_Sprite_DB((sprite_ptr)&mother);

   } // end if start mother up

} // end Control_Mother

///////////////////////////////////////////////////////////////////////////////

void Move_Mother(void)
{

// this moves the mother ship if it is alive

if (mother.state == MOTHER_ALIVE)
   {

   // move x at a constant speed

   mother.x+=mother.motion_speed;

   // modulate the y position by a cosine wave

   mother.y+=cos_look[mother.x];

   // do boundary collisions

   if (mother.y<0) mother.y=0;

   if (mother.x > SCREEN_WIDTH-20 || mother.x < 0)
      mother.state = MOTHER_DEAD;

   } // end if alive

} // end Move_Mother

//////////////////////////////////////////////////////////////////////////////

void Erase_Mother(void)
{

// this function erases the mother ship

if (mother.state == MOTHER_ALIVE)
   {

   Erase_Sprite_DB((sprite_ptr)&mother);

   } // end if alive

} // end Erase_Mother

//////////////////////////////////////////////////////////////////////////////

void Behind_Mother(void)
{

// this function scans the background under the mothership

if (mother.state == MOTHER_ALIVE)
   {

   Behind_Sprite_DB((sprite_ptr)&mother);

   } // end if alive

} // end Behind_Mother

//////////////////////////////////////////////////////////////////////////////

void Draw_Mother(void)
{

// this function draws the mothership

if (mother.state == MOTHER_ALIVE)
   {

   Draw_Sprite_DB((sprite_ptr)&mother);

   } // end if alive

} // end Draw_Mother

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

////////////////////////////////////////////////////////////////////////////////

void Move_Stars(void)
{

int index;


// move the star fields

for (index=0; index<NUM_STARS; index++)
    {

    // move the star and test for off screen condition

    // each star is in a different plane so test which plane star is
    // in so that proper velocity may be used

    switch(stars[index].plane)
          {
          case PLANE_1: // the slowest plane
               {
               stars[index].y+=velocity_1;
               } break;

          case PLANE_2: // the medium speed plane
               {
               stars[index].y+=velocity_2;
               } break;

          case PLANE_3: // the fastest plane (near)
               {
               stars[index].y+=velocity_3;
               } break;

           } // end switch

    // test if star went off screen

    if (stars[index].y > 179 )
       stars[index].y=(stars[index].y-180); // wrap around
    else
    if (stars[index].y < 0) // off left edge?
        stars[index].y = (180+stars[index].y); // wrap around


    } // end for index

} // end Move_Stars

////////////////////////////////////////////////////////////////////////////////

void Behind_Stars(void)
{
// this function scans the background under the stars

int index;

for (index=0; index<NUM_STARS; index++)
    {

    stars[index].back = Get_Pixel_DB(stars[index].x,stars[index].y);

    } // end for index

} // end Behind_Stars

////////////////////////////////////////////////////////////////////////////////

void Draw_Stars(void)
{

// this function draws the stars

int index;

for (index=0; index<NUM_STARS; index++)
    {

    Plot_Pixel_Fast_DB(stars[index].x,stars[index].y,stars[index].color);

    } // end for index

} // end Draw_Stars

////////////////////////////////////////////////////////////////////////////////

void Erase_Stars(void)
{
// this function erases the stars

int index;

for (index=0; index<NUM_STARS; index++)
    {

    Plot_Pixel_Fast_DB(stars[index].x,stars[index].y,stars[index].back);

    } // end for index

} // end Erase_Stars

//////////////////////////////////////////////////////////////////////////////

void Erase_Missiles(void)
{

// this function indexes through all the missiles and if they are active
// erases them by replacing the background color that was under them

int index;

for (index=0; index<NUM_MISSILES; index++)
    {

    // is this missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       Plot_Pixel_Fast_DB(missiles[index].x,missiles[index].y,missiles[index].back);

       } // end if alive

    } // end for index

} // end Erase_Missiles

/////////////////////////////////////////////////////////////////////////////

void Behind_Missiles(void)
{

// this function indexes through all the missiles and if they are active
// scans the background color that is behind them so it can be replaced later

int index;

for (index=0; index<NUM_MISSILES; index++)
    {

    // is this missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       missiles[index].back = Get_Pixel_DB(missiles[index].x,missiles[index].y);

       } // end if alive

    } // end for index

} // end Behind_Missiles

/////////////////////////////////////////////////////////////////////////////

void Draw_Missiles(void)
{

// this function indexes through all the missiles and if they are active
// draws the missile as a bright white pixel on the screen

int index;

for (index=0; index<NUM_MISSILES; index++)
    {

    // is this missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       Plot_Pixel_Fast_DB(missiles[index].x,missiles[index].y,missiles[index].color);

       } // end if alive

    } // end for index

} // end Draw_Missiles

/////////////////////////////////////////////////////////////////////////////

void Move_Missiles(void)
{

// this function moves the missiles and does all the collision detection

int index,     // used for loops
    index2,
    index_3,
    pixel_y,   // used during barrier collsion scan
    delta_x,   // used to help test for bouding box collisions
    delta_y,
    miss_x,   // position of missile
    miss_y,
    creature_hit=0; // flag to jump out of loop when a creature has been hit

unsigned char pixel; // pixel extracted from screen used to test for barrier
                     // collisions
mech_ptr worker;     // the current mech being processed


// loop thru all missiles and perform a lot of tests

for (index=0; index<NUM_MISSILES; index++)
    {

    // is missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       // move the missile

       miss_x = (missiles[index].x += missiles[index].xv);
       miss_y = (missiles[index].y += missiles[index].yv);

      // test if it's hit the edge of the screen or a wall

      if ( (miss_x >= SCREEN_WIDTH) || (miss_x <= 0) ||
          (miss_y > (SCREEN_HEIGHT-16)) || ( miss_y <=0) )
         {
         missiles[index].state = MISS_DEAD;
         continue;

         } // end if off edge of screen

       // test for player->creature collisions
       else
       if (mother.state == MOTHER_ALIVE && missiles[index].tag==PLAYER_MISSILE)
          {

          delta_x = miss_x - mother.x;
          delta_y = miss_y - mother.y;

          // test the bounding box

          if ( (delta_x >= 0 && delta_x <=sprite_width) &&
               (delta_y >= 0 && delta_y <=sprite_height))
             {

             // kill missile

             missiles[index].state = MISS_DEAD;

             // kill mother

             mother.state = MOTHER_DEAD;

             // start explosion

             Start_Explosion(mother.x+10, mother.y+10,1);

             // give the player some points

             player_score+=500;

             // this missile is done so move to next missile

             continue;

             } // end if a hit

          } // end if mother alive

          // test if missiles hit a creature

          creature_hit = 0; // reset this flag

          // make sure missole is players

          if (missiles[index].tag==PLAYER_MISSILE)
             {

             // this missile is from player do test it against all mechs

             for (index_3=0; index_3<num_mechs && !creature_hit; index_3++)
                 {

                 // extract the working mech

                 worker=(mech_ptr)&mech_array[index_3];


                 // test if the mech is a live and it isn't energizing

                 if (worker->state_1==MECH_ALIVE && energize_state>1)
                    {

                    // compute deltas

                    delta_x = miss_x - worker->x;
                    delta_y = miss_y - worker->y;


                    // test for collision

                    if ( (delta_x >= 0 && delta_x <=sprite_width) &&
                         (delta_y >= 0 && delta_y <=sprite_height))
                       {

                       // kill missile

                       missiles[index].state = MISS_DEAD;

                       // kill mech

                       worker->state_1     = MECH_DYING;
                       worker->counter_1   = 0;
                       worker->threshold_1 = 8;

                       creature_hit=1;

                       Play_Sound(SOUND_EXPL3);

                       // start explosion

                       player_score+=50;

                       } // end if a hit

                    } // end if worth testing

                 } // end for index_3

             } // end if missile was from player

       // if there was a hit no need to go any further, next itteration

          if (creature_hit)
             continue;

       // test for creature->player collisions

       if (player.state == PLAYER_ALIVE && missiles[index].tag==ENEMY_MISSILE)
          {

          // compute deltas

          delta_x = miss_x - player.x;
          delta_y = miss_y - player.y;

          // test for collision

          if ( (delta_x >= 0 && delta_x <=sprite_width) &&
               (delta_y >= 0 && delta_y <=sprite_height))
             {

             // reset state of player to dying

             player.state = PLAYER_DYING;

             player.anim_clock = 0;

             // decrease number of ships

             player_ships--;

             // kill missile

             missiles[index].state = MISS_DEAD;

             // start the players death

             Start_PDeath();

             } // end if player hit

          } // end if player is alive to be hit

       // test for barrier collisions by scanning the pixels in the near vicinity
       // to the torpedo

       for (pixel_y=miss_y; pixel_y<miss_y+8; pixel_y++)
           {

           pixel=Get_Pixel_DB(miss_x, pixel_y);

           if (pixel>=BARRIER_START_COLOR && pixel<=BARRIER_END_COLOR)
              {

              // kill missile

              missiles[index].state = MISS_DEAD;

              // start explosion

              Start_Explosion(miss_x, pixel_y,1);

              // smash barrier a bit

              for (index2=0; index2<25; index2++)
                  {

                  Plot_Pixel_Fast_DB(miss_x-4+rand()%8, pixel_y-4+rand()%8, 0);

                  } // end for

              break;

              } // end if barrier hit

           } // end for pixel_y

       } // end if missile alive

    } // end for index

} // end Move_Missiles

/////////////////////////////////////////////////////////////////////////////

void Start_Missile(sprite_ptr who,
                   int x,
                   int y,
                   int xv,
                   int yv,
                   int color,
                   int tag)
{

// this function scans through the missile array and tries to find one that
// isn't being used.  this function could be more efficient.

int index;

// scan for a useable missle

for (index=0; index<NUM_MISSILES; index++)
    {
    // is this missile free?

    if (missiles[index].state == MISS_DEAD)
       {

       // set up fields

       missiles[index].state = MISS_ALIVE;
       missiles[index].x     = x;
       missiles[index].y     = y;
       missiles[index].xv    = xv;
       missiles[index].yv    = yv;
       missiles[index].color = color;
       missiles[index].back  = Get_Pixel_DB(x,y);
       missiles[index].tag   = tag;

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Missile

/////////////////////////////////////////////////////////////////////////////

void Init_Missiles(void)
{
// this function just makes sure all the "state" fields of the missiles are
// dead so that we don't get any strays on start up.  Remember never assume
// that variables are zeroed on instantiation!

int index;

for (index=0; index<NUM_MISSILES; index++)
    missiles[index].state = MISS_DEAD;

} // Init_Missiles

////////////////////////////////////////////////////////////////////////////

void Start_Explosion(int x,int y,int speed)
{
// this function stars a generic explosion
int index;

// scan for a useable explosion

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    if (explosions[index].state == EXPLOSION_DEAD)
       {

       // set up fields

       explosions[index].state      = EXPLOSION_ALIVE;
       explosions[index].x          = x-10;
       explosions[index].y          = y-10;
       explosions[index].curr_frame = 0;
       explosions[index].anim_speed = speed;
       explosions[index].anim_clock = 0;

       // scan background to be safe

       Behind_Sprite_DB((sprite_ptr)&explosions[index]);

       // make sound

       Play_Sound(SOUND_EXPL2);

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Explosion

/////////////////////////////////////////////////////////////////////////////

void Behind_Explosions(void)
{

// this function scans under the explosions

int index;

// scan for a running explosions

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    if (explosions[index].state == EXPLOSION_ALIVE)
       {
       Behind_Sprite_DB((sprite_ptr)&explosions[index]);

       } // end if found a good one

    } // end for index

} // end Behind_Explosions

/////////////////////////////////////////////////////////////////////////////

void Erase_Explosions(void)
{
// this function erases all the current explosions
int index;

// scan for a useable explosion

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    if (explosions[index].state == EXPLOSION_ALIVE)
       {
       Erase_Sprite_DB((sprite_ptr)&explosions[index]);

       } // end if found a good one

    } // end for index

} // end Erase_Explosions

/////////////////////////////////////////////////////////////////////////////

void Draw_Explosions(void)
{
// this function draws the explosion

int index;

// scan for a useable explosion

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    // make sure this explosion is alive

    if (explosions[index].state == EXPLOSION_ALIVE)
       {
       Draw_Sprite_DB((sprite_ptr)&explosions[index]);

       } // end if found a good one

    } // end for index

} // end Draw_Explosions

/////////////////////////////////////////////////////////////////////////////

void Animate_Explosions(void)
{
// this function steps the explosion thru the frames of animation
int index;

// scan for a useable explosion

for (index=0; index<NUM_EXPLOSIONS; index++)
    {
    // test if explosion is alive
    if (explosions[index].state == EXPLOSION_ALIVE)
       {
       // test if it's time to change frames

       if (++explosions[index].anim_clock == explosions[index].anim_speed)
          {
          // is the explosion over?

          if (++explosions[index].curr_frame == 6)
             explosions[index].state = EXPLOSION_DEAD;

          // reset animation clock for future

          explosions[index].anim_clock = 0;

          } // end if time ti change frames

       } // end if found a good one

    } // end for index

} // end Animate_Explosions

//////////////////////////////////////////////////////////////////////////////

void Init_Explosions(void)
{
// reset all explosions

int index;

for (index=0; index<NUM_EXPLOSIONS; index++)
    explosions[index].state = EXPLOSION_DEAD;

} // Init_Explosions

////////////////////////////////////////////////////////////////////////////

void Display_Instruments(void)
{
// this function draws all the information on the control panel

char buffer[128];

// show the ships

sprintf(buffer,"%ld ",player_ships);

Blit_String_DB(42,189,10,buffer,0);

// show the score

sprintf(buffer,"%ld",player_score);

Blit_String_DB(142,189,10,buffer,0);

// show the energy

if (player_energy>=0)
   {
   sprintf(buffer,"%ld ",player_energy);
   Blit_String_DB(276,189,10,buffer,0);
   }
else
   Blit_String_DB(276,189,12,"CHRG",0);

} // end Display_Instruments

///////////////////////////////////////////////////////////////////////////

void Erase_Instruments(void)
{

// this function erases the instruments

Blit_String_DB(42,189,0," ",0);

Blit_String_DB(142,189,0,"      ",0);

Blit_String_DB(276,189,0,"    ",0);

} // end Erase_Instruments

///////////////////////////////////////////////////////////////////////////

void Load_Background(void)
{
// this function loads in thre background imagery

// load in the background image into the double buffer

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load("mechback.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

} // end Load_Background

///////////////////////////////////////////////////////////////////////////

void Do_Intro(void)
{

// this function does the introduction and the instructions

// load intro screen and display for a few secs.

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("mechint.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

// let user see it

Delay(50);

Fade_Lights();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

// load instructions and wait for key press

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("mechins.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

// let user see it

getch();

Melt();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

} // end Do_Intro

///////////////////////////////////////////////////////////////////////////

void Load_Player(void)
{

// this function loads in the imagery an initializes the mech


// load in imagery for player

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("mech.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize player and extract birtmaps

sprite_width  = 18;
sprite_height = 18;

Sprite_Init((sprite_ptr)&player,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,0,0,3);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,1,1,3);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,2,2,3);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,3,3,3);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,4,4,3);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,5,5,3);

player.x          = 160;
player.y          = 168;
player.curr_frame = 0;
player.state      = PLAYER_ALIVE;


} // end Load_Player

///////////////////////////////////////////////////////////////////////////

void Load_Mother(void)

{
// this function loads up the imagery for the mother ship
// load up mother ship

Sprite_Init((sprite_ptr)&mother,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&mother,0,0,4);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&mother,1,1,4);

mother.x          = 0;
mother.y          = 0;
mother.curr_frame = 0;
mother.state      = MOTHER_DEAD;


} // end Load_Mother

///////////////////////////////////////////////////////////////////////////

void Load_Explosions(void)
{
// this function loads in the imagery for the explosions

int index;

// load data for explosions


// load in frames for fire

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    Sprite_Init((sprite_ptr)&explosions[index],0,0,0,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],0,6,3);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],1,7,3);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],2,8,3);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],3,9,3);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],4,10,3);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],5,11,3);

    } // end for

} // end Load_Explosions

///////////////////////////////////////////////////////////////////////////

void Load_Mechs(void)
{
// this function loads the imagery for the mechs

int index;

// load in images for robots

// initialize the sprites used for mechs

Sprite_Init((sprite_ptr)&robot_1,0,0,0,0,0,0);
Sprite_Init((sprite_ptr)&robot_2,0,0,0,0,0,0);
Sprite_Init((sprite_ptr)&robot_3,0,0,0,0,0,0);

// zer these fields out for good measure

robot_1.x          = 0;
robot_1.y          = 0;
robot_1.curr_frame = 0;

robot_2.x          = 0;
robot_2.y          = 0;
robot_2.curr_frame = 0;

robot_3.x          = 0;
robot_3.y          = 0;
robot_3.curr_frame = 0;

// extract all the bitmaps out of pcx file

for (index=0; index<NUM_ROBOT_FRAMES; index++)
    {


    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&robot_1,index,index,0);


    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&robot_2,index,index,1);


    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&robot_3,index,index,2);


    } // end for index

// delete imagery file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);


} // end Load_Mechs

///////////////////////////////////////////////////////////////////////////

void main(int argc,char **argv)
{
// this is the main function

int done=0,       // exit flag for whole system
    fired=0,      // used to track if a player has fired a missile
    index;        // used for loop variable

char buffer[80];  // buffer to hold strings during printing

printf("\nStarting mechs...");

// initialize sound system

sound_available = Initialize_Sound_System();

Delay(100);

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// create a double buffer

if (!Create_Double_Buffer(SCREEN_HEIGHT))
   {
   printf("\nNot enough memory to create double buffer.");

   } // end if

// create cosine look up table

for (index=0; index<320; index++)
    cos_look[index] = (int)(8*cos(3.14159*5*(float)index/180));

// clear the double buffer

Fill_Double_Buffer(0);

// load the background imagery

Load_Background();

// do the introduction

Do_Intro();

// install our ISR

Old_Isr = _dos_getvect(KEYBOARD_INT);

_dos_setvect(KEYBOARD_INT, New_Key_Int);

// load the player

Load_Player();

// load the mechs

Load_Mother();

// load the explosions

Load_Explosions();

// load the mechs

Load_Mechs();

// set up the current wave

waves[0]  = wave_0;
waves[1]  = wave_1;
waves[2]  = wave_2;
waves[3]  = wave_3;
waves[4]  = wave_4;
waves[5]  = wave_5;
waves[6]  = wave_6;
waves[7]  = wave_7;
waves[8]  = wave_8;
waves[9]  = wave_9;
waves[10] = wave_10;
waves[11] = wave_11;
waves[12] = wave_12;
waves[13] = wave_13;
waves[14] = wave_14;

current_wave = waves[wave_number];

// initialize all mechs

Init_Mechs();

// initialize starfield

Init_Stars();

// initialize the explosions

Init_Explosions();

// start the level off

Start_Wave();

// scan under mechs

Behind_Mechs();

// scan the background under player

Behind_Sprite_DB((sprite_ptr)&player);

// scan under the stars

Behind_Stars();

// display instruments for first time

Display_Instruments();

// main event loop

while(!done)
     {

     // reset all event variables

     fired = 0 ;

     // erase all objects

     Erase_Sprite_DB((sprite_ptr)&player);

     Erase_Missiles();

     Erase_Stars();

     Erase_Mother();

     Erase_Mechs();

     Erase_PDeath();

     Erase_Explosions();

     Erase_Instruments();

     // move all objects and trasnform

     if (player.state==PLAYER_ALIVE)
        {

        // wait key was pressed

        if (key_table[INDEX_RIGHT])
           {
           player.x+=PLAYER_X_MOVE;

           if (player.x > SCREEN_WIDTH-20)
               player.x = SCREEN_WIDTH-20;

           } // end if right

        else
        if (key_table[INDEX_LEFT])
           {

           player.x-=PLAYER_X_MOVE;

           if (player.x < 0)
              player.x = 0;

           } // end if  left


        if (key_table[INDEX_Q])
           {
           done=1;

           } // end if q


        if (key_table[INDEX_SPACE])
           {
           if (player_gun_state == PLAYER_NOT_FIRING && player_energy>0)
              {

              player_gun_state = PLAYER_FIRING;
              fired = 1;

              player_energy -= (5+rand()%2);

              if (player_energy < -100)
                 player_energy=-100;

              } // end if not currently firing

           } // end if space

        } // end if player alive

     // move missiles

     Move_Missiles();

     Move_Mother();

     Move_Stars();

     Move_Mechs();

     Animate_PDeath();

     Animate_Explosions();

     // critical area

     // do cannon retraction if player has fired

     if (fired)
        {
        Start_Missile((sprite_ptr)&player,
                      player.x+2,
                      player.y+4,
                      0,
                      -8,
                      12,
                      PLAYER_MISSILE);

        Start_Missile((sprite_ptr)&player,
                      player.x+15,
                      player.y+4,
                      0,
                      -8,
                      12,
                      PLAYER_MISSILE);

        Play_Sound(SOUND_MISSILE);

        } // end if fired

     // do mother ship

     Control_Mother();

     // start new wave here

     if (mechs_killed==num_mechs)
        {
        // next wave

        wave_number++;

        Start_Wave();

        // reset mech counter

        mechs_killed=0;

        } // end if start next wave

     // scan background

     Behind_Sprite_DB((sprite_ptr)&player);

     Behind_Missiles();

     Behind_Mother();

     Behind_Mechs();

     Behind_Stars();

     Behind_Explosions();

     Behind_PDeath();

     // draw objects

     // flicker engine

     if (player.state==PLAYER_ALIVE)
     {
     if (player_gun_state==PLAYER_NOT_FIRING)
        {

        player.curr_frame = rand()%2;

        if (++player_energy > 100) player_energy = 100;


        } // end if not firing
     else
        {
        if (++player.curr_frame > 4)
           {
           player.curr_frame = 0;
           player_gun_state = PLAYER_NOT_FIRING;
           } // end if done retracting

        } // end else player is firing so retract

     } // end if player alive
   else
      player.curr_frame = 5;

     // draw everything

     Draw_Sprite_DB((sprite_ptr)&player);

     Draw_Missiles();

     Draw_Stars();

     Draw_Explosions();

     Draw_Mother();

     Draw_Mechs();

     Draw_PDeath();

     Display_Instruments();

     Blink_Lights();

     // place double buffer

     Show_Double_Buffer(double_buffer);

     // test if player is dead

     if (player_ships==0 && player.anim_clock >= PLAYER_DEATH_TIME)
        {

        sprintf(buffer,"G A M E  O V E R");

        Blit_String_G(160-64,60,4,buffer,1);

        sprintf(buffer,"Final Score %ld",player_score);

        Blit_String_G(160-64,80,1,buffer,1);

        done=1;

        } // end if player done

     // display header for level number

     else
     {
     if (energize_state==1)
        {

        sprintf(buffer,"W A V E  %d",wave_number+1);

        Blit_String_G(160-40,60,10,buffer,1);

        } // end if need to display level
     else
     if (energize_state==2)
        {

        // erase words

        Blit_String_G(160-40,60,0,"             ",0);

        // move on to sequence over

        energize_state++;

        } // end if energize over

     } // end else ok to put up wave

     // wait a second

     // test if this is a really fast machine and either
     // wait 1/70 of a sec or 1/8 of second

     if (argc > 1)
        Delay(1);
     else
        Wait_For_Vsync();

     // update all global counters and timers

     game_clock++;

     } // end while

// play latter sound

Play_Sound(SOUND_END);

Delay(100);

// use one of screen fx as exit

Fade_Lights();

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// delete all the mech data

Delete_Mechs();

// free the double buffer

Delete_Double_Buffer();

// replace old ISR

_dos_setvect(KEYBOARD_INT, Old_Isr);

// close sound system

Close_Sound_System();

// get the hell out of here!!!!!

} // end main


