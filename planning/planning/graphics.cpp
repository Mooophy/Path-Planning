////////////////////////////////////////////////////////////////////////
//
//   Program Name:  Graphics Engine (Implementation File)
//  	Description:  WinBgi package extracted from:
//                  http://csci.biola.edu/csci105/using_winbgi.html  
//
//                  *slightly modified to run for gcc by N.Reyes              
//
//        History:  18 July 2005
//
////////////////////////////////////////////////////////////////////////


#include "graphics.h"

///////////////////////////////////////////////////////////////////////
int bgiemu_handle_redraw = TRUE;
int bgiemu_default_mode = VGAHI; //VGAMAX;
///////////////////////////////////////////////////////////////////////

class char_queue { 
  protected:
    char* buf; 
    int   put_pos;
    int   get_pos;
    int   buf_size; 
  public:
    void  put(char ch) { 
	buf[put_pos] = ch; 
	if (++put_pos == buf_size) { 
	    put_pos = 0;
	}
	if (put_pos == get_pos) { // queue is full
	    (void)get(); // loose one character
	}
    }
    char get() { 
	char ch = buf[get_pos]; 
	if (++get_pos == buf_size) { 
	    get_pos = 0;
	}
	return ch;
    }
    bool is_empty() { 
	return get_pos == put_pos; 
    }
    char_queue(int buf_size = 256) { 
	put_pos = get_pos = 0;
	this->buf_size = buf_size;
	buf = new char[buf_size]; 
    }
    ~char_queue() { 
	delete[] buf; 
    }
};

static char_queue kbd_queue;

inline int convert_userbits(DWORD buf[32], unsigned pattern)
{
    int i = 0, j;
    pattern &= 0xFFFF;

    while (true) { 
	for (j = 0; pattern & 1; j++) pattern >>= 1;
	buf[i++] = j;
	if (pattern == 0) { 
	    buf[i++] = 16 - j;
	    return i;
	}
	for (j = 0; !(pattern & 1); j++) pattern >>= 1;
	buf[i++] = j;
    }
} 


class l2elem { 
  public: 
    l2elem* next;
    l2elem* prev;

    void link_after(l2elem* after) { 
	(next = after->next)->prev = this;
	(prev = after)->next = this;
    }
    void unlink() { 
	prev->next = next;
	next->prev = prev;
    }
    void prune() { 
	next = prev = this;
    }
};

class l2list : public l2elem { 
  public:
    l2list() { prune(); }
};

class pen_cache : public l2list { 
    class pen_cache_item : public l2elem {
      public:
    	HPEN pen;
        int  color;
	int  width;
	int  style;
	unsigned pattern;
    };  
    pen_cache_item* free;
    pen_cache_item  cache[PEN_CACHE_SIZE];

  public: 
    pen_cache() { 
	for (int i = 0; i < PEN_CACHE_SIZE-1; i++) { 
	    cache[i].next = &cache[i+1];
	}
	cache[PEN_CACHE_SIZE-1].next = NULL;
	free = cache;
    }
    void select(int color) 
    {
	for (l2elem* elem = next; elem != this; elem = elem->next) { 
	    pen_cache_item* ci = (pen_cache_item*)elem;
	    if (ci->color == color &&
		ci->style == line_settings.linestyle &&
		ci->width == line_settings.thickness &&
		(line_settings.linestyle != USERBIT_LINE 
		 || line_settings.upattern == ci->pattern))
	    {
		ci->unlink(); // LRU discipline
		ci->link_after(this); 

		if (hPen != ci->pen) { 
		    hPen = ci->pen;
		    SelectObject(hdc[0], hPen);
		    SelectObject(hdc[1], hPen);
		}
		return;	    
	    }
	}
	hPen = NULL;
	if (line_settings.linestyle == USERBIT_LINE) { 
	    LOGBRUSH lb;
	    lb.lbColor = PALETTEINDEX(color);
	    lb.lbStyle = BS_SOLID;
	    DWORD style[32]; 
	    hPen = ExtCreatePen(PS_GEOMETRIC|PS_USERSTYLE, 
				line_settings.thickness, &lb, 
				convert_userbits(style,line_settings.upattern),
				style);
	} 
	if (hPen == NULL) { 
	    hPen = CreatePen(line_style_cnv[line_settings.linestyle], 
			     line_settings.thickness, 
			     PALETTEINDEX(color));
	}
	SelectObject(hdc[0], hPen);
	SelectObject(hdc[1], hPen);
	
	pen_cache_item* p;
	if (free == NULL) {
	    p = (pen_cache_item*)prev; 
	    p->unlink();
	    DeleteObject(p->pen);	    
	} else { 
	    p = free;
	    free = (pen_cache_item*)p->next;
	}
	p->pen   = hPen;
	p->color = color;
	p->width = line_settings.thickness;
	p->style = line_settings.linestyle;
	p->pattern = line_settings.upattern;
	p->link_after(this);
    }  
};	


static pen_cache pcache;



class font_cache : public l2list { 
    class font_cache_item : public l2elem {
      public:
    	HFONT font;
        int   type;
	int   direction;
	int   width, height;
    };  
    font_cache_item* free;
    font_cache_item  cache[FONT_CACHE_SIZE];

  public: 
    font_cache() { 
	for (int i = 0; i < FONT_CACHE_SIZE-1; i++) { 
	    cache[i].next = &cache[i+1];
	}
	cache[FONT_CACHE_SIZE-1].next = NULL;
	free = cache;
    }
    void select(int type, int direction, int width, int height) 
    {
	for (l2elem* elem = next; elem != this; elem = elem->next) { 
	    font_cache_item* ci = (font_cache_item*)elem;
	    if (ci->type == type &&
		ci->direction == direction &&
		ci->width == width &&
		ci->height == height)
	    {
		ci->unlink();
		ci->link_after(this);

		if (hFont != ci->font) { 
		    hFont = ci->font;
		    SelectObject(hdc[0], hFont);
		    SelectObject(hdc[1], hFont);
		}
		return;	    
	    }
	}
	hFont = CreateFont(-height,
			   width,
			   direction*900,
			   (direction&1)*900,
			   font_weight[type],
			   FALSE,
			   FALSE,
			   FALSE,
			   DEFAULT_CHARSET, 
			   OUT_DEFAULT_PRECIS,
			   CLIP_DEFAULT_PRECIS,
			   DEFAULT_QUALITY,
			   font_family[type], 
			   font_name[type]);
	SelectObject(hdc[0], hFont);
	SelectObject(hdc[1], hFont);
	
	font_cache_item* p;
	if (free == NULL) {
	    p = (font_cache_item*)prev; 
	    p->unlink();
	    DeleteObject(p->font);	    
	} else { 
	    p = free;
	    free = (font_cache_item*)p->next;
	}
	p->font = hFont;
	p->type = type;
	p->width = width;
	p->height = height;
	p->direction = direction;
	p->link_after(this);
    }  
};	


static font_cache fcache;


#define FLAGS         PC_NOCOLLAPSE
#define PALETTE_SIZE  256

