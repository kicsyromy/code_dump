
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

#include "graph3.h"   // include our graphics stuff
#include "graph4.h"
#include "graph6.h"
#include "graph8.h"   // fixed point library
#include "graph9.h"   // sound library
#include "graph11.h"  // multitasking and keyboard interrupt driver

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Show_Stats();

void Age_Orders(void);

void Order_Pizza(void);

void Blink_House(int house_number);

void Start_Pizza(int x,
                 int y,
                 int xv,
                 int yv,
                 int direction);

void Erase_Pizzas(void);

void Behind_Pizzas(void);

void Draw_Pizzas(void);

void Move_Pizzas(void);

void Init_Pizzas(void);

void Load_Death(void);

void Start_Boy_Death(void);

void Erase_Boy_Death(void);

void Behind_Boy_Death(void);

void Draw_Boy_Death(void);

void Animate_Boy_Death(void);

void Init_Messages(void);

int Insert_Message(char *string,int preempt);

void Start_Message(int index);

void Display_Message(void);

void Send_Message(void);

void Start_Splat(int x,int y,int speed,int color);

void Behind_Splats(void);

void Erase_Splats(void);

void Draw_Splats(void);

void Animate_Splats(void);

void Init_Splats(void);

void Erase_Humans(void);

void Behind_Humans(void);

void Draw_Humans(void);

int Start_Human(void);

void Move_Humans(void);

void Init_Humans(void);

void Erase_Cars(void);

void Behind_Cars(void);

void Draw_Cars(void);

void Move_Cars(void);

void Init_Cars(void);

unsigned char Get_Pixel_DB(int x,int y);

int Initialize_Sound_System(void);

void Close_Sound_System(void);

void Play_Sound(int sound);

void Do_Intro(void);

void Load_Environment(void);

void Load_Background(void);

void Draw_Gauages(void);

// D E F I N E S /////////////////////////////////////////////////////////////

#define DEBUG           0   // turns on/off debug information

#define FP_SHIFT_D64    12  // converts a fixed point to an int and divides
                            // by 16 in one operation

#define END_MOPEDS            1 // game ended because player used up mopeds
#define END_TIME              2 // game ended because it's past 5:00pm

// defines for pizzas

#define PIZZA_ALIVE           1 // state of a live Pizza
#define PIZZA_DEAD            0 // state of a dead Pizza
#define NUM_PIZZAS            3 // number of Pizzas that can be fired
#define NUM_PIZZA_FRAMES      8 // number of frames in pizza animation

#define START_NUM_COLOR (14*16)

#define END_NUM_COLOR   (14*16+23)


#define START_SPEEDO_COLOR  (10*16)
#define END_SPEEDO_COLOR    (10*16+5)

#define ROOF_COLOR_REG  204      // the roofs color register

#define MAX_HOUSES      32  // the maximum number of houses in the city

#define HOUSE_NO_ORDER  0   // this house has not ordered
#define HOUSE_ORDERED   1   // this house is waiting for a pizza

// game area defines

#define CELL_COLUMNS    20  // number of columns in the cell world
#define CELL_ROWS       11  // number of rows in the cell world

// general Splats

#define NUM_SPLATS       4  // number of Splats that can run at once

#define SPLAT_DEAD       0   // state of a dead Splat
#define SPLAT_ALIVE      1   // state of a live Splat

// human defines

#define NUM_HUMAN_FRAMES 16  // number of animation frames
#define NUM_HUMANS        8  // maximum number of humans

#define HUMAN_DEAD        0  // states of humans
#define HUMAN_NORTH       1
#define HUMAN_SOUTH       2
#define HUMAN_WEST        3
#define HUMAN_EAST        4

// player defines

#define NUM_BOY_FRAMES    16 // number of animation cells for players moped

#define BOY_DEAD          0  // states for vinnie
#define BOY_ALIVE         1
#define BOY_DYING         2

// car defines

#define NUM_CARS          8  // number of cars in the simulation
#define NUM_CAR_FRAMES    20 // number of animation cells for cars

#define CAR_COLOR_1       0  // base offsets for different colors
#define CAR_COLOR_2       4
#define CAR_COLOR_3       8
#define CAR_COLOR_4       12

#define CAR_DEAD          0  // states of cars
#define CAR_STARTING      1
#define CAR_DRIVING       2
#define CAR_SLOWING       3
#define CAR_STOPPED       4

#define CAR_NORTH         1  // directions of cars
#define CAR_SOUTH         2
#define CAR_WEST          3
#define CAR_EAST          4

// player defines for vinnie

#define PLAYER_DEAD       0
#define PLAYER_ALIVE      1
#define PLAYER_DYING      2

// direction of player

#define PLAYER_EAST       0
#define PLAYER_WEST       1
#define PLAYER_NORTH      2
#define PLAYER_SOUTH      3

// define bitmap id's

#define STOP_NORTH_ID     9     // a stop sign on a north bound street
#define STOP_SOUTH_ID     8     // a stop sign on an east bound street

#define HOUSE_WEST_1_ID   33    // id's for the 4 bitmaps that make up a west
#define HOUSE_WEST_2_ID   34    // facing house
#define HOUSE_WEST_3_ID   35
#define HOUSE_WEST_4_ID   36

#define HOUSE_EAST_1_ID   37    // id's for the 4 bitmaps that make up a east
#define HOUSE_EAST_2_ID   38    // facing house
#define HOUSE_EAST_3_ID   39
#define HOUSE_EAST_4_ID   40

#define NUM_MESSAGES      8  // maximum number of pending messages

// states of a message

#define MESS_DEAD         0  // this message is dead
#define MESS_WAIT         1  // this message is waiting

// sound system defines

#define NUM_SOUNDS           14   // number of sounds in system

#define SOUND_CAR_STOP       0  // a car stopping
#define SOUND_CAR_START      1  // a car starting
#define SOUND_MOPED_HORN     2  // the mopeds horn
#define SOUND_CAR_HORN       3  // a cars horn
#define SOUND_MOPED_HIT      4  // the moped being hit
#define SOUND_HUMAN_HIT      5  // a human being hit
#define SOUND_YO_VINNIE      6  // here's an order
#define SOUND_LOST_ORDER     7  // you lost an order
#define SOUND_THANK_YOU_B    8  // thank you from the customer  (boy)
#define SOUND_THANK_YOU_G    9  // thank you from the customer  (girl)
#define SOUND_TOO_LONG       10 // delivery took too long
#define SOUND_COME_HOME      11 // days over
#define SOUND_CAR_SOUND      12 // backgronud car sound
#define SOUND_START          13 // start of game


#define SOUND_DEFAULT_PORT   0x220  // default sound port for sound blaster
#define SOUND_DEFAULT_INT    5      // default interrupt

// M A C R O S //////////////////////////////////////////////////////////////

#define SET_SPRITE_SIZE(w,h) {sprite_width=w; sprite_height=h;}

// S T R U C T U R E S ///////////////////////////////////////////////////////

// this is the structure used to database all the house positions in the city

typedef struct house_typ
        {
        int x;      // position of house
        int y;
        int type;   // type of house i.e. east or west front door
        int num;    // house number

        int state;  // is there an order pending

        int timer;  // the current time left to deliver the pizza

        } house, *house_ptr;

// typedef for the car

typedef struct car_typ
        {
        int x;                   // x position
        int y;                   // y position
        int curr_xv;             // x velocity
        int curr_yv;             // y velocity

        int max_xv;              // max x velocity
        int max_yv;              // max y velocity

        int state;               // the state of the car
        int direction;
        int counter_1;           // use for counting
        int threshold_1;         // the counters threshold

        int counter_2;
        int threshold_2;

        int counter_3;
        int threshold_3;

        int color;              // color of car

        sprite object;          // the sprite

        } car, *car_ptr;


// typedef for the player

typedef struct player_typ
        {
        fixed x;                   // x position
        fixed y;                   // y position

        fixed xv;                  // x velocity
        fixed yv;                  // y velocity

        fixed curr_xv;             // x velocity
        fixed curr_yv;             // y velocity

        fixed max_xv;              // max x velocity
        fixed max_yv;              // max y velocity


        fixed throttle;            // current throttle position
        fixed hp;                  // horse power
        fixed friction;            // the friction of the air, bike, ...
        fixed brakes;              // the braking friction

        fixed max_throttle;        // maximum throttle position

        int state;               // the state of the player
        int direction;
        int counter_1;           // use for counting
        int threshold_1;         // the counters threshold

        int counter_2;

        int threshold_2;

        int counter_3;
        int threshold_3;

        sprite object;          // the sprite

        } player, *player_ptr;

// typedef for a projectile

typedef struct projectile_typ
        {
        int x;                   // x position
        int y;                   // y position
        int xv;                  // x velocity
        int yv;                  // y velocity
        int state;               // the state of the particle
        int counter_1;           // use for counting
        int threshold_1;         // the counters threshold

        int counter_2;
        int threshold_2;

        sprite object;           // the projectile sprite

        } projectile, *projectile_ptr;

// this structure holds the message data

typedef struct message_type
        {

        char string[64];   // holds the message

        int state;         // state of message waiting, playing, dead

        } message, *message_ptr;

// G L O B A L S  ////////////////////////////////////////////////////////////

pcx_picture imagery_pcx,      // the game imagery
            background_pcx,   // the backdrop
            intro_pcx;        // the introduction screen

// the sprites used in the game

sprite roads,                 // the road bitmaps
       builds,                // the building and house bitmaps
       numbers,               // the house numbers
       humans[NUM_HUMANS],    // the humans on the streets
       splats[NUM_SPLATS],    // the array of Splats
       boy_death;             // the death sequence sprite for the boy

projectile pizzas[NUM_PIZZAS];  // the array of pizzas in the world

player boy;                     // vinnie the pizza boy

// statistics of boy

int boy_mopeds    = 3,          // number of mopeds
    boy_pizzas    = 0,          // number of pizzas on moped rack
    boy_xpeds     = 0,          // number of peds player has killed
    boy_tips      = 0,          // current tips
    boy_time      = 540,        // the time in minutes
    orders_filled = 0,          // how many orders have been filled
    total_orders  = 0;          // total pizzas ordered


int throttle_on=0;            // flags if the pizza boy has the throttle on

car cars[NUM_CARS];           // the cars in the game

house house_pos[MAX_HOUSES];  // the houses in the city

int curr_house;               // the last house entered into database

RGB_color roof_color,                // used to hold roof color
          alert_color = {63,63,63};  // used as the house wants a pizza color!

int lite_house =  0;      // debuging var used to light houses up

int difficulty=75;        // difficulty of game, 100 is easy 1 is hardest

long players_score = 0;    // initial score

int players_dir    = PLAYER_SOUTH;  // initial direction of player

message messages[NUM_MESSAGES];   // the array of messages which is really a
                                  // circular queue

int mess_head=-1,                 // head of message queue
    mess_tail=-1;                 // tail of message queue

int message_index  = 0,           // current string index of message
    message_length = 0,           // overall length of message
    message_done   = 1;           // is the message done playing

char message_string[128];         // global message string

// the sound system variables

char far *sound_fx[NUM_SOUNDS];           // pointers to the voc files
unsigned char sound_lengths[NUM_SOUNDS];  // lengths of the voc files

int sound_available = 0;                  // flags if sound is available

