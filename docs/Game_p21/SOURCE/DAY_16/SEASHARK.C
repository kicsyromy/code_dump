
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

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Start_Missile(sprite_ptr who,
                   int x,
                   int y,
                   int xv,
                   int yv,
                   int tag);

void Erase_Missiles(void);

void Behind_Missiles(void);

void Draw_Missiles(void);

void Move_Missiles(void);

void Init_Missiles(void);

void Start_Bubble(int x,
                  int y);

void Erase_Bubbles(void);

void Behind_Bubbles(void);

void Draw_Bubbles(void);

void Move_Bubbles(void);

void Init_Bubbles(void);


void Erase_Subs(void);

void Behind_Subs(void);

void Draw_Subs(void);

void Move_Subs(void);

void Init_Subs(void);

void Start_Sub();

void Start_Explosion(int x,int y,int speed);

void Behind_Explosions(void);

void Erase_Explosions(void);

void Draw_Explosions(void);

void Animate_Explosions(void);

void Draw_Score(void);

void Rotate_Lights(void);

unsigned char Get_Pixel_DB(int x,int y);

int Initialize_Sound_System(void);

void Close_Sound_System(void);

void Play_Sound(int sound);

void Start_Wave(void);

void Erase_End_Wave(void);

void Draw_End_Wave(void);

void Load_Subs(void);

void Load_Player(void);

void Load_Explosions(void);

void Do_Intro(void);

// D E F I N E S /////////////////////////////////////////////////////////////

// constants for player and enemy

#define ENEMY_MISSILE        0 // this is a missile fired by enemy
#define PLAYER_MISSILE       1 // this is a missile fired by player

#define MISS_ALIVE           1 // state of a live missile
#define MISS_DEAD            0 // state of a dead missile
#define NUM_MISSILES         3 // number of missiles that can be fired

// defines for bubbles

#define BUBBLE_CREATED       1  // flags that a bubble has just been created
#define BUBBLE_FLOATING      2  // the state of a floating bubble
#define BUBBLE_DEAD          0  // the bubble is dead (not active)
#define NUM_BUBBLES          50 // maximum bumber of bubbles in system

#define BUBBLE_COLOR_BASE    16 // this is used as the base color to make
                                // bubbles (off white)

// general explosions

#define NUM_EXPLOSIONS       10  // number of explosions that can run at once

#define EXPLOSION_DEAD       0   // state of a dead explosion
#define EXPLOSION_ALIVE      1   // state of a live explosion

// cannon positions

#define NUM_CANNON_POSITIONS 3   // total possible positions missile cannon has

#define CANNON_WEST          0   // cannon is pointing west
#define CANNON_NORTH         1   // cannon is pointing north
#define CANNON_EAST          2   // cannon is pointing east

// defines for enemies

#define NUM_SUBS             12  // maximum number of subs in game

#define SUB_DEAD             0   // a dead sub
#define SUB_ALIVE            1   // a live sub
#define SUB_RIGHT            0   // sub is moving right
#define SUB_LEFT             1   // sub is moving left

#define SUB_SMALL            0   // type for small sub
#define SUB_LARGE            1   // type for large sub
#define SUB_BOAT             2   // type for speed boat

#define WATER_COLOR          156 // the color register of the background water

// sound system defines

#define NUM_SOUNDS           5   // number of sounds in system

#define SOUND_MISSILE        0   // the players missile
#define SOUND_EXPL           1   // the explosion
#define SOUND_WAVE           2   // end of wave message
#define SOUND_SONAR          3   // the soanr pulse of large sub
#define SOUND_END            4   // game over man!

#define SOUND_DEFAULT_PORT   0x220  // default sound port for sound blaster
#define SOUND_DEFAULT_INT    5      // default interrupt


// M A C R O S //////////////////////////////////////////////////////////////

#define SET_SPRITE_SIZE(w,h) {sprite_width=w; sprite_height=h;}

// S T R U C T U R E S ///////////////////////////////////////////////////////

// typedef for a explosion particle and simple bullet

typedef struct particle_typ
        {
        int x;                   // x position
        int y;                   // y position
        int xv;                  // x velocity
        int yv;                  // y velocity
        int xa;                  // x acceleration
        int ya;                  // y acceleration
        unsigned char color;     // the color of the particle
        unsigned char back;      // the color behind the particle
        int state;               // the state of the particle
        int tag;                 // if the particle is a missile then who
                                 // does it belong to?
        int counter_1;           // used for counting
        int threshold_1;         // the counters threshold

        int counter_2;
        int threshold_2;

        } particle, *particle_ptr;

// typedef for a projectile

typedef struct projectile_typ
        {
        int x;                   // x position
        int y;                   // y position
        int xv;                  // x velocity
        int yv;                  // y velocity
        int state;               // the state of the particle
        int tag;                 // if the particle is a missile then who
                                 // does it belong to?
        int counter;             // use for counting
        int threshold;           // the counters threshold

        int counter_2;
        int threshold_2;

        sprite object;           // the missile sprite

        } projectile, *projectile_ptr;

// typedef for water vehicle

