
// Header file for first module of game library GRAPH3.C

// D E F I N E S /////////////////////////////////////////////////////////////

#define VGA256              0x13  // 320x200x256
#define TEXT_MODE           0x03  // 80x25 text mode

#define PALETTE_MASK        0x3C6 // the bit mask register
#define PALETTE_REGISTER_RD 0x3C7 // set read index at this I/O
#define PALETTE_REGISTER_WR 0x3C8 // set write index at this I/O
#define PALETTE_DATA        0x3C9 // the R/W data is here

#define ROM_CHAR_SET_SEG 0xF000   // segment of 8x8 ROM character set
#define ROM_CHAR_SET_OFF 0xFA6E   // begining offset of 8x8 ROM character set

#define CHAR_WIDTH        8       // size of characters
#define CHAR_HEIGHT       8

#define SCREEN_WIDTH      (unsigned int)320 // mode 13h screen dimensions
#define SCREEN_HEIGHT     (unsigned int)200

// S T R U C T U R E S ///////////////////////////////////////////////////////

// this structure holds a RGB triple in three bytes

typedef struct RGB_color_typ
        {

        unsigned char red;    // red   component of color 0-63
        unsigned char green;  // green component of color 0-63
        unsigned char blue;   // blue  component of color 0-63

        } RGB_color, *RGB_color_ptr;


// P R O T O T Y P E S ///////////////////////////////////////////////////////

void H_Line_Fast(int x1,int x2,int y,unsigned int color);

void V_Line(int y1,int y2,int x,unsigned int color);

void H_Line_Fast(int x1,int x2,int y,unsigned int color);

void Set_Palette_Register(int index, RGB_color_ptr color);

void Get_Palette_Register(int index, RGB_color_ptr color);

void Blit_Char(int xc,int yc,char c,int color,int trans_flag);

void Blit_String(int x,int y,int color, char *string,int trans_flag);

void Plot_Pixel(int x,int y,unsigned char color);

void Plot_Pixel_Fast(int x,int y,unsigned char color);

void Set_Video_Mode(int mode);

void Delay(int clicks);

// G L O B A L S /////////////////////////////////////////////////////////////

extern unsigned char far *video_buffer;   // vram byte ptr
extern unsigned int far *video_buffer_w;  // vram word ptr
extern unsigned char far *rom_char_set;   // rom characters 8x8