int sound_port = SOUND_DEFAULT_PORT;      // default sound port
int sound_int  = SOUND_DEFAULT_INT;       // default sound interrupt


// data structures

// the city that pizza boy delivers to

char *city_1[CELL_ROWS] = { "..45[]0145[]{.45[]01",
                            "..76[]3276[]0176[]32",
                            "..45[]01.}[]3245[]01",
                            "..76s]32.}s]{.76s]32",
                            "^^^^##^^^^##^^^^##^^",
                            "vvvv##vvvv##vvvv##vv",
                            "..45[S{.45[S0145[S{.",
                            "..76[]0176[]3276[]01",
                            "cd89[]3245[]{..}[]32",
                            "feba[]0176[]0145[]01",
                            "...}[]32.}[]3276[]32",};


// velocities used to compute trajectory vector of moped

fixed boy_xv[NUM_BOY_FRAMES] =

                    {(fixed)(1.000000*FP_SHIFT_2N),
                     (fixed)(0.923880*FP_SHIFT_2N),
                     (fixed)(0.707107*FP_SHIFT_2N),
                     (fixed)(0.382684*FP_SHIFT_2N),
                     (fixed)(0.000001*FP_SHIFT_2N),
                    (fixed)(-0.382682*FP_SHIFT_2N),
                    (fixed)(-0.707105*FP_SHIFT_2N),
                    (fixed)(-0.923879*FP_SHIFT_2N),
                    (fixed)(-1.000000*FP_SHIFT_2N),
                    (fixed)(-0.923881*FP_SHIFT_2N),
                    (fixed)(-0.707109*FP_SHIFT_2N),
                    (fixed)(-0.382687*FP_SHIFT_2N),
                    (fixed)(-0.000004*FP_SHIFT_2N),
                    (fixed)(0.382679*FP_SHIFT_2N),
                    (fixed)(0.707103*FP_SHIFT_2N),
                    (fixed)(0.923878*FP_SHIFT_2N)};


fixed boy_yv[NUM_BOY_FRAMES] =

                   {(fixed)(0.000000 *FP_SHIFT_2N),
                    (fixed)(-0.382683*FP_SHIFT_2N),
                    (fixed)(-0.707106*FP_SHIFT_2N),
                    (fixed)(-0.923879*FP_SHIFT_2N),
                    (fixed)(-1.000000*FP_SHIFT_2N),
                    (fixed)(-0.923880*FP_SHIFT_2N),
                    (fixed)(-0.707108*FP_SHIFT_2N),
                    (fixed)(-0.382686*FP_SHIFT_2N),
                    (fixed)(-0.000003*FP_SHIFT_2N),
                    (fixed)(0.382681 *FP_SHIFT_2N),
                    (fixed)(0.707104 *FP_SHIFT_2N),
                    (fixed)(0.923878 *FP_SHIFT_2N),
                    (fixed)(1.000000 *FP_SHIFT_2N),
                    (fixed)(0.923881 *FP_SHIFT_2N),
                    (fixed)(0.707110 *FP_SHIFT_2N),
                    (fixed)(0.382688 *FP_SHIFT_2N)};


// an integer version of the fixed point tables used to throw the pizzas with

int pizza_xv[] ={3,
                 3,
                 2,
                 1,
                 0,
                 0,
                -1,
                -2,
                -2,
                -2,
                -1,
                 0,
                 0,
                 1,
                 2,
                 3,};

int pizza_yv[] = {0,
                  0,
                 -1,
                 -2,
                 -2,
                 -2,
                 -1,
                  0,
                  0,
                  1,
                  2,
                  3,
                  3,
                  3,
                  2,
                  1,};


// F U N C T I O N S //////////////////////////////////////////////////////////

void Erase_Boy_Death(void)
{

// this function erases the flying moped


// test if the boy is dying

if (boy.state == BOY_DYING)
   {

   // set sprite size for engine

   SET_SPRITE_SIZE(32,30);

   // erase the sprite

   Erase_Sprite_DB((sprite_ptr)&boy_death);

   } // end if boy is dying

} // end Erase_Boy_Death

//////////////////////////////////////////////////////////////////////////////

void Draw_Boy_Death(void)
{

// this function draws the flying moped


// test if the boy is dying

if (boy.state == BOY_DYING)
   {

   // set sprite size for engine

   SET_SPRITE_SIZE(32,30);

   // draw the sprite

   Draw_Sprite_DB((sprite_ptr)&boy_death);

   } // end if boy is dying

} // end Draw_Boy_Death

///////////////////////////////////////////////////////////////////////////////

void Behind_Boy_Death(void)
{

// this function scans under the flying moped


// test if the boy is dying

if (boy.state == BOY_DYING)
   {

   // set sprite size for engine

   SET_SPRITE_SIZE(32,30);

   // erase the sprite

   Behind_Sprite_DB((sprite_ptr)&boy_death);

   } // end if boy is dying

} // end Behind_Boy_Death

//////////////////////////////////////////////////////////////////////////////

void Animate_Boy_Death(void)
{

// this function moves and animates the flying moped

if (boy.state==BOY_DYING)
   {

   // test if we are moving up or down

   if (boy_death.motion_clock==1) // going up i.e. toward center of screen
      {
      // move player toward center of screen to make it look 3-D

      // x - tracking

      if (boy_death.x<160)
          boy_death.x+=4;
      else
      if  (boy_death.x>160+4)
           boy_death.x-=4;

      // y - tracking

      if (boy_death.y<100)
          boy_death.y+=4;
      else
      if  (boy_death.y>100+4)
           boy_death.y-=4;

      } // end if going up
   else
      {
      // must be falling back to earth

      // move player toward where he was hit

      // x - tracking

      if (boy_death.x<boy_death.x_old)
          boy_death.x+=4;
      else
      if  (boy_death.x>boy_death.x_old+4)
           boy_death.x-=4;

      // y - tracking

      if (boy_death.y<boy_death.y_old )
          boy_death.y+=4;
      else
      if  (boy_death.y>boy_death.y_old+4)
           boy_death.y-=4;

      } // end else

   // do a boundary test to make sure sprite stays on screen

   // x - tests

   if (boy_death.x>320-32)
       boy_death.x=320-32;
   else
   if (boy_death.x<0)
       boy_death.x=0;

   // y - tests

   if (boy_death.y>200-32)
       boy_death.y=200-32;
   else
   if (boy_death.y<0)
       boy_death.y=0;

   // test if it's time to animate

   if (++boy_death.anim_clock==boy_death.anim_speed)
      {

      // reset clock

      boy_death.anim_clock = 0;

      // move to next frame in sequence

      ++boy_death.curr_frame;

      // test if it's time to end death or change direction of motion

      if (boy_death.curr_frame==7)
         {
         // make player fall back down

         boy_death.motion_clock=-1;

         } // end if start player back to earth
      else
      if (boy_death.curr_frame==16)
         {

         // player has gone through death sequence so start him back at
         // the pizza hut

         // set up state information

         boy.state             = BOY_ALIVE;
         boy_death.state       = BOY_ALIVE;

         boy.x                 = Assign_Integer(48);
         boy.y                 = Assign_Integer(144);
         boy.curr_xv           = 0;
         boy.curr_yv           = 0;
         boy.max_xv            = 0;
         boy.max_yv            = 0;
         boy.xv                = 0;
         boy.yv                = 0;


         boy.throttle          = 0;
         boy.hp                = Assign_Float((float).4);
         boy.friction          = Assign_Float((float)-.10);
         boy.brakes            = Assign_Integer(1);

         boy.max_throttle      = Assign_Integer(3);

         boy.counter_1         = 0;
         boy.threshold_1       = 2;

         boy.object.curr_frame = 0;
         boy.direction         = 0;

         } // end if it's all over

      } // end if time to change frames

   } // end if boy is dying

} // end Animate_Boy_Death

/////////////////////////////////////////////////////////////////////////////

void Start_Boy_Death(void)

{

// this function starts the animation sequence for the boy being hit by a car

boy.state                = BOY_DYING;

boy_death.state          = BOY_DYING;
boy_death.x              = (int)(boy.x >> FP_SHIFT)+3-16;
boy_death.y              = (int)(boy.y >> FP_SHIFT)+3-16;

boy_death.x_old          = boy_death.x;
boy_death.y_old          = boy_death.y;

boy_death.anim_clock     = 0;
boy_death.anim_speed     = 2;
boy_death.motion_clock   = 1; // use this to mean going up

// make sure proper animation cell is selected

boy_death.curr_frame = 0;

// set sprite size for engine

SET_SPRITE_SIZE(32,30);

Behind_Sprite_DB((sprite_ptr)&boy_death);

// send a message

Insert_Message("ARRRRGGGGG!!!!!",1);

} // end Start_Boy_Death

/////////////////////////////////////////////////////////////////////////////

void Erase_Pizzas(void)
{

// this function indexes through all the pizzas and if they are active
// erases them by replacing the background color that was under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(6,6);

for (index=0; index<NUM_PIZZAS; index++)
    {

    // is this pizza active

    if (pizzas[index].state == PIZZA_ALIVE)
       {

       // extract proper parameters

       pizzas[index].object.x = pizzas[index].x;
       pizzas[index].object.y = pizzas[index].y;

       // erase the sprite

       Erase_Sprite_DB((sprite_ptr)&pizzas[index].object);

       } // end if alive

    } // end for index

} // end Erase_Pizzas

/////////////////////////////////////////////////////////////////////////////

void Behind_Pizzas(void)
{

// this function indexes through all the pizzas and if they are active
// scans the background color that is behind them so it can be replaced later

int index;

// set sprite size for engine

SET_SPRITE_SIZE(6,6);

for (index=0; index<NUM_PIZZAS; index++)
    {

    // is this pizza active

    if (pizzas[index].state == PIZZA_ALIVE)
       {

       // extract proper parameters

       pizzas[index].object.x = pizzas[index].x;
       pizzas[index].object.y = pizzas[index].y;

       // scan begind the sprite

       Behind_Sprite_DB((sprite_ptr)&pizzas[index].object);

       } // end if alive

    } // end for index

} // end Behind_Pizzas

/////////////////////////////////////////////////////////////////////////////

void Draw_Pizzas(void)
{

// this function indexes through all the pizzas and if they are active
// draws the missile as a bright white pixel on the screen

int index;

// set sprite size for engine

SET_SPRITE_SIZE(6,6);

for (index=0; index<NUM_PIZZAS; index++)
    {

    // is this pizza active

    if (pizzas[index].state == PIZZA_ALIVE)
       {

       // extract proper parameters

       pizzas[index].object.x = pizzas[index].x;
       pizzas[index].object.y = pizzas[index].y;

       // draw the sprite

       Draw_Sprite_DB((sprite_ptr)&pizzas[index].object);

       } // end if alive

    } // end for index

} // end Draw_Pizzas

/////////////////////////////////////////////////////////////////////////////

