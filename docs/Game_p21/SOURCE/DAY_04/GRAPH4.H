
// GRAPH4.H

// D E F I N E S  ////////////////////////////////////////////////////////////

#define SPRITE_WIDTH      16
#define SPRITE_HEIGHT     16

#define MAX_SPRITE_FRAMES 24
#define SPRITE_DEAD       0
#define SPRITE_ALIVE      1
#define SPRITE_DYING      2

// S T R U C T U R E S ///////////////////////////////////////////////////////

typedef struct pcx_header_typ
        {
        char manufacturer;
        char version;
        char encoding;
        char bits_per_pixel;
        int x,y;
        int width,height;
        int horz_res;
        int vert_res;
        char ega_palette[48];
        char reserved;
        char num_color_planes;
        int bytes_per_line;
        int palette_type;
        char padding[58];

        } pcx_header, *pcx_header_ptr;


typedef struct pcx_picture_typ
        {
        pcx_header header;
        RGB_color palette[256];
        char far *buffer;

        } pcx_picture, *pcx_picture_ptr;


typedef struct sprite_typ
        {
        int x,y;            // position of sprite
        int x_old,y_old;    // old position of sprite
        int width,height;   // dimensions of sprite in pixels
        int anim_clock;     // the animation clock
        int anim_speed;     // the animation speed
        int motion_speed;   // the motion speed
        int motion_clock;   // the motion clock

        char far *frames[MAX_SPRITE_FRAMES]; // array of pointers to the images
        int curr_frame;                      // current frame being displayed
        int num_frames;                      // total number of frames
        int state;                           // state of sprite, alive, dead...
        char far *background;                // whats under the sprite
        void far *extra_data;                // an auxialliary pointer to more
                                             // data if needed
        } sprite, *sprite_ptr;


// E X T E R N A L S /////////////////////////////////////////////////////////

// G L O B A L S  ////////////////////////////////////////////////////////////

extern unsigned int sprite_width;
extern unsigned int sprite_height;

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void PCX_Init(pcx_picture_ptr image);

void PCX_Load(char *filename, pcx_picture_ptr image,int enable_palette);

void PCX_Delete(pcx_picture_ptr image);

void PCX_Show_Buffer(pcx_picture_ptr image);

void Sprite_Init(sprite_ptr sprite,int x,int y,int ac,int as,int mc,int ms);

void Sprite_Delete(sprite_ptr sprite);

void PCX_Grab_Bitmap(pcx_picture_ptr image,
                     sprite_ptr sprite,
                     int sprite_frame,
                     int grab_x, int grab_y);

void Behind_Sprite(sprite_ptr sprite);

void Erase_Sprite(sprite_ptr sprite);

void Draw_Sprite(sprite_ptr sprite);

unsigned char Get_Pixel(int x,int y);

int Sprite_Collide(sprite_ptr sprite_1, sprite_ptr sprite_2);