typedef struct boat_typ
        {

        int state;        // state of boat
        int direction;    // direction of motion
        int type;         // type of water vehicle

        int xv;           // x velocity
        int yv;           // y velocity

        int x,y;          // position of vehicle

        sprite object;    // the sprite for the boat

        } boat, *boat_ptr;

// G L O B A L S  ////////////////////////////////////////////////////////////

projectile missiles[NUM_MISSILES];   // the array of missiles in the world

int active_missiles = 0;             // tracks number of active missiles

int cannon_pos_x[NUM_CANNON_POSITIONS] = { 2,9,12 };

int cannon_pos_y[NUM_CANNON_POSITIONS] = { 0,-4,0 };

int cannon_vel_x[NUM_CANNON_POSITIONS] = { -3,0,3 };

int cannon_vel_y[NUM_CANNON_POSITIONS] = { -4,-6,-4};

particle bubbles[NUM_BUBBLES];       // the bubbbles in the world

sprite explosions[NUM_EXPLOSIONS];   // the array of explosions

pcx_picture imagery_pcx,      // the game imagery
            background_pcx,   // the backdrop
            intro_pcx;        // the introduction screen

// the sprites used in the game

sprite player;                // the player

sprite icons;                 // used to hold the ship icons during the score
                              // phase

boat subs[NUM_SUBS];          // the enemy subs and boats

// some of the global game state variables

long players_score   = 0;       // the score of the player

int wave_number     = 0;       // the current game wave

int got_away_this_wave = 0;    // the number of ships that got away this wave

int total_got_away  = 0;       // the total number of ships that got away

int hits_this_wave  = 0;       // number of ships player hit this wave

int ships_large_hit = 0;       // number of large ships hit

int ships_small_hit = 0;       // number of small ships hit

int ships_boat_hit  = 0;       // number of boats ships hit

int ships_this_wave = 0;       // the number of ships that will come out this wave

int ships_so_far    = 0;       // number of ships that have come out so far

int wave_end        = 0;       // flags if it's the end of the wave

int wave_clock      = 0;       // used to count number of frames to display
                               // wave stats to player

int game_over       = 0;       // flags if the game is over

int game_over_clock = 0;       // used to count how many frames to play the
                               // game over sequence

RGB_color color_red     = {255,0,0};    // generic colors used on cannon display
RGB_color color_green   = {0,255,0};

// the sound system variables

char far *sound_fx[NUM_SOUNDS];           // pointers to the voc files
unsigned char sound_lengths[NUM_SOUNDS];  // lengths of the voc files

int sound_available = 1;                  // flags if sound is available

int sound_port = SOUND_DEFAULT_PORT;      // default sound port
int sound_int  = SOUND_DEFAULT_INT;       // default sound interrupt

// F U N C T I O N S //////////////////////////////////////////////////////////

int Initialize_Sound_System(void)
{

// this function loads in the ct-voice.drv driver and the configuration file
// and sets up the sound driver appropriately

FILE *fp;

// test if driver is on disk

if ( (fp=fopen("ct-voice.drv","rb") )==NULL)
   {
   return(0);
   } // end if not file

fclose(fp);

// load up sound configuration file

if ( (fp=fopen("seashark.cfg","r"))==NULL)
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

sound_fx[SOUND_MISSILE ] = Voc_Load_Sound("smissile.voc",&sound_lengths[SOUND_MISSILE ]);
sound_fx[SOUND_EXPL    ] = Voc_Load_Sound("sexpl.voc",   &sound_lengths[SOUND_EXPL    ]);
sound_fx[SOUND_WAVE    ] = Voc_Load_Sound("swave.voc",   &sound_lengths[SOUND_WAVE    ]);
sound_fx[SOUND_SONAR   ] = Voc_Load_Sound("ssonar.voc",  &sound_lengths[SOUND_SONAR   ]);
sound_fx[SOUND_END     ] = Voc_Load_Sound("sover.voc",   &sound_lengths[SOUND_END]);

// turn on speaker

Voc_Set_Speaker(1);

// success

return(1);

} // end Initialize_Sound_System

/////////////////////////////////////////////////////////////////////////////

void Close_Sound_System(void)
{
// make sure there is sound

if (sound_available)
   {
   // turn off speaker

   Voc_Set_Speaker(0);

   // unload sounds

   Voc_Unload_Sound(sound_fx[SOUND_MISSILE ]);
   Voc_Unload_Sound(sound_fx[SOUND_EXPL    ]);
   Voc_Unload_Sound(sound_fx[SOUND_WAVE    ]);
   Voc_Unload_Sound(sound_fx[SOUND_SONAR   ]);
   Voc_Unload_Sound(sound_fx[SOUND_END     ]);

   Voc_Terminate_Driver();

   } // end if sound

} // end Close_Sound_System

/////////////////////////////////////////////////////////////////////////////

void Play_Sound(int sound)
{
// this function plays a sound by turning off one if there is a sound playing
// and then playing the sent sound

// make sure there is a sound system first

if (sound_available)
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

///////////////////////////////////////////////////////////////////////////////

void Erase_Missiles(void)
{

// this function indexes through all the missiles and if they are active
// erases them by replacing the background color that was under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(8,8);

for (index=0; index<NUM_MISSILES; index++)
    {

    // is this missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       // extract proper parameters

       missiles[index].object.x = missiles[index].x;
       missiles[index].object.y = missiles[index].y;

       // erase the sprite

       Erase_Sprite_DB((sprite_ptr)&missiles[index].object);

       } // end if alive

    } // end for index

} // end Erase_Missiles

