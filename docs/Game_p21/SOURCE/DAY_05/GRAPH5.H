
// graph5.h //////////////////////////////////////////////////////////////////

// D E F I N E S /////////////////////////////////////////////////////////////

// global clipping region default value

#define POLY_CLIP_MIN_X   0
#define POLY_CLIP_MIN_Y   0

#define POLY_CLIP_MAX_X   319
#define POLY_CLIP_MAX_Y   199

#define MAX_VERTICES          16   // maximum numbr of vertices in a polygon

// S T R U C T U R E S ///////////////////////////////////////////////////////

typedef struct vertex_typ
        {

        float x,y;        // the vertex in 2-D

        } vertex, *vertex_ptr;

//////////////////////////////////////////////////////////////////////////////

// the polygon structure

typedef struct polygon_typ
        {

        int b_color;                    // border color
        int i_color;                    // interior color
        int closed;                     // is the polygon closed
        int filled;                     // is this polygon filled

        int lxo,lyo;                    // local origin of polygon
        int num_vertices;               // number of defined vertices
        vertex vertices[MAX_VERTICES];  // the vertices of the polygon

        } polygon, *polygon_ptr;

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Create_Tables(void);

void Rotate_Polygon(polygon_ptr poly, int angle);

void Scale_Polygon(polygon_ptr poly, float scale);

void Translate_Polygon(polygon_ptr poly, int dx,int dy);

void Draw_Polygon(polygon_ptr poly);

int Clip_Line(int *x1,int *y1,int *x2, int *y2);

void Draw_Polygon_Clip(polygon_ptr poly);

void Bline(int xo, int yo, int x1,int y1, unsigned char color);

void Draw_Boundary(int color);

// G L O B A L S  ////////////////////////////////////////////////////////////

extern float sin_look[],   // look up tables for sin and cosine
             cos_look[];

// the clipping region, set it to default on start up

extern int poly_clip_min_x,
           poly_clip_min_y,
           poly_clip_max_x,
           poly_clip_max_y;


