
// GRAPH6.H

// D E F I N E S /////////////////////////////////////////////////////////////

#define NUM_WORMS 320

#define VGA_INPUT_STATUS_1   0x3DA // vga status reg 1, bit 3 is the vsync
                                   // when 1 - retrace in progress
                                   // when 0 - no retrace

#define VGA_VSYNC_MASK 0x08        // masks off unwanted bit of status reg

// S T R U C T U R E S //////////////////////////////////////////////////////

typedef struct worm_typ
        {
        int y;       // current y position of worm
        int color;   // color of worm
        int speed;   // speed of worm
        int counter; // counter

        } worm, *worm_ptr;

// E X T E R N A L S /////////////////////////////////////////////////////////

extern unsigned char far *double_buffer;

extern unsigned int buffer_height;

extern unsigned int buffer_size;

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Show_Double_Buffer(char far *buffer);

int Create_Double_Buffer(int num_lines);

void Fill_Double_Buffer(int color);

void Delete_Double_Buffer(void);

void Plot_Pixel_Fast_DB(int x,int y,unsigned char color);

void Scale_Sprite(sprite_ptr sprite,float scale);

void Fade_Lights(void);

void Disolve(void);

void Melt(void);

void Sheer(void);

void Wait_For_Vsync(void);

void Behind_Sprite_DB(sprite_ptr sprite);

void Erase_Sprite_DB(sprite_ptr sprite);

void Draw_Sprite_DB(sprite_ptr sprite);

// slipped these in to see if you are paying attention!

void Blit_Char_DB(int xc,int yc,char c,int color,int trans_flag);

void Blit_String_DB(int x,int y,int color, char *string,int trans_flag);