static PALETTEENTRY BGIcolor[64] = {
		{ 0, 0, 0, FLAGS },        // 0
		{ 0, 0, 255, FLAGS },        // 1
		{ 0, 255, 0, FLAGS },        // 2
		{ 0, 255, 255, FLAGS },        // 3
		{ 255, 0, 0, FLAGS },        // 4
		{ 255, 0, 255, FLAGS },         // 5
		{ 165, 42, 42, FLAGS },        // 6
		{ 211, 211, 211, FLAGS },         // 7
		{ 47, 79, 79, FLAGS },        // 8
		{ 173, 216, 230, FLAGS },        // 9
		{ 32, 178, 170, FLAGS },        // 10
		{ 224, 255, 255, FLAGS },        // 11
		{ 240, 128, 128, FLAGS },        // 12
		{ 219, 112, 147, FLAGS },        // 13
		{ 255, 255, 0, FLAGS },        // 14
		{ 255, 255, 255, FLAGS },        // 15
        { 0xF0, 0xF8, 0xFF, FLAGS },        // 16
        { 0xFA, 0xEB, 0xD7, FLAGS },        // 17
        { 0x22, 0x85, 0xFF, FLAGS },        // 18
        { 0x7F, 0xFF, 0xD4, FLAGS },        // 19
        { 0xF0, 0xFF, 0xFF, FLAGS },        // 20
        { 0xF5, 0xF5, 0xDC, FLAGS },        // 21
        { 0xFF, 0xE4, 0xC4, FLAGS },        // 22
        { 0xFF, 0x7B, 0xCD, FLAGS },        // 23
        { 0x00, 0x00, 0xFF, FLAGS },        // 24
        { 0x8A, 0x2B, 0xE2, FLAGS },        // 25
        { 0xA5, 0x2A, 0x2A, FLAGS },        // 26
        { 0xDE, 0xB8, 0x87, FLAGS },        // 27
        { 0x5F, 0x9E, 0xA0, FLAGS },        // 28
        { 0x7F, 0xFF, 0x00, FLAGS },        // 29
        { 0xD2, 0x50, 0x1E, FLAGS },        // 30
        { 0xFF, 0x7F, 0x50, FLAGS },        // 31
        { 0x64, 0x95, 0xED, FLAGS },        // 32
        { 0xFF, 0xF8, 0xDC, FLAGS },        // 33
        { 0xDC, 0x14, 0x3C, FLAGS },        // 34
        { 0x68, 0xCF, 0xDF, FLAGS },        // 35
        { 0x00, 0x00, 0x8B, FLAGS },        // 36
        { 0x00, 0x8B, 0x8B, FLAGS },        // 37
        { 0xB8, 0x86, 0x0B, FLAGS },        // 38
        { 0xA9, 0xA9, 0xA9, FLAGS },        // 39
        { 0x00, 0x64, 0x00, FLAGS },        // 40
        { 0xBD, 0xB7, 0x6B, FLAGS },        // 41
        { 0x8B, 0x00, 0x8B, FLAGS },        // 42
        { 0x55, 0x6B, 0x2F, FLAGS },        // 43
        { 0xFF, 0x8C, 0x00, FLAGS },        // 44
        { 0xB9, 0x82, 0xFC, FLAGS },        // 45
        { 0x8B, 0x00, 0x00, FLAGS },        // 46
        { 0xE9, 0x96, 0x7A, FLAGS },        // 47
        { 0x8F, 0xBC, 0x8F, FLAGS },        // 48
        { 0x48, 0x3D, 0x8B, FLAGS },        // 49
        { 0x2F, 0x4F, 0x4F, FLAGS },        // 50
        { 0x00, 0xCE, 0xD1, FLAGS },        // 51
        { 0x94, 0x00, 0xD3, FLAGS },        // 52
        { 0xFF, 0x14, 0x93, FLAGS },        // 53
        { 0x00, 0xBF, 0xFF, FLAGS },        // 54
        { 0x69, 0x69, 0x69, FLAGS },        // 55
        { 0x1E, 0x90, 0xFF, FLAGS },        // 56
        { 0xB2, 0x22, 0x22, FLAGS },        // 57
        { 0xFF, 0xFA, 0xF0, FLAGS },        // 58
        { 0x22, 0x8B, 0x22, FLAGS },        // 59
        { 0xFF, 0x00, 0xFF, FLAGS },        // 60
        { 0xDC, 0xDC, 0xDC, FLAGS },        // 61
        { 0xF8, 0xF8, 0xBF, FLAGS },        // 62
        { 0xFF, 0xD7, 0x00, FLAGS },        // 63
};
    
static PALETTEENTRY BGIpalette[64];

static short SolidBrushBitmap[8] = 
  {~0xFF, ~0xFF, ~0xFF, ~0xFF, ~0xFF, ~0xFF, ~0xFF, ~0xFF};  
static short LineBrushBitmap[8] = 
  {~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0xFF};
static short LtslashBrushBitmap[8] = 
  {~0x01, ~0x02, ~0x04, ~0x08, ~0x10, ~0x20, ~0x40, ~0x80};
static short SlashBrushBitmap[8] = 
  {~0x81, ~0x03, ~0x06, ~0x0C, ~0x18, ~0x30, ~0x60, ~0xC0};
static short BkslashBrushBitmap[8] = 
  {~0xC0, ~0x60, ~0x30, ~0x18, ~0x0C, ~0x06, ~0x03, ~0x81};
static short LtbkslashBrushBitmap[8] = 
  {~0x80, ~0x40, ~0x20, ~0x10, ~0x08, ~0x04, ~0x02, ~0x01};
static short HatchBrushBitmap[8] = 
  {~0x01, ~0x01, ~0x01, ~0x01, ~0x01, ~0x01, ~0x01, ~0xFF};
static short XhatchBrushBitmap[8] = 
  {~0x81, ~0x42, ~0x24, ~0x18, ~0x18, ~0x24, ~0x42, ~0x81};
static short InterleaveBrushBitmap[8] = 
  {~0x55, ~0xAA, ~0x55, ~0xAA, ~0x55, ~0xAA, ~0x55, ~0xAA};
static short WidedotBrushBitmap[8] = 
  {~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x01};
static short ClosedotBrushBitmap[8] = 
  {~0x44, ~0x00, ~0x11, ~0x00, ~0x44, ~0x00, ~0x11, ~0x00};
	
char* grapherrormsg(int code) {	
    static char buf[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 
    	          NULL, code, 0, 
    	          buf, sizeof buf, NULL);
    return buf;
}

static int gdi_error_code;

int graphresult()
{
    return gdi_error_code; 
}

void setcolor(int c)
{
    c &= MAXCOLORS;
    color = c;
    SetTextColor(hdc[0], PALETTEINDEX(c+BG));
    SetTextColor(hdc[1], PALETTEINDEX(c+BG));
}

int getmaxcolor()
{
    return WHITE;
}

int getmaxmode()
{
    return VGAMAX;
}

char* getmodename(int mode)
{
    static char mode_str[32];
    sprintf(mode_str, "%d x %d %s", window_width, window_height, 
	    mode < 2 ? "EGA" : "VGA");
    return mode_str;
}

int getx()
{
    POINT pos;
    GetCurrentPositionEx(hdc[active_page == visual_page ? 0 : 1], &pos);
    return pos.x;
}

int gety()
{
    POINT pos;
    GetCurrentPositionEx(hdc[active_page == visual_page ? 0 : 1], &pos);
    return pos.y;
}

int getmaxx()
{
    return window_width-1;
}

int getmaxy()
{
    return window_height-1;
}


int getcolor()
{
    return color;
}

