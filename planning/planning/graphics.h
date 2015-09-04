//////////////////////////////////////////////////////////////////////////////////
//
//   Program Name:  Graphics Engine (Implementation File)
//                  graphics.h 
//  	Description:  WinBgi package extracted from:
//                  http://csci.biola.edu/csci105/using_winbgi.html  
//
//                  *slightly modified to run for gcc by N.Reyes              
//
//        History:  18 July 2005
//
//////////////////////////////////////////////////////////////////////////////////

// You don't need to edit this file, or print it out when submitting your project.

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__


#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>


#include <stdlib.h>
#include <time.h>
#include <string.h>



///////////////////////////////

#define far
#define huge
#define random(range) (rand() % (range))

enum colors { 
    BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY, DARKGRAY, 
    LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE
};

enum write_modes { 
    COPY_PUT, XOR_PUT, OR_PUT, AND_PUT, NOT_PUT 
};
enum line_styles { 
    SOLID_LINE, DOTTED_LINE, CENTER_LINE, DASHED_LINE, USERBIT_LINE
};
enum fill_styles { 
    EMPTY_FILL, SOLID_FILL, LINE_FILL, LTSLASH_FILL, SLASH_FILL, BKSLASH_FILL,
    LTBKSLASH_FILL, HATCH_FILL, XHATCH_FILL, INTERLEAVE_FILL, WIDE_DOT_FILL, 
    CLOSE_DOT_FILL, USER_FILL
};
enum text_directions { 
    HORIZ_DIR, VERT_DIR
};
enum font_types { 
    DEFAULT_FONT, TRIPLEX_FONT, SMALL_FONT, SANSSERIF_FONT, GOTHIC_FONT 
};

#define LEFT_TEXT					0
#define CENTER_TEXT				1
#define RIGHT_TEXT				2
#define BOTTOM_TEXT				0
#define TOP_TEXT					2

#define NORM_WIDTH				1
#define THICK_WIDTH				3

#define DOTTEDLINE_LENGTH			2
#define CENTRELINE_LENGTH			4

#define USER_CHAR_SIZE				0
#define MAXCOLORS					63 

#define CLIP_ON					1
#define CLIP_OFF					0

#define TOP_ON						1
#define TOP_OFF				   0

#define LEFT_BUTTON				0
#define RIGHT_BUTTON			   1

enum graphics_errors {
	grOk = 0,
	grNoInitGraph = -1,
	grNotDetected = -2,
	grFileNotFound = -3,
	grInvalidDriver	= -4,
	grNoLoadMem = -5,
	grNoScanMem = -6,
	grNoFloodMem = -7,
	grFontNotFound = -8,
	grNoFontMem = -9,
	grInvalidMode =	-10,
	grError = -11,
	grIOerror = -12,
	grInvalidFont = -13,
	grInvalidFontNum = -14,
	grInvalidDeviceNum = -15,
	grInvalidVersion = -18
};


/* Graphics drivers constants, includes X11 which is particular to XBGI. */
#define DETECT					0
#define CGA						1
#define MCGA					2
#define EGA						3
#define EGA64					4
#define EGAMONO				5
#define IBM8514				6
#define HERCMONO			   7
#define ATT400					8
#define VGA						9
#define PC3270				  10

/* Graphics modes constants. */
#define CGAC0						0
#define CGAC1						1
#define CGAC2						2
#define CGAC3						3
#define CGAHI						4

#define MCGAC0						0
#define MCGAC1						1
#define MCGAC2						2
#define MCGAC3						3
#define MCGAMED					4
#define MCGAHI						5

#define EGALO						0
#define EGAHI						1

#define EGA64LO					0
#define EGA64HI					1

#define EGAMONOHI					3

#define HERCMONOHI				0

#define ATT400C0					0
#define ATT400C1					1
#define ATT400C2					2
#define ATT400C3					3
#define ATT400MED					4
#define ATT400HI					5

#define VGALO						0
#define VGAMED						1
#define VGAHI						2
#define VGAMAX                3