/////////////////////////////////////////////////////////////////////////////

void Behind_Missiles(void)
{

// this function indexes through all the missiles and if they are active
// scans the background color that is behind them so it can be replaced later

int index;

// set sprite size for engine

SET_SPRITE_SIZE(8,8);

for (index=0; index<NUM_MISSILES; index++)
    {

    // is this missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       // extract proper parameters

       missiles[index].object.x = missiles[index].x;
       missiles[index].object.y = missiles[index].y;

       // scan begind the sprite

       Behind_Sprite_DB((sprite_ptr)&missiles[index].object);

       } // end if alive

    } // end for index

} // end Behind_Missiles

/////////////////////////////////////////////////////////////////////////////

void Draw_Missiles(void)
{

// this function indexes through all the missiles and if they are active
// draws the missile as a bright white pixel on the screen

int index;

// set sprite size for engine

SET_SPRITE_SIZE(8,8);

for (index=0; index<NUM_MISSILES; index++)
    {

    // is this missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       // extract proper parameters

       missiles[index].object.x = missiles[index].x;
       missiles[index].object.y = missiles[index].y;

       // draw the sprite

       Draw_Sprite_DB((sprite_ptr)&missiles[index].object);

       } // end if alive

    } // end for index

} // end Draw_Missiles

/////////////////////////////////////////////////////////////////////////////

void Move_Missiles(void)
{

// this function moves the missiles and does all the collision detection

int index,              // used for loops
    index_2,
    index_3,
    miss_x,             // position of missile
    miss_y,
    miss_x_center,      // center of missile
    miss_y_center,
    sub_x,              // position of test sub
    sub_y,
    num_explosions,     // temporary var that holds number of explosions
    sub_hit=0;          // flags if a sub was hit


// test if all missiles are active so that ready light on cannon can be
// illuminated properly

if (active_missiles==NUM_MISSILES)
   {
   // turn ready light red

   Set_Palette_Register(254,(RGB_color_ptr)&color_red);

   } // no more missles can be fired
else
   {
   // turn ready light green

   Set_Palette_Register(254,(RGB_color_ptr)&color_green);

   } // end else there is a missile ready

// loop thru all missiles and perform a lot of tests

for (index=0; index<NUM_MISSILES; index++)
    {

    // is missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       // move the missile

       miss_x = (missiles[index].x += missiles[index].xv);
       miss_y = (missiles[index].y += missiles[index].yv);

       // test if missile hit an object

       // compute center of missile for ease of computations

       miss_x_center = miss_x+4;
       miss_y_center = miss_y+4;

       // reset sub hit flag

       sub_hit = 0;

       // for each active sub test missile to see if it has hit it

       for (index_2=0; index_2<NUM_SUBS; index_2++)
           {

           // is this sub alive

           if (subs[index_2].state==SUB_ALIVE)
              {

              // extract upper left hand corner of sub

              sub_x = subs[index_2].x;
              sub_y = subs[index_2].y;

              // do standard bounding box test for collision

              if ( (miss_x_center >= sub_x) &&
                   (miss_x_center <= sub_x+subs[index_2].object.width) &&
                   (miss_y_center >= sub_y) &&
                   (miss_y_center <= sub_y+subs[index_2].object.height) )
                 {

                 // terminate sub and missile

                 subs[index_2].state   = SUB_DEAD;

                 hits_this_wave++;

                 missiles[index].state = MISS_DEAD;

                 // decrement number of active missiles

                 active_missiles--;

                 // start a couple explosions in the area

                 num_explosions = 1 + rand()%2;

                 for (index_3=0; index_3<num_explosions; index_3++)
                     {

                     Start_Explosion(miss_x+4-30 + rand()%24,miss_y+4-20,1+rand()%2);

                     } // end for start explosions

                 // set hit flag

                 sub_hit = 1;

                 // increment number of hits this wave

                 hits_this_wave++;

                 // update players score based on type of sub

                 switch(subs[index_2].type)
                       {

                       case SUB_SMALL: // hard to hit!
                            {

                            players_score += 250;

                            ships_small_hit++;

                            } break;

                       case SUB_LARGE: // easy to hit
                            {

                            players_score += 50;

                            ships_large_hit++;

                            } break;

                       case SUB_BOAT: // hard to hit
                            {

                            players_score += 300;

                            ships_boat_hit++;

                            } break;

                       default:break;

                       } // end switch

                 // missile has hit something so break out of loop

                 break;

                 } // end if hit

              } // end if alive

           } // end for sub test

       // don't do any further processing of this missile if there has been a
       // sub hit

       if (!sub_hit)
          {

          // test if it's hit the edge of the screen or a wall

          if ( (miss_x >= SCREEN_WIDTH) || (miss_x <= 0) ||
               (miss_y > (SCREEN_HEIGHT-16)) || ( miss_y <= 16) )
               {

               // kill missile and start a surface explosion

               missiles[index].state = MISS_DEAD;

               // decrement number of active missiles

               active_missiles--;

               Start_Explosion(miss_x+4-16,miss_y+4-16,1);

               // process next missile

               continue;

               } // end if off edge of screen

          // start a bubble ?

          if (miss_y > 64 )
             {

             Start_Bubble(miss_x+4, miss_y+4);

             } // end if time to start bubble

          } // end if sub not hit

       } // end if missile alive

    } // end for index

} // end Move_Missiles