char* getdrivername()
{
    return "EGAVGA";
}

void setlinestyle(int style, unsigned int pattern, int thickness)
{
    line_settings.linestyle = style;
    line_settings.thickness = thickness;
    line_settings.upattern  = pattern;
}

void getlinesettings(linesettingstype* ls)
{
    *ls = line_settings;
}

void setwritemode(int mode)
{
    write_mode = mode;
}

void setpalette(int index, int color)
{
    color &= MAXCOLORS;
    BGIpalette[index] = BGIcolor[color];
    current_palette.colors[index] = color;
    SetPaletteEntries(hPalette, BG+index, 1, &BGIpalette[index]);
    RealizePalette(hdc[0]);
    if (index == 0) { 
	bkcolor = 0;
    }
}

void setrgbpalette(int index, int red, int green, int blue)
{
    BGIpalette[index].peRed = red & 0xFC;
    BGIpalette[index].peGreen = green & 0xFC;
    BGIpalette[index].peBlue = blue & 0xFC;
    SetPaletteEntries(hPalette, BG+index, 1, &BGIpalette[index]);
    RealizePalette(hdc[0]);
    if (index == 0) { 
	bkcolor = 0;
    }
}

void setallpalette(palettetype* pal)
{
    for (int i = 0; i < pal->size; i++) { 
	current_palette.colors[i] = pal->colors[i] & MAXCOLORS;
	BGIpalette[i] = BGIcolor[pal->colors[i] & MAXCOLORS];
    }
    SetPaletteEntries(hPalette, BG, pal->size, BGIpalette);
    RealizePalette(hdc[0]);
    bkcolor = 0;
}

palettetype* getdefaultpalette()
{
    static palettetype default_palette = { 64,
      { BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY, DARKGRAY, 
        LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
		33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
		50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
      }};
    return &default_palette;
}

void getpalette(palettetype* pal)
{
    *pal = current_palette;
}

int getpalettesize() 
{
    return MAXCOLORS+1;
}

void setbkcolor(int color)
{
    color &= MAXCOLORS;
    BGIpalette[0] = BGIcolor[color];
    SetPaletteEntries(hPalette, BG, 1, &BGIpalette[0]);
    RealizePalette(hdc[0]);
    bkcolor = color;
}

int getbkcolor()
{
    return bkcolor;
}

void setfillstyle(int style, int color)
{
    fill_settings.pattern = style;
    fill_settings.color = color & MAXCOLORS;
    SelectObject(hdc[0], hBrush[style]);
    SelectObject(hdc[1], hBrush[style]);
}

void getfillsettings(fillsettingstype* fs)
{
    *fs = fill_settings;
}

static fillpatterntype userfillpattern = 
{-1, -1, -1, -1, -1, -1, -1, -1};

void setfillpattern(char const* upattern, int color)
{
    static HBITMAP hFillBitmap;
    static short bitmap_data[8];
    for (int i = 0; i < 8; i++) { 
	bitmap_data[i] = (unsigned char)~upattern[i];
	userfillpattern[i] = upattern[i];
    }
    HBITMAP h = CreateBitmap(8, 8, 1, 1, bitmap_data);
    HBRUSH hb = CreatePatternBrush(h);
    DeleteObject(hBrush[USER_FILL]);
    if (hFillBitmap) { 
	DeleteObject(hFillBitmap);
    }
    hFillBitmap = h;
    hBrush[USER_FILL] = hb;
    SelectObject(hdc[0], hb);
    SelectObject(hdc[1], hb);
    fill_settings.color = color & MAXCOLORS;
    fill_settings.pattern = USER_FILL;
}

void getfillpattern(fillpatterntype fp)
{
    memcpy(fp, userfillpattern, sizeof userfillpattern);
}


inline void select_fill_color()
{
    if (text_color != fill_settings.color) { 
	text_color = fill_settings.color;
	SetTextColor(hdc[0], PALETTEINDEX(text_color+BG));
	SetTextColor(hdc[1], PALETTEINDEX(text_color+BG));
    }
}

void setusercharsize(int multx, int divx, int multy, int divy)
{
    font_mul_x = multx;
    font_div_x = divx;
    font_mul_y = multy;
    font_div_y = divy;
    text_settings.charsize = 0;
}

void moveto(int x, int y)
{
    if (bgiemu_handle_redraw || visual_page != active_page) { 
	MoveToEx(hdc[1], x, y, NULL);
    }
    if (visual_page == active_page) { 
	MoveToEx(hdc[0], x, y, NULL);
    } 
}

void moverel(int dx, int dy)
{
    POINT pos;
    GetCurrentPositionEx(hdc[1], &pos);
    moveto(pos.x + dx, pos.y + dy);
}

static void select_font()
{
    if (text_settings.charsize == 0) { 
	fcache.select(text_settings.font, text_settings.direction, 
		      font_metrics[text_settings.font]
		                  [normal_font_size[text_settings.font]].width
		                  *font_mul_x/font_div_x,
		      font_metrics[text_settings.font]
		                  [normal_font_size[text_settings.font]].height
		                  *font_mul_y/font_div_y); 
    } else { 
	fcache.select(text_settings.font, text_settings.direction, 
	    font_metrics[text_settings.font][text_settings.charsize].width,
	    font_metrics[text_settings.font][text_settings.charsize].height);
    }
}

static void text_output(int x, int y, const char* str)
{ 
    select_font();
    if (text_color != color) { 
	text_color = color;
	SetTextColor(hdc[0], PALETTEINDEX(text_color+BG));
	SetTextColor(hdc[1], PALETTEINDEX(text_color+BG));
    }
    if (bgiemu_handle_redraw || visual_page != active_page) { 
        TextOut(hdc[1], x, y, str, strlen(str));
    }
    if (visual_page == active_page) { 
        TextOut(hdc[0], x, y, str, strlen(str));
    } 
}


void settextstyle(int font, int direction, int char_size)
{
    if (char_size > 10) { 
	char_size = 10;
    }
    text_settings.direction = direction;
    text_settings.font = font;
    text_settings.charsize = char_size;
    text_align_mode = ALIGN_NOT_SET;
}

void settextjustify(int horiz, int vert)
{
    text_settings.horiz = horiz;
    text_settings.vert = vert;
    text_align_mode = ALIGN_NOT_SET;
}

void gettextsettings(textsettingstype* ts)
{
    *ts = text_settings;
}


int textheight(const char* str)
{
    SIZE ss;
    select_font();
    GetTextExtentPoint32(hdc[0], str, strlen(str), &ss);
    return ss.cy;
}

int textwidth(const char* str)
{
    SIZE ss;
    select_font();
    GetTextExtentPoint32(hdc[0], str, strlen(str), &ss);
    return ss.cx;
}

void outtext(const char* str)
{
    if (text_align_mode != UPDATE_CP) {
	text_align_mode = UPDATE_CP;
	int align = (text_settings.direction == HORIZ_DIR)
	            ? (TA_UPDATECP | 
		       text_halign_cnv[text_settings.horiz] | 
		       text_valign_cnv[text_settings.vert])
	            : (TA_UPDATECP |
		       text_valign_cnv[text_settings.horiz] | 
		       text_halign_cnv[text_settings.vert]);
	SetTextAlign(hdc[0], align); 
	SetTextAlign(hdc[1], align);
    }
    text_output(0, 0, str);
}