#define PC3270HI					0

#define IBM8514LO					0
#define IBM8514HI					1

typedef struct arccoordstype {
    int x;
    int y;
    int xstart;
    int ystart;
    int xend;
    int yend;
} arccoordstype;

typedef char fillpatterntype[8];

typedef struct fillsettingstype {
    int pattern;
    int color;
} fillsettingstype;

typedef struct linesettingstype {
    int linestyle;
    unsigned int upattern;
    int thickness;
} linesettingstype;

typedef struct palettetype {
    unsigned char size;
    signed char colors[64];
} palettetype;

typedef struct textsettingstype {
    int font;	
    int direction;  
    int charsize;
    int horiz;
    int vert;
} textsettingstype;

typedef struct viewporttype {
    int left;
    int top;
    int right;
    int bottom;
    int clip;
} viewporttype;



//////////////////////////////////////////////////////////////////////////////
#define MAX_PAGES 16

static HDC hdc[4];

static HPEN hPen;
static HRGN hRgn;
static HFONT hFont;
static NPLOGPALETTE pPalette;
static PAINTSTRUCT ps;
static HWND hWnd;
static HBRUSH hBrush[USER_FILL+1];
static HBRUSH hBackgroundBrush;

static HPALETTE hPalette;
static HBITMAP hBitmap[MAX_PAGES];
static HBITMAP hPutimageBitmap;

static int timeout_expired;

#define PEN_CACHE_SIZE   8
#define FONT_CACHE_SIZE  8 
#define BG               64
#define TIMER_ID         1

#define ESC 0x1b



///////////////////////////////////////////////////////////////////////////////

//
// When XOR or NOT write modes are used for drawing high BG bit is cleared, so
// drawing colors should be adjusted to preserve this bit
// 
#define ADJUSTED_MODE(mode) ((mode) == XOR_PUT || (mode) == NOT_PUT)



static int screen_width;
static int screen_height;
static int window_width;
static int window_height;

//Mouse info    (Added 1-Oct-2000, Matthew Weathers)
static bool bMouseUp = false;
static bool bMouseDown = false;
static int iCurrentMouseX = 0;
static int iCurrentMouseY = 0;
static int iClickedMouseX = 0;
static int iClickedMouseY = 0;
static int iWhichMouseButton = LEFT_BUTTON;

static int line_style_cnv[] = {
    PS_SOLID, PS_DOT, PS_DASHDOT, PS_DASH, 
    PS_DASHDOTDOT /* if user style lines are not supported */
}; 
static int write_mode_cnv[] = 
  {R2_COPYPEN, R2_XORPEN, R2_MERGEPEN, R2_MASKPEN, R2_NOTCOPYPEN};
static int bitblt_mode_cnv[] = 
  {SRCCOPY, SRCINVERT, SRCPAINT, SRCAND, NOTSRCCOPY};

static int font_weight[] = 
{ 
    FW_BOLD,    // DefaultFont
    FW_NORMAL,  // TriplexFont
    FW_NORMAL,  // SmallFont
    FW_NORMAL,  // SansSerifFont
    FW_NORMAL,  // GothicFont
    FW_NORMAL,  // ScriptFont
    FW_NORMAL,  // SimplexFont
    FW_NORMAL,  // TriplexScriptFont
    FW_NORMAL,  // ComplexFont
    FW_NORMAL,  // EuropeanFont
    FW_BOLD     // BoldFont
};

static int font_family[] = 
{
    FIXED_PITCH|FF_DONTCARE,     // DefaultFont
    VARIABLE_PITCH|FF_ROMAN,     // TriplexFont
    VARIABLE_PITCH|FF_MODERN,    // SmallFont
    VARIABLE_PITCH|FF_DONTCARE,  // SansSerifFont
    VARIABLE_PITCH|FF_SWISS,     // GothicFont
    VARIABLE_PITCH|FF_SCRIPT,    // ScriptFont
    VARIABLE_PITCH|FF_DONTCARE,  // SimplexFont
    VARIABLE_PITCH|FF_SCRIPT,    // TriplexScriptFont
    VARIABLE_PITCH|FF_DONTCARE,  // ComplexFont
    VARIABLE_PITCH|FF_DONTCARE,  // EuropeanFont
    VARIABLE_PITCH|FF_DONTCARE   // BoldFont
  };