void Move_Pizzas(void)
{

// this function moves the pizzas and does all the collision detection

int index,              // used for loops
    hindex,             // used to index through houses
    pizza_x,            // position of pizza
    pizza_y,
    pizza_x_center,     // center of pizza
    pizza_y_center,
    cell_x,cell_y,cell_id;  // used to test if pizza has hit a background cell

float tip;              // used to compute tip

char buffer[128];  // used to build up strings

// loop thru all pizzas and perform a lot of tests

for (index=0; index<NUM_PIZZAS; index++)
    {

    // is missile active

    if (pizzas[index].state == PIZZA_ALIVE)
       {

       // move the pizza

       pizza_x = (pizzas[index].x += pizzas[index].xv);
       pizza_y = (pizzas[index].y += pizzas[index].yv);

       // animate the pizza

       if (++pizzas[index].object.curr_frame>=NUM_PIZZA_FRAMES)
          pizzas[index].object.curr_frame = 0;

       // compute center of pizza for ease of computations

       pizza_x_center = pizza_x+3;
       pizza_y_center = pizza_y+3;

       // comment this stuff out for now
       // cell_x = pizza_x_center  >> 4;  // divide by 16 since cells are 16x16 pixels
       // cell_y = pizza_y_center  >> 4;
       // what is the cell at this location
       // cell_id =


       // test if pizza has entered into a house that has ordered space

       for (hindex=0; hindex<=curr_house; hindex++)
           {

           // test if this house has a pending order

           if (house_pos[hindex].state == HOUSE_ORDERED)
              {

              // test if pizza has hit it taking into consideration direction
              // house is facing

              if (house_pos[hindex].type == 1 ) // west facing house
                 {
                 // do collision test for west facing

                 if (pizza_x_center>=house_pos[hindex].x+5 &&
                     pizza_x_center<=house_pos[hindex].x+17 &&
                     pizza_y_center>=house_pos[hindex].y+3 &&
                     pizza_y_center<=house_pos[hindex].y+20 )
                    {
                    // pizza boy made the delivery!

                    // kill pizza

                    pizzas[index].state = PIZZA_DEAD;

                    // increase number of orders filled

                    orders_filled++;

                    // take house off delivery list

                    house_pos[hindex].state = HOUSE_NO_ORDER;

                    // compute tip based on elapsed time

                    tip = 5*((float)house_pos[hindex].timer/(float)500);

                    boy_tips+=(int)tip;

                    // build up message depending on amount of time

                    if (house_pos[hindex].timer<100)
                       {

                       sprintf(buffer,"It took long enough!");

                       // send it

                       Insert_Message(buffer,1);

                       // play sound

                       Play_Sound(SOUND_TOO_LONG);

                       } // end if took too long
                    else
                       {

                       sprintf(buffer,"Thank you!");

                       // send it

                       Insert_Message(buffer,1);

                       if (rand()%2==1)
                          Play_Sound(SOUND_THANK_YOU_B );
                       else
                          Play_Sound(SOUND_THANK_YOU_G );



                       } // end else good service

                    } // end if we have a winner

                 } // end if house facing west
              else
                 {
                 // house is facing east

                 // do collision test for east facing

                 if (pizza_x_center>=house_pos[hindex].x-1 &&
                     pizza_x_center<=house_pos[hindex].x+11 &&
                     pizza_y_center>=house_pos[hindex].y+3 &&
                     pizza_y_center<=house_pos[hindex].y+19 )
                    {
                    // pizza boy made the delivery!

                    // kill pizza

                    pizzas[index].state = PIZZA_DEAD;

                    // increase number of orders filled

                    orders_filled++;

                    // take house off delivery list

                    house_pos[hindex].state = HOUSE_NO_ORDER;

                    // compute tip based on elapsed time

                    tip = 5*((float)house_pos[hindex].timer/(float)500);

                    boy_tips+=(int)tip;

                    // build up message depending on amount of time

                    if (house_pos[hindex].timer<100)
                       {

                       sprintf(buffer,"It took long enough!");

                       // send it with urgency

                       Insert_Message(buffer,0);

                       Play_Sound(SOUND_TOO_LONG );

                       } // end if took too long
                    else
                       {

                       sprintf(buffer,"Thank you!");

                       // send it with urgency

                       Insert_Message(buffer,0);

                       if (rand()%2==1)
                          Play_Sound(SOUND_THANK_YOU_B );
                       else
                          Play_Sound(SOUND_THANK_YOU_G );


                       } // end else good service

                    } // end if we have a winner

                 } // end else house facing east

              } // end if house has ordered

           } // end for hindex

       // test if it's hit the edge of the screen or a wall

       if ( (pizza_x >= 320-8) || (pizza_x <= 0) ||
            (pizza_y > (176-8)) || (pizza_y <= 0) )
          {

          // kill pizza

          pizzas[index].state = PIZZA_DEAD;

          } // end if off edge of screen

       // test if pizza has timed out

       if (++pizzas[index].counter_1>=pizzas[index].threshold_1)
          {

          // kill pizza

          pizzas[index].state = PIZZA_DEAD;

          } // end if timed out

       } // end if pizza alive

    } // end for index

} // end Move_Pizzas

/////////////////////////////////////////////////////////////////////////////

void Start_Pizza(int x,
                 int y,
                 int xv,
                 int yv,
                 int direction)