void outtextxy(int x, int y, const char* str)
{
    if (text_align_mode != NOT_UPDATE_CP) {
	text_align_mode = NOT_UPDATE_CP;
	int align = (text_settings.direction == HORIZ_DIR)
	            ? (TA_NOUPDATECP | 
		       text_halign_cnv[text_settings.horiz] | 
		       text_valign_cnv[text_settings.vert])
	            : (TA_NOUPDATECP |
		       text_valign_cnv[text_settings.horiz] | 
		       text_halign_cnv[text_settings.vert]);
	SetTextAlign(hdc[0], align);
	SetTextAlign(hdc[1], align);
    }
    text_output(x, y, str);
}

void setviewport(int x1, int y1, int x2, int y2, int clip)
{
    view_settings.left = x1;
    view_settings.top = y1;
    view_settings.right = x2;
    view_settings.bottom = y2;
    view_settings.clip = clip;

    if (hRgn) { 
	DeleteObject(hRgn);
    }
    hRgn = clip ? CreateRectRgn(x1, y1, x2, y2) : NULL;
    SelectClipRgn(hdc[1], hRgn);
    SetViewportOrgEx(hdc[1], x1, y1, NULL);

    SelectClipRgn(hdc[0], hRgn);
    SetViewportOrgEx(hdc[0], x1, y1, NULL);
    
    moveto(0,0);
}

void getviewsettings(viewporttype *viewport)
{
     *viewport = view_settings;
}

const double pi = 3.14159265358979323846;

inline void arc_coords(double angle, double rx, double ry, int& x, int& y)
{ 
    if (rx == 0 || ry == 0) { 
	x = y = 0;
	return;
    }
    double s = sin(angle*pi/180.0);
    double c = cos(angle*pi/180.0);
    if (fabs(s) < fabs(c)) { 
	double tg = s/c;
	double xr = sqrt((double)rx*rx*ry*ry/(ry*ry+rx*rx*tg*tg));
	x = int((c >= 0) ? xr : -xr);
	y = int((s >= 0) ? -xr*tg : xr*tg);
    } else { 
	double ctg = c/s;
	double yr = sqrt((double)rx*rx*ry*ry/(rx*rx+ry*ry*ctg*ctg));
        x = int((c >= 0) ? yr*ctg : -yr*ctg);
	y = int((s >= 0) ? -yr : yr);
    }
}