/////////////////////////////////////////////////////////////////////////////

void Start_Missile(sprite_ptr who,
                   int x,
                   int y,
                   int xv,
                   int yv,
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
       missiles[index].tag   = tag;

       // make sure proper animation cell is selected

       missiles[index].object.curr_frame = player.curr_frame;

       // extract proper parameters

       missiles[index].object.x = x;
       missiles[index].object.y = y;

       // set sprite size for engine

       SET_SPRITE_SIZE(8,8);

       Behind_Sprite_DB((sprite_ptr)&missiles[index].object);

       // increment number of active missiles

       active_missiles++;

       // play sound

       Play_Sound(SOUND_MISSILE);

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


///////////////////////////////////////////////////////////////////////////////

void Erase_Bubbles(void)
{
// this function erases all the bubbles

int index;

// process each bubble

for (index=0; index<NUM_BUBBLES; index++)
    {

    // is this bubble active

    if (bubbles[index].state != BUBBLE_DEAD)
       {

       Plot_Pixel_Fast_DB(bubbles[index].x,bubbles[index].y,bubbles[index].back);

       } // end if alive

    } // end for index

} // end Erase_Bubbles

/////////////////////////////////////////////////////////////////////////////

void Behind_Bubbles(void)
{
// this function scans the background under all the bubbles

int index;

// process each bubble

for (index=0; index<NUM_BUBBLES; index++)
    {

    // is this bubble active

    if (bubbles[index].state != BUBBLE_DEAD)
       {

       bubbles[index].back = Get_Pixel_DB(bubbles[index].x, bubbles[index].y);

       } // end if alive

    } // end for index

} // end Behind_Bubbles

/////////////////////////////////////////////////////////////////////////////

void Draw_Bubbles(void)
{
// this function draws all the bubbles

int index;

// process each bubble

for (index=0; index<NUM_BUBBLES; index++)
    {

    // is this bubble active

    if (bubbles[index].state != BUBBLE_DEAD)
       {
       Plot_Pixel_Fast_DB(bubbles[index].x,bubbles[index].y,bubbles[index].color);

       } // end if alive

    } // end for index

} // end Draw_Bubbles

/////////////////////////////////////////////////////////////////////////////

void Move_Bubbles(void)
{

// this function moves the bubbles

int index,     // used for loops
    bubb_x,   // position of missile
    bubb_y;

// loop thru all bubbles

for (index=0; index<NUM_BUBBLES; index++)
    {

    // is bubble active

    if (bubbles[index].state != BUBBLE_DEAD)
       {

       // move the bubble if time to using y velocity

       bubb_x = bubbles[index].x;

       if (bubbles[index].counter_1 > bubbles[index].threshold_2)
           bubb_y = (bubbles[index].y += bubbles[index].yv);
       else
           bubb_y = bubbles[index].y;

       // test if it's hit the edge of the screen or a wall

       if ( (bubb_x >= SCREEN_WIDTH) || (bubb_x <= 0) ||
            (bubb_y > (SCREEN_HEIGHT-16)) || ( bubb_y <=0) )
            {
            bubbles[index].state = BUBBLE_DEAD;
            continue;

            } // end if off edge of screen

       // is it time to pop bubble

       if (++bubbles[index].counter_1 > bubbles[index].threshold_1)
          {
          // kill bubble and process next

          bubbles[index].state = BUBBLE_DEAD;
          continue;

          } // end if timed out

       } // end if bubble alive

    } // end for index

} // end Move_Bubbles

/////////////////////////////////////////////////////////////////////////////

void Start_Bubble(int x, int y)
{

// this function scans through the bubble array and tries to find one that
// isn't being used.  this function could be more efficient.

int index;

// scan for a useable bubble

for (index=0; index<NUM_BUBBLES; index++)
    {
    // is this bubble free?

    if (bubbles[index].state == BUBBLE_DEAD)
       {

       // set up fields

       bubbles[index].state = BUBBLE_CREATED;
       bubbles[index].x     = -2 + x + rand()%4;
       bubbles[index].y     = -2 + y + rand()%4;
       bubbles[index].xv    = 0;
       bubbles[index].yv    = -(rand()%3);

       // select a random lifetime before bursting

       bubbles[index].counter_1   = 0;
       bubbles[index].threshold_1 = 25 + rand()%10;

       bubbles[index].threshold_2 = 10 + rand()%5;

       // select a random shade of white

       bubbles[index].color = BUBBLE_COLOR_BASE+rand()%8;

       // scan the background

       bubbles[index].back  = Get_Pixel_DB(bubbles[index].x,bubbles[index].y);

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Bubble

/////////////////////////////////////////////////////////////////////////////

void Init_Bubbles(void)
{
// this function sets the state of all bubbles to dead

int index;

for (index=0; index<NUM_BUBBLES; index++)
    bubbles[index].state = BUBBLE_DEAD;

} // Init_Bubbles

////////////////////////////////////////////////////////////////////////////

void Start_Explosion(int x,int y,int speed)
{
// this function stars a generic explosion

int index;

SET_SPRITE_SIZE(32,32);

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

       // play sound

       Play_Sound(SOUND_EXPL);

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Explosion

/////////////////////////////////////////////////////////////////////////////

void Behind_Explosions(void)
{

// this function scans under the explosions

int index;

SET_SPRITE_SIZE(32,32);

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

SET_SPRITE_SIZE(32,32);

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

SET_SPRITE_SIZE(32,32);

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

void Init_Subs(void)
{
// set the state of all subs to dead

int index;

for (index=0; index<NUM_SUBS; index++)
    subs[index].state = SUB_DEAD;

} // Init_Subs

////////////////////////////////////////////////////////////////////////////

void Behind_Subs(void)
{
// this function scans the background under each sub

int index; // loop variable

// process each sub

for (index=0; index<NUM_EXPLOSIONS; index++)
    {
    // test if sub is active

    if (subs[index].state == SUB_ALIVE)
       {

       // extract proper sprite size

       sprite_width  = subs[index].object.width;
       sprite_height = subs[index].object.height;

       // scan behind sprite

       subs[index].object.x = subs[index].x;
       subs[index].object.y = subs[index].y;

       Behind_Sprite_DB((sprite_ptr)&subs[index].object);

       } // end if found a good one

    } // end for index

} // end Behind_Subs

////////////////////////////////////////////////////////////////////////////

void Erase_Subs(void)
{
// this function erases all the subs

int index; // loop variable

// process each sub

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    // test if sub is active

    if (subs[index].state == SUB_ALIVE)
       {


       // extract proper sprite size

       sprite_width  = subs[index].object.width;
       sprite_height = subs[index].object.height;

       subs[index].object.x = subs[index].x;
       subs[index].object.y = subs[index].y;

       Erase_Sprite_DB((sprite_ptr)&subs[index].object);

       } // end if found a good one

    } // end for index

} // end Erase_Subs

////////////////////////////////////////////////////////////////////////////

void Draw_Subs(void)
{

int index;

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    if (subs[index].state == SUB_ALIVE)
       {

       // extract proper sprite size

       sprite_width  = subs[index].object.width;
       sprite_height = subs[index].object.height;

       subs[index].object.x = subs[index].x;
       subs[index].object.y = subs[index].y;

       Draw_Sprite_DB((sprite_ptr)&subs[index].object);

       } // end if found a good one

    } // end for index

} // end Draw_Subs

////////////////////////////////////////////////////////////////////////////

void Start_Sub(void)
{

// this function scans through the array of subs and tests if any of them
// can be used i.e. are not active, if one is found then it is started up

int index,             // loop index
    sub_type,          // type of sub small, large, boat
    range_start,       // where to look for the specific type of sub in array
    range_end;

    // select type of enemy

    sub_type = rand()%3;

    // find a free enemy of that type

    switch(sub_type)
          {

          case SUB_SMALL: // select proper scan range for small sub
               {
               // select scan range

               range_start = 0;
               range_end   = 3;

               } break;

          case SUB_LARGE: // select proper scan range for large sub
               {
               // select scan range

               range_start = 4;
               range_end   = 7;

               // play sound if it's a big sub

               if ((rand()%2)==0)
                   Play_Sound(SOUND_SONAR);

               } break;

          case SUB_BOAT: // select proper scan range for boat
               {
               // select scan range

               range_start = 8;
               range_end   = 11;

               } break;

          default:break;

          } // end switch

// now that we know where in array to look for a boat of the requested type,
// let's find one

for (index=range_start; index<=range_end; index++)
    {

    // is this one free

    if (subs[index].state == SUB_DEAD)
       {

       // set the water vehicle up

       subs[index].state = SUB_ALIVE;

       // record that another brave sole was sent out

       ships_so_far++;

       // set y velocity to zero for now

       subs[index].yv = 0;

       if (sub_type==SUB_BOAT)
          subs[index].y = 16;
       else
          subs[index].y = 24 + rand()%130;

       // select direction of motion

       if ((rand()%2)==SUB_LEFT)
          {
          // set all fields

          subs[index].xv                = -2 - rand()%6;
          subs[index].x                 = 320 - subs[index].object.width;
          subs[index].direction         = SUB_LEFT;
          subs[index].object.curr_frame = 1;

          } // end if left
       else
          {
          // set all fields

          subs[index].xv                = 2 + rand()%6;
          subs[index].x                 = 0;
          subs[index].direction         = SUB_RIGHT;
          subs[index].object.curr_frame = 0;

          } // end else direction right

       // scan background under sub

       // extract proper sprite size

       sprite_width  = subs[index].object.width;
       sprite_height = subs[index].object.height;

       // scan behind sprite

       subs[index].object.x = subs[index].x;
       subs[index].object.y = subs[index].y;

       Behind_Sprite_DB((sprite_ptr)&subs[index].object);

       // done so bail

       break;

       } // end if dead

    } // end for index

} // end Start_Sub

///////////////////////////////////////////////////////////////////////////

void Move_Subs(void)
{
// thjis function moves all the subs and kills them if they hit the edge
// of the screen

int index,  // used for loop variable
    sx,sy;  // used for temporary position variables

// process each sub or boat in a rray

for (index=0; index<NUM_SUBS; index++)
    {

    // is this vehicle active ?

    if (subs[index].state == SUB_ALIVE)
       {

       // move the sub

       sx = (subs[index].x+=subs[index].xv);
       sy = (subs[index].y+=subs[index].yv);

       // do boundary tests

       if (sy>155)
          subs[index].y = 155;
       else
       if ((subs[index].type != SUB_BOAT) && sy<16)
          subs[index].y = 16;

       if ( ((subs[index].direction==SUB_LEFT) && (sx<0)) ||
            ((subs[index].direction==SUB_RIGHT) &&
             (sx>320-subs[index].object.width)) )
          {

          // terminate sub or boat

          subs[index].state = SUB_DEAD;

          // increment number of ships that got away

          got_away_this_wave++;

          } // end if off screen

       } // end if alive

    } // end for index

} // end Move_Subs

////////////////////////////////////////////////////////////////////////////

void Draw_Score(void)
{

// this function draws the players score in the display module

char buffer[128];

// show the score

sprintf(buffer,"%ld",players_score);

Blit_String_DB(55,180,10,buffer,0);

} // end Draw_Score

/////////////////////////////////////////////////////////////////////////////

void Rotate_Lights(void)
{
// this is a self contained functions that uses color rotation to make the
// lights on the cannon rotate

static int clock=0,       // used for timing, note: they are static!
           entered_yet=0;

RGB_color color,          // used to hold color values during processing
          color_1,
          color_2,
          color_3;

// test if function is being called for first time

if (!entered_yet)
   {

   // reset the palette registers to bright blue, dark blue, dark blue

   color.red   = 0;
   color.green = 0;
   color.blue  = 63;

   Set_Palette_Register(246,(RGB_color_ptr)&color);

   color.blue  = 20;

   Set_Palette_Register(247,(RGB_color_ptr)&color);
   Set_Palette_Register(248,(RGB_color_ptr)&color);

   // system has initialized, so flag it

   entered_yet=1;

   } // end if first time into function

// try and rotate the light colors i.e. color rotation

   if (++clock==5)  // is it time to rotate
      {
      // get the colors

      Get_Palette_Register(246,(RGB_color_ptr)&color_1);
      Get_Palette_Register(247,(RGB_color_ptr)&color_2);
      Get_Palette_Register(248,(RGB_color_ptr)&color_3);

      // set the colors

      Set_Palette_Register(248,(RGB_color_ptr)&color_1);
      Set_Palette_Register(246,(RGB_color_ptr)&color_2);
      Set_Palette_Register(247,(RGB_color_ptr)&color_3);

      // reset the clock

      clock=0;

      } // end if time to rotate

} // end Rotate_Lights

////////////////////////////////////////////////////////////////////////////

void Start_Wave(void)
{
// this function is used to start a wave, it resets all the system variables

// reset number of ships that got away

got_away_this_wave        = 0;

// reset wave end and clock

wave_end        = 0;

wave_clock      = 0;

// reset number of ships hit this wave

hits_this_wave  = 0;

ships_large_hit = 0;

ships_small_hit = 0;

ships_boat_hit  = 0;

// reset number of ships so far

ships_so_far    = 0;

// more ships as the wave number increases

ships_this_wave = 10 + wave_number*2;

// increase wave number

wave_number++;

} // end Start_Wave

/////////////////////////////////////////////////////////////////////////////

void Erase_End_Wave(void)
{
// this function is used to erase the imagery drawn by the end of wave display

int index;  // loop index

// draw a big  blue rectangle in area where stats will be displayed

for (index=40; index<=132; index++)
    {

    _fmemset((char far *)(double_buffer+((index<<8)+(index<<6))+88),
              WATER_COLOR,162);

    } // end for index

} // end Erase_End_Wave

/////////////////////////////////////////////////////////////////////////////

void Draw_End_Wave(void)
{
// this function is used to draw the imagery drawn by the end of wave display
// also note how it is self contained

static int entered=0;  // used to flag if the function has been entered once

char buffer[128];      // used for string printing

int bonus;             // used to compute players bonus

// should we play sound

if (!entered)
   {
   Play_Sound(SOUND_WAVE);
   entered=1;
   } // end if first time

// draw text

sprintf(buffer,"WAVE #%d...COMPLETE!",wave_number);

Blit_String_DB(88,40,12,buffer,1);

sprintf(buffer,"Hits        Target");

Blit_String_DB(88,50,15,buffer,1);

sprintf(buffer,"%d",ships_large_hit);

Blit_String_DB(96,64,15,buffer,1);

sprintf(buffer,"%d",ships_small_hit);

Blit_String_DB(96,64+20,15,buffer,1);

sprintf(buffer,"%d",ships_boat_hit);

Blit_String_DB(96,64+40,15,buffer,1);

// compute bonus and display

bonus = (ships_this_wave - got_away_this_wave) * 25;

sprintf(buffer,"BONUS  %d",bonus);

Blit_String_DB(88,64+60,10,buffer,1);

// draw icons

SET_SPRITE_SIZE(48,64);

Draw_Sprite_DB((sprite_ptr)&icons);

// increment counter and test

if (++wave_clock>75)
   {

   // add up total ships that got away to running total

   total_got_away+=got_away_this_wave;

   // start system back up

   Start_Wave();

   Erase_End_Wave();

   // add bonus to players score

   players_score+=bonus;

   // reset static

   entered=0;

   } // end if

} // end Draw_End_Wave

////////////////////////////////////////////////////////////////////////////

void Load_Subs(void)
{
// this function loads the imagery for the subs

int index;  // looping index

// load in imagery for small subs

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("sharksb1.pcx", (pcx_picture_ptr)&imagery_pcx,1);


SET_SPRITE_SIZE(32,10);

// load in frames for subs

for (index=0; index<=3; index++)
    {


    Sprite_Init((sprite_ptr)&subs[index].object,0,0,0,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&subs[index].object,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&subs[index].object,1,1,0);

    // set up fields

    subs[index].object.curr_frame = 0;
    subs[index].type              = SUB_SMALL;
    subs[index].state             = SUB_DEAD;
    subs[index].object.width      = 32;
    subs[index].object.height     = 10;

    } // end for index

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);



// load in imagery for large subs

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("sharksb2.pcx", (pcx_picture_ptr)&imagery_pcx,1);

SET_SPRITE_SIZE(48,20);

for (index=4; index<=7; index++)
    {

    Sprite_Init((sprite_ptr)&subs[index].object,0,0,0,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&subs[index].object,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&subs[index].object,1,1,0);

    // set up fields

    subs[index].object.curr_frame = 0;
    subs[index].type              = SUB_LARGE;
    subs[index].state             = SUB_DEAD;
    subs[index].object.width      = 48;
    subs[index].object.height     = 20;

    } // end for index

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);



// load in imagery for boats

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("sharkbot.pcx", (pcx_picture_ptr)&imagery_pcx,1);

SET_SPRITE_SIZE(24,8);

for (index=8; index<=11; index++)
    {

    Sprite_Init((sprite_ptr)&subs[index].object,0,0,0,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&subs[index].object,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&subs[index].object,1,1,0);

    // set up fields

    subs[index].object.curr_frame = 0;
    subs[index].type              = SUB_BOAT;
    subs[index].state             = SUB_DEAD;
    subs[index].object.width      = 24;
    subs[index].object.height     = 8;

    } // end for index

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

// load in imagery for ship icons

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("sharkicn.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize icons and extract bitmaps

SET_SPRITE_SIZE(48,64);

Sprite_Init((sprite_ptr)&icons,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&icons,0,0,0);

icons.x          = 200;
icons.y          = 56;
icons.curr_frame = 0;
icons.state      = 1;

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Subs

///////////////////////////////////////////////////////////////////////////////

void Load_Player(void)
{
// this function loads the imagery for the players

int index;  // loop index

// load in imagery for player's gun

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("sharkgun.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize player and extract bitmaps

SET_SPRITE_SIZE(24,24);

Sprite_Init((sprite_ptr)&player,0,0,0,0,0,0);

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,0,0,0);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,1,1,0);
PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,(sprite_ptr)&player,2,2,0);

player.x          = 148;
player.y          = 149;
player.curr_frame = CANNON_NORTH;
player.state      = 1;

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

// load in imagery for missiles

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("sharkmis.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize player and extract bitmaps

SET_SPRITE_SIZE(8,8);

// initialize the sprite for each missile

for (index=0; index<NUM_MISSILES; index++)
    {

    Sprite_Init((sprite_ptr)&missiles[index].object,0,0,0,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&missiles[index].object,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&missiles[index].object,1,1,0);


    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&missiles[index].object,2,2,0);

    // set some important fields

    missiles[index].x                 = 0;
    missiles[index].y                 = 0;
    missiles[index].object.curr_frame = 0;
    missiles[index].state             = MISS_DEAD;

    } // end for

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);


} // end Load_Player

///////////////////////////////////////////////////////////////////////////////

void Load_Explosions(void)
{
// this function loads the imagery for the explosions (note there are 3 versions)

int index,  // loop variable
    row;    // used to select a random explosion

// load in imagery for explosions

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("sharkexp.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize the explosions and extract bitmaps

SET_SPRITE_SIZE(32,32);

// load in frames for explosions

for (index=0; index<NUM_EXPLOSIONS; index++)
    {

    // select a random explosion to place in sprite

    row = rand()%2;

    Sprite_Init((sprite_ptr)&explosions[index],0,0,0,0,0,0);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],0,0,row);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],1,1,row);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],2,2,row);

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&explosions[index],3,3,row);

    } // end for

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Explosions