{

// this function scans through the pizzas array and tries to find one that
// isn't being used.  this function could be more efficient.

int index;

// scan for a useable pizza

for (index=0; index<NUM_PIZZAS; index++)
    {
    // is this pizza free?

    if (pizzas[index].state == PIZZA_DEAD)
       {

       // set up fields

       pizzas[index].state       = PIZZA_ALIVE;
       pizzas[index].x           = x;
       pizzas[index].y           = y;
       pizzas[index].xv          = xv;
       pizzas[index].yv          = yv;
       pizzas[index].counter_1   = 0;
       pizzas[index].threshold_1 = 25;

       // make sure proper animation cell is selected

       pizzas[index].object.curr_frame = 0;

       // extract proper parameters

       pizzas[index].object.x = x;
       pizzas[index].object.y = y;

       // set sprite size for engine

       SET_SPRITE_SIZE(6,6);

       Behind_Sprite_DB((sprite_ptr)&pizzas[index].object);

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Pizza

/////////////////////////////////////////////////////////////////////////////

void Init_Pizzas(void)
{
// this function just makes sure all the "state" fields of the pizzas are
// dead so that we don't get any strays on start up.  Remember never assume
// that variables are zeroed on instantiation!

int index;

for (index=0; index<NUM_PIZZAS; index++)
    pizzas[index].state = PIZZA_DEAD;

} // Init_Pizzas

//////////////////////////////////////////////////////////////////////////////

void Init_Messages(void)
{
// this function initializes the messages data structure

int index;  // loop index

for (index=0; index<NUM_MESSAGES; index++)
    {

    strcpy(messages[index].string,"");

    // set state to dead

    messages[index].state = MESS_DEAD;

    } // end for index

} // end Init_Messages

///////////////////////////////////////////////////////////////////////////////

int Insert_Message(char *string,int preempt)
{
// this function inserts a message into the message queue, but doesn't guarantee
// that it will be played right now unless the preempt flag is true

// test for preempt

if (preempt)
   {

   // force feed message into display

   message_index=0;

   message_done=0;

   strcpy(message_string,"...........");
   strcat(message_string,string);

   // compute length before adding on the end dots

   message_length = strlen(message_string);

   strcat(message_string,"...........");

   return(1);

   } // end if preempt

// advance message index

if (++mess_head >= NUM_MESSAGES)
   mess_head=0;

// insert message into array

strcpy(messages[mess_head].string,string);

// set state of message to waiting

messages[mess_head].state = MESS_WAIT;

return(1);

} // end Insert_Message

///////////////////////////////////////////////////////////////////////////////

void Start_Message(int index)
{
// this message conditions the new message to be played and resets all the
// appropriate variables

message_index=0;

message_done=0;

strcpy(message_string,"...........");
strcat(message_string,messages[index].string);

// compute length before adding on the end dots

message_length = strlen(message_string);

strcat(message_string,"...........");

} // end Start_Message

/////////////////////////////////////////////////////////////////////////////

void Display_Message(void)
{

static int counter=0,   // used as a time reference to slow scrolling of mess.
           entered=0;   // standard first time entered flag

char buffer[128];       // working buffer

// this is an autonomous function that is called every cycle to update
// the message center

if (!entered)
   {

   // set flag

   entered = 1;

   // do any initialization chores

   } // end if first time


// test fi it's time to process

if (++counter>1)
   {

   // reset counter

   counter=0;

   // test if message is not done

   if (!message_done)
      {

      // update display
      // we need to copy the proper number of bytes from the message string to the
      // buffer string

      memcpy((char *)buffer,(char *)&message_string[message_index],11);

      // set the null character

      buffer[11] = 0;

      Blit_String_DB(142,190,15,buffer,0);

      // update index and test for completion of message

      if (++message_index>message_length)
         {

         // send message to system that message is done

         message_done=1;

         } // end if message complete


      } // end if message not done

   } // end if it's time to process

} // end Display_Message

///////////////////////////////////////////////////////////////////////////////

void Send_Message(void)
{
// this function will be called every cycle and will try to send the message
// display another if it can

int index; // loop variable

// first test if current message in display is complete

if (message_done)
   {

   // is there another message

   while(mess_head!=mess_tail)
      {
      // advance tail

      if (++mess_tail>=NUM_MESSAGES)
         mess_tail=0;

      // test if this message is a good one

      if (messages[mess_tail].state!=MESS_DEAD)
         {
         // start the message

         Start_Message(mess_tail);

         // set state of message to dead since it is now being played

         messages[mess_tail].state = MESS_DEAD;

         // blaze!!!

         return;

         } // end if we found a live one

      } // end while there is a message waiting

   } // end if message_done

} // end Send_Message

/////////////////////////////////////////////////////////////////////////////

void Start_Splat(int x,int y,int speed,int color)
{
// this function stars a generic Splat

int index;

SET_SPRITE_SIZE(10,10);

// scan for a useable Splat

for (index=0; index<NUM_SPLATS; index++)
    {

    if (splats[index].state == SPLAT_DEAD)
       {

       // set up fields

       splats[index].state        = SPLAT_ALIVE;
       splats[index].x            = x;
       splats[index].y            = y;
       splats[index].curr_frame   = 0+color;
       splats[index].anim_speed   = speed;
       splats[index].anim_clock   = 0;
       splats[index].motion_speed = 0;

       // scan background to be safe

       Behind_Sprite_DB((sprite_ptr)&splats[index]);

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Splat

/////////////////////////////////////////////////////////////////////////////

void Behind_Splats(void)
{

// this function scans under the Splats

int index;

SET_SPRITE_SIZE(10,10);

// scan for a running Splats

for (index=0; index<NUM_SPLATS; index++)
    {

    if (splats[index].state == SPLAT_ALIVE)
       {
       Behind_Sprite_DB((sprite_ptr)&splats[index]);

       } // end if found a good one

    } // end for index

} // end Behind_Splats

/////////////////////////////////////////////////////////////////////////////

void Erase_Splats(void)
{
// this function erases all the current Splats

int index;

SET_SPRITE_SIZE(10,10);

// scan for a useable Splat

for (index=0; index<NUM_SPLATS; index++)
    {

    if (splats[index].state == SPLAT_ALIVE)
       {
       Erase_Sprite_DB((sprite_ptr)&splats[index]);

       } // end if found a good one

    } // end for index

} // end Erase_Splats

/////////////////////////////////////////////////////////////////////////////

void Draw_Splats(void)
{
// this function draws the Splat

int index;

SET_SPRITE_SIZE(10,10);

// scan for a useable Splat

for (index=0; index<NUM_SPLATS; index++)
    {

    // make sure this Splat is alive

    if (splats[index].state == SPLAT_ALIVE)
       {

       Draw_Sprite_DB((sprite_ptr)&splats[index]);

       } // end if found a good one

    } // end for index

} // end Draw_Splats

/////////////////////////////////////////////////////////////////////////////

void Animate_Splats(void)
{
// this function steps the Splat thru the frames of animation
int index;

// scan for a useable Splat

for (index=0; index<NUM_SPLATS; index++)
    {
    // test if Splat is alive
    if (splats[index].state == SPLAT_ALIVE)
       {
       // test if it's time to change frames

       if (++splats[index].anim_clock == splats[index].anim_speed)
          {
          // is the Splat over?

          ++splats[index].curr_frame;

          if (++splats[index].motion_speed == 4)
             splats[index].state = SPLAT_DEAD;

          // reset animation clock for future

          splats[index].anim_clock = 0;

          } // end if time ti change frames

       } // end if found a good one

    } // end for index

} // end Animate_Splats

//////////////////////////////////////////////////////////////////////////////

void Init_Splats(void)
{
// reset all Splats

int index;

for (index=0; index<NUM_SPLATS; index++)
    splats[index].state = SPLAT_DEAD;

} // Init_Splats

//////////////////////////////////////////////////////////////////////////////

void Erase_Cars(void)
{

// this function indexes through all the Cars and if they are active
// erases them by replacing the background color that was under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(12,12);

for (index=0; index<NUM_CARS; index++)
    {

    // is this Car alive

    if (cars[index].state != CAR_DEAD)
       {

       // copy position to sprite structure

       cars[index].object.x = cars[index].x;
       cars[index].object.y = cars[index].y;

       // erase the sprite

       Erase_Sprite_DB((sprite_ptr)&cars[index].object);

       } // end if alive

    } // end for index

} // end Erase_Cars

/////////////////////////////////////////////////////////////////////////////

void Draw_Cars(void)
{

// this function indexes through all the Cars and if they are active
// draws them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(12,12);

for (index=0; index<NUM_CARS; index++)
    {

    // is this Car alive

    if (cars[index].state != CAR_DEAD)
       {

       // copy position to sprite structure

       cars[index].object.x = cars[index].x;
       cars[index].object.y = cars[index].y;

       // erase the sprite

       Draw_Sprite_DB((sprite_ptr)&cars[index].object);

       } // end if alive

    } // end for index

} // end Draw_Cars

/////////////////////////////////////////////////////////////////////////////

void Behind_Cars(void)
{

// this function indexes through all the Cars and if they are active
// scan under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(12,12);

for (index=0; index<NUM_CARS; index++)
    {

    // is this Car alive

    if (cars[index].state != CAR_DEAD)
       {

       // copy position to sprite structure

       cars[index].object.x = cars[index].x;
       cars[index].object.y = cars[index].y;

       // erase the sprite

       Behind_Sprite_DB((sprite_ptr)&cars[index].object);

       } // end if alive

    } // end for index

} // end Behind_Cars

/////////////////////////////////////////////////////////////////////////////

void Init_Cars(void)
{
// this function sets the state of all Cars to dead

int index;

for (index=0; index<NUM_CARS; index++)
    {
    cars[index].state             = CAR_DEAD;
    cars[index].object.curr_frame = 0;
    cars[index].object.x          = 0;
    cars[index].object.y          = 0;
    } // end for

} // end Init_Cars

//////////////////////////////////////////////////////////////////////////////

void Move_Cars(void)
{
// this function moves the Cars

int index,           // loop var
    cx,cy,           // used for quick variable access i.e. aliasing
    cell_x,cell_y,   // position of Car in cell world
    cell_id,         // id of cell bitmap
    tx,ty,           // used to hold position of test object
    new_state = 0,   // the next state
    state_change = 0;// has a state change occured


// traverse list and move all of the live ones

for (index=0; index<NUM_CARS; index++)
    {

    // test if Car is alive

    if (cars[index].state!=CAR_DEAD)
       {

       // select animation and motion based on direction if it's time to
       // process object

       if (++cars[index].counter_1==cars[index].threshold_1)
          {

          // reset state change variable

          state_change=0;

          // first reset animation counter

          cars[index].counter_1=0;

          // which state is the car in ?

          switch(cars[index].state)
                {

                case CAR_STARTING:
                     {

                     // move the car

                     cars[index].x+=cars[index].curr_xv;
                     cars[index].y+=cars[index].curr_yv;

                     // which way are we accelerating the car

                     switch(cars[index].direction)
                           {

                           case CAR_NORTH:
                                {

                                // press the pedal until desired speed

                                if (--cars[index].curr_yv <= cars[index].max_yv)
                                   cars[index].curr_yv = cars[index].max_yv;


                                   // test if we are done

                                if (++cars[index].counter_3 >= cars[index].threshold_3)
                                   {
                                   // now we are driving

                                   state_change = 1;
                                   new_state    = CAR_DRIVING;

                                   } // end if reached max speed

                                } break;

                           case CAR_SOUTH:
                                {

                                // press the pedal until desired speed

                                if (++cars[index].curr_yv >= cars[index].max_yv)
                                    cars[index].curr_yv = cars[index].max_yv;


                                if (++cars[index].counter_3 >= cars[index].threshold_3)
                                   {
                                   // now we are driving

                                   state_change = 1;
                                   new_state    = CAR_DRIVING;

                                   } // end if reached max speed

                                } break;

                           case CAR_WEST:
                                {
                                // not implemented currently


                                } break;

                           case CAR_EAST:
                                {
                                // not implemented currently


                                } break;

                           default:break;

                           } // end switch direction

                     } break;

                case CAR_DRIVING:
                     {

                     // test if we should play sound

                     if (rand()%500==1)
                        Play_Sound(SOUND_CAR_SOUND);

                     if (rand()%500==2)
                        Play_Sound(SOUND_CAR_HORN);

                     // move the car

                     cars[index].x+=cars[index].curr_xv;
                     cars[index].y+=cars[index].curr_yv;

                     // test for a stop sign

                     // which way is the car going

                     switch(cars[index].direction)
                           {

                           case CAR_NORTH:
                                {

                                // compute bitmap in front of car

                                cell_x = (cars[index].x+6) >> 4;
                                cell_y = (cars[index].y+10) >> 4;

                                // get bitmap id

                                cell_id = city_1[cell_y][cell_x];

                                // is this a stop sign

                                if (cell_id == STOP_NORTH_ID)
                                   {
                                   state_change = 1;
                                   new_state    = CAR_SLOWING;

                                   } // end if hit a stop sigh

                                } break;

                           case CAR_SOUTH:
                                {

                                // compute bitmap in front of car

                                cell_x = (cars[index].x+6) >> 4;
                                cell_y = (cars[index].y+0) >> 4;

                                // get bitmap id

                                cell_id = city_1[cell_y][cell_x];

                                // is this a stop sign

                                if (cell_id == STOP_SOUTH_ID)
                                   {
                                   state_change = 1;
                                   new_state    = CAR_SLOWING;

                                   } // end if hit a stop sigh

                                } break;

                           case CAR_WEST:
                                {
                                // not implemented currently
                                } break;

                           case CAR_EAST:
                                {
                                // not implemented currently
                                } break;

                           default:break;

                           } // end switch direction


                     } break;


                case CAR_SLOWING:
                     {
                     // continue hitting breaks


                     // which way is the car going

                     switch(cars[index].direction)
                           {

                           case CAR_NORTH:
                                {
                                // enage breaks

                                if (++cars[index].curr_yv>=0)
                                   {

                                   state_change=1;
                                   new_state   = CAR_STOPPED;

                                   } // end if stopped

                                } break;

                           case CAR_SOUTH:
                                {

                                // enage breaks

                                if (--cars[index].curr_yv<=0)
                                   {

                                   state_change=1;
                                   new_state   = CAR_STOPPED;

                                   } // end if stopped

                                } break;

                           case CAR_WEST:
                                {
                                // not implemented currently


                                } break;

                           case CAR_EAST:
                                {
                                // not implemented currently


                                } break;

                           default:break;

                           } // end switch direction

                     } break;

                case CAR_STOPPED:
                     {

                     // have we looked both ways?

                     if (++cars[index].counter_2 > cars[index].threshold_2)
                        {

                        // start car back up

                        state_change = 1;
                        new_state    = CAR_STARTING;

                        } // end if sat for long enough

                     } break;

                default:break;

                } // end switch

         // test for a state change

         if (state_change)
            {

            // what is the new state

            switch(new_state)
                  {

                  case CAR_STARTING:
                       {

                       cars[index].state       = CAR_STARTING;
                       cars[index].counter_3   = 0;
                       cars[index].threshold_3 = 8;

                       // play the sound

                       Play_Sound(SOUND_CAR_START);

                       } break;

                  case CAR_DRIVING:
                       {

                       cars[index].state = CAR_DRIVING;

                       } break;

                  case CAR_SLOWING:
                       {

                       cars[index].state = CAR_SLOWING;

                       // play the sound

                       Play_Sound(SOUND_CAR_STOP);

                       } break;

                  case CAR_STOPPED:
                       {

                       cars[index].counter_2   = 0;
                       cars[index].threshold_2 = 20 + rand()%50;
                       cars[index].state       = CAR_STOPPED;

                       } break;

                  default: break;

                  } // end switch new state

            } // end if

          } // end if time to process

       // do collision detection with borders

       if (cars[index].x > 320-12 || cars[index].x < 0 ||
           cars[index].y > 163    || cars[index].y < 0 )
          {
          // kill ther car

          cars[index].state = CAR_DEAD;

          } // end if car off screen
      else
         {
         // see if car has hit pizza boy
         // only if car is moving fast

         if (cars[index].state==CAR_DRIVING && boy.state==BOY_ALIVE)
            {

            // convert pizza boys position to integer space

            tx = ((int)(boy.x >> FP_SHIFT))+6;
            ty = ((int)(boy.y >> FP_SHIFT))+8;

            // perform standard collision test

            if (tx>cars[index].x+2 && tx<cars[index].x+9 &&
                ty>cars[index].y+2 && ty<cars[index].y+9)
               {

               // one less moped

               --boy_mopeds;

               // start death sequence

               Start_Boy_Death();

               Play_Sound(SOUND_MOPED_HIT);

               } // end if car hit moped

            } // end if car could posscible hit pizza boy

         } // end else test cars against moped

       } // end if alive

    } // end for index

} // end Move_Cars

//////////////////////////////////////////////////////////////////////////////

int Start_Car(void)
{

// this function is used to start a Car up

int index;

// look up tables for starting positions of Cars

static int x_south_start[] = {64,160,257}; // south bound
static int x_north_start[] = {84,180,277}; // north bound
static int y_west_start[]  = {66};         // west bound
static int y_east_start[]  = {84};         // east bound

// find a Car that isn't being used

for (index=0; index<NUM_CARS; index++)
    {

    // try and find a Car to start

    if (cars[index].state == CAR_DEAD)
       {

       // select direction Car will be traveling in

       switch(1+rand()%4)
             {

             case CAR_NORTH:
                  {

                  // set up fields

                  cars[index].x          = x_north_start[rand()%3];
                  cars[index].y          = 163;
                  cars[index].curr_xv    = 0;
                  cars[index].curr_yv    = -4;
                  cars[index].max_xv     = 0;
                  cars[index].max_yv     = -2-(rand()%2);
                  cars[index].direction  = CAR_NORTH;
                  cars[index].object.curr_frame = 0+4*(rand()%5);

                  } break;

             case CAR_SOUTH:
                  {

                  // set up fields

                  cars[index].x          = x_south_start[rand()%3];
                  cars[index].y          = 0;
                  cars[index].curr_xv    = 0;
                  cars[index].curr_yv    = 4;
                  cars[index].max_xv     = 0;
                  cars[index].max_yv     = 4+(rand()%2);
                  cars[index].direction  = CAR_SOUTH;
                  cars[index].object.curr_frame = 1+(4*(rand()%5));

                  } break;

             case CAR_WEST:
                  {

                  // set up fields

                  cars[index].x          = 320-12;
                  cars[index].y          = y_west_start[0];
                  cars[index].curr_xv    = -4;
                  cars[index].curr_yv    = 0;
                  cars[index].max_xv     = -4-(rand()%2);
                  cars[index].max_yv     = 0;
                  cars[index].direction  = CAR_WEST;
                  cars[index].object.curr_frame = 2+(4*(rand()%5));

                  } break;

             case CAR_EAST:
                  {

                  // set up fields

                  cars[index].x          = 0;
                  cars[index].y          = y_east_start[0];
                  cars[index].curr_xv    = 4;
                  cars[index].curr_yv    = 0;
                  cars[index].max_xv     = 4+(rand()%2);
                  cars[index].max_yv     = 0;
                  cars[index].direction  = CAR_EAST;
                  cars[index].object.curr_frame = 3+(4*(rand()%5));

                  } break;

             } // end switch

       // set common fields (mostly counters)

       cars[index].state       = CAR_DRIVING;

       cars[index].counter_1   = 0;
       cars[index].threshold_1 = 1;

       cars[index].counter_2   = 0;
       cars[index].threshold_2 = 0;

       cars[index].counter_3   = 0;
       cars[index].threshold_3 = 0;

       // color field isn't really being used at this time

       cars[index].color       = 0;

       // set the sprite size properly

       SET_SPRITE_SIZE(12,12);

       // scan under the sprite

       Behind_Sprite_DB((sprite_ptr)&cars[index].object);

       // break out of loop

       return(1);

       } // end if dead

    } // end for index

return(0);

} // end Start_Car

/////////////////////////////////////////////////////////////////////////////

void Erase_Humans(void)
{

// this function indexes through all the humans and if they are active
// erases them by replacing the background color that was under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(6,10);

for (index=0; index<NUM_HUMANS; index++)
    {

    // is this human alive

    if (humans[index].state != HUMAN_DEAD)
       {

       // erase the sprite

       Erase_Sprite_DB((sprite_ptr)&humans[index]);

       } // end if alive

    } // end for index

} // end Erase_Humans

/////////////////////////////////////////////////////////////////////////////

void Draw_Humans(void)
{

// this function indexes through all the humans and if they are active
// draws them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(6,10);

for (index=0; index<NUM_HUMANS; index++)
    {

    // is this human alive

    if (humans[index].state != HUMAN_DEAD)
       {

       // erase the sprite

       Draw_Sprite_DB((sprite_ptr)&humans[index]);

       } // end if alive

    } // end for index

} // end Draw_Humans

/////////////////////////////////////////////////////////////////////////////

void Behind_Humans(void)
{

// this function indexes through all the humans and if they are active
// scan under them

int index;

// set sprite size for engine

SET_SPRITE_SIZE(6,10);

for (index=0; index<NUM_HUMANS; index++)
    {

    // is this human alive

    if (humans[index].state != HUMAN_DEAD)
       {

       // erase the sprite

       Behind_Sprite_DB((sprite_ptr)&humans[index]);

       } // end if alive

    } // end for index

} // end Behind_Humans

/////////////////////////////////////////////////////////////////////////////

void Init_Humans(void)
{
// this function sets the state of all humans to dead

int index;

for (index=0; index<NUM_HUMANS; index++)
    humans[index].state = HUMAN_DEAD;

} // end Init_Humans

//////////////////////////////////////////////////////////////////////////////

void Move_Humans(void)
{
// this function moves the humans

int index,           // loop var
    index_2,
    hx,hy,           // used for quick variable access i.e. aliasing
    cell_x,cell_y,   // position of human in cell world
    cell_id,         // id of cell bitmap
    tx,ty;           // used for temporary position vars

// traverse list and move all of the live ones

for (index=0; index<NUM_HUMANS; index++)
    {

    // test if human is alive

    if (humans[index].state!=HUMAN_DEAD)
       {

       // select animation and motion based on direction if it's time to
       // process object

       if (++humans[index].anim_clock==humans[index].anim_speed)
          {
          // first reset animation counter

          humans[index].anim_clock=0;

          // now move and animate human

          switch(humans[index].state)
                {

                case HUMAN_NORTH:
                     {

                     // do translation

                     humans[index].y-=2;

                     // do animation

                     if (++humans[index].curr_frame>3)
                        humans[index].curr_frame = 0;

                     } break;

                case HUMAN_SOUTH:
                     {

                     // do translation

                     humans[index].y+=2;

                     // do animation

                     if (++humans[index].curr_frame>7)
                        humans[index].curr_frame = 4;

                     } break;

                case HUMAN_WEST:
                     {

                     // do translation

                     humans[index].x-=2;

                     // do animation

                     if (++humans[index].curr_frame>11)
                        humans[index].curr_frame = 8;

                     } break;

                case HUMAN_EAST:
                     {

                     // do translation

                     humans[index].x+=2;

                     // do animation

                     if (++humans[index].curr_frame>15)
                        humans[index].curr_frame = 12;

                     } break;

                default:break;

                } // end switch direction

          // do collision detection

          // boundary detection

          hx = humans[index].x;
          hy = humans[index].y;

          // test if human has walked into a house

          switch(humans[index].state)
                {

                case HUMAN_NORTH:
                     {
                     // compute test point on sprite

                     cell_x = (hx + 3) >> 4;
                     cell_y = (hy + 10) >> 4;

                     } break;

                case HUMAN_SOUTH:
                     {
                     // compute test point on sprite

                     cell_x = (hx + 3) >> 4;
                     cell_y = (hy) >> 4;


                     } break;

                case HUMAN_WEST:
                     {
                     // compute test point on sprite

                     cell_x = (hx + 6) >> 4;
                     cell_y = (hy + 5) >> 4;

                     } break;

                case HUMAN_EAST:
                     {
                     // compute test point on sprite

                     cell_x = (hx) >> 4;
                     cell_y = (hy + 5) >> 4;

                     } break;

                default:break;

                } // end switch

           // extract cell from world

           cell_id = city_1[cell_y][cell_x];

           // test for house

           if (cell_id==HOUSE_WEST_2_ID || cell_id==HOUSE_EAST_1_ID)
              humans[index].state = HUMAN_DEAD;

          // test against screen borders
          else
          if (hx>320-8 || hx<0 || hy>200-24-8 || hy<0)
             {
             // kill the human

             humans[index].state = HUMAN_DEAD;

             } // end if hit edge

          else
             {

             // test human against cars

             for (index_2=0; index_2<NUM_CARS; index_2++)
                 {

                 // test if the car is alive

                 if (cars[index_2].state!=CAR_DEAD && cars[index_2].state!=CAR_STOPPED)
                    {

                    // test if humans center is within bounding box of car

                    if ( (hx+3 > cars[index_2].x) && (hx+3 < cars[index_2].x+11) &&
                         (hy+5 > cars[index_2].y) && (hy+5 < cars[index_2].y+11))
                       {
                       // kill the human

                       humans[index].state = HUMAN_DEAD;

                       // start an splat

                       Start_Splat(hx,hy,2+rand()%2,humans[index].motion_clock);

                       // play the sound

                       Play_Sound(SOUND_HUMAN_HIT);

                       break;

                       } // end if human is hit

                    } // end if car active

                 } // end for index_2

                 // test human against moped


                 if (humans[index].state != HUMAN_DEAD && boy.throttle>128 &&
                    boy.state==BOY_ALIVE)
                    {

                    // extract position of boy (convert to int)

                    tx = (int)(boy.x >> FP_SHIFT);
                    ty = (int)(boy.y >> FP_SHIFT);

                    if ( (hx+3 > tx+2) && (hx+3 < tx+9) &&
                         (hy+5 > ty+2) && (hy+5 < ty+9))
                       {
                       // kill the human

                       humans[index].state = HUMAN_DEAD;

                       // one more dead ped

                       boy_xpeds++;

                       // start a splat

                       Start_Splat(hx,hy,2+rand()%2,humans[index].motion_clock);

                       Play_Sound(SOUND_HUMAN_HIT);

                       } // end if human is hit

                    } // end if test human against moped

              } // end else test for human hit car

          } // end if time to process

       } // end if alive

    } // end for index

} // end Move_Humans

//////////////////////////////////////////////////////////////////////////////

int Start_Human(void)
{

// this function is used to start a human up

int index;

// look up tables for starting positions of humans

static int x_start[] = {2,14,26,59,97,155,192,250,287};
static int y_start[] = {22,52,90,136,164};

// find a human that isn't being used

for (index=0; index<NUM_HUMANS; index++)
    {

    // try and find a human to start

    if (humans[index].state == HUMAN_DEAD)
       {


       // select direction human will be traveling in

       switch(1+rand()%4)
             {

             case HUMAN_NORTH:
                  {

                  // set up fields

                  humans[index].x          = x_start[rand()%9];
                  humans[index].y          = 170;
                  humans[index].state      = HUMAN_NORTH;
                  humans[index].curr_frame = 0;

                  } break;

             case HUMAN_SOUTH:
                  {

                  // set up fields

                  humans[index].x          = x_start[rand()%9];
                  humans[index].y          = 0;
                  humans[index].state      = HUMAN_SOUTH;
                  humans[index].curr_frame = 4;

                  } break;

             case HUMAN_WEST:
                  {

                  // set up fields

                  humans[index].x          = 320-8;
                  humans[index].y          = y_start[rand()%5];
                  humans[index].state      = HUMAN_WEST;
                  humans[index].curr_frame = 8;

                  } break;

             case HUMAN_EAST:
                  {

                  // set up fields

                  humans[index].x          = 0;
                  humans[index].y          = y_start[rand()%5];
                  humans[index].state      = HUMAN_EAST;
                  humans[index].curr_frame = 12;

                  } break;

             } // end switch

       // set common fields

       humans[index].anim_clock=0;
       humans[index].anim_speed=1+rand()%3;

       SET_SPRITE_SIZE(6,10);

       // scan under the sprite

       Behind_Sprite_DB((sprite_ptr)&humans[index]);

       // break out of loop

       return(1);

       } // end if dead

    } // end for index

return(0);

} // end Start_Human

/////////////////////////////////////////////////////////////////////////////

void Draw_Screen(char **screen)
{
// this function draws a screen by using the data in the universe array
// each element in the universe array is a 2-D matrix of cells, these
// cells are ASCII characters that represent the requested bitmap that
// should be placed in the cell location

char *curr_row;

int index,
    index_x,        // index vars
    index_y,
    cell_id;        // the bitmap id

// translation table for screen database used to convert the ASCII
// characters into id numbers

static char ascii_to_id[128] =

//    !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /

  {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,32,0 ,

// 0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
   33,34,35,36,37,38,39,40,45,46,0 ,0 ,0 ,0 ,0 ,0 ,

// @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O
   0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,


// P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
   0 ,0 ,0 ,9 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,4 ,0 ,5 ,6 ,0 ,


// `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
   0 ,47,48,41,42,43,44,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,

// p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  DEL
  0  ,0 ,0 ,8 ,0 ,0 ,7 ,0 ,0 ,0 ,0 ,49,0 ,50,0  ,0 ,};

// clear out the double buffer

// Fill_Double_Buffer(0);

SET_SPRITE_SIZE(16,16);

// now draw the screen row by row

for (index_y = 0; index_y<CELL_ROWS; index_y++)
    {

    // get the current row for speed

    curr_row = screen[index_y];

    // do the row

    for (index_x = 0; index_x<CELL_COLUMNS; index_x++)
        {
        // extract cell out of data structure and blit it onto screen

        cell_id = ascii_to_id[curr_row[index_x]-32];

        // write id number into data structure

        curr_row[index_x] = cell_id;

        // test which sprite should be used, either roads or building?

        if (cell_id <32) // use roads
           {

           // compute proper sprite position

           roads.x = index_x * sprite_width;
           roads.y = index_y * sprite_height;

           // set current frame to bitmap id

           roads.curr_frame = cell_id;

           // draw the bitmap

           Draw_Sprite_DB((sprite_ptr)&roads);

           } // end if road sprite

        else
           {
           // use building sprite

           // compute proper sprite position

           builds.x = index_x * sprite_width;
           builds.y = index_y * sprite_height;

           // set current frame to bitmap id

           cell_id-=32;

           builds.curr_frame = cell_id;

           // draw the bitmap

           Draw_Sprite_DB((sprite_ptr)&builds);

           // test if this is a house, if so insert house position into
           // database and draw house number

           if (cell_id==1 || cell_id==6)
              {
              // 1 is westward front door and 6 is eastward fron door

              // insert house position into database

              house_pos[curr_house].x    = index_x*sprite_width;
              house_pos[curr_house].y    = index_y*sprite_height;
              house_pos[curr_house].type = cell_id;
              house_pos[curr_house].num  = curr_house;

              house_pos[curr_house].state=HOUSE_NO_ORDER;
              house_pos[curr_house].timer=0;

              // we are on the next house

              curr_house++;

              } // end if front of house

           } // end else

        } // end for index_x

    } // end for index_y

// now that the entire city has been drawn, let's draw the house numbers
// on top of the houses

for (index=0; index<curr_house; index++)
    {
    // what kind of house is it?

    if (house_pos[index].type == 1)
       {

       // extract position data and blit proper numerical bitmap

       numbers.x          = (house_pos[index].x)+16;
       numbers.y          = (house_pos[index].y);
       numbers.curr_frame = house_pos[index].num;

       Draw_Sprite_DB((sprite_ptr)&numbers);

       } // end if westward pointing
    else
       {
       // must be an eastward pointing house

       // extract position data and blit proper numerical bitmap

       numbers.x          = (house_pos[index].x)-8;
       numbers.y          = (house_pos[index].y);
       numbers.curr_frame = house_pos[index].num;

       Draw_Sprite_DB((sprite_ptr)&numbers);

       } // end else eastward

    } // end for index

// remap the color registers for the house numbers so that they are white

Get_Palette_Register(ROOF_COLOR_REG,(RGB_color_ptr)&roof_color);

for (index=START_NUM_COLOR; index<=END_NUM_COLOR; index++)
    {
    Set_Palette_Register(index,(RGB_color_ptr)&roof_color);

    } // end for index

// make curr_house reflect array bounds

curr_house--;

} // end Draw_Screen

/////////////////////////////////////////////////////////////////////////////

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

if ( (fp=fopen("simpizza.cfg","r"))==NULL )
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

sound_fx[SOUND_CAR_STOP   ]  = Voc_Load_Sound("SMCSTOP.VOC ",&sound_lengths[SOUND_CAR_STOP   ]);
sound_fx[SOUND_CAR_START  ]  = Voc_Load_Sound("SMCSTART.VOC",&sound_lengths[SOUND_CAR_START  ]);
sound_fx[SOUND_MOPED_HORN ]  = Voc_Load_Sound("SMMHORN.VOC ",&sound_lengths[SOUND_MOPED_HORN ]);
sound_fx[SOUND_CAR_HORN   ]  = Voc_Load_Sound("SMCHORN.VOC ",&sound_lengths[SOUND_CAR_HORN   ]);
sound_fx[SOUND_MOPED_HIT  ]  = Voc_Load_Sound("SMMHIT.VOC  ",&sound_lengths[SOUND_MOPED_HIT  ]);
sound_fx[SOUND_HUMAN_HIT  ]  = Voc_Load_Sound("SMHHIT.VOC  ",&sound_lengths[SOUND_HUMAN_HIT  ]);
sound_fx[SOUND_YO_VINNIE  ]  = Voc_Load_Sound("SMYO.VOC    ",&sound_lengths[SOUND_YO_VINNIE  ]);
sound_fx[SOUND_LOST_ORDER ]  = Voc_Load_Sound("SMLOST.VOC  ",&sound_lengths[SOUND_LOST_ORDER ]);
sound_fx[SOUND_THANK_YOU_B]  = Voc_Load_Sound("SMTHANK1.VOC",&sound_lengths[SOUND_THANK_YOU_B]);
sound_fx[SOUND_THANK_YOU_G]  = Voc_Load_Sound("SMTHANK2.VOC",&sound_lengths[SOUND_THANK_YOU_G]);
sound_fx[SOUND_TOO_LONG   ]  = Voc_Load_Sound("SMTOO.VOC   ",&sound_lengths[SOUND_TOO_LONG   ]);
sound_fx[SOUND_COME_HOME  ]  = Voc_Load_Sound("SMCOME.VOC  ",&sound_lengths[SOUND_COME_HOME  ]);
sound_fx[SOUND_CAR_SOUND  ]  = Voc_Load_Sound("SMCAR.VOC   ",&sound_lengths[SOUND_CAR_SOUND  ]);
sound_fx[SOUND_START      ]  = Voc_Load_Sound("SMSTART.VOC ",&sound_lengths[SOUND_START      ]);

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

   Voc_Unload_Sound(sound_fx[SOUND_CAR_STOP   ]);
   Voc_Unload_Sound(sound_fx[SOUND_CAR_START  ]);
   Voc_Unload_Sound(sound_fx[SOUND_MOPED_HORN ]);
   Voc_Unload_Sound(sound_fx[SOUND_CAR_HORN   ]);
   Voc_Unload_Sound(sound_fx[SOUND_MOPED_HIT  ]);
   Voc_Unload_Sound(sound_fx[SOUND_HUMAN_HIT  ]);
   Voc_Unload_Sound(sound_fx[SOUND_YO_VINNIE  ]);
   Voc_Unload_Sound(sound_fx[SOUND_LOST_ORDER ]);
   Voc_Unload_Sound(sound_fx[SOUND_THANK_YOU_B]);
   Voc_Unload_Sound(sound_fx[SOUND_THANK_YOU_G]);
   Voc_Unload_Sound(sound_fx[SOUND_TOO_LONG   ]);
   Voc_Unload_Sound(sound_fx[SOUND_COME_HOME  ]);
   Voc_Unload_Sound(sound_fx[SOUND_CAR_SOUND  ]);
   Voc_Unload_Sound(sound_fx[SOUND_START      ]);

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

//////////////////////////////////////////////////////////////////////////////

void Do_Intro(void)
{
// this function displays the introduction screen and then melts it

// load intro screen and display for a few secs.

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("simint.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

// let user see it

Delay(50);

Fade_Lights();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

} // end Do_Intro

///////////////////////////////////////////////////////////////////////////////

void Load_Environment(void)
{
// this function loads the imagery for the environment

int index;  // loop variables

// load in imagery

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("simimg.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize the road and building sprites

SET_SPRITE_SIZE(16,16);

Sprite_Init((sprite_ptr)&roads,0,0,0,0,0,0);
Sprite_Init((sprite_ptr)&builds,0,0,0,0,0,0);
Sprite_Init((sprite_ptr)&numbers,0,0,0,0,0,0);

// load in frames for roads

for (index=0; index<10; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&roads,index,index,0);

    } // end for

// now load the buildings

for (index=0; index<17; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&builds,index,index,1);
    } // end for

// load last two sidewalk bitmaps

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                (sprite_ptr)&builds,index,0,2);

index++;

PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                (sprite_ptr)&builds,index,1,2);

// now load the house numbers

for (index=0; index<16; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&numbers,index,index,4);
    } // end for

for (index=0; index<8; index++)
    {

    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&numbers,index+16,index,5);
    } // end for

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Environment

///////////////////////////////////////////////////////////////////////////////

void Load_Humans(void)
{
// this function loads the imagery for the humans

int index,    // loop indices
    index_2,
    color;    // used to select color of human which is reall the row in the
              // pcx file
// load in the human imagery

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("simimg3.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// set the proper sprite size

SET_SPRITE_SIZE(6,10);

// load in the humans

for (index=0; index<NUM_HUMANS; index++)
    {

    Sprite_Init((sprite_ptr)&humans[index],0,0,0,0,0,0);

    color = rand()%2;

    for (index_2=0; index_2<NUM_HUMAN_FRAMES; index_2++)
         {
         PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                        (sprite_ptr)&humans[index],index_2,index_2,color);

         // set color field (use the motion_clock field in the structure)

         humans[index].motion_clock = color*4;

         } // end for index_2

    // set up state information

    humans[index].state      = HUMAN_DEAD;
    humans[index].x          = 0;
    humans[index].y          = 0;
    humans[index].curr_frame = 0;

    } // end for

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Humans

//////////////////////////////////////////////////////////////////////////////

void Load_Cars(void)
{
// this function loads the imagery for the cars

int index,    // loop indices
    index_2;

// load in the human imagery

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("simimg2.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// set the proper sprite size

SET_SPRITE_SIZE(12,12);

// load in the cars

for (index=0; index<NUM_CARS; index++)
    {

    Sprite_Init((sprite_ptr)&cars[index].object,0,0,0,0,0,0);

    for (index_2=0; index_2<NUM_CAR_FRAMES; index_2++)
         PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                        (sprite_ptr)&cars[index].object,index_2,index_2,0);

    // set up state information

    cars[index].state             = CAR_DEAD;
    cars[index].x                 = 0;
    cars[index].y                 = 0;
    cars[index].object.curr_frame = 0;

    } // end for


// load in the frames for the pizza boy's moped

Sprite_Init((sprite_ptr)&boy.object,0,0,0,0,0,0);

for (index=0; index<NUM_BOY_FRAMES; index++)
     PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&boy.object,index,15-index,2);

// set up state information

boy.state             = BOY_ALIVE;
boy.x                 = Assign_Integer(48);
boy.y                 = Assign_Integer(144);
boy.curr_xv           = 0;
boy.curr_yv           = 0;
boy.max_xv            = 0;
boy.max_yv            = 0;
boy.xv                = 0;
boy.yv                = 0;


boy.throttle          = 0;
boy.hp                = Assign_Float((float).4);
boy.friction          = Assign_Float((float)-.10);
boy.brakes            = Assign_Integer(1);

boy.max_throttle      = Assign_Integer(3);

boy.counter_1         = 0;
boy.threshold_1       = 2;

boy.object.curr_frame = 0;
boy.direction         = 0;

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Cars

////////////////////////////////////////////////////////////////////////////////

void Load_Splats(void)
{
// this function loads the splats

int index,  // loop var
    index_2;

// load in imagery for explosions

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("simimg4.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize the splats and extract bitmaps

SET_SPRITE_SIZE(10,10);

// load in frames for splats

for (index=0; index<NUM_SPLATS; index++)
    {

    Sprite_Init((sprite_ptr)&splats[index],0,0,0,0,0,0);

    for (index_2=0; index_2<8; index_2++)
        PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                        (sprite_ptr)&splats[index],index_2,index_2,0);

    } // end for

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Splats

////////////////////////////////////////////////////////////////////////////////

void Load_Death(void)
{
// this function loads the death frames

int index;  // loop var

// load in imagery for death frames

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("simimg6.pcx",(pcx_picture_ptr)&imagery_pcx,1);

// initialize the death sprite and extract bitmaps

SET_SPRITE_SIZE(32,30);

// load in frames for death sequence

Sprite_Init((sprite_ptr)&boy_death,0,0,0,0,0,0);

for (index=0; index<16; index++)
    PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                    (sprite_ptr)&boy_death,index,index%8,index/8);

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Death

////////////////////////////////////////////////////////////////////////////////

void Load_Pizzas(void)
{
// this function loads the pizzas

int index,  // loop var
    index_2;

// load in imagery for explosions

PCX_Init((pcx_picture_ptr)&imagery_pcx);

PCX_Load("simimg5.pcx", (pcx_picture_ptr)&imagery_pcx,1);

// initialize the splats and extract bitmaps

SET_SPRITE_SIZE(6,6);

// load in frames for pizzas

for (index=0; index<NUM_PIZZAS; index++)
    {

    Sprite_Init((sprite_ptr)&pizzas[index].object,0,0,0,0,0,0);

    for (index_2=0; index_2<8; index_2++)
        PCX_Grab_Bitmap((pcx_picture_ptr)&imagery_pcx,
                        (sprite_ptr)&pizzas[index].object,index_2,index_2,0);

    } // end for

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&imagery_pcx);

} // end Load_Pizzas

//////////////////////////////////////////////////////////////////////////////

void Load_Background(void)
{

// load in the background image into the double buffer

PCX_Init((pcx_picture_ptr)&background_pcx);

PCX_Load("simbak.pcx", (pcx_picture_ptr)&background_pcx,1);

// copy the background into the double buffer

_fmemcpy((char far *)double_buffer,
         (char far *)(background_pcx.buffer),
         SCREEN_WIDTH*SCREEN_HEIGHT);

PCX_Delete((pcx_picture_ptr)&background_pcx);

} // Load_Background

///////////////////////////////////////////////////////////////////////////////

void Show_Instructions(void)
{
// this function displays the instructions and then disolves them

// load instruction screen and display it until a key press

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("simins.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

// let user see it

while(!kbhit()){};

getch();

// let's try this screen transition

Sheer();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

} // end Show_Instructions

///////////////////////////////////////////////////////////////////////////////

void Draw_Gauges(void)
{

static int time_counter=0,         // track virtual time
           seconds_counter=0,      // tracks virtual seconds indicator
           difficulty_counter=0;   // tracks virtual hours to modify difficulty

// this function draws the players statistics in the display module

char buffer[128],min_string[8];

int hours, minutes;

// show the number of mopeds

sprintf(buffer,"%d",boy_mopeds);

Blit_String_DB(76,179,15,buffer,0);


// show the number of pizzas in rack

sprintf(buffer,"%d ",boy_pizzas);

Blit_String_DB(76,189,15,buffer,0);

// show the number of dead peds

sprintf(buffer,"%d",boy_xpeds);

Blit_String_DB(267+10,179,15,buffer,0);

// show the tips

sprintf(buffer,"$%d",boy_tips);

Blit_String_DB(251+10,189,15,buffer,0);


// show the time of day

// first convert minutes to hours:mins

hours   = boy_time / 60;

minutes = boy_time % 60;

// make sure not to display military time

if (hours>12)
    hours-=12;

// test if it's time to clean the display up

if (boy_time==780) // 12:00 pm
   Blit_String_DB(184,179,12,"     ",0);

// advance time

if (++time_counter==25)
   {
   // reset counter

   time_counter=0;

   // increment time

   boy_time++;

   // adjust difficulty as day progreses

   if (++difficulty_counter==60)
      {
      // make game a little harder

      difficulty-=3;

      // reset counter

      difficulty_counter = 0;

      } // end if it's the end of an hour

   } // end if one virtual minute

// format minutes ...this sucks!

if (minutes<10)
   sprintf(min_string,"0%d",minutes);
else
   sprintf(min_string,"%d",minutes);

// make little digital separator blink

if (++seconds_counter <= 25)
   sprintf(buffer,"%d %s",hours,min_string);
else
if (seconds_counter > 25 && seconds_counter<50)
   sprintf(buffer,"%d:%s",hours,min_string);
else
   {
   seconds_counter = 0;
   sprintf(buffer,"%d:%s",hours,min_string);
   } // end else reset

// draw the time

Blit_String_DB(184,179,12,buffer,0);


} // end Draw_Gauges

///////////////////////////////////////////////////////////////////////////////

void Animate_Speedo(void)
{
// this function does the color palette animation for the speedometer

static int entered=0;                    // used to track first call to function
static RGB_color color_dark,color_light; // used as working colors

int index;                               // looping index

fixed speed;                             // holds percentage of maximum power

// test if this is first time in function

if (!entered)
   {
   // set flag

   entered = 1;

   // create a dark blue

   color_dark.red   = 0;
   color_dark.green = 0;
   color_dark.blue  = 30;


   // create a dark blue

   color_light.red   = 0;
   color_light.green = 0;
   color_light.blue  = 63;

   // set all speedo colors to dark blue

   for (index=START_SPEEDO_COLOR; index<=END_SPEEDO_COLOR; index++)
       {

       Set_Palette_Register(index,(RGB_color_ptr)&color_dark);

       } // end for

   } // end if first time
else
   {
   // do normal processing

   // based on throttle position illuminate speedo

   // convert fixed point ratio of max throttle/current throttle to
   // speed

   speed =  (((boy.throttle << FP_SHIFT)/boy.max_throttle)*(fixed)6) >> FP_SHIFT;

   // illumninate lights on dash

   for (index=START_SPEEDO_COLOR; index<START_SPEEDO_COLOR+speed; index++)
       {

       Set_Palette_Register(index,(RGB_color_ptr)&color_light);

       } // end for

    // turn the rest off

    for (; index<=END_SPEEDO_COLOR; index++)
        {

        Set_Palette_Register(index,(RGB_color_ptr)&color_dark);

        } // end for

   } // end else normal processing

} // end Animate_Speedo

///////////////////////////////////////////////////////////////////////////////

void Order_Pizza(void)
{
// this function orders a pizza

int index,
    house_number;

char buffer[64];

// select a home that hasn't ordered yet

while(1)
     {
     // select a random house

     house_number = rand()%(curr_house+1);

     // test if this one hasn't ordered

     if (house_pos[house_number].state==HOUSE_NO_ORDER)
        {
        // this is the order and set up house as ordered

        house_pos[house_number].state  = HOUSE_ORDERED;
        house_pos[house_number].timer  = 500+5*rand()%25; // about 30 virtual mins.

        // build up message

        sprintf(buffer,"Yo Vinnie...take a pizza to house %d",house_number+1);

        // send it

        Insert_Message(buffer,0);

        // play sound

        Play_Sound(SOUND_YO_VINNIE);

        // increase number of pizzas ordered

        total_orders++;

        // tell the house to blink

        Blink_House(house_number);

        break;

        } // end if

     } // end while

} // end Order_Pizza

///////////////////////////////////////////////////////////////////////////////

void Age_Orders(void)
{
// this function traverses the pending orders and decrements their timers
// if a timer expires then the pizza order is nullified and a message is sent

int index;        // loop index
char buffer[64];  // used to build up message string

// traverse pizza order list for each house

for (index=0; index<=curr_house; index++)
    {
    // test if there is an order for this house

    if (house_pos[index].state==HOUSE_ORDERED)
       {
       // decrement timer

       if (--house_pos[index].timer <= 0)
          {
          // took too long, forget it!

          house_pos[index].state = HOUSE_NO_ORDER;

          // send a nasty message

          // build up message

          sprintf(buffer,"Vinnie!..we just lost number %d",index+1);

          // play sound

          Play_Sound(SOUND_LOST_ORDER);

          // send it with urgency

          Insert_Message(buffer,1);

          } // end if time has ran out

       } // end if house is waiting for a pizza

    } // end for index

} // end Age_Orders

////////////////////////////////////////////////////////////////////////////////

void Blink_House(int house_number)
{

static int counter   = 0,   // internal timer
           the_house = -1;  // the house being blinked (if any)

// this function will blink the house that it is sent

// test if house number is -1, this means do normal processing

if (house_number==-1)
   {

   // test if there is currently a blinking house

   if (the_house!=-1)
      {

      // increment timer

      ++counter;

      // see if it's time to change color
      // this sequence will blink the house 4 times

      if (counter==15)
         Set_Palette_Register(the_house+START_NUM_COLOR,
                             (RGB_color_ptr)&roof_color);
      else
      if (counter==25)
         Set_Palette_Register(the_house+START_NUM_COLOR,
                             (RGB_color_ptr)&alert_color);
      else
      if (counter==40)
         Set_Palette_Register(the_house+START_NUM_COLOR,
                             (RGB_color_ptr)&roof_color);
      else
      if (counter==50)
         Set_Palette_Register(the_house+START_NUM_COLOR,
                             (RGB_color_ptr)&alert_color);
      else
      if (counter==65)
         Set_Palette_Register(the_house+START_NUM_COLOR,
                             (RGB_color_ptr)&roof_color);
      else
      if (counter==75)
         Set_Palette_Register(the_house+START_NUM_COLOR,
                             (RGB_color_ptr)&alert_color);
      else
      if (counter==90)
         {
         Set_Palette_Register(the_house+START_NUM_COLOR,
                             (RGB_color_ptr)&roof_color);


         // reset system

         the_house=-1;

         } // end if end of road

      } // end if there is a blinking house

   } // end if do normal processing
else
   {
   // user is bliking a new house so set up the variables appropriately

   // test if there is a house being blinked and if so preempt it

   if (the_house!=-1)
      Set_Palette_Register(the_house+START_NUM_COLOR,
                          (RGB_color_ptr)&roof_color);

   // set the house to on

   the_house = house_number;

   // reset the counter

   counter   = 0;

   // turn the color register on

   Set_Palette_Register(the_house+START_NUM_COLOR,
                       (RGB_color_ptr)&alert_color);

   } // send else

} // end Blink_House

/////////////////////////////////////////////////////////////////////////////

void Show_Stats(void)
{
// this function displays the stats screen


float hours,         // hours worked
      pay,           // pay due to salary
      cost_mopeds,   // cost of breaking mopeds
      cost_lost,     // cost due to lost orders
      cost_injuries, // cost of hurting peds
      gross,         // gross pay
      net,           // net pay after taxes and deductions
      final;         // total finally pay

char buffer[64];  // used to build up stat strings

// load instruction screen and display it until a key press

PCX_Init((pcx_picture_ptr)&intro_pcx);

PCX_Load("simstats.pcx", (pcx_picture_ptr)&intro_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

// display stats

// hours worked

hours = (float)(boy_time-540)/(float)60;

sprintf(buffer,"%.2f",hours);

Blit_String(202,48,15,buffer,1);

// total pay

pay = 4.25*hours;

sprintf(buffer,"$%.2f",pay);

Blit_String(202,59,15,buffer,1);

// crashed mopeds

cost_mopeds = 50*(3-boy_mopeds);

sprintf(buffer,"$%.2f",cost_mopeds);

Blit_String(202,95,15,buffer,1);

// injuries

cost_injuries = 100*boy_xpeds;

sprintf(buffer,"$%.2f",cost_injuries);

Blit_String(202,107,15,buffer,1);

// lost orders

cost_lost = 3*(total_orders-orders_filled);

sprintf(buffer,"$%.2f",cost_lost);

Blit_String(202,119,15,buffer,1);

// gross pay

gross = pay - cost_mopeds - cost_injuries - cost_lost;

if (gross<0)
    gross=0;

sprintf(buffer,"$%.2f",gross);

Blit_String(230,143,15,buffer,1);

// after taxes

net = gross*.7;

sprintf(buffer,"$%.2f",net);

Blit_String(230,155,15,buffer,1);

// plus tips

final = net+boy_tips;

sprintf(buffer,"$%.2f",final);

Blit_String(230,167,15,buffer,1);

// wait for exit

while(kbhit())
      getch();

// let user see it

while(!kbhit()){};

getch();

PCX_Delete((pcx_picture_ptr)&intro_pcx);

} // end Show_Stats

// M A I N ////////////////////////////////////////////////////////////////////

void main(void)
{

int done=0,         // main event loop exit flag
    sent=0,         // used to flag end sequence has been initiated
    cell_x,         // cell x position
    cell_y,         // cell y position
    cell_id,        // bitmap id
    tx,ty,          // test position
    direction;      // temporary direction variable

char buffer[128]; // used for string printing

// begin the program

printf("\nStarting SIM-Pizza...");

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

Load_Humans();

Load_Cars();

Load_Splats();

Load_Pizzas();

Load_Death();

Load_Background();

// initialize everything

Init_Humans();

Init_Cars();

Init_Pizzas();

Init_Messages();

Insert_Message("Boda Boom! Bada Bing!",0);

// install the new keyboard driver

Install_Keyboard();

// draw the screen

Draw_Screen((char **)city_1);

// scan under player first time before loop

SET_SPRITE_SIZE(12,12);

boy.object.x = (int)(boy.x >> FP_SHIFT);
boy.object.y = (int)(boy.y >> FP_SHIFT);

Behind_Sprite_DB((sprite_ptr)&boy.object);

// say what's up

Play_Sound(SOUND_START);

// begin main event loop

while(!done)
     {
     // erase everything

     if (boy.state==BOY_ALIVE)
     {

     SET_SPRITE_SIZE(12,12);
     boy.object.x = (int)(boy.x >> FP_SHIFT);
     boy.object.y = (int)(boy.y >> FP_SHIFT);
     Erase_Sprite_DB((sprite_ptr)&boy.object);
     } // end if boy alive

     Erase_Humans();

     Erase_Cars();

     Erase_Splats();

     Erase_Pizzas();

     Erase_Boy_Death();

     // reset throttle flag

     throttle_on = 0;

     // is user pressing a key

     if ((key_table[INDEX_RIGHT] || key_table[INDEX_LEFT] ||
          key_table[INDEX_UP]    || key_table[INDEX_DOWN] ||
          key_table[INDEX_ALT]   ||
          key_table[INDEX_SPACE] || key_table[INDEX_ESC] ) &&
          (boy.state==BOY_ALIVE))
        {
        // which key?

              if (key_table[INDEX_ESC]) // exit game
                 {
                 // exit system

                 done=1;

                 } // end if

              if (key_table[INDEX_RIGHT])  // move right
                 {
                   // turn moped right

                   if (++boy.counter_1==boy.threshold_1)
                      {
                      // reset counter

                      boy.counter_1 = 0;

                      // turn moped right

                      if (--boy.direction<0)
                            boy.direction=15;

                      boy.object.curr_frame = boy.direction;

                      } // end if time to process

                  } // end if right
              else
              if (key_table[INDEX_LEFT])  // move left
                   {
                   // turn moped left

                   if (++boy.counter_1==boy.threshold_1)
                   {
                   // reset counter

                   boy.counter_1 = 0;

                   // turn moped left

                   if (++boy.direction>15)
                         boy.direction=0;

                   boy.object.curr_frame = boy.direction;

                   } // end if time to process

                   } // end if left

              if (key_table[INDEX_ALT]) // blast horn
                 {

                 Play_Sound(SOUND_MOPED_HORN);

                 } // end if horn

              if (key_table[INDEX_UP])  // move up
                 {
                 // accelerate

                 boy.throttle+=boy.hp;

                 // test we are at maximum speed

                 if (boy.throttle>boy.max_throttle)
                     boy.throttle = boy.max_throttle;

                 // set flag to denote that throttle is engaged

                 throttle_on = 1;

                 } // end if up

              else
              if (key_table[INDEX_DOWN]) // brakes
                 {
                 // hit the brakes!

                 boy.throttle-=boy.brakes;

                 // test if we have stopped

                 if (boy.throttle<0)
                     boy.throttle = 0;

                 } // end if brakes


              if (key_table[INDEX_SPACE]) // throw pizza
                 {
                 // throw a pizza

                 // are there any pizzas to throw

                 if (--boy_pizzas<0)
                    boy_pizzas = 0;
                 else
                    {
                    // send message

                    Insert_Message("Here's your pizza mister!",1);

                    // rotate pizza direction 90 CC

                    direction = boy.direction - 4;

                    if (direction<0)
                        direction+=16;

                    // send a pizza sailing

                    Start_Pizza(((int)(boy.x >> FP_SHIFT)+5),
                                ((int)(boy.y >> FP_SHIFT)+5),
                                pizza_xv[direction],
                                pizza_yv[direction],
                                0);

                    } // end else

                 } // end if throw a pizza

        } // end if kbhit

     // translate moped

     boy.x=boy.x + ((boy.throttle * boy_xv[boy.direction])>>FP_SHIFT);
     boy.y=boy.y + ((boy.throttle * boy_yv[boy.direction])>>FP_SHIFT);

     // test if not on road

     cell_x = ((int)(boy.x >> FP_SHIFT)+6)>>4;
     cell_y = ((int)(boy.y >> FP_SHIFT)+6)>>4;

     // extract bitmap id

     cell_id = cell_id = city_1[cell_y][cell_x];

     // test if cell is not road

     if ((cell_id>=32 && cell_id<=46) ||
         (cell_id>=49 && cell_id<=50) )
        {
        // back moped up

        boy.x=boy.x - ((boy.throttle * boy_xv[boy.direction])>>FP_SHIFT);
        boy.y=boy.y - ((boy.throttle * boy_yv[boy.direction])>>FP_SHIFT);

        // turn off throttle

        boy.throttle = 0;

        } // end if moped on property

     // do edge tests

     if (boy.x > (fixed)(320-12)<<FP_SHIFT)
        boy.x = 0;
     else
     if (boy.x < 0)
        boy.x = (fixed)(320-12)<<FP_SHIFT;

     if (boy.y > (fixed)(176-12)<<FP_SHIFT)
        boy.y = 0;
     else
     if (boy.y < 0)
        boy.y = (fixed)(176-12)<<FP_SHIFT;

     // test if throttle is disengaged, if so, activate friction

     if (!throttle_on)
        {
        // decrease throttle

        boy.throttle+=boy.friction;

        // test we are at maximum speed

        if (boy.throttle < 0)
            boy.throttle = 0;

        } // end if throttle off


     // test if moped has got a new load from the pizza hut

     tx = ((int)(boy.x >> FP_SHIFT))+6;
     ty = ((int)(boy.y >> FP_SHIFT))+8;

     if (tx>=40 && tx<=52 && ty>=136 && ty<=150)
        boy_pizzas = 5;

     // move everything

     Move_Humans();

     Move_Cars();

     Move_Pizzas();

     Animate_Splats();

     Animate_Boy_Death();

     Age_Orders();

     // start objects here

     if (rand()%(difficulty)==1)
        Start_Human();

     if (rand()%(difficulty)==1)
        Start_Car();

     if (rand()%(difficulty*5)==1)
        Order_Pizza();

     // test if it's close to quiting time

     if (boy_time==1010 && !sent) // 4:50 pm
        {
        Insert_Message("Vinnie!...time to go home",1);
        Play_Sound(SOUND_COME_HOME);
        // set a flag so this doesn't happen any more
        sent=1;

        }

     // scan under objects

     if (boy.state==BOY_ALIVE)
     {

     SET_SPRITE_SIZE(12,12);

     boy.object.x = (int)(boy.x >> FP_SHIFT);
     boy.object.y = (int)(boy.y >> FP_SHIFT);

     Behind_Sprite_DB((sprite_ptr)&boy.object);
     } // end if boy alive

     Behind_Humans();

     Behind_Cars();

     Behind_Splats();

     Behind_Pizzas();

     Behind_Boy_Death();

     // draw objects

     Draw_Cars();

     Draw_Humans();

     Draw_Boy_Death();

     if (boy.state==BOY_ALIVE)
     {

     SET_SPRITE_SIZE(12,12);

     // boy.object.x = (int)(boy.x >> FP_SHIFT);
     // boy.object.y = (int)(boy.y >> FP_SHIFT);

     Draw_Sprite_DB((sprite_ptr)&boy.object);
     } // end if boy alive

     Draw_Splats();

     Draw_Pizzas();

     // draw all instrumentation

     Draw_Gauges();

     Send_Message();

     Display_Message();

     Animate_Speedo();

     Blink_House(-1); // note: -1 means do normal processing i.e. it's a command

     // display double buffer

     Show_Double_Buffer((char far *)double_buffer);

     // wait a sec

     Delay(1);

     // test if it's time to bail

     if (boy_mopeds==0 && boy.state==BOY_ALIVE)
        {

        done=END_MOPEDS;   // end game because out of mopeds

        } // end if ran out of mopeds
     else
     if (boy_time>1020)  // if it's after 5:00, let's end game also
        {

        done=END_TIME;   // end game because of time

        } // end if ran out of time

     } // end while

// wait a second

Delay(50);

// do a screen transition

Fade_Lights();

// remove keyboard driver

Delete_Keyboard();

// let user see what he did

Show_Stats();

// exit system with a cool transition

Melt();

// reset the video mode back to text

Set_Video_Mode(TEXT_MODE);

// free the double buffer

Delete_Double_Buffer();

// close sound system

Close_Sound_System();

printf("\nSIM-Pizza shutting down...");
printf("\nAll resources released...exiting back to DOS.\n");

} // end main