void ellipse(int x, int y, int start_angle, int end_angle, 
		       int rx, int ry)
{
    ac.x = x;
    ac.y = y;
    arc_coords(start_angle, rx, ry, ac.xstart, ac.ystart);
    arc_coords(end_angle,  rx, ry, ac.xend, ac.yend);
    ac.xstart += x; ac.ystart += y;
    ac.xend += x; ac.yend += y;

    pcache.select(color+BG); 
    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Arc(hdc[1], x-rx, y-ry, x+rx, y+ry, 
	    ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
    if (visual_page == active_page) { 
	Arc(hdc[0], x-rx, y-ry, x+rx, y+ry, 
	    ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
}

void fillellipse(int x, int y, int rx, int ry)
{
    pcache.select(color+BG); 
    select_fill_color();
    if (bgiemu_handle_redraw || visual_page != active_page) { 
	Ellipse(hdc[1], x-rx, y-ry, x+rx, y+ry); 
    }
    if (visual_page == active_page) { 
	Ellipse(hdc[0], x-rx, y-ry, x+rx, y+ry); 
    }
}

static void allocate_new_graphic_page(int page)
{
    RECT scr;
    scr.left = -view_settings.left;
    scr.top = -view_settings.top; 
    scr.right = screen_width-view_settings.left-1;
    scr.bottom = screen_height-view_settings.top-1;
    hBitmap[page] = CreateCompatibleBitmap(hdc[0],screen_width,screen_height);
    SelectObject(hdc[1], hBitmap[page]);	    
    SelectClipRgn(hdc[1], NULL);
    FillRect(hdc[1], &scr, hBackgroundBrush);
    SelectClipRgn(hdc[1], hRgn);
}

void setactivepage(int page)
{
    if (hBitmap[page] == NULL) { 
	allocate_new_graphic_page(page);
    } else { 
	SelectObject(hdc[1], hBitmap[page]);	    
    }
    if (!bgiemu_handle_redraw && active_page == visual_page) {
	POINT pos;
	GetCurrentPositionEx(hdc[0], &pos);
	MoveToEx(hdc[1], pos.x, pos.y, NULL);
    }
    active_page = page;
}


void setvisualpage(int page)
{
    POINT pos;
    if (hdc[page] == NULL) { 
	allocate_new_graphic_page(page);
    }
    if (!bgiemu_handle_redraw && active_page == visual_page) { 
	SelectObject(hdc[1], hBitmap[visual_page]);	    
	SelectClipRgn(hdc[1], NULL);
        BitBlt(hdc[1], -view_settings.left, -view_settings.top, 
	       window_width, window_height, 
	       hdc[0], -view_settings.left, -view_settings.top, 
	       SRCCOPY);
	SelectClipRgn(hdc[1], hRgn);
	GetCurrentPositionEx(hdc[0], &pos);
	MoveToEx(hdc[1], pos.x, pos.y, NULL);
    }
    SelectClipRgn(hdc[0], NULL);
    SelectClipRgn(hdc[1], NULL);
    SelectObject(hdc[1], hBitmap[page]);	    
    BitBlt(hdc[0], -view_settings.left, 
	   -view_settings.top, window_width, window_height, 
	   hdc[1], -view_settings.left, -view_settings.top, SRCCOPY);
    SelectClipRgn(hdc[0], hRgn);
    SelectClipRgn(hdc[1], hRgn);

    if (page != active_page) { 
	SelectObject(hdc[1], hBitmap[active_page]);	    
    }
    if (active_page != visual_page) { 
	GetCurrentPositionEx(hdc[1], &pos);
	MoveToEx(hdc[0], pos.x, pos.y, NULL);
    }
    visual_page = page;
}


void setaspectratio(int ax, int ay)
{
    aspect_ratio_x = ax;
    aspect_ratio_y = ay;
}

void getaspectratio(int* ax, int* ay)
{
    *ax = aspect_ratio_x;
    *ay = aspect_ratio_y;
}

void circle(int x, int y, int radius)
{
    pcache.select(color+BG); 
    int ry = (unsigned)radius*aspect_ratio_x/aspect_ratio_y;
    int rx = radius;
    if (bgiemu_handle_redraw || visual_page != active_page) { 
	Arc(hdc[1], x-rx, y-ry, x+rx, y+ry, x+rx, y, x+rx, y);
    }
    if (visual_page == active_page) { 
        Arc(hdc[0], x-rx, y-ry, x+rx, y+ry, x+rx, y, x+rx, y);
    }    
}

void arc(int x, int y, int start_angle, int end_angle, int radius)
{
    ac.x = x;
    ac.y = y;
    ac.xstart = x + int(radius*cos(start_angle*pi/180.0));
    ac.ystart = y - int(radius*sin(start_angle*pi/180.0));
    ac.xend = x + int(radius*cos(end_angle*pi/180.0));
    ac.yend = y - int(radius*sin(end_angle*pi/180.0));

    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Arc(hdc[1], x-radius, y-radius, x+radius, y+radius, 
	ac.xstart, ac.ystart, ac.xend, ac.yend);
    }
    if (visual_page == active_page) { 
	Arc(hdc[0], x-radius, y-radius, x+radius, y+radius, 
	    ac.xstart, ac.ystart, ac.xend, ac.yend);
    }
}

void getarccoords(arccoordstype *arccoords)
{
    *arccoords = ac;
}

void pieslice(int x, int y, int start_angle, int end_angle, 
	      int radius)
{
    pcache.select(color+BG); 
    select_fill_color();
    ac.x = x;
    ac.y = y;
    ac.xstart = x + int(radius*cos(start_angle*pi/180.0));
    ac.ystart = y - int(radius*sin(start_angle*pi/180.0));
    ac.xend = x + int(radius*cos(end_angle*pi/180.0));
    ac.yend = y - int(radius*sin(end_angle*pi/180.0));

    if (bgiemu_handle_redraw || visual_page != active_page) { 
	Pie(hdc[1], x-radius, y-radius, x+radius, y+radius, 
	    ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
    if (visual_page == active_page) { 
	Pie(hdc[0], x-radius, y-radius, x+radius, y+radius, 
    	    ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
}


void sector(int x, int y, int start_angle, int end_angle, 
		      int rx, int ry)
{
    ac.x = x;
    ac.y = y;
    arc_coords(start_angle, rx, ry, ac.xstart, ac.ystart);
    arc_coords(end_angle, rx, ry, ac.xend, ac.yend);
    ac.xstart += x; ac.ystart += y;
    ac.xend += x; ac.yend += y;

    pcache.select(color+BG); 
    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Pie(hdc[1], x-rx, y-ry, x+rx, y+ry, 
	    ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
    if (visual_page == active_page) { 
	Pie(hdc[0], x-rx, y-ry, x+rx, y+ry, 
    	    ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
}

void bar(int left, int top, int right, int bottom)
{
    RECT r;
    if (left > right) {	/* Turbo C corrects for badly ordered corners */   
	r.left = right;
	r.right = left;
    } else {
	r.left = left;
	r.right = right;
    }
    if (bottom < top) {	/* Turbo C corrects for badly ordered corners */   
	r.top = bottom;
	r.bottom = top;
    } else {
	r.top = top;
	r.bottom = bottom;
    }
    select_fill_color();
    if (bgiemu_handle_redraw || visual_page != active_page) { 
	FillRect(hdc[1], &r, hBrush[fill_settings.pattern]);
    }
    if (visual_page == active_page) { 
	FillRect(hdc[0], &r, hBrush[fill_settings.pattern]);
    }
}


void bar3d(int left, int top, int right, int bottom, int depth, int topflag)
{
    int temp;
    const double tan30 = 1.0/1.73205080756887729352;
    if (left > right) {     /* Turbo C corrects for badly ordered corners */
	temp = left;
	left = right;
	right = temp;
    }
    if (bottom < top) {
	temp = bottom;
	bottom = top;
	top = temp;
    }
    bar(left+line_settings.thickness, top+line_settings.thickness, 
	right-line_settings.thickness+1, bottom-line_settings.thickness+1);

    if (write_mode != COPY_PUT) { 
	SetROP2(hdc[0], write_mode_cnv[write_mode]);
	SetROP2(hdc[1], write_mode_cnv[write_mode]);
    } 
    pcache.select(ADJUSTED_MODE(write_mode) ? color : color + BG);
    int dy = int(depth*tan30);
    POINT p[11];
    p[0].x = right, p[0].y = bottom;
    p[1].x = right, p[1].y = top;
    p[2].x = left,  p[2].y = top;
    p[3].x = left,  p[3].y = bottom;
    p[4].x = right, p[4].y = bottom;
    p[5].x = right+depth, p[5].y = bottom-dy;
    p[6].x = right+depth, p[6].y = top-dy;
    p[7].x = right, p[7].y = top;

    if (topflag) { 
	p[8].x = right+depth, p[8].y = top-dy;
	p[9].x = left+depth, p[9].y = top-dy;
	p[10].x = left, p[10].y = top;	
    }
    if (bgiemu_handle_redraw || visual_page != active_page) { 
	Polyline(hdc[1], p, topflag ? 11 : 8);
    }
    if (visual_page == active_page) { 
	Polyline(hdc[0], p, topflag ? 11 : 8);
    }
    if (write_mode != COPY_PUT) { 
	SetROP2(hdc[0], R2_COPYPEN);
	SetROP2(hdc[1], R2_COPYPEN);
    }
}

void lineto(int x, int y)
{
    if (write_mode != COPY_PUT) { 
	SetROP2(hdc[0], write_mode_cnv[write_mode]);
	SetROP2(hdc[1], write_mode_cnv[write_mode]);
    } 
    pcache.select(ADJUSTED_MODE(write_mode) ? color : color + BG);
    if (bgiemu_handle_redraw || visual_page != active_page) { 
	LineTo(hdc[1], x, y);
    }
    if (visual_page == active_page) { 
	LineTo(hdc[0], x, y);
    }
    if (write_mode != COPY_PUT) { 
	SetROP2(hdc[0], R2_COPYPEN);
	SetROP2(hdc[1], R2_COPYPEN);
    }
}

void linerel(int dx, int dy)
{
    POINT pos;
    GetCurrentPositionEx(hdc[1], &pos);
    lineto(pos.x + dx, pos.y + dy);
}

void drawpoly(int n_points, int* points) 
{ 
    if (write_mode != COPY_PUT) { 
	SetROP2(hdc[0], write_mode_cnv[write_mode]);
	SetROP2(hdc[1], write_mode_cnv[write_mode]);
    } 
    pcache.select(ADJUSTED_MODE(write_mode) ? color : color + BG);

    if (bgiemu_handle_redraw || visual_page != active_page) { 
	Polyline(hdc[1], (POINT*)points, n_points);
    }
    if (visual_page == active_page) { 
	Polyline(hdc[0], (POINT*)points, n_points);
    }

    if (write_mode != COPY_PUT) { 
	SetROP2(hdc[0], R2_COPYPEN);
	SetROP2(hdc[1], R2_COPYPEN);
    }
}

void line(int x0, int y0, int x1, int y1)
{
    POINT line[2];
    line[0].x = x0;
    line[0].y = y0;
    line[1].x = x1;
    line[1].y = y1;
    drawpoly(2, (int*)&line);
}

void rectangle(int left, int top, int right, int bottom)
{
    POINT rect[5];
    rect[0].x = left, rect[0].y = top;
    rect[1].x = right, rect[1].y = top;
    rect[2].x = right, rect[2].y = bottom;
    rect[3].x = left, rect[3].y = bottom;
    rect[4].x = left, rect[4].y = top;
    drawpoly(5, (int*)&rect);
}   
    
void fillpoly(int n_points, int* points)
{
    pcache.select(color+BG);
    select_fill_color();
    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Polygon(hdc[1], (POINT*)points, n_points);
    }
    if (visual_page == active_page) { 
	Polygon(hdc[0], (POINT*)points, n_points);
    }
}

void floodfill(int x, int y, int border)
{
    select_fill_color();
    if (bgiemu_handle_redraw || visual_page != active_page) { 
	FloodFill(hdc[1], x, y, PALETTEINDEX(border+BG));
    }
    if (visual_page == active_page) { 
	FloodFill(hdc[0], x, y, PALETTEINDEX(border+BG));
    } 
}


    
static bool handle_input(bool wait = 0)
{
    MSG lpMsg;
    if (wait ? GetMessage(&lpMsg, NULL, 0, 0) 
	     : PeekMessage(&lpMsg, NULL, 0, 0, PM_REMOVE)) 
    {
	TranslateMessage(&lpMsg);
	DispatchMessage(&lpMsg);
	return true;
    }
    return false;
}


void delay(unsigned msec)
{
    timeout_expired = false;
    SetTimer(hWnd, TIMER_ID, msec, NULL); 
    while (!timeout_expired) handle_input(true);
}
    
// The Mouse functions    (1-Oct-2000, Matthew Weathers)
bool mouseup() {
    while (handle_input(false));
	if (bMouseUp) {
		bMouseUp=false;
		return true;
	} else {
		return false;
	}
}
bool mousedown() {
    while (handle_input(false));
	if (bMouseDown) {
		bMouseDown=false;
		return true;
	} else {
		return false;
	}
}
void clearmouse() {
	iClickedMouseX=0;
	iClickedMouseY=0;
	iCurrentMouseX=0;
	iCurrentMouseY=0;
	bMouseDown=false;
	bMouseUp=false;
}
int mouseclickx() {
	return iClickedMouseX;
}
int mouseclicky(){
	return iClickedMouseY;
}
int mousecurrentx(){
	return iCurrentMouseX;
}
int mousecurrenty(){
	return iCurrentMouseY;
}
int whichmousebutton(){
	return iWhichMouseButton;
}

int kbhit()
{
    while (handle_input(false));
    return !kbd_queue.is_empty();
}

int getch()
{
    while (kbd_queue.is_empty()) handle_input(true);
    return (unsigned char)kbd_queue.get();
}

void cleardevice()
{	    
    RECT scr;
    scr.left = -view_settings.left;
    scr.top = -view_settings.top; 
    scr.right = screen_width-view_settings.left-1;
    scr.bottom = screen_height-view_settings.top-1;

    if (bgiemu_handle_redraw || visual_page != active_page) { 
	if (hRgn != NULL) { 
	    SelectClipRgn(hdc[1], NULL);
	}
	FillRect(hdc[1], &scr, hBackgroundBrush);
	if (hRgn != NULL) { 
	    SelectClipRgn(hdc[1], hRgn);
	}
    }
    if (visual_page == active_page) { 
	if (hRgn != NULL) { 
	    SelectClipRgn(hdc[0], NULL);
	}
	FillRect(hdc[0], &scr, hBackgroundBrush);
	if (hRgn != NULL) { 
	    SelectClipRgn(hdc[0], hRgn);
	}
    }
    moveto(0,0);
}

void clearviewport()
{
    RECT scr;
    scr.left = 0;
    scr.top = 0; 
    scr.right = view_settings.right-view_settings.left;
    scr.bottom = view_settings.bottom-view_settings.top;
    if (bgiemu_handle_redraw || visual_page != active_page) { 
        FillRect(hdc[1], &scr, hBackgroundBrush);
    }
    if (visual_page == active_page) { 
	FillRect(hdc[0], &scr, hBackgroundBrush);
    }
    moveto(0,0);
}

void detectgraph(int *graphdriver, int *graphmode)
{
    *graphdriver = VGA;
    *graphmode = bgiemu_default_mode;
}

int getgraphmode()
{
    return bgiemu_default_mode;
}

void setgraphmode(int) {}

void putimage(int x, int y, void* image, int bitblt)
{
    BGIimage* bi = (BGIimage*)image;
    static int putimage_width, putimage_height;

    if (hPutimageBitmap == NULL ||
	putimage_width < bi->width || putimage_height < bi->height)
    {
	if (putimage_width < bi->width) { 
	    putimage_width = (bi->width+7) & ~7;
	}
	if (putimage_height < bi->height) { 
	    putimage_height = bi->height;
	}
	HBITMAP h = CreateCompatibleBitmap(hdc[0], putimage_width, 
					   putimage_height);
	SelectObject(hdc[2], h);
	if (hPutimageBitmap) { 
	    DeleteObject(hPutimageBitmap);
	}
	hPutimageBitmap = h;
    }
    int mask = ADJUSTED_MODE(bitblt) ? 0 : BG;
    for (int i = 0; i <= MAXCOLORS; i++) { 
	bminfo.color_table[i] = i + mask;
    }
    bminfo.hdr.biHeight = bi->height; 
    bminfo.hdr.biWidth = bi->width; 
    SetDIBits(hdc[2], hPutimageBitmap, 0, bi->height, bi->bits, 
	      (BITMAPINFO*)&bminfo, DIB_PAL_COLORS);
    if (bgiemu_handle_redraw || visual_page != active_page) { 
	BitBlt(hdc[1], x, y, bi->width, bi->height, hdc[2], 0, 0, 
	       bitblt_mode_cnv[bitblt]);
    }
    if (visual_page == active_page) { 
        BitBlt(hdc[0], x, y, bi->width, bi->height, hdc[2], 0, 0, 
	       bitblt_mode_cnv[bitblt]);
    }
}

unsigned int imagesize(int x1, int y1, int x2, int y2)
{
    return 8 + (((x2-x1+8) & ~7) >> 1)*(y2-y1+1); 
}

void getimage(int x1, int y1, int x2, int y2, void* image)
{
    BGIimage* bi = (BGIimage*)image;
    int* image_bits; 
    bi->width = x2-x1+1;
    bi->height = y2-y1+1;
    bminfo.hdr.biHeight = bi->height; 
    bminfo.hdr.biWidth = bi->width; 
    for (int i = 0; i <= MAXCOLORS; i++) { 
	bminfo.color_table[i] = i + BG;
    }
    HBITMAP hb = CreateDIBSection(hdc[3], (BITMAPINFO*)&bminfo, 
	DIB_PAL_COLORS, (void**)&image_bits, 0, 0); 
    HBITMAP hdb = (HBITMAP__*) SelectObject(hdc[3], hb);
    BitBlt(hdc[3], 0, 0, bi->width, bi->height, 
	   hdc[visual_page != active_page || bgiemu_handle_redraw ? 1 : 0], 
	   x1, y1, SRCCOPY);
    GdiFlush();
    memcpy(bi->bits, image_bits, (((bi->width+7) & ~7) >> 1)*bi->height);
    SelectObject(hdc[3], hdb);
    DeleteObject(hb);
}

int getpixel(int x, int y)
{ 
    int color;
    COLORREF rgb = GetPixel(hdc[visual_page != active_page 
			       || bgiemu_handle_redraw ? 1 : 0], x, y);

    if (rgb == CLR_INVALID) { 
	return -1;
    }
    int red = GetRValue(rgb);
    int blue = GetBValue(rgb);
    int green = GetGValue(rgb);
    for (color = 0; color <= MAXCOLORS; color++) { 
	if (BGIpalette[color].peRed == red &&
	    BGIpalette[color].peGreen == green &&
	    BGIpalette[color].peBlue == blue)
	{
	    return color;
	}
    }
    return -1;
}
    	    
void putpixel(int x, int y, int c)
{
    c &= MAXCOLORS;
    if (bgiemu_handle_redraw || visual_page != active_page) { 
	SetPixel(hdc[1], x, y, PALETTEINDEX(c+BG));
    }
    if (visual_page == active_page) { 
	SetPixel(hdc[0], x, y, PALETTEINDEX(c+BG));
    }
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, 
			 WPARAM wParam, LPARAM lParam)
{
    int i;
    static bool palette_changed = false;

    switch (messg) { 
      case WM_PAINT: 
	if (hdc[0] == 0) {
	    hdc[0] = BeginPaint(hWnd, &ps);
            SelectPalette(hdc[0], hPalette, FALSE);
	    RealizePalette(hdc[0]);
	    hdc[1] = CreateCompatibleDC(hdc[0]);
            SelectPalette(hdc[1], hPalette, FALSE);
	    hdc[2] = CreateCompatibleDC(hdc[0]);
            SelectPalette(hdc[2], hPalette, FALSE);
	    hdc[3] = CreateCompatibleDC(hdc[0]);
            SelectPalette(hdc[3], hPalette, FALSE);

	    screen_width = GetDeviceCaps(hdc[0], HORZRES);
	    screen_height = GetDeviceCaps(hdc[0], VERTRES);
	    hBitmap[active_page] = 
		CreateCompatibleBitmap(hdc[0], screen_width, screen_height);
	    SelectObject(hdc[1], hBitmap[active_page]);	    

	    SetTextColor(hdc[0], PALETTEINDEX(text_color+BG));
	    SetTextColor(hdc[1], PALETTEINDEX(text_color+BG));
	    SetBkColor(hdc[0], PALETTEINDEX(BG));
	    SetBkColor(hdc[1], PALETTEINDEX(BG));

	    SelectObject(hdc[0], hBrush[fill_settings.pattern]);
	    SelectObject(hdc[1], hBrush[fill_settings.pattern]);

	    RECT scr;
	    scr.left = -view_settings.left;
	    scr.top = -view_settings.top; 
	    scr.right = screen_width-view_settings.left-1;
	    scr.bottom = screen_height-view_settings.top-1;
	    FillRect(hdc[1], &scr, hBackgroundBrush);
	}
	if (hRgn != NULL) { 
	    SelectClipRgn(hdc[0], NULL);
	}
	if (visual_page != active_page) { 
	    SelectObject(hdc[1], hBitmap[visual_page]); 
	} 
        BitBlt(hdc[0], -view_settings.left, 
	       -view_settings.top, window_width, window_height, 
	       hdc[1], -view_settings.left, -view_settings.top, 
	       SRCCOPY);
	if (hRgn != NULL) { 
	    SelectClipRgn(hdc[0], hRgn);
	}
	if (visual_page != active_page) { 
	    SelectObject(hdc[1], hBitmap[active_page]); 
	} 
	ValidateRect(hWnd, NULL);
	break;
      case WM_SETFOCUS:
	if (palette_changed) { 
	    HPALETTE new_palette = CreatePalette(pPalette);
	    SelectPalette(hdc[0], new_palette, FALSE);
	    RealizePalette(hdc[0]);
	    SelectPalette(hdc[1], new_palette, FALSE);
	    SelectPalette(hdc[2], new_palette, FALSE);
	    SelectPalette(hdc[3], new_palette, FALSE);
	    DeleteObject(hPalette);
	    hPalette = new_palette;
	    palette_changed = false;
	}
	break;
      case WM_PALETTECHANGED: 
	RealizePalette(hdc[0]);
	UpdateColors(hdc[0]);
	palette_changed = true;
	break;
      case WM_DESTROY: 
        EndPaint(hWnd, &ps);
	hdc[0] = 0;
	DeleteObject(hdc[1]);
	DeleteObject(hdc[2]);
	DeleteObject(hdc[3]);
	if (hPutimageBitmap) { 
	    DeleteObject(hPutimageBitmap);
	    hPutimageBitmap = NULL;
	}
	for (i = 0; i < MAX_PAGES; i++) { 
	    if (hBitmap[i] != NULL) {
		DeleteObject(hBitmap[i]);
		hBitmap[i] = 0;
	    }
	}
	DeleteObject(hPalette);
	hPalette = 0;
	PostQuitMessage(0);
	break;
      case WM_SIZE: 
	window_width = LOWORD(lParam);
	window_height = HIWORD(lParam);
	break;
      case WM_TIMER:
	KillTimer(hWnd, TIMER_ID);
	timeout_expired = true;
	break;
      case WM_CHAR:
	kbd_queue.put((TCHAR) wParam);
	break;

		// Handle some mouse events, too (1-Oct-2000, Matthew Weathers, Erik Habbestad)
	  case WM_LBUTTONDOWN:
		  iClickedMouseX = LOWORD(lParam);
		  iClickedMouseY = HIWORD(lParam);
		  bMouseDown = true;
		  iWhichMouseButton = LEFT_BUTTON;
		  break;
	  case WM_LBUTTONUP:
		  iClickedMouseX = LOWORD(lParam);
		  iClickedMouseY = HIWORD(lParam);
		  bMouseUp = true;
		  iWhichMouseButton = LEFT_BUTTON;
		  break;
	  case WM_RBUTTONDOWN:
		  iClickedMouseX = LOWORD(lParam);
		  iClickedMouseY = HIWORD(lParam);
		  bMouseDown = true;
		  iWhichMouseButton = RIGHT_BUTTON;
		  break;
	  case WM_RBUTTONUP:
		  iClickedMouseX = LOWORD(lParam);
		  iClickedMouseY = HIWORD(lParam);
		  bMouseUp = true;
		  iWhichMouseButton = RIGHT_BUTTON;
		  break;
	  case WM_MOUSEMOVE:
		  iCurrentMouseX = LOWORD(lParam);
		  iCurrentMouseY = HIWORD(lParam);
		  break;

      default:
	return DefWindowProc(hWnd, messg, wParam, lParam);
    }
    return 0;
}

void closegraph()
{
    DestroyWindow(hWnd);
    while(handle_input(true));
}


static void detect_mode(int* gd, int* gm)
{
    switch (*gd) {
      case CGA:
	window_height = 200;
	switch (*gm) {
  	  case CGAC0:
	  case CGAC1:
	  case CGAC2:
	  case CGAC3:
	    window_width = 320;
	    break;
	  case CGAHI:
	    window_width = 640;
	    break;
	  default:
	    window_width = 320;
	    break;
	}
	break;
      case MCGA:
	window_height = 200;
	switch (*gm) {
	  case MCGAC0:
	  case MCGAC1:
	  case MCGAC2:
	  case MCGAC3:
	    window_width = 320;
	    break;
	  case MCGAMED:
  	    window_width = 640;
	    break;
	  case MCGAHI:
	    window_width = 640;
	    window_height = 480;
	    break;
	  default:
	    window_width = 320;
	    break;
	}
	break;
      case EGA:
	window_width = 640;
	switch (*gm) {
	  case EGALO:
	    window_height = 200;
	    break;
	  case EGAHI:
	    window_height = 350;
	    break;
	  default:
	    window_height = 350;
	    break;
	}
	break;
      case EGA64:
        window_width = 640;
	switch (*gm) {
	  case EGA64LO:
	    window_height = 200;
	    break;
	  case EGA64HI:
	    window_height = 350;
	    break;
	  default:
	    window_height = 350;
	    break;
	}
	break;
      case EGAMONO:
	window_width = 640;
	window_height = 350;
	break;
      case HERCMONO:
	window_width = 720;
	window_height = 348;
	break;
      case ATT400:
	window_height = 200;
	switch (*gm) {
	  case ATT400C0:
	  case ATT400C1:
	  case ATT400C2:
	  case ATT400C3:
	    window_width = 320;
	    break;
	  case ATT400MED:
	    window_width = 640;
	    break;
	  case ATT400HI:
	    window_width = 640;
	    window_height = 400;
	    break;
	  default:
	    window_width = 320;
	    break;
	}
	break;
      default:
      case DETECT:
	*gd = VGA;
	*gm = bgiemu_default_mode;		   
      case VGA:
        window_width = 900; //Default WIDTH
        switch (*gm) {
          case VGALO:
	    window_height = 200;
	    break;
	  case VGAMED:
	    window_height = 350;
	    break;
	  case VGAHI:
	    window_height = 480;
	    break;
	  default:
	    window_height = 640; //DEFAULT HEIGHT
	    break;
	}
	break;
      case PC3270:
	window_width = 720;
	window_height = 350;
	break;
      case IBM8514:
	switch (*gm) {
  	  case IBM8514LO:
 	    window_width = 640;
	    window_height = 480;
	    break;
	  case IBM8514HI:
	    window_width = 1024;
	    window_height = 768;
	    break;
	  default:
	    window_width = 1024;
	    window_height = 768;
	    break;
	}
	break;	
    } 
}

static void set_defaults()
{
    color = text_color = WHITE;
    bkcolor = 0;
    line_settings.thickness = 1;
    line_settings.linestyle = SOLID_LINE;
    line_settings.upattern = ~0;
    fill_settings.pattern = SOLID_FILL;
    fill_settings.color = WHITE;
    write_mode = COPY_PUT;

    text_settings.direction = HORIZ_DIR;
    text_settings.font = DEFAULT_FONT;
    text_settings.charsize = 1;
    text_settings.horiz = LEFT_TEXT;
    text_settings.vert = TOP_TEXT;
    text_align_mode = ALIGN_NOT_SET;

    active_page = visual_page = 0;
    
    view_settings.left = 0;
    view_settings.top = 0;
    view_settings.right = window_width-1;
    view_settings.bottom = window_height-1;
    
    aspect_ratio_x = aspect_ratio_y = 10000;
}

void initgraph(int* device, int* mode, char const* /*pathtodriver*/, 
			   int size_width, int size_height)
{
    int index;
    static WNDCLASS wcApp;

    gdi_error_code = grOk;

    if (wcApp.lpszClassName == NULL) { 
	wcApp.lpszClassName = "BGIlibrary";
	wcApp.hInstance = 0;
	wcApp.lpfnWndProc = WndProc;
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcApp.hIcon = 0;
	wcApp.lpszMenuName = 0;
	wcApp.hbrBackground = (HBRUSH__ *) GetStockObject(BLACK_BRUSH);
	wcApp.style = CS_SAVEBITS;
	wcApp.cbClsExtra = 0;
	wcApp.cbWndExtra = 0;
	
	if (!RegisterClass(&wcApp)) { 
	    gdi_error_code = GetLastError();
	    return;
	}
	
	pPalette = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED,
	    sizeof(LOGPALETTE)+sizeof(PALETTEENTRY)*PALETTE_SIZE);
	
	pPalette->palVersion = 0x300;
	pPalette->palNumEntries = PALETTE_SIZE;
	memset(pPalette->palPalEntry, 0, sizeof(PALETTEENTRY)*PALETTE_SIZE); 
	for (index = 0; index < BG; index++) {
	    pPalette->palPalEntry[index].peFlags = PC_EXPLICIT;
	    pPalette->palPalEntry[index].peRed = index;
	    pPalette->palPalEntry[PALETTE_SIZE-BG+index].peFlags = PC_EXPLICIT;
	    pPalette->palPalEntry[PALETTE_SIZE-BG+index].peRed = 
		PALETTE_SIZE-BG+index;
	}		
	hBackgroundBrush = CreateSolidBrush(PALETTEINDEX(BG));
	hBrush[EMPTY_FILL] = (HBRUSH__*) GetStockObject(NULL_BRUSH);
	hBrush[SOLID_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, SolidBrushBitmap));
	hBrush[LINE_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, LineBrushBitmap));
	hBrush[LTSLASH_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, LtslashBrushBitmap));
	hBrush[SLASH_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, SlashBrushBitmap));
	hBrush[BKSLASH_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, BkslashBrushBitmap));
	hBrush[LTBKSLASH_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, LtbkslashBrushBitmap));
	hBrush[HATCH_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, HatchBrushBitmap));
	hBrush[XHATCH_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, XhatchBrushBitmap));
	hBrush[INTERLEAVE_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, InterleaveBrushBitmap));
	hBrush[WIDE_DOT_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, WidedotBrushBitmap));
	hBrush[CLOSE_DOT_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, ClosedotBrushBitmap));
	hBrush[USER_FILL] = 
	    CreatePatternBrush(CreateBitmap(8, 8, 1, 1, SolidBrushBitmap));
    }
    memcpy(BGIpalette, BGIcolor, sizeof BGIpalette);
    current_palette.size = MAXCOLORS+1;
    for (index = 10; index <= MAXCOLORS; index++) {
	pPalette->palPalEntry[index] = BGIcolor[0];
    }
    for (index = 0; index <= MAXCOLORS; index++) {
	current_palette.colors[index] = index;
	pPalette->palPalEntry[index+BG] = BGIcolor[index];
    }
    hPalette = CreatePalette(pPalette);
    detect_mode(device, mode);
    set_defaults();

	if (size_width) window_width=size_width;
	if (size_height) window_height=size_height;

    hWnd = CreateWindow("BGIlibrary", "Windows BGI", 
			WS_OVERLAPPEDWINDOW,
		        0, 0, window_width+BORDER_WIDTH, 
			window_height+BORDER_HEIGHT,
			(HWND)NULL,  (HMENU)NULL,
	    		0, NULL);
    if (hWnd == NULL) { 
	gdi_error_code = GetLastError();
	return;
    }
    ShowWindow(hWnd, *mode == VGAMAX ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
    UpdateWindow(hWnd);
}


void graphdefaults()
{
    set_defaults();

   for (int i = 0; i <= MAXCOLORS; i++) { 
   	current_palette.colors[i] = i;
   	BGIpalette[i] = BGIcolor[i];
    }
    SetPaletteEntries(hPalette, BG, MAXCOLORS+1, BGIpalette);
    RealizePalette(hdc[0]);

    SetTextColor(hdc[0], PALETTEINDEX(text_color+BG));
    SetTextColor(hdc[1], PALETTEINDEX(text_color+BG));
    SetBkColor(hdc[0], PALETTEINDEX(BG));
    SetBkColor(hdc[1], PALETTEINDEX(BG));

    SelectClipRgn(hdc[0], NULL);
    SelectClipRgn(hdc[1], NULL);
    SetViewportOrgEx(hdc[0], 0, 0, NULL);
    SetViewportOrgEx(hdc[1], 0, 0, NULL);

    SelectObject(hdc[0], hBrush[fill_settings.pattern]);
    SelectObject(hdc[1], hBrush[fill_settings.pattern]);

    moveto(0,0);
}

void restorecrtmode() {}


/////////////////////////////////////////////////////////////////////////////////////////////
