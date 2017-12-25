
// D E F I N E S  ////////////////////////////////////////////////////////////


#define JOYPORT      0x201  // joyport is at 201 hex

#define BUTTON_1_1   0x10   // joystick 1, button 1
#define BUTTON_1_2   0x20   // joystick 1, button 2
#define BUTTON_2_1   0x40   // joystick 2, button 1
#define BUTTON_2_2   0x80   // joystick 2, button 2

#define JOYSTICK_1   0x01   // joystick 1, in general
#define JOYSTICK_2   0x02   // joystick 2, in general

#define JOYSTICK_1_X 0x01   // joystick 1, x axis
#define JOYSTICK_1_Y 0x02   // joystick 1, y axis
#define JOYSTICK_2_X 0x04   // joystick 2, x axis
#define JOYSTICK_2_Y 0x08   // joystick 2, y axis


#define JOY_1_CAL       1   // command to calibrate joystick #1
#define JOY_2_CAL       2   // command to calibrate joystick #2


// G L O B A L S  ////////////////////////////////////////////////////////////

extern unsigned int joy_1_max_x,   // global joystick calibration variables
                    joy_1_max_y,
                    joy_1_min_x,
                    joy_1_min_y,
                    joy_1_cx,
                    joy_1_cy,
                    joy_2_max_x,
                    joy_2_max_y,
                    joy_2_min_x,
                    joy_2_min_y,
                    joy_2_cx,
                    joy_2_cy;


// P R O T O T Y P E S ///////////////////////////////////////////////////////

unsigned char Buttons(unsigned char button);

unsigned int Joystick(unsigned char stick);

unsigned int Joystick_Bios(unsigned char stick);

unsigned char Buttons_Bios(unsigned char button);

void Joystick_Calibrate(int stick);

int Joystick_Available(int stick_num);


