
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
#include "graph9.h"   // sound library
#include "graph11.h"  // multitasking and keyboard interrupt driver

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Start_Explosion(int x,int y,int speed);

void Behind_Explosions(void);

void Erase_Explosions(void);

void Draw_Explosions(void);

void Animate_Explosions(void);

void Init_Explosions(void);

void Init_Monsters(void);

void Start_Monsters(int wx,int wy);

void Save_Monsters(int wx,int wy);

void Erase_Monsters(void);

void Behind_Monsters(void);

void Draw_Monsters(void);

int Start_Monster(int x,int y);

void Move_Monsters(void);

void Start_Arrow(int x,
                 int y,
                 int xv,
                 int yv,
                 int direction);

void Erase_Arrows(void);

void Behind_Arrows(void);

void Draw_Arrows(void);

void Move_Arrows(void);

void Init_Arrows(void);

void Start_Fireball(int x,
                    int y,
                    int xv,
                    int yv);

void Erase_Fireballs(void);

void Behind_Fireballs(void);

void Draw_Fireballs(void);

void Move_Fireballs(void);

void Init_Fireballs(void);


void Erase_Bat(void);

void Behind_Bat(void);

void Draw_Bat(void);

void Move_Bat(void);

void Control_Bat(void);

unsigned char Get_Pixel_DB(int x,int y);

int Initialize_Sound_System(void);

void Close_Sound_System(void);

void Play_Sound(int sound);

void Do_Intro(void);

// D E F I N E S /////////////////////////////////////////////////////////////

#define DEBUG           0   // turns on/off debug information

// defines for players weapon

#define ARROW_ALIVE           1 // state of a live arrow
#define ARROW_DEAD            0 // state of a dead arrow
#define NUM_ARROWS            3 // number of arrows that can be fired

// defines for monsters weapons

#define FIREBALL_ALIVE        1  // state of a live fireball
#define FIREBALL_DEAD         0  // state of a dead fireball
#define NUM_FIREBALLS         8  // number of fireballs that can be fired

#define FIREBALL_SPEED        6  // the velocity of a fireball

#define NUM_FIREBALL_FRAMES   3  // number of frames of animation in fireball
                                 // sequence
// general explosions

#define NUM_EXPLOSIONS       3  // number of explosions that can run at once

#define EXPLOSION_DEAD       0   // state of a dead explosion
#define EXPLOSION_ALIVE      1   // state of a live explosion

#define RED_BASE 32              // start of the reds in default palette

// game area defines

#define CELL_ROWS       12  // number of rows in the cell world
#define CELL_COLUMNS    14  // number of columns in the cell world

#define NUM_MONSTERS    8  // maximum number of monsters per screen

#define NUM_MONSTER_FRAMES 16 // number of animation frames

#define WORLD_ROWS      4   // the world is a 4x4 matrix of screens or a
#define WORLD_COLUMNS   4   // total of 16 screens
                            //
                            //  .... player can be within 1 of 16 screens
                            //  ....
                            //  ....  '.' represents a screen
                            //  ....

// defines for monsters

#define MONSTER_DEAD     0   // the monster is dead
#define MONSTER_STILL    1   // the monster is sitting still thinking
#define MONSTER_TURN     2   // the monster is sitting and turning
#define MONSTER_CHASE    3   // the monster is chasing the player
#define MONSTER_EVADE    4   // the monster is evading the player
#define MONSTER_RANDOM   5   // the monster is moving in a random pre-selected
                             // direction

#define MONSTER_DYING    6   // the monster is dying

// directions for the monsters

#define MONSTER_EAST     0
#define MONSTER_WEST     1
#define MONSTER_NORTH    2
#define MONSTER_SOUTH    3

// defines for bat

#define BAT_DEAD        0   // the bat is dead, long live joker!
#define BAT_WAVE        1   // the bat is doing a sinwave across the screen
#define BAT_RANDOM      2   // the bat is moving in a random direction across
                            // the screen

#define NUM_BAT_FRAMES  5   // number of bat animation frames

// player defines

#define PLAYER_DEAD     0
#define PLAYER_ALIVE    1
#define PLAYER_DYING    2

// direction of player

#define PLAYER_EAST     0
#define PLAYER_WEST     1
#define PLAYER_NORTH    2
#define PLAYER_SOUTH    3

// bitmap id's for special objects

#define FLOOR_1_ID      32
#define FLOOR_2_ID      33

#define SILVER_ID       34
#define GOLD_ID         35
#define POTION_ID       36
#define FOOD_ID         37
#define ARROWS_ID       38
#define EXIT_ID         39
#define DAGGER_ID       40

// sound system defines

#define NUM_SOUNDS           16     // number of sounds in system

#define SOUND_HURT           0      // archer is hurt
#define SOUND_MDIE           1      // a monster is dying
#define SOUND_HEALTH         2      // low health
#define SOUND_MFIRE          3      // a monster firing
#define SOUND_MONEY          4      // the sound of money
#define SOUND_EAT            5      // archer eating
#define SOUND_FARROW         6      // finding arrows
#define SOUND_POTION         7      // finding a potion
#define SOUND_ARROW          8      // an arrow hiting a monster
#define SOUND_BOW            9      // the sound of the bow
#define SOUND_BAT            10     // the sound of the bat
#define SOUND_INTRO          11     // the introduction
#define SOUND_DAGGER         12     // player finding dagger
#define SOUND_END            13     // it's all over
#define SOUND_DEATH          14     // the player being killed
#define SOUND_GOAL           15

#define SOUND_DEFAULT_PORT   0x220  // default sound port for sound blaster
#define SOUND_DEFAULT_INT    5      // default interrupt

// M A C R O S //////////////////////////////////////////////////////////////

#define SET_SPRITE_SIZE(w,h) {sprite_width=w; sprite_height=h;}

// S T R U C T U R E S ///////////////////////////////////////////////////////


// this is used to track the position of a monster and it's state when the
// screen is first paged to and to recall when the screen is left and
// revisted so that mosnters that are killed stay dead and so forth

typedef struct monster_pos_typ
        {

        int state;  // state of monster
        int x,y;    // position of the monster is pixel coordinates

        } monster_pos, *monster_pos_ptr;

// this is the data structure that holds a single screen

typedef struct screen_typ
        {

        int cells[CELL_ROWS][CELL_COLUMNS];  // the data storage for cell id's

        int num_monsters;                    // number of monsters in the screen

        monster_pos positions[NUM_MONSTERS]; // this holds the state and positions
                                             // of the monsters on entry and exit

        } screen, *screen_ptr;

// typedef for a projectile

typedef struct projectile_typ
        {
        int x;                   // x position
        int y;                   // y position
        int xv;                  // x velocity
        int yv;                  // y velocity
        int state;               // the state of the particle
        int counter;             // use for counting
        int threshold;           // the counters threshold

        int counter_2;
        int threshold_2;

        sprite object;           // the projectile sprite

        } projectile, *projectile_ptr;

// typedef for the monster

typedef struct monster_typ
        {
        int x;                   // x position
        int y;                   // y position
        int xv;                  // x velocity
        int yv;                  // y velocity
        int state;               // the state of the monster
        int direction;
        int counter;             // use for counting
        int threshold;           // the counters threshold

        int counter_2;
        int threshold_2;

        int counter_3;
        int threshold_3;

        sprite object;           // the monster sprite

        } monster, *monster_ptr;

// G L O B A L S  ////////////////////////////////////////////////////////////

pcx_picture imagery_pcx,      // the game imagery
            background_pcx,   // the backdrop
            intro_pcx;        // the introduction screen

// the sprites used in the game

sprite player,                // the player
       wall_1,                // holds the blue walls bitmaps
       wall_2,                // holds the green walls bitmaps
       floors;                // holds the floor bitmaps


sprite explosions[NUM_EXPLOSIONS];   // the array of explosions

projectile arrows[NUM_ARROWS];   // the array of arrows in the world

projectile fireballs[NUM_FIREBALLS];  // the array of fireballs in the world

projectile bat;                  // the bat

monster monsters[NUM_MONSTERS];  // the monsters

// velocity look up tables for players arrow, east, west, north and south

int arrow_vel_x[4] = {6,-6,0,0};
int arrow_vel_y[4] = {0,0,-6,6};

int cos_look[320];   // cosine lookup table

int sqrt_table[400]; // square root look up

// players inventory

long players_score = 0;    // initial score

int players_dir    = PLAYER_SOUTH;  // initial direction of player

int silver_pieces  = 0;    // amount of silver pieces player has

int gold_pieces    = 0;    // amount of gold pieces player has

int number_potions = 0;    // number of magical potions (smart bombs)

int health         = 100;   // start health off at 100 percent

int num_arrows     = 50;   // player has 50 arrows to start with

int dagger_found   = 0;    // flags if the player has found the dagger

int weak_counter   = 350;  // used to count how many cycles before
                           // verbal health message is repeated

int start_death    = 0;    // flags if players death sequence has started


// the world

screen world[WORLD_ROWS][WORLD_COLUMNS];  // the game world

int screen_x      = 0,        // current active screen coordinates
    screen_y      = 0,
    old_screen_x  = 0,        // last active screen coords
    old_screen_y  = 0,
    screen_change = 0;        // has there been a screen change

// the sound system variables

char far *sound_fx[NUM_SOUNDS];           // pointers to the voc files
unsigned char sound_lengths[NUM_SOUNDS];  // lengths of the voc files

int sound_available = 0;                  // flags if sound is available