static char* font_name[] = 
{
    "Console",          // DefaultFont
    "Times New Roman",  // TriplexFont
    "Small Fonts",      // SmallFont
    "MS Sans Serif",    // SansSerifFont
    "Arial",            // GothicFont
    "Script",           // ScriptFont
    "Times New Roman",  // SimplexFont
    "Script",           // TriplexScriptFont
    "Courier New",      // ComplexFont
    "Times New Roman",  // EuropeanFont
    "Courier New Bold", // BoldFont
};

static int text_halign_cnv[] = {TA_LEFT, TA_CENTER, TA_RIGHT};  
static int text_valign_cnv[] = {TA_BOTTOM, TA_BASELINE, TA_TOP};

static palettetype current_palette;

static struct { int width; int height; } font_metrics[][11] = { 
{{0,0},{8,8},{16,16},{24,24},{32,32},{40,40},{48,48},{56,56},{64,64},{72,72},{80,80}}, // DefaultFont
{{0,0},{13,18},{14,20},{16,23},{22,31},{29,41},{36,51},{44,62},{55,77},{66,93},{88,124}}, // TriplexFont
{{0,0},{3,5},{4,6},{4,6},{6,9},{8,12},{10,15},{12,18},{15,22},{18,27},{24,36}}, // SmallFont
{{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // SansSerifFont 
{{0,0},{13,19},{14,21},{16,24},{22,32},{29,42},{36,53},{44,64},{55,80},{66,96},{88,128}}, // GothicFont
// I am not sure about metrics of following fonts
{{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // ScriptFont
{{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // SimplexFont
{{0,0},{13,18},{14,20},{16,23},{22,31},{29,41},{36,51},{44,62},{55,77},{66,93},{88,124}}, // TriplexScriptFont
{{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // ComplexFont
{{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // EuropeanFont
{{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}} // BoldFont
}; 

struct BGIimage { 
    short width;
    short height;
    int   reserved; // let bits be aligned to DWORD boundary
    char  bits[1];
};

struct BGIbitmapinfo { 
    BITMAPINFOHEADER hdr;
    short            color_table[64];
};
    
static BGIbitmapinfo bminfo = {
    {sizeof(BITMAPINFOHEADER), 0, 0, 1, 4, BI_RGB}
};

static int* image_bits; 

static int normal_font_size[] = { 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

static linesettingstype line_settings;
static fillsettingstype fill_settings;

static int color;
static int bkcolor;
static int text_color;

static int aspect_ratio_x, aspect_ratio_y;

static textsettingstype text_settings;

static viewporttype view_settings;

static int font_mul_x, font_div_x, font_mul_y, font_div_y;

static enum { ALIGN_NOT_SET, UPDATE_CP, NOT_UPDATE_CP } text_align_mode; 

#define BORDER_WIDTH  8 
#define BORDER_HEIGHT 27

static int write_mode;

static int visual_page;
static int active_page;

static arccoordstype ac;





/////////////////////////////////////////////////////////////////////

#ifndef NOT_USE_PROTOTYPES
#define PROTO(ARGS) ARGS
#else
#define PROTO(ARGS) ()
#endif

#if defined(__cplusplus) 
extern "C" {
#endif

//
// Setting this variable to 0 increase speed of drawing but
// correct redraw is not possible. By default this variable is initialized by 1
//
extern int bgiemu_handle_redraw;

//
// Default mode choosed by WinBGI if DETECT value is specified for 
// device parameter of initgraoh(). Default value is VGAMAX which
// cause creation of maximized window (resolution depends on display mode)
//
extern int bgiemu_default_mode;

void _graphfreemem PROTO((void *ptr, unsigned int size));
void* _graphgetmem PROTO((unsigned int size));
void arc PROTO((int, int, int, int, int));
void bar PROTO((int, int, int, int));
void bar3d PROTO((int, int, int, int, int, int));
void circle PROTO((int, int, int));
void cleardevice PROTO((void));
void clearviewport PROTO((void));
void closegraph PROTO((void));
void detectgraph PROTO((int *, int *));
void drawpoly PROTO((int, int *));
void ellipse PROTO((int, int, int, int, int, int));
void fillellipse PROTO((int, int, int, int));
void fillpoly PROTO((int, int *));
void floodfill PROTO((int, int, int));
void getarccoords PROTO((arccoordstype *));
void getaspectratio PROTO((int *, int *));
int getbkcolor PROTO((void));
int getcolor PROTO((void));
palettetype* getdefaultpalette PROTO((void));
char* getdrivername PROTO((void));
void getfillpattern PROTO((char const *));
void getfillsettings PROTO((fillsettingstype *));
int getgraphmode PROTO((void));
void getimage PROTO((int, int, int, int, void *));
void getlinesettings PROTO((linesettingstype *));
int getmaxcolor PROTO((void));
int getmaxmode PROTO((void));
int getmaxx PROTO((void));
int getmaxy PROTO((void));
char* getmodename PROTO((int));
void getmoderange PROTO((int, int *, int *));
void getpalette PROTO((palettetype *));
int getpalettesize PROTO((void));
int getpixel PROTO((int, int));
void gettextsettings PROTO((textsettingstype *));
void getviewsettings PROTO((viewporttype *));
int getx PROTO((void));
int gety PROTO((void));
void graphdefaults PROTO((void));
char* grapherrormsg PROTO((int));
int graphresult PROTO((void));
unsigned int imagesize PROTO((int, int, int, int));
void initgraph PROTO((int *, int *, char const *,int , int));
int installuserdriver PROTO((char const *, int *));
int installuserfont PROTO((char const *));
void line PROTO((int, int, int, int));
void linerel PROTO((int, int));
void lineto PROTO((int, int));
void moverel PROTO((int, int));
void moveto PROTO((int, int));
void outtext PROTO((char const *));
void outtextxy PROTO((int, int, char const *));
void pieslice PROTO((int, int, int, int, int));
void putimage PROTO((int, int, void *, int));
void putpixel PROTO((int, int, int));
void rectangle PROTO((int, int, int, int));
int registerbgidriver PROTO((void *));
int registerbgifont PROTO((void *));
void restorecrtmode PROTO((void));
void sector PROTO((int, int, int, int, int, int));
void setactivepage PROTO((int));
void setallpalette PROTO((palettetype *));
void setaspectratio PROTO((int, int));
void setbkcolor PROTO((int));
void setcolor PROTO((int));
void setfillpattern PROTO((char const *, int));
void setfillstyle PROTO((int, int));
unsigned int setgraphbufsize PROTO((unsigned int));
void setgraphmode PROTO((int));
void setlinestyle PROTO((int, unsigned int, int));
void setpalette PROTO((int, int));
void setrgbpalette PROTO((int, int, int, int));
void settextjustify PROTO((int, int));
void settextstyle PROTO((int, int, int));
void setusercharsize PROTO((int, int, int, int));
void setviewport PROTO((int, int, int, int, int));
void setvisualpage PROTO((int));
void setwritemode PROTO((int));
int textheight PROTO((char const *));
int textwidth PROTO((char const *));
int getch PROTO((void));
int kbhit PROTO((void));		
void delay PROTO((unsigned msec));
void restorecrtmode PROTO((void));

bool mouseup();
bool mousedown();
void clearmouse();
int mouseclickx();
int mouseclicky();
int mousecurrentx();
int mousecurrenty();
int whichmousebutton();

#if defined(__cplusplus) 
};
#endif

#endif