//////////////////////////////////////////////////////////////////////////////

void Do_Intro(void)
{
// this function displays the introduction screen and then melts it

// load intro screen and display for a few secs.

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("sharkint.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

// let user see it

Delay(50);

Fade_Lights();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

// load in the background image into the double buffer

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load("sharkbak.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);


} // end Do_Intro

//////////////////////////////////////////////////////////////////////////////

void Show_Instructions(void)
{
// this function displays the instructions and then disolves them

// load instruction screen and display it until a key press

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("sharkins.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

// let user see it

while(!kbhit()){};

getch();

Disolve();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

} // end Show_Instructions

// M A I N /////////////////////////////////////////////////////////////////

void main(void)
{

int done=0,    // main event loop exit flag
    index;     // loop variable

char buffer[128]; // used for string printing

printf("\nStarting Sea Shark...");

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

// clear the double buffer

Fill_Double_Buffer(0);

// impress user (at least try to)

Do_Intro();

// show instructions

Show_Instructions();

// load imagery for game

Load_Player();

Load_Explosions();

Load_Subs();

// initialize everything

Init_Missiles();

Init_Bubbles();

Init_Explosions();

Init_Subs();

// start up wave one

Start_Wave();

// scan background first time around under everything

// set sprite size for engine and scan background under player

SET_SPRITE_SIZE(24,24);

Behind_Sprite_DB((sprite_ptr)&player);

// begin main event loop

while(!done)
     {

     // erase all objects

     // set sprite size for engine

     SET_SPRITE_SIZE(24,24);

     Erase_Sprite_DB((sprite_ptr)&player);

     // erase missiles

     Erase_Missiles();

     // erase bubbles

     Erase_Bubbles();

     // erase explosions

     Erase_Explosions();

     // erase subs

     Erase_Subs();

     if (wave_end)
        Erase_End_Wave();

     // move all objects

     // test for keyboard press

     if (kbhit())
        {

        // what does user want to do?

        switch(getch())
              {

              case 'a': // move cannon one to the left
                      {

                      // decrement animation cell

                      if (--player.curr_frame<0)
                         player.curr_frame=0;

                      } break;

              case 's': // move cannon one to the right
                      {

                      // increment animation cell

                      if (++player.curr_frame==3)
                         player.curr_frame=2;

                      } break;


              case ' ': // start a missile
                      {

                      // try and start a missile
                      // remember to use direction and velocity look up
                      // so that direction of cannon is taken into
                      // consideration!

                      Start_Missile((sprite_ptr)&player,
                                    player.x+cannon_pos_x[player.curr_frame],
                                    player.y+cannon_pos_y[player.curr_frame],
                                    cannon_vel_x[player.curr_frame],
                                    cannon_vel_y[player.curr_frame],
                                    PLAYER_MISSILE);

                      } break;

              case 'q': // trying to exit
                      {
                      // set global exit flag
                      done=1;

                      } break;

              default:break;

              } // end switch

        } // end if kbhit

     // move subs

     Move_Subs();

     // move missiles

     Move_Missiles();

     // move bubbles

     Move_Bubbles();

     // animate explosions

     Animate_Explosions();

     // test if it's end of a wave

     if (game_over==0 && (got_away_this_wave+hits_this_wave >= ships_this_wave-1))
        {
        // set end of wave flag

        wave_end=1;

        // reset all subs so that they disapear

        Init_Subs();

        } // end if end of wave

     // BEGIN CRITICAL SECTION - this is where we can start new objects and
     // not have a problem

     // should a sub be sent out

     if (game_over==0 && wave_end==0 && (rand()%50)==0)
         Start_Sub();

     // test if the game is over

     if (total_got_away > 100 && game_over==0)
        {
        // set flag that game is over

        game_over=1;

        // allow game over sequence to last for 100 frames

        game_over_clock=100;

        } // end if game over

     // should we do the game over sequence

     if (game_over && game_over_clock > 0)
        {

        // start an explosion somewhere

        Start_Explosion(rand()%300, rand()%160, rand()%3);

        // draw score

        sprintf(buffer,"G A M E  O V E R !",wave_number);

        Blit_String_DB(84,64,12,buffer,1);

        sprintf(buffer,"Final Score %ld",players_score);

        Blit_String_DB(84,80,10,buffer,1);

        // test if it's time to exit game

        if (--game_over_clock==0)
           done=1;  // set main event loop exit flag

        } // end if game over sequence


     // END CRITICAL SECTION


     // now we scan and draw all objects

     // set sprite size for engine

     SET_SPRITE_SIZE(24,24);
     Behind_Sprite_DB((sprite_ptr)&player);

     Behind_Missiles();

     Behind_Subs();

     Behind_Bubbles();

     Behind_Explosions();

     // set sprite size for engine

     SET_SPRITE_SIZE(24,24);
     Draw_Sprite_DB((sprite_ptr)&player);

     Draw_Subs();

     Draw_Missiles();

     Draw_Bubbles();

     Draw_Explosions();

     Draw_Score();

     // test if the wave is over and statistics need to be displayed

     if (wave_end)
        Draw_End_Wave();

     // display double buffer

     Show_Double_Buffer((char far *)double_buffer);

     // do all color palette animation

     Rotate_Lights();

     // wait a sec

     Delay(1);

     } // end while

// were out of here!

Play_Sound(SOUND_END);

// exit system with a cool transition

Melt();

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

// close sound system

Close_Sound_System();

} // end main