int sound_port = SOUND_DEFAULT_PORT;      // default sound port
int sound_int  = SOUND_DEFAULT_INT;       // default sound interrupt

// F U N C T I O N S //////////////////////////////////////////////////////////

void Start_Explosion(int x,int y,int speed)
{
// this function stars a generic explosion

int index;

SET_SPRITE_SIZE(16,16);

// scan for a useable explosion

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    if (explosions[index].state == EXPLOSION_DEAD)
       {

       // set up fields

       explosions[index].state      = EXPLOSION_ALIVE;
       explosions[index].x          = x;
       explosions[index].y          = y;
       explosions[index].curr_frame = 0;
       explosions[index].anim_speed = speed;
       explosions[index].anim_clock = 0;

       // scan background to be safe

       Behind_Sprite_DB((sprite_ptr)&explosions[index]);

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Explosion

/////////////////////////////////////////////////////////////////////////////

void Behind_Explosions(void)
{

// this function scans under the explosions

int index;

SET_SPRITE_SIZE(16,16);

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

SET_SPRITE_SIZE(16,16);

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

SET_SPRITE_SIZE(16,16);

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

          if (++explosions[index].curr_frame == 4)
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

/////////////////////////////////////////////////////////////////////////////

void Create_Lookups(void)
{
// this function creates all the look up tables

int index; // loop index

// create cosine look up table

for (index=0; index<320; index++)
    cos_look[index] = (int)(4*cos(3.14159*5*(float)index/180));

// create sqrt table

for (index=0; index<400; index++)
    sqrt_table[index] = (int)((float).5 + (float)sqrt(index));

} // end Create_Lookups

///////////////////////////////////////////////////////////////////////////////

void Erase_Monsters(void)
{

// this function indexes through all the monsters and if they are active
// erases them by replacing the background color that was under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(16,16);

for (index=0; index<NUM_MONSTERS; index++)
    {

    // is this monster active

    if (monsters[index].state != MONSTER_DEAD)
       {

       // extract proper parameters

       monsters[index].object.x = monsters[index].x;
       monsters[index].object.y = monsters[index].y;

       // erase the sprite

       Erase_Sprite_DB((sprite_ptr)&monsters[index].object);

       } // end if alive

    } // end for index

} // end Erase_Monsters

/////////////////////////////////////////////////////////////////////////////

void Behind_Monsters(void)
{

// this function indexes through all the monsters and if they are active
// scans the background under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(16,16);

for (index=0; index<NUM_MONSTERS; index++)
    {

    // is this monster active

    if (monsters[index].state != MONSTER_DEAD)
       {

       // extract proper parameters

       monsters[index].object.x = monsters[index].x;
       monsters[index].object.y = monsters[index].y;

       // scan behind the sprite

       Behind_Sprite_DB((sprite_ptr)&monsters[index].object);

       } // end if alive

    } // end for index

} // end Behind_Monsters

/////////////////////////////////////////////////////////////////////////////

void Draw_Monsters(void)
{

// this function indexes through all the monsters and if they are active
// draws them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(16,16);

for (index=0; index<NUM_MONSTERS; index++)
    {

    // is this monster active

    if (monsters[index].state != MONSTER_DEAD)
       {

       // extract proper parameters

       monsters[index].object.x = monsters[index].x;
       monsters[index].object.y = monsters[index].y;

       // draw the sprite (test for dying case i.e. melting blitter)

       if (monsters[index].state==MONSTER_DYING)
          Draw_Sprite_DBM((sprite_ptr)&monsters[index].object);
       else
          Draw_Sprite_DB((sprite_ptr)&monsters[index].object);

       } // end if alive

    } // end for index

} // end Draw_Monsters

/////////////////////////////////////////////////////////////////////////////

void Move_Monsters(void)
{
// this function moves all the monsters

// defines the animation cells to be used when the monster is spining

static int spin_table[4] = {8,4,8,0};

int index,          // loop index
    change_state,   // flags if a state change is needed for the current monster
    mx,my,          // position of monster
    mdx,mdy,        // amount to move monster
    pdx,pdy,        // delta from player
    cell_x,cell_y,  // cell positions
    cell_id,        // cell id number
    animate=0;      // tracks if its time to animate monster

float radicand;

// scan through monster array and test for actives

for (index=0; index<NUM_MONSTERS; index++)
    {
    // is this a live one

    if (monsters[index].state != MONSTER_DEAD)
       {

       // reset state change flag

       change_state = 0;

       mdx=mdy=0;

       // process monster

       switch(monsters[index].state)
             {

             case MONSTER_DYING:

             case MONSTER_STILL: // do nothing
                  {

                  // is it time to change state

                  // do nothing!

                  mx = monsters[index].x+8;
                  my = monsters[index].y+14;

                  mdx=mdy=0;

                  // reset animate flag

                  animate=0;

                  } break;

             case MONSTER_TURN: // turn the monster around
                  {


                  if (++monsters[index].counter >= monsters[index].threshold)
                     {
                     // reset counter

                     monsters[index].counter = 0;

                     // increment frame to make monster look like it's spinning

                     monsters[index].object.curr_frame
                     = spin_table[monsters[index].counter_3];

                     if (++monsters[index].counter_3 > 3)
                        monsters[index].counter_3 = 0;

                     } // end if end change frames

                  mx = monsters[index].x+8;
                  my = monsters[index].y+14;

                  mdx=mdy=0;

                  // reset animate flag

                  animate=0;

                  } break;


             case MONSTER_CHASE: // the monster is tracking the player
                  {

                  mx = monsters[index].x+8;
                  my = monsters[index].y+14;

                  // is is time to process monster

                  if (++monsters[index].counter >= monsters[index].threshold)
                     {
                     // reset counter

                     monsters[index].counter = 0;

                     if (player.x+8 > mx)
                        mdx=2;
                     else
                     if (player.x+8 < mx)
                        mdx=-2;

                     if (player.y+14 > my)
                        mdy=2;
                     else
                     if (player.y+14 < my)
                        mdy=-2;

                     // set animate flag

                     animate=1;

                     } // end if end process monster

                  } break;

             case MONSTER_EVADE: // the monster is evading the player
                  {
                  // obtain center of monster

                  mx = monsters[index].x+8;
                  my = monsters[index].y+14;

                  // is is time to process monster

                  if (++monsters[index].counter >= monsters[index].threshold)
                     {
                     // reset counter

                     monsters[index].counter = 0;

                     // test where player is relative to monster and create
                     // trajectory vector

                     if (player.x+8 < mx)
                        mdx=2;
                     else
                     if (player.x+8 > mx)
                        mdx=-2;
                     else
                        mdx=0;

                     if (player.y+14 < my)
                        mdy=2;
                     else
                     if (player.y+14 > my)
                        mdy=-2;
                     else
                        mdy=0;

                     // set animate flag

                     animate=1;

                     } // end if process monster

                  } break;

             case MONSTER_RANDOM: // the monster has selected a random direction
                  {
                  // obtain center of monster

                  mx = monsters[index].x+8;
                  my = monsters[index].y+14;

                  mdx=mdy=0;

                  // is is time to process monster

                  if (++monsters[index].counter >= monsters[index].threshold)
                     {
                     // reset counter

                     monsters[index].counter = 0;

                     // compute translation factors

                     mdx = monsters[index].xv;
                     mdy = monsters[index].yv;

                     // set animate flag

                     animate=1;

                     } // end if end process monster

                  } break;

             default: break;

             } // end switch


       // try and fire a fireball

       if (rand()%60==1)
          {

          // compute trajectory vector

          pdx = player.x - monsters[index].x;
          pdy = player.y - monsters[index].y;

          // make sure the monster is not on top of player this would cause
          // a zero in the denominator and a pole to infinity

          if ((pdx+pdy)>0)
             {

             // normalize vector and multiple by speed

             radicand = (float)pdx*(float)pdx+(float)pdy*(float)pdy;

             pdx = (int)(.5+((float)FIREBALL_SPEED*(float)pdx)/sqrt(radicand));
             pdy = (int)(.5+((float)FIREBALL_SPEED*(float)pdy)/sqrt(radicand));

             // using computed trajectory vector throw a fireball at
             // player

             Start_Fireball(monsters[index].x+8,monsters[index].y+8,
                            pdx,pdy);

            } // end if ok to start a fireball

          } // end if fire a fireball

       // do horizontal translation

       mx+=mdx;
       monsters[index].x+=mdx;

       // test if a object has been hit

       cell_x = mx >> 4;
       cell_y = my >> 4;

       cell_id = world[screen_y][screen_x].cells[cell_y][cell_x];

       // test for horizontal collisions

       if (mx>224-8 || mx < 8 || cell_id < 32)
          {
          mx-=mdx;
          monsters[index].x-=mdx;
          }

       // do vertical translation

       my+=mdy;
       monsters[index].y+=mdy;

       // test if a object has been hit

       cell_x = mx >> 4;
       cell_y = my >> 4;

       cell_id = world[screen_y][screen_x].cells[cell_y][cell_x];

       // test for horizontal collisions

       if (my>180-8 || my < 8 || cell_id < 32)
          {
          my-=mdy;
          monsters[index].y-=mdy;
          } // end if a horizontal collision

       // test if the monster is on top of the player

       if ( monsters[index].state!=MONSTER_DYING &&
            (mx > player.x) && (mx < player.x+16) &&
            (my > player.y) && (my < player.y+16) )
          {
          // hurt player a little

          if (--health<0)
             health=0;

          // play a hurt sound

          if (rand()%5==1)
             Play_Sound(SOUND_HURT);

          } // end if player is bitting player

       // do animation of monster

       if (animate)
          {

          // move to next frame in sequence depending on direction of motion

          if ((mdx!=0 && mdy!=0) || (mdx!=0 && mdy==0 ))
             {

             if (mdx>0)
                {
                // display next frame of right motion

                if (++monsters[index].object.curr_frame > 3)
                   monsters[index].object.curr_frame = 0;
                } // end if moving right
             else
                {
                // monster moving left

                if (++monsters[index].object.curr_frame > 7)
                   monsters[index].object.curr_frame = 4;

                } // end else left motion

             } // end if diagonal motion
          else
          if (mdy!=0 && mdx==0)
             {

             if (mdy>0)
                {
                // display next frame of downward motion

                if (++monsters[index].object.curr_frame > 11)
                   monsters[index].object.curr_frame = 8;

                } // end if moving down
             else
                {
                // monster moving up

                if (++monsters[index].object.curr_frame > 15)
                   monsters[index].object.curr_frame = 12;

                } // end else upward motion

             } // end if vertical

          } // end if time to animate

       // test if a state change is needed

       if (++monsters[index].counter_2 >= monsters[index].threshold_2)
          {

          // we need to change state

          change_state=1;

          } // end if need to change state

       // test for a pre-emptive collision


       // test if there was a state change

       if (change_state)
          {

          // test if last state was dying

          if (monsters[index].state==MONSTER_DYING)
             {
             // kill the monster

             monsters[index].state=MONSTER_DEAD;

             continue;

             } // end if kill monster

          monsters[index].object.curr_frame = 8;

          // select a new state, note the use of random numbers to create
          // a probability density

          switch(rand()%10)
                {

                case 0: // 10% chance of monster still
                     {
                     monsters[index].xv           = 0;
                     monsters[index].yv           = 0;
                     monsters[index].state        = MONSTER_STILL;
                     monsters[index].direction    = 0;
                     monsters[index].counter      = 0;
                     monsters[index].threshold    = 1 + rand()%2;
                     monsters[index].counter_2    = 0;
                     monsters[index].threshold_2  = 10 + rand()%10;
                     monsters[index].object.curr_frame = 8;

                     } break;

                case 1: // 10% chance of monster turn
                     {
                     monsters[index].xv           = 0;
                     monsters[index].yv           = 0;
                     monsters[index].state        = MONSTER_TURN;
                     monsters[index].direction    = 0;
                     monsters[index].counter      = 0;
                     monsters[index].threshold    = 3 + rand()%3;
                     monsters[index].counter_2    = 0;
                     monsters[index].counter_3    = 0;
                     monsters[index].threshold_2  = 20 + rand()%20;
                     monsters[index].object.curr_frame = 8;

                     } break;


                case 2: // 20% chance of monster random
                case 3:
                     {

                     // select a direction

                     switch((monsters[index].direction = rand()%4))
                           {

                           case MONSTER_EAST:
                                {
                                monsters[index].xv = 2 + 2*rand()%2;

                                } break;

                           case MONSTER_WEST:
                                {
                                monsters[index].xv = -2 - 2*rand()%2;

                                } break;

                           case MONSTER_NORTH:
                                {
                                monsters[index].yv = -2 - 2*rand()%2;

                                } break;


                           case MONSTER_SOUTH:
                                {
                                monsters[index].yv = 2 + 2*rand()%2;

                                } break;

                           default: break;

                           } // end switch

                     // set up remaining fields

                     monsters[index].state        = MONSTER_RANDOM;
                     monsters[index].direction    = 0;
                     monsters[index].counter      = 0;
                     monsters[index].threshold    = 1 + rand()%2;
                     monsters[index].counter_2    = 0;
                     monsters[index].threshold_2  = 20 + rand()%20;
                     monsters[index].object.curr_frame = 8;

                     } break;

                case 4:     // 40% chance of monster chasing player
                case 5:
                case 6:
                case 7:
                     {
                     monsters[index].xv           = 0;
                     monsters[index].yv           = 0;

                     monsters[index].state        = MONSTER_CHASE;
                     monsters[index].direction    = 0;
                     monsters[index].counter      = 0;
                     monsters[index].threshold    = 1 + rand()%2;
                     monsters[index].counter_2    = 0;
                     monsters[index].threshold_2  = 100 + rand()%50;
                     monsters[index].object.curr_frame = 8;

                     } break;

                case 8: // 20% chance of monster evading player
                case 9:
                     {
                     monsters[index].xv           = 0;
                     monsters[index].yv           = 0;

                     monsters[index].state        = MONSTER_EVADE;
                     monsters[index].direction    = 0;
                     monsters[index].counter      = 0;
                     monsters[index].threshold    = 1 + rand()%2;
                     monsters[index].counter_2    = 0;
                     monsters[index].threshold_2  = 20 + rand()%20;
                     monsters[index].object.curr_frame = 8;

                     } break;

                default:break;

                } // end switch

          } // end if time to change state

       } // end if not dead

    } // end for index

} // end Move_Monsters

/////////////////////////////////////////////////////////////////////////////

int Start_Monster(int x, int y)
{

// this function is used to start a monster up

int index;

// find a monster that isn't being used

for (index=0; index<NUM_MONSTERS; index++)
    {

    // try and find a monster to start

    if (monsters[index].state == MONSTER_DEAD)
       {

       // set up fields

       monsters[index].x            = x;
       monsters[index].y            = y;
       monsters[index].xv           = 0;
       monsters[index].yv           = 0;
       monsters[index].state        = MONSTER_STILL;
       monsters[index].direction    = 0;
       monsters[index].counter      = 0;
       monsters[index].threshold    = 1 + rand()%2;
       monsters[index].counter_2    = 0;
       monsters[index].threshold_2  = 10 + rand()%10;
       monsters[index].object.curr_frame = 8;

       // scan background
       // extract proper parameters

       monsters[index].object.x = monsters[index].x;
       monsters[index].object.y = monsters[index].y;

       // erase the sprite

       Behind_Sprite_DB((sprite_ptr)&monsters[index].object);

       // break out of loop

       return(1);

       } // end if dead

    } // end for index

return(0);

} // end Start_Monster

/////////////////////////////////////////////////////////////////////////////

void Control_Bat(void)
{

// this function controls the bat

if (bat.state == BAT_DEAD && rand()%250==1)
   {

   // play sound

   Play_Sound(SOUND_BAT);

   // send the bat out

   // first select what the bat is going to do

   switch(rand()%2)
         {

         case 0:   // move in a random direction
              {

              bat.state = BAT_RANDOM;

              // select side of screen to start bat from

              if (rand()%2==1)
                 {
                 // start bat from right edge of screen

                 bat.x  = 224-16;
                 bat.xv = -4 - rand()%4;

                 } // end if right side
              else
                 {
                 // start bat from left edge of screen

                 bat.x  = 0;
                 bat.xv = 4 + rand()%4;

                 } // end else left side

              // set y position and velocity

              bat.y  = rand()%164;
              bat.yv = -4 + rand()%8;

              } break;

         case 1:   // move in a sinusoidal wave
              {

              bat.state = BAT_WAVE;

              // select side of screen to start bat from

              if (rand()%2==1)
                 {
                 // start bat from right edge of screen

                 bat.x  = 224-16;
                 bat.xv = -4 - rand()%4;

                 } // end if right side
              else
                 {
                 // start bat from left edge of screen

                 bat.x  = 0;
                 bat.xv = 4 + rand()%4;

                 } // end else left side

              // set y position and velocity

              bat.y       = 64 + rand()%100;
              bat.yv      = -1 + rand()%3;

              // set index into sin look up

              bat.counter   = 0;
              bat.threshold = 1 + rand()%3;  // modulate frequency of sin wave
                                             // that bat follows
              } break;

         } // end switch

   // set current frame

   bat.object.curr_frame=0;

   // get background

   SET_SPRITE_SIZE(16,16);
   bat.object.x = bat.x;
   bat.object.y = bat.y;

   Behind_Sprite_DB((sprite_ptr)&bat.object);

   } // end if start bat

} // end Control_Bat

///////////////////////////////////////////////////////////////////////////////

void Move_Bat(void)
{

// this moves the bat if it is alive

if (bat.state != BAT_DEAD)
   {

   // first translate

   bat.x+=bat.xv;
   bat.y+=bat.yv;

   // now test state to add on sin modulation

   if (bat.state==BAT_WAVE)
      {
      bat.y+=cos_look[bat.counter];

      // increment counter

      if ((bat.counter+=bat.threshold) >= 320)
           bat.counter=0;

      } // end if sin state

   // do animation

   if (++bat.object.curr_frame >= NUM_BAT_FRAMES)
      bat.object.curr_frame = 0;

   // now do boundary detection

   if ( (bat.x > 224-16) || (bat.x < 0) ||
        (bat.y > 199-16) || (bat.y < 0) )
      {
      // kill bat

      bat.state = BAT_DEAD;

      } // end if bat went off screen

   } // end if alive

} // end Move_Bat

//////////////////////////////////////////////////////////////////////////////

void Erase_Bat(void)
{

// this function erases the bat

if (bat.state != BAT_DEAD)
   {

   SET_SPRITE_SIZE(16,16);
   bat.object.x = bat.x;
   bat.object.y = bat.y;

   Erase_Sprite_DB((sprite_ptr)&bat.object);

   } // end if alive

} // end Erase_Bat

//////////////////////////////////////////////////////////////////////////////

void Behind_Bat(void)
{

// this function scans the background under the bat

if (bat.state != BAT_DEAD)
   {
   SET_SPRITE_SIZE(16,16);

   bat.object.x = bat.x;
   bat.object.y = bat.y;

   Behind_Sprite_DB((sprite_ptr)&bat.object);

   } // end if alive

} // end Behind_Mother

//////////////////////////////////////////////////////////////////////////////

void Draw_Bat(void)
{

// this function draws the mothership

if (bat.state != BAT_DEAD)
   {
   SET_SPRITE_SIZE(16,16);

   bat.object.x = bat.x;
   bat.object.y = bat.y;

   Draw_Sprite_DB((sprite_ptr)&bat.object);

   } // end if alive

} // end Draw_Bat

//////////////////////////////////////////////////////////////////////////////

void Erase_Arrows(void)
{

// this function indexes through all the arrows and if they are active
// erases them by replacing the background color that was under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(8,8);

for (index=0; index<NUM_ARROWS; index++)
    {

    // is this arrow active

    if (arrows[index].state == ARROW_ALIVE)
       {

       // extract proper parameters

       arrows[index].object.x = arrows[index].x;
       arrows[index].object.y = arrows[index].y;

       // erase the sprite

       Erase_Sprite_DB((sprite_ptr)&arrows[index].object);

       } // end if alive

    } // end for index

} // end Erase_Arrows

/////////////////////////////////////////////////////////////////////////////

void Behind_Arrows(void)
{

// this function indexes through all the arrows and if they are active
// scans the background color that is behind them so it can be replaced later

int index;

// set sprite size for engine

SET_SPRITE_SIZE(8,8);

for (index=0; index<NUM_ARROWS; index++)
    {

    // is this arrow active

    if (arrows[index].state == ARROW_ALIVE)
       {

       // extract proper parameters

       arrows[index].object.x = arrows[index].x;
       arrows[index].object.y = arrows[index].y;

       // scan begind the sprite

       Behind_Sprite_DB((sprite_ptr)&arrows[index].object);

       } // end if alive

    } // end for index

} // end Behind_Arrows

/////////////////////////////////////////////////////////////////////////////

void Draw_Arrows(void)
{

// this function indexes through all the arrows and if they are active
// draws the missile as a bright white pixel on the screen

int index;

// set sprite size for engine

SET_SPRITE_SIZE(8,8);

for (index=0; index<NUM_ARROWS; index++)
    {

    // is this arrow active

    if (arrows[index].state == ARROW_ALIVE)
       {

       // extract proper parameters

       arrows[index].object.x = arrows[index].x;
       arrows[index].object.y = arrows[index].y;

       // draw the sprite

       Draw_Sprite_DB((sprite_ptr)&arrows[index].object);

       } // end if alive

    } // end for index

} // end Draw_Arrrows

/////////////////////////////////////////////////////////////////////////////

void Move_Arrows(void)
{

// this function moves the arrows and does all the collision detection

int index,              // used for loops
    mindex,             // used to index through monsters
    arrow_x,            // position of arrow
    arrow_y,
    arrow_x_center,     // center of arrow
    arrow_y_center,
    cell_x,cell_y,cell_id;  // used to test if arrow has hit a background cell

// loop thru all arrows and perform a lot of tests

for (index=0; index<NUM_ARROWS; index++)
    {

    // is missile active

    if (arrows[index].state == ARROW_ALIVE)
       {

       // move the arrow

       arrow_x = (arrows[index].x += arrows[index].xv);
       arrow_y = (arrows[index].y += arrows[index].yv);

       // compute center of arrow for ease of computations

       arrow_x_center = arrow_x+4;
       arrow_y_center = arrow_y+4;

       // test if arrow hit an object

       for (mindex=0; mindex<NUM_MONSTERS; mindex++)
           {

           // test arrow against bounding box of monster

           if (monsters[mindex].state != MONSTER_DEAD &&
               monsters[mindex].state != MONSTER_DYING &&
               arrow_x_center > monsters[mindex].x &&
               arrow_x_center < monsters[mindex].x+16 &&
               arrow_y_center > monsters[mindex].y &&
               arrow_y_center < monsters[mindex].y+16)
              {

              // kill the monster

              monsters[mindex].state        = MONSTER_DYING;
              monsters[mindex].counter_2    = 0;
              monsters[mindex].threshold_2  = 20;

              // play the death sound

              Play_Sound(SOUND_MDIE);

              // kill the arrow

              arrows[index].state=ARROW_DEAD;

              // increase the players score

              players_score+=250;

              // break out of this for loop

              break;

              } // end if this arrow hit the monster

           } // end for mindex

       // test if it's hit the edge of the screen or a wall

       cell_x = arrow_x_center  >> 4;  // divide by 16 since cells are 16x16 pixels
       cell_y = arrow_y_center  >> 4;

       // what is the cell at this location

       cell_id = world[screen_y][screen_x].cells[cell_y][cell_x];

       if ( (arrow_x >= 224) || (arrow_x <= 0) ||
            (arrow_y > (SCREEN_HEIGHT-16)) || (arrow_y <= 0) ||
            (cell_id < 32))
            {

            // kill arrow

            arrows[index].state = ARROW_DEAD;

            } // end if off edge of screen

       } // end if arrow alive

    } // end for index

} // end Move_Arrows

/////////////////////////////////////////////////////////////////////////////

void Start_Arrow(int x,
                 int y,
                 int xv,
                 int yv,
                 int direction)

{

// this function scans through the arrows array and tries to find one that
// isn't being used.  this function could be more efficient.

int index;

// make sure user has an arrow to fire

if (num_arrows<=0)
   return;

// scan for a useable arrow

for (index=0; index<NUM_ARROWS; index++)
    {
    // is this arrow free?

    if (arrows[index].state == ARROW_DEAD)
       {

       // one less arrow!

       num_arrows--;

       // set up fields

       arrows[index].state = ARROW_ALIVE;
       arrows[index].x     = x;
       arrows[index].y     = y;
       arrows[index].xv    = xv;
       arrows[index].yv    = yv;

       // make sure proper animation cell is selected

       arrows[index].object.curr_frame = direction;

       // extract proper parameters

       arrows[index].object.x = x;
       arrows[index].object.y = y;

       // set sprite size for engine

       SET_SPRITE_SIZE(8,8);

       Behind_Sprite_DB((sprite_ptr)&arrows[index].object);

       // play sound

       Play_Sound(SOUND_BOW);

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Arrow

/////////////////////////////////////////////////////////////////////////////

void Init_Arrows(void)
{
// this function just makes sure all the "state" fields of the arrows are
// dead so that we don't get any strays on start up.  Remember never assume
// that variables are zeroed on instantiation!

int index;

for (index=0; index<NUM_ARROWS; index++)
    arrows[index].state = ARROW_DEAD;

} // Init_Arrows

//////////////////////////////////////////////////////////////////////////////

void Erase_Fireballs(void)
{

// this function indexes through all the fireballs and if they are active
// erases them by replacing the background color that was under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(8,8);

for (index=0; index<NUM_FIREBALLS; index++)
    {

    // is this fireball active

    if (fireballs[index].state == FIREBALL_ALIVE)
       {

       // extract proper parameters

       fireballs[index].object.x = fireballs[index].x;
       fireballs[index].object.y = fireballs[index].y;

       // erase the sprite

       Erase_Sprite_DB((sprite_ptr)&fireballs[index].object);

       } // end if alive

    } // end for index

} // end Erase_Fireballs

/////////////////////////////////////////////////////////////////////////////

void Behind_Fireballs(void)
{

// this function indexes through all the fireballs and if they are active
// scans the background color that is behind them so it can be replaced later

int index;

// set sprite size for engine

SET_SPRITE_SIZE(8,8);

for (index=0; index<NUM_FIREBALLS; index++)
    {

    // is this fireball active

    if (fireballs[index].state == FIREBALL_ALIVE)
       {

       // extract proper parameters

       fireballs[index].object.x = fireballs[index].x;
       fireballs[index].object.y = fireballs[index].y;

       // scan begind the sprite

       Behind_Sprite_DB((sprite_ptr)&fireballs[index].object);

       } // end if alive

    } // end for index

} // end Behind_Fireballs

/////////////////////////////////////////////////////////////////////////////

void Draw_Fireballs(void)
{

// this function indexes through all the fireballs and if they are active
// draws the missile as a bright white pixel on the screen

int index;

// set sprite size for engine

SET_SPRITE_SIZE(8,8);

for (index=0; index<NUM_ARROWS; index++)
    {

    // is this arrow active

    if (fireballs[index].state == FIREBALL_ALIVE)
       {

       // extract proper parameters

       fireballs[index].object.x = fireballs[index].x;
       fireballs[index].object.y = fireballs[index].y;

       // draw the sprite

       Draw_Sprite_DB((sprite_ptr)&fireballs[index].object);

       } // end if alive

    } // end for index

} // end Draw_Fireballs

/////////////////////////////////////////////////////////////////////////////

void Move_Fireballs(void)
{

// this function moves the fireballs and does all the collision detection

int index,                 // used for loops
    fireball_x,            // position of fireball
    fireball_y,
    fireball_x_center,     // center of fireball
    fireball_y_center,
    cell_x,cell_y,cell_id; // used to test if fireball has hit a background cell


// loop thru all fireballs and perform a lot of tests

for (index=0; index<NUM_FIREBALLS; index++)
    {

    // is fireball active

    if (fireballs[index].state == FIREBALL_ALIVE)
       {

       // move the fireball

       fireball_x = (fireballs[index].x += fireballs[index].xv);
       fireball_y = (fireballs[index].y += fireballs[index].yv);

       // animate fireball

       if (++fireballs[index].object.curr_frame == NUM_FIREBALL_FRAMES)
          fireballs[index].object.curr_frame = 0;

       // test if fireball hit an object

       // compute center of fireball for ease of computations

       fireball_x_center = fireball_x+4;
       fireball_y_center = fireball_y+4;

       // test to see if fireball has hit player first

       if ((fireball_x_center > player.x)    &&
           (fireball_x_center < player.x+16) &&
           (fireball_y_center > player.y)    &&
           (fireball_y_center < player.y+16) )
          {

          // hurt player a little

          if ((health-=10)<0)
             health=0;

          // play a hurt sound

          Play_Sound(SOUND_HURT);

          // start an explosion

          Start_Explosion(player.x, player.y,1);

          // kill fireball

          fireballs[index].state = FIREBALL_DEAD;

          // process next interation

          continue;

          } // end if player hit

       // test if it's hit the edge of the screen or a wall

       cell_x = fireball_x_center  >> 4;  // divide by 16 since cells are 16x16 pixels
       cell_y = fireball_y_center  >> 4;

       // what is the cell at this location

       cell_id = world[screen_y][screen_x].cells[cell_y][cell_x];

       if ( (fireball_x >= 224) || (fireball_x <= 0) ||
            (fireball_y > (SCREEN_HEIGHT-16)) || (fireball_y <= 0) ||
            (cell_id < 32))
            {

            // kill fireball

            fireballs[index].state = FIREBALL_DEAD;

            // start an explosion

            Start_Explosion(fireball_x,fireball_y,1+rand()%2);

            // process next fireball

            continue;

            } // end if off edge of screen

       } // end if fireball alive

    } // end for index

} // end Move_Fireballs

/////////////////////////////////////////////////////////////////////////////

void Start_Fireball(int x,
                    int y,
                    int xv,
                    int yv)

{

// this function scans through the fireballs array and tries to find one that
// isn't being used.  this function could be more efficient.

int index;

// scan for a useable fireball

for (index=0; index<NUM_FIREBALLS; index++)
    {
    // is this fireball free?

    if (fireballs[index].state == FIREBALL_DEAD)
       {

       // set up fields

       fireballs[index].state = FIREBALL_ALIVE;
       fireballs[index].x     = x;
       fireballs[index].y     = y;
       fireballs[index].xv    = xv;
       fireballs[index].yv    = yv;
       fireballs[index].object.curr_frame = 0;

       // extract proper parameters

       fireballs[index].object.x = x;
       fireballs[index].object.y = y;

       // set sprite size for engine

       SET_SPRITE_SIZE(8,8);

       Behind_Sprite_DB((sprite_ptr)&fireballs[index].object);

       // play sound

       Play_Sound(SOUND_MFIRE);

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Fireball

/////////////////////////////////////////////////////////////////////////////

void Init_Fireballs(void)
{
// this function just makes sure all the "state" fields of the fireballs are
// dead so that we don't get any strays on start up.  Remember never assume
// that variables are zeroed on instantiation!

int index;

for (index=0; index<NUM_FIREBALLS; index++)
    fireballs[index].state = FIREBALL_DEAD;

} // Init_Fireballs

///////////////////////////////////////////////////////////////////////////////

int Load_Screens(char *filename)
{
// this functions opens the screens data file and loads and converts the screen
// database into the proper format.  The screen database is in the format
// of 14 rows of 12 characters representing the screen cells then an integer
// that indicates the number of mosnters on a screen and then a set of coordinate
// pairs that position each of monsters, then the next screen and so on.
// the data is standard ASCII text

// this table is used to convert the ascii characters that represent bitmaps
// into integers

static char ascii_to_id[128] =

//    !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /

  {0 ,40,0 ,0 ,34,35,0 ,0 ,0 ,0 ,36,37,33,0 ,32,0 ,

// 0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
   0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,39,

// @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O
   0 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10,11,12,13,14,


// P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
   15,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,38,0 ,


// `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
   0 ,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,

// p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  DEL
  31 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,99 ,0 ,};

FILE *fp;                // the file we will read from

char row[128];           // used to load a row of characters

int cell_id,             // the final cell id
    x,y,                 // used for looping
    index_x,index_y,     // used for looping
    index,               // used for looping
    num_monsters,        // number of monsters on current screen
    monster_x,monster_y; // position of a monster (in pixels/screen coords)

// try and open screen file

if (!(fp = fopen(filename,"r")))
   {
   printf("\nScreen Loader Error: Can't find file->%s",filename);
   return(0);
   } // end if couldn't find file

printf("\nLoading universe database...");

// load all sixteen screens, note: they are in the database in row major form
// i.e. the screens are loaded in this order:
//
//  1  2  3  4
//  5  6  7  8  <---4 X 4
//  9  10 11 12
//  13 14 15 16





for (index_y=0; index_y<WORLD_ROWS; index_y++)
    {

    for (index_x=0; index_x<WORLD_COLUMNS; index_x++)
        {

        // load the screen

#if DEBUG
           printf("\nLoading screen #%d or (%d,%d)\n",index_x+index_y*4,index_x,index_y);
#endif

        // reset number of monsters

        world[index_y][index_x].num_monsters = 0;

        // reset monster index

        index=0;

        for (y=0; y<CELL_ROWS; y++)
            {

            // load in next row of ASCII characters

            fscanf(fp,"%s",row);

#if DEBUG
            printf("\n%s\n",row);
#endif

            // process row

            for (x=0; x<CELL_COLUMNS; x++)
                {

                // convert id using ascii to id look up table

                cell_id = ascii_to_id[row[x]-32];

#if DEBUG
                printf("%d,",cell_id);
#endif

                // place id into data structure at proper location

                if (cell_id < 99)
                    world[index_y][index_x].cells[y][x] = cell_id;
                else
                   {
                   // this must be a monster so install it into database

                   // scale monster position

                   monster_x = x << 4;  // * 16
                   monster_y = y << 4;  // * 16

#if DEBUG
                   printf("\n monster at (%d,%d)",monster_x,monster_y);
#endif
                   // record data in database

                   world[index_y][index_x].positions[index].x = monster_x;
                   world[index_y][index_x].positions[index].y = monster_y;

                   index++;

                   // place a neutral tile at this location

                   world[index_y][index_x].cells[y][x] = 32;

                   // record number of monsters in database

                   world[index_y][index_x].num_monsters++;

                   } // end else

                } // end for x

            } // end for y

        } // end for index_x

    } // end for index_y

} // end Load_Screens

///////////////////////////////////////////////////////////////////////////////

void Draw_Screen(int wx,int wy)
{
// this function uses the sent coords to select one of the screens from the
// world database to draw into the double buffer. note: everything that
// is currently in the double bufer will be obliterated after this call to
// reflect the new matrix ...Wrath of Khan

screen_ptr current_screen;   // pointer to current screen

int cell_id,                 // the cell id
    index_x,index_y;         // looping variables

// set sprite size

SET_SPRITE_SIZE(16,16);

// obtain a pointer to the current screen

current_screen = &world[wy][wx];

for (index_y=0; index_y<CELL_ROWS; index_y++)
    {

    for (index_x=0; index_x<CELL_COLUMNS; index_x++)
        {

        // extract the cell id

        cell_id = current_screen->cells[index_y][index_x];

        // blit it into double buffer

        // test for texture type

        if (cell_id < 16)
           {
           // use the sprite that holds the blue walls

           // set up sprite frame and position

           wall_1.curr_frame = cell_id;

           // compute screen coordinates of bitmap cell

           wall_1.x          = index_x << 4;  // * 16
           wall_1.y          = index_y << 4;  // * 16

           Draw_Sprite_DB((sprite_ptr)&wall_1);

           } // end if 0-15 blue walls
        else
        if (cell_id < 32)
           {
           // use the sprite that holds the green walls

           // set up sprite frame and position

           wall_2.curr_frame = cell_id-16;

           // compute screen coordinates of bitmap cell

           wall_2.x          = index_x << 4;  // * 16
           wall_2.y          = index_y << 4;  // * 16

           Draw_Sprite_DB((sprite_ptr)&wall_2);

           } // end if 16-31 green walls
        else
           {
           // must be floor texture

           // set up sprite frame and position

           floors.curr_frame = cell_id-32;

           // compute screen coordinates of bitmap cell

           floors.x          = index_x << 4;  // * 16
           floors.y          = index_y << 4;  // * 16

           Draw_Sprite_DB((sprite_ptr)&floors);

           } // end else must be floor

        } // end for x

    } // end for y

} // end Draw_Screen

///////////////////////////////////////////////////////////////////////////////

void Init_Monsters(void)
{
// this function sets the state of all monsters to dead

int index;

for (index=0; index<NUM_MONSTERS; index++)
    monsters[index].state = MONSTER_DEAD;

} // end Init_Monsters

//////////////////////////////////////////////////////////////////////////////

void Start_Monsters(int wx,int wy)
{

// this function refers to the world database and uses the monster info.
// to start up monsters for the screen

int index,
    num_monsters;

// first kill all the monsters

Init_Monsters();

// extract number of monsters from database

num_monsters = world[wy][wx].num_monsters;

// start the monsters up

for (index=0;index<num_monsters; index++)
    {

    Start_Monster(world[wy][wx].positions[index].x,
                  world[wy][wx].positions[index].y);

    } // end for index

} // end Start_Monsters

///////////////////////////////////////////////////////////////////////////////

void Save_Monsters(int wx,int wy)
{
// this function is used to save the position information about each monster
// back into the world database before a screen warp occurs

int index;            // loop variable

// reset counter

world[wy][wx].num_monsters = 0;

// try and find active monsters and then store their positions back into
// the world database so when the player comes back to the room it seems
// like the monsters didn't start from their original positions

for (index=0; index<NUM_MONSTERS; index++)
    {
    // is this a live one

    if (monsters[index].state!=MONSTER_DEAD)
       {

       // insert into data base

       world[wy][wx].positions[index].x = monsters[index].x;
       world[wy][wx].positions[index].y = monsters[index].y;

       // increment number of monsters

       world[wy][wx].num_monsters++;

       } // end if not dead

    } // end for index

// at this point the database has been updated and a screen warp can be made
// to another screen and then back with continuity

} // end Save_Monsters

///////////////////////////////////////////////////////////////////////////////

int Initialize_Sound_System(void)
{

// this function loads in the ct-voice.drv driver and the configuration file
// and sets up the sound driver appropriately

FILE *fp;

// test if driver is on disk

if ( (fp=fopen("ct-voice.drv","rb"))==NULL)
   {
   return(0);
   } // end if not file

fclose(fp);

// load up sound configuration file

if ( (fp=fopen("venture.cfg","r"))==NULL )
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
Voc_Set_Status_Addr((char far *)&ct_voice_status);

// load in sounds

sound_fx[SOUND_HURT   ]  = Voc_Load_Sound("VHURT.VOC   ",&sound_lengths[SOUND_HURT    ]);
sound_fx[SOUND_HEALTH ]  = Voc_Load_Sound("VHEALTH.VOC ",&sound_lengths[SOUND_HEALTH  ]);
sound_fx[SOUND_MDIE   ]  = Voc_Load_Sound("VMDIE.VOC   ",&sound_lengths[SOUND_MDIE    ]);
sound_fx[SOUND_MFIRE  ]  = Voc_Load_Sound("VMFIRE.VOC  ",&sound_lengths[SOUND_MFIRE   ]);
sound_fx[SOUND_MONEY  ]  = Voc_Load_Sound("VMONEY.VOC  ",&sound_lengths[SOUND_MONEY   ]);
sound_fx[SOUND_EAT    ]  = Voc_Load_Sound("VEAT.VOC    ",&sound_lengths[SOUND_EAT     ]);
sound_fx[SOUND_FARROW ]  = Voc_Load_Sound("VFARROW.VOC ",&sound_lengths[SOUND_FARROW  ]);
sound_fx[SOUND_POTION ]  = Voc_Load_Sound("VPOTION.VOC ",&sound_lengths[SOUND_POTION  ]);
sound_fx[SOUND_ARROW  ]  = Voc_Load_Sound("VARROW.VOC  ",&sound_lengths[SOUND_ARROW   ]);
sound_fx[SOUND_BOW    ]  = Voc_Load_Sound("VBOW.VOC    ",&sound_lengths[SOUND_BOW     ]);
sound_fx[SOUND_BAT    ]  = Voc_Load_Sound("VBAT.VOC    ",&sound_lengths[SOUND_BAT     ]);
sound_fx[SOUND_INTRO  ]  = Voc_Load_Sound("VINTRO.VOC  ",&sound_lengths[SOUND_INTRO   ]);
sound_fx[SOUND_DAGGER ]  = Voc_Load_Sound("VDAGGER.VOC ",&sound_lengths[SOUND_DAGGER  ]);
sound_fx[SOUND_END    ]  = Voc_Load_Sound("VEND.VOC    ",&sound_lengths[SOUND_END     ]);
sound_fx[SOUND_DEATH  ]  = Voc_Load_Sound("VDEATH.VOC  ",&sound_lengths[SOUND_DEATH   ]);
sound_fx[SOUND_GOAL   ]  = Voc_Load_Sound("VGOAL.VOC   ",&sound_lengths[SOUND_GOAL    ]);

// turn on speaker

Voc_Set_Speaker(1);

// success

return(1);

} // end Initialize_Sound_System

/////////////////////////////////////////////////////////////////////////////

void Close_Sound_System(void)
{

// this function closes down the sound system

// make sure there is sound

if (sound_available)
   {
   // turn off speaker

   Voc_Set_Speaker(0);

   // unload sounds

   Voc_Unload_Sound(sound_fx[ SOUND_HURT   ]);
   Voc_Unload_Sound(sound_fx[ SOUND_HEALTH ]);
   Voc_Unload_Sound(sound_fx[ SOUND_MDIE   ]);
   Voc_Unload_Sound(sound_fx[ SOUND_MFIRE  ]);
   Voc_Unload_Sound(sound_fx[ SOUND_MONEY  ]);
   Voc_Unload_Sound(sound_fx[ SOUND_EAT    ]);
   Voc_Unload_Sound(sound_fx[ SOUND_FARROW ]);
   Voc_Unload_Sound(sound_fx[ SOUND_POTION ]);
   Voc_Unload_Sound(sound_fx[ SOUND_ARROW  ]);
   Voc_Unload_Sound(sound_fx[ SOUND_BOW    ]);
   Voc_Unload_Sound(sound_fx[ SOUND_BAT    ]);
   Voc_Unload_Sound(sound_fx[ SOUND_DAGGER ]);
   Voc_Unload_Sound(sound_fx[ SOUND_INTRO  ]);
   Voc_Unload_Sound(sound_fx[ SOUND_END    ]);
   Voc_Unload_Sound(sound_fx[ SOUND_DEATH  ]);
   Voc_Unload_Sound(sound_fx[ SOUND_GOAL   ]);

   Voc_Terminate_Driver();

   } // end if sound

} // end Close_Sound_System

/////////////////////////////////////////////////////////////////////////////

void Play_Sound(int sound)
{
// this function plays a sound by turning off one if there is a sound playing
// and then playing the sent sound

// make sure there is a sound system first

if (sound_available && start_death!=1)
   {

   // stop the current sound (if there is one)

   Voc_Stop_Sound();

   // play sent sound

   Voc_Play_Sound(sound_fx[sound] , sound_lengths[sound]);

   } // end if sound available

} // end Play_Sound

///////////////////////////////////////////////////////////////////////////////

unsigned char Get_Pixel_DB(int x,int y)
{

// gets the color value of pixel at (x,y) from the double buffer

return double_buffer[((y<<8) + (y<<6)) + x];

} // end Get_Pixel_DB

//////////////////////////////////////////////////////////////////////////////

void Do_Intro(void)
{
// this function displays the introduction screen and then melts it

// load intro screen and display for a few secs.

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("ventint.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

// play a sound

Play_Sound(SOUND_INTRO );

// let user see it

Delay(50);

Fade_Lights();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

} // end Do_Intro

///////////////////////////////////////////////////////////////////////////////

void Load_Environment(void)
{
// this function loads the imagery for the environment

int index,  // loop variables
    index_2;

// load in imagery

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("ventimg.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize the wall sprites and floor sprites

SET_SPRITE_SIZE(16,16);

Sprite_Init((sprite_ptr)&wall_1,0,0,0,0,0,0);
Sprite_Init((sprite_ptr)&wall_2,0,0,0,0,0,0);
Sprite_Init((sprite_ptr)&floors,0,0,0,0,0,0);

// load in frames for walls

for (index=0; index<16; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&wall_1,index,index,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&wall_2,index,index,1);

    } // end for

// now load the floor textures

for (index=0; index<9; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&floors,index,index,2);
    } // end for

// now load the player

Sprite_Init((sprite_ptr)&player,0,0,0,0,0,0);

// load in frames for player

for (index=0; index<16; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&player,index,index,3);

    } // end for

// death sequence

for (index=16; index<21; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&player,index,index-16,6);

    } // end for


// set up state information

player.x             = 112-16;
player.y             = 120;
player.state         = PLAYER_ALIVE;
player.motion_speed  = 1;
player.motion_clock  = 0;

player.curr_frame = 9;

// load in the monsters

for (index=0; index<NUM_MONSTERS; index++)
    {

    Sprite_Init((sprite_ptr)&monsters[index].object,0,0,0,0,0,0);


    for (index_2=0; index_2<NUM_MONSTER_FRAMES; index_2++)
         PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                        (sprite_ptr)&monsters[index].object,index_2,index_2,4);


    // set up state information

    monsters[index].x                 = 0;
    monsters[index].y                 = 0;
    monsters[index].state             = MONSTER_DEAD;
    monsters[index].object.curr_frame = 8;

    } // end for

// now load the bat

Sprite_Init((sprite_ptr)&bat.object,0,0,0,0,0,0);

// load in frames for bat

for (index=0; index<NUM_BAT_FRAMES; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&bat.object,index,index,5);

    } // end for

// set up state information

bat.x                 = 0;
bat.y                 = 0;
bat.state             = BAT_DEAD;

bat.object.curr_frame = 0;

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Environment

////////////////////////////////////////////////////////////////////////////

void Load_Weapons(void)
{

int index;

// load in imagery for missiles

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("ventweap.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize arrows and extract bitmaps

SET_SPRITE_SIZE(8,8);

// initialize the sprite for each arrow

for (index=0; index<NUM_ARROWS; index++)
    {

    Sprite_Init((sprite_ptr)&arrows[index].object,0,0,0,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&arrows[index].object,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&arrows[index].object,1,1,0);


    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&arrows[index].object,2,2,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&arrows[index].object,3,3,0);

    // set some important fields

    arrows[index].x                 = 0;
    arrows[index].y                 = 0;
    arrows[index].object.curr_frame = 0;
    arrows[index].state             = ARROW_DEAD;

    } // end for

// initialize the sprite for each fireball

for (index=0; index<NUM_FIREBALLS; index++)
    {

    Sprite_Init((sprite_ptr)&fireballs[index].object,0,0,0,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&fireballs[index].object,0,0,1);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&fireballs[index].object,1,1,1);


    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&fireballs[index].object,2,2,1);

    // set some important fields

    fireballs[index].x                 = 0;
    fireballs[index].y                 = 0;
    fireballs[index].object.curr_frame = 0;
    fireballs[index].state             = FIREBALL_DEAD;

    } // end for

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);


// load in imagery for explosions

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("ventexpl.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize the explosions and extract bitmaps

SET_SPRITE_SIZE(16,16);

// load in frames for explosions

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    Sprite_Init((sprite_ptr)&explosions[index],0,0,0,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],1,1,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],2,2,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],3,3,0);

    } // end for

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Weapons

////////////////////////////////////////////////////////////////////////////

void Load_Background(void)
{

// load in the background image into the double buffer

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load("ventbak.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

} // Load_Background

///////////////////////////////////////////////////////////////////////////////

void Draw_Stats(void)
{

// this function draws the players statistics in the display module

char buffer[128];

// show the score

sprintf(buffer,"%ld",players_score);

Blit_String_DB(268+2,53,9,buffer,0);

// show the health

sprintf(buffer,"%d%% ",health);

// draw health in red if weak

if (health>=20)
    {
    Blit_String_DB(270+2,78,10,buffer,0);

    // set counter almost at threshold
    weak_counter=350;
    }
else
    {
    Blit_String_DB(270+2,78,12,buffer,0);

    // test if we have verbally told player he is weak

    if (++weak_counter>400)
       {

       Play_Sound(SOUND_HEALTH);

       // reset counter

       weak_counter=0;

       } // end if speak

    } // end else player weak

// show the arrows

sprintf(buffer,"%d ",num_arrows);

Blit_String_DB(275+2,103,6,buffer,0);

// show the gold

sprintf(buffer,"%d",gold_pieces);

Blit_String_DB(259+2,127,14,buffer,0);

// show the silver

sprintf(buffer,"%d",silver_pieces);

Blit_String_DB(268+2,152,7,buffer,0);

} // end Draw_Stats

///////////////////////////////////////////////////////////////////////////////

void Glow_Dagger(void)
{
// this is a self contained functions that makes the dagger glow

static int clock=0,       // used for timing, note: they are static!
           entered_yet=0,
           ci=2;          // used to make color increase or decrease

RGB_color color;          // used to hold color values during processing

// test if function is being called for first time

if (!entered_yet)
   {

   // reset the palette registers to bright blue, dark blue, dark blue

   color.red   = 32;
   color.green = 0;
   color.blue  = 0;

   Set_Palette_Register(254,(RGB_color_ptr)&color);

   // system has initialized, so flag it

   entered_yet=1;

   } // end if first time into function

// try and glow dagger

   if (++clock==1)  // is it time to rotate
      {
      // get the color

      Get_Palette_Register(254,(RGB_color_ptr)&color);

      // increase or decrease color

      color.red+=ci;

      // test if max or min or range has been hit

      if (color.red>63 || color.red<2)
         ci=-ci;

      // set the colors

      Set_Palette_Register(254,(RGB_color_ptr)&color);

      // reset the clock

      clock=0;

      } // end if time to rotate

} // end Glow_Dagger

//////////////////////////////////////////////////////////////////////////////

void Show_Instructions(void)
{
// this function displays the instructions and then disolves them

// load instruction screen and display it until a key press

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("ventins.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

// let user see it

while(!kbhit()){};

getch();

// let's try this screen transition

Sheer();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

} // end Show_Instructions

////////////////////////////////////////////////////////////////////////////////

void Do_Goal(void)
{
// this function displays the goal screen and then disolves it

// load instruction screen and display it until a key press

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("ventgoal.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

while(kbhit())
      getch();

// let user see it

while(!kbhit()){};

getch();

// let's try this screen transition

Disolve();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

} // end Do_Goal

// M A I N /////////////////////////////////////////////////////////////////

void main(void)
{

int done=0,         // main event loop exit flag
    index,          // loop variable
    cell_x,cell_y,  // cell location
    dx,dy,          // general deltas
    cell_id;        // bitmap id of a cell

char buffer[128]; // used for string printing

// begin the program

printf("\nStarting Venture...");

Load_Screens("venture.dat");

// initialize sound system

sound_available = Initialize_Sound_System();

// let user think the computer is working hard

Delay(50);

// set video mode to 320x200 256 color mode

Set_Video_Mode(VGA256);

// create a double buffer

if (!Create_Double_Buffer(SCREEN_HEIGHT))
   {

   printf("\nNot enough memory to create double buffer.");

   } // end if

// clear the double buffer

Fill_Double_Buffer(0);

// impress user (at least try to)

Do_Intro();

// show the instructions

Show_Instructions();

// load imagery for game

Load_Environment();

// load the background into double buffer

Load_Background();

// load in imagery for weapons

Load_Weapons();

// initialize everything

Init_Arrows();

Init_Fireballs();

Init_Explosions();

// create all look up tables

Create_Lookups();

// place player in top left corner of universe
// x...
// ....
// ....
// ....

screen_x = 0;
screen_y = 0;

// install the new keyboard driver

Install_Keyboard();



// draw the initial view

Draw_Screen(screen_x, screen_y);

// the the monsters

Start_Monsters(screen_x, screen_y);

// scan under player

SET_SPRITE_SIZE(16,16);
Behind_Sprite_DB((sprite_ptr)&player);

// begin main event loop

while(!done)
     {
     // erase everything

     SET_SPRITE_SIZE(16,16);

     Erase_Sprite_DB((sprite_ptr)&player);

     Erase_Arrows();

     Erase_Fireballs();

     Erase_Explosions();

     Erase_Bat();

     Erase_Monsters();

     // reset screen change flag

     screen_change=0;

     dx=dy=0;

     // test if user has pressed a key

     if ((key_table[INDEX_RIGHT] || key_table[INDEX_LEFT] ||
          key_table[INDEX_UP]    || key_table[INDEX_DOWN] ||
          key_table[INDEX_SPACE] || key_table[INDEX_ESC] ) &&
          (player.state==PLAYER_ALIVE))
        {

        // what key was pressed


              if (key_table[INDEX_ESC]) // exit game
                 {
                   done=2;

                 } // end if esc
              else
              if (key_table[INDEX_LEFT]) // move left
                 {
                 // only process if enough cycles have passed

                 if (++player.motion_clock==player.motion_speed)
                    {

                    player.motion_clock = 0;
                    ++player.curr_frame;

                    // test frame range for left walking

                    if (player.curr_frame<4 || player.curr_frame>7)
                       {
                       player.curr_frame = 4;

                       } // end if out of frame range

                    // set translation factors

                    dx=-3;

                    // set direction

                    players_dir = PLAYER_WEST;

                    } // end if time to move

                 } // end if right
              else
              if (key_table[INDEX_RIGHT])  // move right
                   {
                   // only process if enough cycles have passed

                   if (++player.motion_clock==player.motion_speed)
                      {

                      player.motion_clock = 0;
                      ++player.curr_frame;

                      // test frame range for right walking

                      if (player.curr_frame<0 || player.curr_frame>3)
                         {
                         player.curr_frame = 0;

                         } // end if out of frame range

                      // set translation factors

                      dx=3;

                      // set direction

                      players_dir = PLAYER_EAST;

                      } // end if time to move

                   } // end if right
              else
              if (key_table[INDEX_UP])  // move up
                   {
                   // only process if enough cycles have passed

                   if (++player.motion_clock==player.motion_speed)
                      {

                      // test frame range for upward walking

                      player.motion_clock = 0;
                      ++player.curr_frame;

                      if (player.curr_frame<12 || player.curr_frame>15)
                         {
                         player.curr_frame = 12;

                         } // end if out of frame range


                      // set translation factors

                      dy=-3;

                      // set direction

                      players_dir = PLAYER_NORTH;

                      } // end if time to move

                   } // end if up
              else
              if (key_table[INDEX_DOWN]) // move down
                   {
                   // only process if enough cycles have passed

                   if (++player.motion_clock==player.motion_speed)
                      {

                      player.motion_clock = 0;
                      ++player.curr_frame;

                      // test frame range for downward motion

                      if (player.curr_frame<8 || player.curr_frame>11)
                         {
                         player.curr_frame = 8;

                         } // end if out of frame range

                      // set translation factors

                      dy=3;

                      // set direction

                      players_dir = PLAYER_SOUTH;

                      } // end if time to move

                   } // end if down

              if (key_table[INDEX_SPACE]) // fire an arrow
                   {

                   // are there any arrows left?

                   if (num_arrows>0)
                      {

                      Start_Arrow(player.x+4,
                                  player.y+4,
                                  arrow_vel_x[players_dir],
                                  arrow_vel_y[players_dir],
                                  players_dir);

                      } // end if any arrows

                   } // end if fire weapon

        } // end if kbhit
     else
     if (player.state==PLAYER_DYING && player.curr_frame <= 20)
        {
        // this code takes care of the death sequence

        // only animate if the time is right

        if (++player.motion_clock==player.motion_speed)
           {
           // reset motion clock
           player.motion_clock = 0;

           // test if death is over

           if (++player.curr_frame==21)
              {
              done=1;
              player.state=PLAYER_DEAD;
              player.curr_frame--;
              Delay(40);

              } // end if player is dead

           } // end if ok to animate

        } // end else if dying

     // test if player should be killed

     if (health==0 && !start_death)
        {
        // set player to dying

        player.state = PLAYER_DYING;

        // set up sprite structure

        player.curr_frame   = 16;
        player.motion_clock = 0;
        player.motion_speed = 5;

        // play his last words

        Play_Sound(SOUND_DEATH);

        // flag that player has moaned

        start_death=1;

        } // end if player has had it!

     // move everything

     Move_Arrows();

     Move_Fireballs();

     Animate_Explosions();

     Move_Bat();

     Move_Monsters();

     // translate player and perform collision detection

     player.x+=dx;
     player.y+=dy;

     // save old screen position

     old_screen_x = screen_x;
     old_screen_y = screen_y;

     // first test if player has moved off screen and we need to make a screen
     // change

     if (player.x>224-16)
         {

         if (++screen_x>3)
            {
            screen_x=3;
            player.x=224-16;
            }
         else
            {
            screen_change=1;
            player.x=0;

            } // end else warp

         } // end if off right edge
     else
     if (player.x<0)
         {

         if (--screen_x<0)
            {

            screen_x=0;
            player.x=0;

            }
         else
            {
            screen_change=1;
            player.x=224-16;

            } // end else warp

         } // end if off left edge

     if (player.y>190-16)
         {

         if (++screen_y>3)
            {
            screen_y=3;
            player.y=190-16;
            }
         else
            {
            screen_change=1;
            player.y=0;

            } // end else warp

         } // end if off bottom edge
     else
     if (player.y<0)
         {

         if (--screen_y<0)
            {

            screen_y=0;
            player.y=0;

            }
         else
            {
            screen_change=1;
            player.y=190-16;

            } // end else warp

         } // end if off top edge

     else
     {

     // obtain cell location of players feet since this is a pseudo 3-D view
     // and his feet are more meaningful than his center

     cell_x = (player.x+8)  >> 4;  // divide by 16 since cells are 16x16 pixels
     cell_y = (player.y+14) >> 4;

     // what is the cell at this location

     cell_id = world[screen_y][screen_x].cells[cell_y][cell_x];

     // is this cell a wall or obstruction?

     if (cell_id < 32)
        {

        // back player up

        player.x-=dx;
        player.y-=dy;

        } // end if hit obstacle
     else
        { // player must be touching either a floor tile or a potion, food..

        // test which id the player has touched

        switch(cell_id)
              {

              case SILVER_ID:
                   {
                   // increase amount of silver and update data structure
                   // along with visual bitmap

                   silver_pieces+=100+rand()%50;

                   // position bitmap and upodate screen

                   floors.x = cell_x << 4;
                   floors.y = cell_y << 4;
                   floors.curr_frame = 0;
                   SET_SPRITE_SIZE(16,16);
                   Draw_Sprite_DB((sprite_ptr)&floors);

                   // update database

                   world[screen_y][screen_x].cells[cell_y][cell_x] = FLOOR_1_ID;

                   // play a sound

                   Play_Sound(SOUND_MONEY );

                   } break;

              case GOLD_ID:
                   {
                   // increase amount of gold and update data structure
                   // along with visual bitmap

                   gold_pieces+=100+rand()%50;

                   // position bitmap and upodate screen

                   floors.x = cell_x << 4;
                   floors.y = cell_y << 4;
                   floors.curr_frame = 0;
                   SET_SPRITE_SIZE(16,16);
                   Draw_Sprite_DB((sprite_ptr)&floors);

                   // update database

                   world[screen_y][screen_x].cells[cell_y][cell_x] = FLOOR_1_ID;

                   // play a sound

                   Play_Sound(SOUND_MONEY  );

                   } break;

              case POTION_ID:
                   {
                   // increase number of potions and update data structure
                   // along with visual bitmap

                   number_potions++;

                   // position bitmap and upodate screen

                   floors.x = cell_x << 4;
                   floors.y = cell_y << 4;
                   floors.curr_frame = 0;
                   SET_SPRITE_SIZE(16,16);
                   Draw_Sprite_DB((sprite_ptr)&floors);

                   // update control panel

                   floors.x = 268;
                   floors.y = 176;
                   floors.curr_frame = 4;
                   Draw_Sprite_DB((sprite_ptr)&floors);

                   // update database

                   world[screen_y][screen_x].cells[cell_y][cell_x] = FLOOR_1_ID;


                   // play a sound

                   Play_Sound(SOUND_POTION);

                   } break;

              case FOOD_ID:
                   {
                   // increase amount of health and update data structure
                   // along with visual bitmap

                   health=health+10+rand()%5;

                   // max health out a 100 percent

                   if (health>100) health=100;

                   // position bitmap and upodate screen

                   floors.x = cell_x << 4;
                   floors.y = cell_y << 4;
                   floors.curr_frame = 0;
                   SET_SPRITE_SIZE(16,16);
                   Draw_Sprite_DB((sprite_ptr)&floors);

                   // update database

                   world[screen_y][screen_x].cells[cell_y][cell_x] = FLOOR_1_ID;

                   // play a sound

                   Play_Sound(SOUND_EAT  );

                   } break;

              case ARROWS_ID:
                   {
                   // increase number of arrows and update data structure
                   // along with visual bitmap

                   num_arrows+=10;

                   // position bitmap and upodate screen

                   floors.x = cell_x << 4;
                   floors.y = cell_y << 4;
                   floors.curr_frame = 0;
                   SET_SPRITE_SIZE(16,16);
                   Draw_Sprite_DB((sprite_ptr)&floors);

                   // update database

                   world[screen_y][screen_x].cells[cell_y][cell_x] = FLOOR_1_ID;

                   // play a sound

                   Play_Sound(SOUND_FARROW  );

                   } break;

             case EXIT_ID:  // archer is trying to exit
                  {
                  // exit if dagger found

                  if (dagger_found)
                      done=1;

                  } break;

             case DAGGER_ID: // player has found the dagger
                  {

                  // position bitmap and upodate screen

                  floors.x = cell_x << 4;
                  floors.y = cell_y << 4;
                  floors.curr_frame = 0;
                  SET_SPRITE_SIZE(16,16);
                  Draw_Sprite_DB((sprite_ptr)&floors);

                  // update control panel

                  floors.x = 268+18;
                  floors.y = 176;
                  floors.curr_frame = 8;
                  Draw_Sprite_DB((sprite_ptr)&floors);

                  // update database

                  world[screen_y][screen_x].cells[cell_y][cell_x] = FLOOR_1_ID;

                  // note that the dagger has been found

                  dagger_found=1;

                  // play the sound

                  Play_Sound(SOUND_DAGGER);

                  Delay(50);

                  // increase the players score

                  players_score+=1000;

                  } break;

              default:break;

              } // end switch

        } // end else test for special objects

     } // end else no screen warp

     // start things up here

     Control_Bat();

     // has there been a screen change

     if (screen_change)
        {
        // need to scroll over to the next page

        // kill all weapons

        Init_Arrows();

        Init_Fireballs();

        // kill any explosions

        Init_Explosions();

        // first save the monsters positions in the database

        Save_Monsters(old_screen_x,old_screen_y);

        // now draw the screen

        Draw_Screen(screen_x,screen_y);

        // now start the monsters

        Start_Monsters(screen_x,screen_y);

        } // end if screen change

     // scan background under objects

     SET_SPRITE_SIZE(16,16);
     Behind_Sprite_DB((sprite_ptr)&player);

     // scan under arrows

     Behind_Arrows();

     Behind_Fireballs();

     Behind_Explosions();

     Behind_Bat();

     Behind_Monsters();

     // draw objects

     Draw_Arrows();

     Draw_Fireballs();

     Draw_Monsters();

     // now draw the player

     SET_SPRITE_SIZE(16,16);
     Draw_Sprite_DB((sprite_ptr)&player);

     Draw_Explosions();

     Draw_Bat();

     // draw statistics

     Draw_Stats();

     // display double buffer

     Show_Double_Buffer((char far *)double_buffer);

     // do color effects

     Glow_Dagger();

     // wait a sec

     Delay(1);

     } // end while

// remove keyboard driver

Delete_Keyboard();

// were out of here, test if we should say the good message or the bad one

if (health==0 || !dagger_found || done==2)
   {
   // allow sound system to play a sound

   start_death=0;

   // play the end message

   Play_Sound(SOUND_END);

   // make sure player has heard it

   Delay(150);

   } // end if archer failed
else
   { // player did well

   // allow sound system to play a sound

   start_death=0;

   // play the goal message

   Play_Sound(SOUND_GOAL);

   // put up goal screen

   Do_Goal();

   } // put up happy screen

// exit system with a cool transition

Melt();

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

// close sound system

Close_Sound_System();

printf("\nVenture shut down...");
printf("\nAll resources released...exiting back to DOS.\n");

} // end main


