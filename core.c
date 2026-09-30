/*
 * ============================================================================
 * CORE (TRON Arcade Vector Edition - 50 FPS Locked)
 * Platform:    Atari ST / Mega ST / STE (ST-Low 320x200 16-Color, 50 FPS)
 * Toolchain:   vbcc (vc +tos -O2 -o core.prg core.c)
 * Dependencies: Zero external headers (Standalone bare-metal OS/HW traps)
 * ============================================================================
 */

/* --- Hardware & OS Variables (Global for inline assembly access) --- */
long  os_arg1_l;
long  os_arg2_l;
short os_arg1_w;
short os_arg2_w;
long  os_ret_l;
short os_ret_w;

/* --- Direct 68000 Trap Interfaces --- */

void *os_physbase(void)
{
    __asm(
        "\tmove.w  #2,-(sp)\n"
        "\ttrap    #14\n"
        "\taddq.l  #2,sp\n"
        "\tmove.l  d0,_os_ret_l\n"
    );
    return (void *)os_ret_l;
}

void *os_logbase(void)
{
    __asm(
        "\tmove.w  #3,-(sp)\n"
        "\ttrap    #14\n"
        "\taddq.l  #2,sp\n"
        "\tmove.l  d0,_os_ret_l\n"
    );
    return (void *)os_ret_l;
}

short os_getrez(void)
{
    __asm(
        "\tmove.w  #4,-(sp)\n"
        "\ttrap    #14\n"
        "\taddq.l  #2,sp\n"
        "\tmove.w  d0,_os_ret_w\n"
    );
    return os_ret_w;
}

void os_setscreen(void *log, void *phys, short rez)
{
    os_arg1_l = (long)log;
    os_arg2_l = (long)phys;
    os_arg1_w = rez;
    __asm(
        "\tmove.w  _os_arg1_w,-(sp)\n"
        "\tmove.l  _os_arg2_l,-(sp)\n"
        "\tmove.l  _os_arg1_l,-(sp)\n"
        "\tmove.w  #5,-(sp)\n"
        "\ttrap    #14\n"
        "\tlea      12(sp),sp\n"
    );
}

void os_setpalette(const short *pal)
{
    os_arg1_l = (long)pal;
    __asm(
        "\tmove.l  _os_arg1_l,-(sp)\n"
        "\tmove.w  #6,-(sp)\n"
        "\ttrap    #14\n"
        "\taddq.l  #6,sp\n"
    );
}

void os_vsync(void)
{
    __asm(
        "\tmove.w  #37,-(sp)\n"
        "\ttrap    #14\n"
        "\taddq.l  #2,sp\n"
    );
}

void os_psg_write(short reg, short val)
{
    os_arg1_w = (short)(reg | 0x80);  /* register number (bit 7 set = write) */
    os_arg2_w = val;                   /* data value to write */
    __asm(
        "\tmove.w  _os_arg1_w,-(sp)\n"
        "\tmove.w  _os_arg2_w,-(sp)\n"
        "\tmove.w  #28,-(sp)\n"
        "\ttrap    #14\n"
        "\taddq.l  #6,sp\n"
    );
}

short os_key_check(void)
{
    __asm(
        "\tmove.w  #11,-(sp)\n"
        "\ttrap    #1\n"
        "\taddq.l  #2,sp\n"
        "\tmove.w  d0,_os_ret_w\n"
    );
    return os_ret_w;
}

long os_key_read(void)
{
    __asm(
        "\tmove.w  #8,-(sp)\n"
        "\ttrap    #1\n"
        "\taddq.l  #2,sp\n"
        "\tmove.l  d0,_os_ret_l\n"
    );
    return os_ret_l;
}

long os_super(long stack)
{
    os_arg1_l = stack;
    __asm(
        "\tmove.l  _os_arg1_l,-(sp)\n"
        "\tmove.w  #32,-(sp)\n"
        "\ttrap    #1\n"
        "\taddq.l  #6,sp\n"
        "\tmove.l  d0,_os_ret_l\n"
    );
    return os_ret_l;
}

/* --- Bare-Metal GEMDOS File I/O Traps --- */
short os_fopen(const char *name, short mode)
{
    os_arg1_l = (long)name;
    os_arg1_w = mode;
    __asm(
        "\tmove.w  _os_arg1_w,-(sp)\n"
        "\tmove.l  _os_arg1_l,-(sp)\n"
        "\tmove.w  #$3D,-(sp)\n"
        "\ttrap    #1\n"
        "\taddq.l  #8,sp\n"
        "\tmove.w  d0,_os_ret_w\n"
    );
    return os_ret_w;
}

short os_fcreate(const char *name, short attr)
{
    os_arg1_l = (long)name;
    os_arg1_w = attr;
    __asm(
        "\tmove.w  _os_arg1_w,-(sp)\n"
        "\tmove.l  _os_arg1_l,-(sp)\n"
        "\tmove.w  #$3C,-(sp)\n"
        "\ttrap    #1\n"
        "\taddq.l  #8,sp\n"
        "\tmove.w  d0,_os_ret_w\n"
    );
    return os_ret_w;
}

short os_setdrv(short drv)
{
    os_arg1_w = drv;
    __asm(
        "\tmove.w  _os_arg1_w,-(sp)\n"
        "\tmove.w  #$0E,-(sp)\n"
        "\ttrap    #1\n"
        "\taddq.l  #4,sp\n"
        "\tmove.w  d0,_os_ret_w\n"
    );
    return os_ret_w;
}

short os_getdrv(void)
{
    __asm(
        "\tmove.w  #$19,-(sp)\n"
        "\ttrap    #1\n"
        "\taddq.l  #2,sp\n"
        "\tmove.w  d0,_os_ret_w\n"
    );
    return os_ret_w;
}

long os_fread(short handle, long count, void *buf)
{
    os_arg1_w = handle;
    os_arg1_l = count;
    os_arg2_l = (long)buf;
    __asm(
        "\tmove.l  _os_arg2_l,-(sp)\n"
        "\tmove.l  _os_arg1_l,-(sp)\n"
        "\tmove.w  _os_arg1_w,-(sp)\n"
        "\tmove.w  #$3F,-(sp)\n"
        "\ttrap    #1\n"
        "\tlea      12(sp),sp\n"
        "\tmove.l  d0,_os_ret_l\n"
    );
    return os_ret_l;
}

long os_fwrite(short handle, long count, const void *buf)
{
    os_arg1_w = handle;
    os_arg1_l = count;
    os_arg2_l = (long)buf;
    __asm(
        "\tmove.l  _os_arg2_l,-(sp)\n"
        "\tmove.l  _os_arg1_l,-(sp)\n"
        "\tmove.w  _os_arg1_w,-(sp)\n"
        "\tmove.w  #$40,-(sp)\n"
        "\ttrap    #1\n"
        "\tlea      12(sp),sp\n"
        "\tmove.l  d0,_os_ret_l\n"
    );
    return os_ret_l;
}

short os_fclose(short handle)
{
    os_arg1_w = handle;
    __asm(
        "\tmove.w  _os_arg1_w,-(sp)\n"
        "\tmove.w  #$3E,-(sp)\n"
        "\ttrap    #1\n"
        "\taddq.l  #4,sp\n"
        "\tmove.w  d0,_os_ret_w\n"
    );
    return os_ret_w;
}

/* --- Line-A Mouse Cursor Management (Devpac $ Syntax) --- */
void os_hide_mouse(void)
{
    __asm(
        "\tmovem.l d0-d2/a0-a2,-(sp)\n"
        "\tdc.w    $a00a\n"
        "\tmovem.l (sp)+,d0-d2/a0-a2\n"
    );
}

void os_show_mouse(void)
{
    __asm(
        "\tmovem.l d0-d2/a0-a2,-(sp)\n"
        "\tdc.w    $a009\n"
        "\tmovem.l (sp)+,d0-d2/a0-a2\n"
    );
}

/* --- Blitter Detection & Hardware Driver --- */
short has_blitter = 0;

short detect_blitter(void)
{
    long *cookie_jar = *(long **)0x5A0L;
    if (cookie_jar) {
        while (*cookie_jar) {
            if (*cookie_jar == 0x5F424C54L) { /* '_BLT' cookie */
                return (short)((cookie_jar[1] & 1) ? 1 : 0);
            }
            cookie_jar += 2;
        }
    }
    return 0;
}

void blitter_clear(void *dst)
{
    *(volatile unsigned short *)0xFFFF8A28UL = 0xFFFF; /* Endmask 1 */
    *(volatile unsigned short *)0xFFFF8A2AUL = 0xFFFF; /* Endmask 2 */
    *(volatile unsigned short *)0xFFFF8A2CUL = 0xFFFF; /* Endmask 3 */
    *(volatile short *)0xFFFF8A2EUL          = 2;      /* Dst X Inc */
    *(volatile short *)0xFFFF8A30UL          = 2;      /* Dst Y Inc */
    *(volatile unsigned long *)0xFFFF8A32UL  = (unsigned long)dst;
    *(volatile unsigned short *)0xFFFF8A36UL = 80;     /* 80 words = 160 bytes/line */
    *(volatile unsigned short *)0xFFFF8A38UL = 200;    /* 200 scanlines */
    *(volatile unsigned char *)0xFFFF8A3AUL  = 0;      /* Halftone OP: all 1s */
    *(volatile unsigned char *)0xFFFF8A3BUL  = 0;      /* Logic OP: Clear to 0 */
    *(volatile unsigned char *)0xFFFF8A3CUL  = 0;      /* Line Number / Smudge */
    *(volatile unsigned char *)0xFFFF8A3DUL  = 0xC0;   /* Control: HOG mode | BUSY */
}

/* --- Optimized 68000 CPU Screen Clear (Burst MOVEM.L) --- */
void fast_clear(void *dst)
{
    os_arg1_l = (long)dst + 32000L;
    __asm(
        "\tmove.l  _os_arg1_l,a0\n"
        "\tmovem.l d2-d7/a2-a6,-(sp)\n"
        "\tmoveq   #0,d1\n"
        "\tmoveq   #0,d2\n"
        "\tmoveq   #0,d3\n"
        "\tmoveq   #0,d4\n"
        "\tmoveq   #0,d5\n"
        "\tmoveq   #0,d6\n"
        "\tmoveq   #0,d7\n"
        "\tsub.l   a1,a1\n"
        "\tsub.l   a2,a2\n"
        "\tsub.l   a3,a3\n"
        "\tsub.l   a4,a4\n"
        "\tsub.l   a5,a5\n"
        "\tsub.l   a6,a6\n"
        "\tmove.w  #499,d0\n"
    "fast_clr_lp:\n"
        "\tmovem.l d1-d7/a1-a6,-(a0)\n"
        "\tmovem.l d1-d3,-(a0)\n"
        "\tdbra    d0,fast_clr_lp\n"
        "\tmovem.l (sp)+,d2-d7/a2-a6\n"
    );
}

void clear_screen(void *dst)
{
    if (has_blitter) {
        blitter_clear(dst);
    } else {
        fast_clear(dst);
    }
}

/* --- PRNG --- */
unsigned short rng_seed = 0x9B13;
short st_rand(void)
{
    rng_seed = (unsigned short)(rng_seed * 31421 + 6927);
    return (short)(rng_seed & 0x7FFF);
}

/*
 * --- Authentic TRON Arcade Master Palette (0x0RGB Format) ---
 */
const short base_neon_palette[16] = {
    0x000, /* 0:  Void Black (Grid Floor) */
    0x013, /* 1:  Deep Cobalt Blue (Inactive Outer Vector Ring) */
    0x025, /* 2:  Electric Neon Blue */
    0x147, /* 3:  Bright Cyan-Blue Primary Track */
    0x267, /* 4:  Intense High-Luminance Ring Highlight */
    0x750, /* 5:  High-Voltage Ion Amber */
    0x770, /* 6:  Vivid Pure Ion Yellow (Jump Open / Intercept Lock) */
    0x777, /* 7:  Laser White (Filament Core / Flash) */
    0x075, /* 8:  MCP Cyan Vector / Bit Glow */
    0x037, /* 9:  Phosphor Blue Shadow */
    0x700, /* 10: Sark Red / Warning Crimson */
    0x400, /* 11: Dark Threat Crimson */
    0x705, /* 12: TRON Logo Horizon Magenta/Pink */
    0x103, /* 13: Deep Indigo / Shadow Blue */
    0x015, /* 14: Sub-Pixel Ambient Glow */
    0x577  /* 15: Supercharged White-Cyan Bloom */
};

short live_palette[16];

const short default_tos_palette[16] = {
    0x777, 0x700, 0x070, 0x770, 0x007, 0x707, 0x077, 0x555,
    0x333, 0x733, 0x373, 0x773, 0x337, 0x737, 0x377, 0x000
};

/* --- Fast Sine Table (256 units = 360 deg) --- */
const short qsin[65] = {
      0,   6,  13,  19,  25,  31,  38,  44,  50,  56,  62,  68,  74,  80,  86,  92,
     98, 104, 109, 115, 121, 126, 132, 137, 142, 147, 152, 157, 162, 167, 172, 177,
    181, 186, 190, 194, 198, 202, 206, 210, 213, 217, 220, 223, 226, 229, 231, 234,
    236, 238, 240, 242, 244, 245, 247, 248, 249, 250, 251, 252, 253, 254, 255, 255,
    256
};

short f_sin(unsigned char a)
{
    if (a <= 64)  return qsin[a];
    if (a <= 128) return qsin[128 - a];
    if (a <= 192) return -qsin[a - 128];
    return -qsin[256 - a];
}

short f_cos(unsigned char a)
{
    return f_sin((unsigned char)(a + 64));
}

/* --- Geometry Definitions & Scanline Tables --- */
#define NUM_RINGS 4
#define CENTER_X  160
#define CENTER_Y  108

const short ring_rx[NUM_RINGS] = { 36, 62, 90, 120 };
const short ring_ry[NUM_RINGS] = { 22, 37, 54,  72 };

#define SEG_COUNT 20
const unsigned char seg_angles[21] = {
      0,  13,  26,  38,  51,  64,  77,  90, 102, 115,
    128, 141, 154, 166, 179, 192, 205, 218, 230, 243, 255
};

typedef struct {
    short x0, y0;
    short x1, y1;
} SegmentCoords;

SegmentCoords ring_geom[NUM_RINGS][SEG_COUNT];

short y_table[200];
const unsigned short pmask[16] = {
    0x8000, 0x4000, 0x2000, 0x1000,
    0x0800, 0x0400, 0x0200, 0x0100,
    0x0080, 0x0040, 0x0020, 0x0010,
    0x0008, 0x0004, 0x0002, 0x0001
};

void init_tables(void)
{
    short y, r, i;
    for (y = 0; y < 200; y++) {
        y_table[y] = y * 160;
    }

    for (r = 0; r < NUM_RINGS; r++) {
        for (i = 0; i < SEG_COUNT; i++) {
            unsigned char a0 = seg_angles[i];
            unsigned char a1 = seg_angles[i + 1];
            ring_geom[r][i].x0 = CENTER_X + ((ring_rx[r] * f_cos(a0)) >> 8);
            ring_geom[r][i].y0 = CENTER_Y + ((ring_ry[r] * f_sin(a0)) >> 8);
            ring_geom[r][i].x1 = CENTER_X + ((ring_rx[r] * f_cos(a1)) >> 8);
            ring_geom[r][i].y1 = CENTER_Y + ((ring_ry[r] * f_sin(a1)) >> 8);
        }
    }
}

/* --- Hardware Palette Modulation --- */
unsigned char glow_tick = 0;
void update_neon_glow(void)
{
    short i;
    short cycle;

    for (i = 0; i < 16; i++) {
        live_palette[i] = base_neon_palette[i];
    }

    glow_tick += 6;
    cycle = (glow_tick >> 4) & 7;

    if (cycle == 0 || cycle == 6) live_palette[4] = 0x157;
    else if (cycle == 1 || cycle == 5) live_palette[4] = 0x267;
    else if (cycle == 2 || cycle == 4) live_palette[4] = 0x377;
    else if (cycle == 3) live_palette[4] = 0x577;

    if (glow_tick & 16) live_palette[6] = 0x770;
    else                live_palette[6] = 0x772;

    os_setpalette(live_palette);
}

/* --- Optimized Inlined Pixel Plotter --- */
#define PLOT_PIXEL(screen, px, py, col) do { \
    if ((unsigned short)(px) < 320 && (unsigned short)(py) < 200) { \
        unsigned short *pl = (unsigned short *)((screen) + y_table[(py)] + (((px) >> 4) << 3)); \
        unsigned short mk = pmask[(px) & 15]; \
        if ((col) & 1) pl[0] |= mk; \
        if ((col) & 2) pl[1] |= mk; \
        if ((col) & 4) pl[2] |= mk; \
        if ((col) & 8) pl[3] |= mk; \
    } \
} while (0)

/* --- Incremental Pointer-Stepped Bresenham Line Engine --- */
void draw_line_fast(char *screen, short x0, short y0, short x1, short y1, short color)
{
    short dx = x1 - x0;
    short dy = y1 - y0;
    short sx, sy, err, e2;
    unsigned short *pl;
    unsigned short mk;
    short c0, c1, c2, c3;

    if ((unsigned short)x0 >= 320 || (unsigned short)y0 >= 200 ||
        (unsigned short)x1 >= 320 || (unsigned short)y1 >= 200) return;

    c0 = color & 1;
    c1 = color & 2;
    c2 = color & 4;
    c3 = color & 8;

    pl = (unsigned short *)(screen + y_table[y0] + ((x0 >> 4) << 3));
    mk = pmask[x0 & 15];

    if (dx >= 0) { sx = 1; } else { sx = -1; dx = -dx; }
    if (dy >= 0) { sy = 1; } else { sy = -1; dy = -dy; }
    err = dx - dy;

    for (;;) {
        if (c0) pl[0] |= mk;
        if (c1) pl[1] |= mk;
        if (c2) pl[2] |= mk;
        if (c3) pl[3] |= mk;

        if (x0 == x1 && y0 == y1) break;
        e2 = err << 1;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
            if (sx > 0) {
                mk >>= 1;
                if (!mk) { mk = 0x8000; pl += 4; }
            } else {
                mk <<= 1;
                if (!mk) { mk = 0x0001; pl -= 4; }
            }
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
            if (sy > 0) { pl += 80; } else { pl -= 80; }
        }
    }
}

/* --- TRON Arcade Styled "CORE" Logo --- */
void draw_core_span(char *screen, short x1, short x2, short y, short dy)
{
    short x, fill_color;

    if (x1 > x2) { short t = x1; x1 = x2; x2 = t; }

    if (dy == 0 || dy == 29) {
        for (x = x1; x <= x2; x++) PLOT_PIXEL(screen, x, y, 10);
        return;
    }

    if (dy <= 12)       fill_color = 2;
    else if (dy == 13)  fill_color = 3;
    else if (dy == 14)  fill_color = 15;
    else if (dy == 15)  fill_color = 7;
    else if (dy == 16)  fill_color = 12;
    else                fill_color = 1;

    PLOT_PIXEL(screen, x1, y, 10);
    PLOT_PIXEL(screen, x2, y, 10);

    for (x = x1 + 1; x < x2; x++) {
        PLOT_PIXEL(screen, x, y, fill_color);
    }
}

void draw_tron_core_logo(char *screen, short sx, short sy)
{
    short dy;

    for (dy = 0; dy < 30; dy++) {
        short y = sy + dy;

        /* 'C' */
        if (dy == 0)                           draw_core_span(screen, sx + 4, sx + 29, y, dy);
        else if (dy <= 3)                       draw_core_span(screen, sx + (4 - dy), sx + 29, y, dy);
        else if (dy <= 6)                       draw_core_span(screen, sx + 0, sx + 29, y, dy);
        else if (dy <= 22)                      draw_core_span(screen, sx + 0, sx + 7,  y, dy);
        else if (dy <= 25)                      draw_core_span(screen, sx + 0, sx + 29, y, dy);
        else if (dy <= 28)                      draw_core_span(screen, sx + (dy - 25), sx + 29, y, dy);
        else                                    draw_core_span(screen, sx + 4, sx + 29, y, dy);

        /* 'O' */
        {
            short ox = sx + 38;
            if (dy == 0)                       draw_core_span(screen, ox + 4, ox + 25, y, dy);
            else if (dy <= 3)                   draw_core_span(screen, ox + (4 - dy), ox + (25 + dy), y, dy);
            else if (dy <= 6)                   draw_core_span(screen, ox + 0, ox + 29, y, dy);
            else if (dy <= 22) {
                draw_core_span(screen, ox + 0,  ox + 7,  y, dy);
                draw_core_span(screen, ox + 22, ox + 29, y, dy);
            }
            else if (dy <= 25)                  draw_core_span(screen, ox + 0, ox + 29, y, dy);
            else if (dy <= 28)                  draw_core_span(screen, ox + (dy - 25), ox + (29 - (dy - 25)), y, dy);
            else                                draw_core_span(screen, ox + 4, ox + 25, y, dy);
        }

        /* 'R' */
        {
            short rx = sx + 76;
            if (dy == 0)                       draw_core_span(screen, rx + 0, rx + 25, y, dy);
            else if (dy <= 3)                   draw_core_span(screen, rx + 0, rx + (25 + dy), y, dy);
            else if (dy <= 6)                   draw_core_span(screen, rx + 0, rx + 29, y, dy);
            else if (dy <= 12) {
                draw_core_span(screen, rx + 0,  rx + 7,  y, dy);
                draw_core_span(screen, rx + 22, rx + 29, y, dy);
            }
            else if (dy <= 17)                  draw_core_span(screen, rx + 0, rx + 29, y, dy);
            else {
                short leg_x = rx + 14 + (dy - 17);
                if (leg_x > rx + 23) leg_x = rx + 23;
                draw_core_span(screen, rx + 0, rx + 7, y, dy);
                draw_core_span(screen, leg_x, rx + 29, y, dy);
            }
        }

        /* 'E' */
        {
            short ex = sx + 114;
            if (dy == 0)                       draw_core_span(screen, ex + 0, ex + 25, y, dy);
            else if (dy <= 3)                   draw_core_span(screen, ex + 0, ex + (25 + dy), y, dy);
            else if (dy <= 6)                   draw_core_span(screen, ex + 0, ex + 29, y, dy);
            else if (dy <= 11)                  draw_core_span(screen, ex + 0, ex + 7,  y, dy);
            else if (dy <= 17)                  draw_core_span(screen, ex + 0, ex + 24, y, dy);
            else if (dy <= 22)                  draw_core_span(screen, ex + 0, ex + 7,  y, dy);
            else if (dy <= 25)                  draw_core_span(screen, ex + 0, ex + 29, y, dy);
            else if (dy <= 28)                  draw_core_span(screen, ex + 0, ex + (29 - (dy - 25)), y, dy);
            else                                draw_core_span(screen, ex + 0, ex + 25, y, dy);
        }
    }

    draw_line_fast(screen, sx - 10, sy + 34, sx + 154, sy + 34, 10);
    draw_line_fast(screen, sx - 6,  sy + 36, sx + 150, sy + 36, 3);
}

/* --- Optimized Row-Stepped Font Renderer --- */
const unsigned char font5x7[59][5] = {
    {0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x5F,0x00,0x00}, {0x00,0x07,0x00,0x07,0x00},
    {0x14,0x7F,0x14,0x7F,0x14}, {0x24,0x2A,0x7F,0x2A,0x12}, {0x23,0x13,0x08,0x64,0x62},
    {0x36,0x49,0x55,0x22,0x50}, {0x00,0x05,0x03,0x00,0x00}, {0x00,0x1C,0x22,0x41,0x00},
    {0x00,0x41,0x22,0x1C,0x00}, {0x08,0x2A,0x1C,0x2A,0x08}, {0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00}, {0x08,0x08,0x08,0x08,0x08}, {0x00,0x60,0x60,0x00,0x00},
    {0x20,0x10,0x08,0x04,0x02}, {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31}, {0x18,0x14,0x12,0x7F,0x10},
    {0x27,0x45,0x45,0x45,0x39}, {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E}, {0x00,0x36,0x36,0x00,0x00},
    {0x00,0x56,0x36,0x00,0x00}, {0x08,0x14,0x22,0x41,0x00}, {0x14,0x14,0x14,0x14,0x14},
    {0x00,0x41,0x22,0x14,0x08}, {0x02,0x01,0x51,0x09,0x06}, {0x32,0x49,0x79,0x41,0x3E},
    {0x7E,0x11,0x11,0x11,0x7E}, {0x7F,0x49,0x49,0x49,0x36}, {0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C}, {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01},
    {0x3E,0x41,0x49,0x49,0x7A}, {0x7F,0x08,0x08,0x08,0x7F}, {0x00,0x41,0x7F,0x41,0x00},
    {0x20,0x40,0x41,0x3F,0x01}, {0x7F,0x08,0x14,0x22,0x41}, {0x7F,0x40,0x40,0x40,0x40},
    {0x7F,0x02,0x0C,0x02,0x7F}, {0x7F,0x04,0x08,0x10,0x7F}, {0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06}, {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46},
    {0x46,0x49,0x49,0x49,0x31}, {0x01,0x01,0x7F,0x01,0x01}, {0x3F,0x40,0x40,0x40,0x3F},
    {0x1F,0x20,0x40,0x20,0x1F}, {0x7F,0x20,0x18,0x20,0x7F}, {0x63,0x14,0x08,0x14,0x63},
    {0x07,0x08,0x70,0x08,0x07}, {0x61,0x51,0x49,0x45,0x43}
};

void draw_char(char *screen, short x, short y, char c, short color)
{
    short col, row;
    const unsigned char *glyph;
    short c0, c1, c2, c3;

    if (c >= 'a' && c <= 'z') c -= 32;
    if (c < 32 || c > 90) return;

    glyph = font5x7[c - 32];
    c0 = color & 1; c1 = color & 2; c2 = color & 4; c3 = color & 8;

    for (row = 0; row < 7; row++) {
        short py = y + row;
        if ((unsigned short)py < 200) {
            short row_bit = 1 << row;
            for (col = 0; col < 5; col++) {
                if (glyph[col] & row_bit) {
                    short px = x + col;
                    if ((unsigned short)px < 320) {
                        unsigned short *pl = (unsigned short *)(screen + y_table[py] + ((px >> 4) << 3));
                        unsigned short mk = pmask[px & 15];
                        if (c0) pl[0] |= mk;
                        if (c1) pl[1] |= mk;
                        if (c2) pl[2] |= mk;
                        if (c3) pl[3] |= mk;
                    }
                }
            }
        }
    }
}

void draw_text(char *screen, short x, short y, const char *str, short color)
{
    while (*str) {
        draw_char(screen, x, y, *str, color);
        x += 6;
        str++;
    }
}

void draw_number(char *screen, short x, short y, long val, short digits, short color)
{
    char buf[10];
    short i = digits;
    buf[i] = '\0';
    while (i > 0) {
        i--;
        buf[i] = (char)('0' + (val % 10));
        val /= 10;
    }
    draw_text(screen, x, y, buf, color);
}

/* --- YM2149 Chiptune Music & Audio Engine --- */
static const unsigned short note_periods[73] = {
       0, 3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551, 2408, 2273, 2145,
    2025, 1911, 1804, 1703, 1607, 1517, 1432, 1351, 1276, 1204, 1136, 1073,
    1012,  956,  902,  851,  804,  758,  716,  676,  638,  602,  568,  536,
     506,  478,  451,  426,  402,  379,  358,  338,  319,  301,  284,  268,
     253,  239,  225,  213,  201,  190,  179,  169,  159,  150,  142,  134,
     127,  119,  113,  106,  100,   95,   89,   84,   80,   75,   71,   67,
      63
};

static const unsigned char lead_vol_env[10] = { 10, 10, 10, 10, 9, 9, 8, 7, 4, 0 };
static const unsigned char bass_vol_env[10] = {  8,  8,  7,  7, 6, 5, 4, 0, 0, 0 };

static const unsigned char music_lead_notes[64] = {
    /* Section 1: D minor synth theme */
    39, 42, 46, 42,  39, 41, 42, 44,
    42, 39, 35, 39,  42, 44, 46, 47,
    44, 41, 37, 41,  44, 46, 47, 49,
    46, 41, 37, 41,  46, 44, 41, 38,

    /* Section 2: Soaring Octave variation */
    51, 46, 42, 46,  51, 53, 54, 56,
    54, 51, 47, 51,  54, 56, 58, 59,
    56, 53, 49, 53,  56, 58, 59, 61,
    58, 56, 53, 50,  46, 42, 38,  0
};

static const unsigned char music_bass_notes[64] = {
    /* Section 1 Bass */
    15, 27, 15, 22,  15, 27, 18, 20,
    11, 23, 11, 18,  11, 23, 15, 18,
    13, 25, 13, 20,  13, 25, 17, 20,
    10, 22, 10, 17,  10, 22, 14, 17,

    /* Section 2 Bass */
    15, 27, 15, 22,  15, 27, 18, 20,
    11, 23, 11, 18,  11, 23, 15, 18,
    13, 25, 13, 20,  13, 25, 17, 20,
    10, 22, 10, 17,  10, 14, 17, 22
};

short sound_decay = 0;
short sound_sfx_pitch = 0;
short music_enabled = 1;
static short music_step = 0;
static short music_tick = 0;

/* Multi-frame SFX system: descending sweep for drop, ascending for core */
#define SFX_NONE    0
#define SFX_DROP    1
#define SFX_LAND    2
#define SFX_CRASH   3
#define SFX_CORE    4
static short sfx_type = SFX_NONE;
static short sfx_frame = 0;

void sound_init(void)
{
    music_step = 0;
    music_tick = 0;
    sound_decay = 0;
    sound_sfx_pitch = 0;
    sfx_type = SFX_NONE;
    sfx_frame = 0;
    os_psg_write(7, 0xF8); /* Tone A, B, C enabled; Noise disabled; Port A/B outputs ACTIVE for floppy */
    os_psg_write(8, 0);
    os_psg_write(9, 0);
    os_psg_write(10, 0);
}

void sound_play(short pitch, short vol)
{
    sound_sfx_pitch = pitch;
    sound_decay = vol;
    sfx_type = SFX_NONE;
    os_psg_write(0, pitch & 0xFF);
    os_psg_write(1, (pitch >> 8) & 0x0F);
    os_psg_write(8, vol);
}

void sound_play_sfx(short type)
{
    sfx_type = type;
    sfx_frame = 0;
    sound_decay = 0; /* SFX system takes over channel A */
}

void sound_update(void)
{
    unsigned char lead_n, bass_n;
    unsigned short pitch_val;

    /* Channel A: Sound Effects */
    if (sfx_type != SFX_NONE) {
        short sfx_vol = 0;
        unsigned short sfx_p = 0;

        switch (sfx_type) {
        case SFX_DROP:
            /* Descending pitch swoosh — 12 frames */
            sfx_p = 200 + sfx_frame * 40;  /* pitch descends (period rises) */
            sfx_vol = (sfx_frame < 8) ? 12 : (12 - (sfx_frame - 8) * 3);
            if (sfx_frame >= 12) sfx_type = SFX_NONE;
            break;
        case SFX_LAND:
            /* Quick bright ping — 8 frames */
            sfx_p = 120 + (sfx_frame < 3 ? 0 : sfx_frame * 8);
            sfx_vol = (sfx_frame < 2) ? 15 : (15 - sfx_frame * 2);
            if (sfx_frame >= 8) sfx_type = SFX_NONE;
            break;
        case SFX_CRASH:
            /* Low thud — 10 frames, descending pitch */
            sfx_p = 600 + sfx_frame * 60;
            sfx_vol = (sfx_frame < 3) ? 15 : (15 - sfx_frame);
            if (sfx_frame >= 10) sfx_type = SFX_NONE;
            break;
        case SFX_CORE:
            /* Ascending triumphant arpeggio — 20 frames, 4 ascending notes */
            if      (sfx_frame < 5)  sfx_p = note_periods[39]; /* D4 */
            else if (sfx_frame < 10) sfx_p = note_periods[42]; /* F4 */
            else if (sfx_frame < 15) sfx_p = note_periods[46]; /* A4 */
            else                     sfx_p = note_periods[51]; /* D5 */
            sfx_vol = (sfx_frame < 16) ? 15 : (15 - (sfx_frame - 16) * 4);
            if (sfx_frame >= 20) sfx_type = SFX_NONE;
            break;
        }

        if (sfx_vol < 0) sfx_vol = 0;
        if (sfx_type != SFX_NONE) {
            os_psg_write(0, sfx_p & 0xFF);
            os_psg_write(1, (sfx_p >> 8) & 0x0F);
            os_psg_write(8, sfx_vol);
        } else {
            os_psg_write(8, 0);
        }
        sfx_frame++;
    } else if (sound_decay > 0) {
        sound_decay--;
        os_psg_write(0, sound_sfx_pitch & 0xFF);
        os_psg_write(1, (sound_sfx_pitch >> 8) & 0x0F);
        os_psg_write(8, sound_decay);
    } else {
        os_psg_write(8, 0);
    }

    if (!music_enabled) {
        os_psg_write(9, 0);
        os_psg_write(10, 0);
        return;
    }

    /* Channel B: Melodic Synth Lead (clean singing tone) */
    lead_n = music_lead_notes[music_step];
    if (lead_n == 0 || lead_n >= 73) {
        os_psg_write(9, 0);
    } else {
        pitch_val = note_periods[lead_n];
        /* Subtle gentle vibrato on frames 5..7 */
        if (music_tick >= 5 && music_tick <= 7) {
            pitch_val += (music_tick & 1) ? 2 : -2;
        }
        os_psg_write(2, pitch_val & 0xFF);
        os_psg_write(3, (pitch_val >> 8) & 0x0F);
        os_psg_write(9, lead_vol_env[music_tick]);
    }

    /* Channel C: Deep Warm Bassline */
    bass_n = music_bass_notes[music_step];
    if (bass_n == 0 || bass_n >= 73) {
        os_psg_write(10, 0);
    } else {
        pitch_val = note_periods[bass_n];
        os_psg_write(4, pitch_val & 0xFF);
        os_psg_write(5, (pitch_val >> 8) & 0x0F);
        os_psg_write(10, bass_vol_env[music_tick]);
    }

    /* Advance Music Clock (10 frames = 0.20s per note, steady 150 BPM) */
    music_tick++;
    if (music_tick >= 10) {
        music_tick = 0;
        music_step = (music_step + 1) & 63;
    }
}

void sound_stop(void)
{
    os_psg_write(8, 0);
    os_psg_write(9, 0);
    os_psg_write(10, 0);
    os_psg_write(7, 0xFF); /* Mute all sound, keep Port A/B outputs ACTIVE for floppy */
    sound_decay = 0;
    sfx_type = SFX_NONE;
}


/* --- Gameplay Architecture --- */
#define DIFF_NOVICE 0
#define DIFF_PILOT  1
#define DIFF_MASTER 2

short current_difficulty = DIFF_PILOT;

typedef struct {
    unsigned char rot;
    signed char   speed;
    unsigned char gate_offset;
    unsigned char gate_width;
} Ring;

Ring rings[NUM_RINGS];

/* --- Background Cosmic Starfield --- */
#define NUM_STARS 56
typedef struct {
    short x, y;
    short layer; /* 0 = deep space, 1 = mid star, 2 = near bright star */
} Star;
Star stars[NUM_STARS];
unsigned short star_tick = 0;

void init_stars(void)
{
    short i;
    for (i = 0; i < NUM_STARS; i++) {
        stars[i].x = st_rand() % 320;
        stars[i].y = 16 + (st_rand() % 178);
        stars[i].layer = i % 3;
    }
}

void update_stars(void)
{
    short i;
    star_tick++;
    for (i = 0; i < NUM_STARS; i++) {
        if (stars[i].layer == 0) {
            if ((star_tick % 3) != 0) continue;
        } else if (stars[i].layer == 1) {
            if ((star_tick & 1) != 0) continue;
        }
        stars[i].x--;
        if (stars[i].x < 2) {
            stars[i].x = 317;
            stars[i].y = 16 + (st_rand() % 178);
        }
    }
}

void render_stars(char *screen)
{
    short i, c;
    for (i = 0; i < NUM_STARS; i++) {
        if (stars[i].layer == 0) {
            c = 13; /* Deep Indigo / shadow blue (distant dim star) */
        } else if (stars[i].layer == 1) {
            c = (i & 1) ? 2 : 14; /* Electric Blue / Ambient Glow */
        } else {
            /* Near twinkling stars: alternating between white and bright cyan */
            c = ((star_tick + (i << 2)) & 16) ? 7 : 15;
        }
        PLOT_PIXEL(screen, stars[i].x, stars[i].y, c);
    }
}

/* --- Trailing Pixels / Ion Exhaust Motes --- */
#define MAX_TRAIL_MOTES 24
typedef struct {
    short x, y;
    short vx, vy;
    short life;
    short color_base;
} TrailMote;
TrailMote trail_motes[MAX_TRAIL_MOTES];

void init_trail_motes(void)
{
    short i;
    for (i = 0; i < MAX_TRAIL_MOTES; i++) {
        trail_motes[i].life = 0;
    }
}

void spawn_trail_mote(short px, short py, short vx, short vy, short locked)
{
    short i;
    for (i = 0; i < MAX_TRAIL_MOTES; i++) {
        if (trail_motes[i].life <= 0) {
            trail_motes[i].x = px << 4;
            trail_motes[i].y = py << 4;
            trail_motes[i].vx = vx;
            trail_motes[i].vy = vy;
            trail_motes[i].life = 8 + (st_rand() % 6);
            trail_motes[i].color_base = locked ? 6 : 8;
            break;
        }
    }
}

void update_and_render_trail_motes(char *screen)
{
    short i, c;
    for (i = 0; i < MAX_TRAIL_MOTES; i++) {
        if (trail_motes[i].life > 0) {
            trail_motes[i].x += trail_motes[i].vx;
            trail_motes[i].y += trail_motes[i].vy;
            trail_motes[i].life--;

            if (trail_motes[i].life > 8) {
                c = 7; /* Laser White */
            } else if (trail_motes[i].life > 5) {
                c = trail_motes[i].color_base; /* Ion Yellow or MCP Cyan */
            } else if (trail_motes[i].life > 3) {
                c = 2; /* Electric Neon Blue */
            } else if (trail_motes[i].life > 1) {
                c = 1; /* Deep Cobalt Blue */
            } else {
                c = 13; /* Deep Indigo / Shadow Blue */
            }

            PLOT_PIXEL(screen, trail_motes[i].x >> 4, trail_motes[i].y >> 4, c);
        }
    }
}

#define MAX_SPARKS 14
typedef struct {
    short x, y;
    short vx, vy;
    short life;
    short color;
} Spark;
Spark sparks[MAX_SPARKS];

short current_ring;
unsigned char player_ang;

short is_dropping;
short drop_frame;
short drop_max_frames;
short drop_from_rx, drop_to_rx;
short drop_from_ry, drop_to_ry;
short intercept_locked;

long  score;
long  high_score = 5000;
short high_score_dirty = 0;

#define HI_MAGIC 0x434F5245L /* 'CORE' */

static char hi_filename[16] = "CORE.HI";
static long hi_io_buf[2];

void init_drive(void)
{
    short cur = os_getdrv();
    long drv_map = *(long *)0x4C2L;

    /*
     * If launched from drive C:, or if C:\CORE2.PRG exists (e.g. Hatari HDD),
     * use drive C:. Otherwise, target the boot drive (e.g. Floppy A: on real hardware).
     */
    if (cur == 2) {
        hi_filename[0] = 'C';
        hi_filename[1] = ':';
        hi_filename[2] = '\\';
        hi_filename[3] = 'C';
        hi_filename[4] = 'O';
        hi_filename[5] = 'R';
        hi_filename[6] = 'E';
        hi_filename[7] = '.';
        hi_filename[8] = 'H';
        hi_filename[9] = 'I';
        hi_filename[10] = '\0';
    } else if (cur == 0 && (drv_map & 4)) {
        short test_fh = os_fopen("C:\\CORE2.PRG", 0);
        if (test_fh >= 0) {
            os_fclose(test_fh);
            os_setdrv(2);
            hi_filename[0] = 'C';
            hi_filename[1] = ':';
            hi_filename[2] = '\\';
            hi_filename[3] = 'C';
            hi_filename[4] = 'O';
            hi_filename[5] = 'R';
            hi_filename[6] = 'E';
            hi_filename[7] = '.';
            hi_filename[8] = 'H';
            hi_filename[9] = 'I';
            hi_filename[10] = '\0';
        } else {
            /* Real floppy A: on hardware */
            hi_filename[0] = 'A';
            hi_filename[1] = ':';
            hi_filename[2] = '\\';
            hi_filename[3] = 'C';
            hi_filename[4] = 'O';
            hi_filename[5] = 'R';
            hi_filename[6] = 'E';
            hi_filename[7] = '.';
            hi_filename[8] = 'H';
            hi_filename[9] = 'I';
            hi_filename[10] = '\0';
        }
    } else {
        hi_filename[0] = (char)('A' + cur);
        hi_filename[1] = ':';
        hi_filename[2] = '\\';
        hi_filename[3] = 'C';
        hi_filename[4] = 'O';
        hi_filename[5] = 'R';
        hi_filename[6] = 'E';
        hi_filename[7] = '.';
        hi_filename[8] = 'H';
        hi_filename[9] = 'I';
        hi_filename[10] = '\0';
    }
}

void load_high_score(void)
{
    short fh;
    /* Ensure PSG Port A outputs are active for floppy drive controller */
    os_psg_write(7, 0xF8);

    fh = os_fopen(hi_filename, 0);
    if (fh >= 0) {
        long n = os_fread(fh, 8, hi_io_buf);
        os_fclose(fh);
        if (n == 8 && hi_io_buf[0] == HI_MAGIC && hi_io_buf[1] >= 0 && hi_io_buf[1] < 10000000L) {
            high_score = hi_io_buf[1];
        }
    }
}

void save_high_score(void)
{
    short fh;
    /*
     * Mute sound channels and ensure Port A/B output lines (floppy drive select)
     * are active during disk access.
     */
    os_psg_write(8, 0);
    os_psg_write(9, 0);
    os_psg_write(10, 0);
    os_psg_write(7, 0xFF); /* Mute all sound, keep Port A/B outputs enabled */

    fh = os_fcreate(hi_filename, 0);
    if (fh >= 0) {
        hi_io_buf[0] = HI_MAGIC;
        hi_io_buf[1] = high_score;
        os_fwrite(fh, 8, hi_io_buf);
        os_fclose(fh);
        high_score_dirty = 0;
    }

    /* Restore sound tone generators */
    os_psg_write(7, 0xF8);
}

short shields;
short level;
short shake_frames;
short victory_pulse;

void spawn_sparks(short x, short y, short color)
{
    short i;
    for (i = 0; i < MAX_SPARKS; i++) {
        sparks[i].x = x << 4;
        sparks[i].y = y << 4;
        sparks[i].vx = (st_rand() % 49) - 24;
        sparks[i].vy = (st_rand() % 49) - 24;
        sparks[i].life = 8 + (st_rand() % 6);
        sparks[i].color = (i & 1) ? 7 : color;
    }
}

void update_sparks(char *screen)
{
    short i;
    for (i = 0; i < MAX_SPARKS; i++) {
        if (sparks[i].life > 0) {
            sparks[i].x += sparks[i].vx;
            sparks[i].y += sparks[i].vy;
            sparks[i].life--;
            PLOT_PIXEL(screen, sparks[i].x >> 4, sparks[i].y >> 4, sparks[i].color);
        }
    }
}

void init_level(short lvl)
{
    short r;
    short base_speed;
    short gate_base_w;

    level = lvl;
    current_ring = NUM_RINGS - 1;
    player_ang = 64;
    is_dropping = 0;
    intercept_locked = 0;
    shake_frames = 0;
    victory_pulse = 0;
    init_trail_motes();

    if (current_difficulty == DIFF_NOVICE) {
        drop_max_frames = 4;
        gate_base_w = 34;
        base_speed = 3;
    } else if (current_difficulty == DIFF_PILOT) {
        drop_max_frames = 6;
        gate_base_w = 26;
        base_speed = 4;
    } else {
        drop_max_frames = 8;
        gate_base_w = 18;
        base_speed = 6;
    }

    rings[0].speed = (signed char)( (base_speed + 2) + (lvl >> 1));
    rings[1].speed = (signed char)(-(base_speed + 1) - (lvl >> 1));
    rings[2].speed = (signed char)(  base_speed      + (lvl >> 1));
    rings[3].speed = (signed char)(-(base_speed - 1) - (lvl >> 1));

    for (r = 0; r < NUM_RINGS; r++) {
        rings[r].rot = (unsigned char)(r * 64);
        rings[r].gate_offset = (unsigned char)(r * 72 + 36);
        rings[r].gate_width = (unsigned char)(gate_base_w - (lvl > 4 ? 4 : lvl));
        if (rings[r].gate_width < 10) rings[r].gate_width = 10;
    }
}

void init_game(void)
{
    score = 0;
    shields = 3;
    init_level(1);
}

/* --- Render Scene Routine --- */
void render_scene(char *screen)
{
    short r, i, c;
    short px, py;
    short target_ring;
    unsigned char gate_mid;

    /* 0. Background Cosmic Starfield */
    render_stars(screen);

    /* 1. Circuit Board Corner Vector Accents */
    draw_line_fast(screen, 6, 16, 28, 16, 2);
    draw_line_fast(screen, 6, 16, 6, 38, 2);
    draw_line_fast(screen, 28, 16, 34, 22, 3);

    draw_line_fast(screen, 313, 16, 291, 16, 2);
    draw_line_fast(screen, 313, 16, 313, 38, 2);
    draw_line_fast(screen, 291, 16, 285, 22, 3);

    draw_line_fast(screen, 6, 195, 28, 195, 2);
    draw_line_fast(screen, 6, 195, 6, 173, 2);
    draw_line_fast(screen, 28, 195, 34, 189, 3);

    draw_line_fast(screen, 313, 195, 291, 195, 2);
    draw_line_fast(screen, 313, 195, 313, 173, 2);
    draw_line_fast(screen, 291, 195, 285, 189, 3);

    /* 2. Concentric Vector Circles: Blue Tracks, Ion Yellow at Jump Gate */
    for (r = 0; r < NUM_RINGS; r++) {
        gate_mid = (unsigned char)(rings[r].rot + rings[r].gate_offset);

        for (i = 0; i < SEG_COUNT; i++) {
            unsigned char a0 = seg_angles[i];
            unsigned char a1 = seg_angles[i + 1];
            unsigned char seg_mid = (unsigned char)((a0 + a1) >> 1);
            unsigned char diff = (unsigned char)(seg_mid - gate_mid);
            if (diff > 128) diff = (unsigned char)(256 - diff);

            if (diff <= rings[r].gate_width) {
                if (r == current_ring - 1) {
                    c = (intercept_locked && shields > 0) ? ((glow_tick & 4) ? 7 : 6) : 6;
                } else {
                    c = 5;
                }
            } else {
                if (r == current_ring)          c = 4;
                else if (r == current_ring - 1) c = 3;
                else                            c = 1;
            }

            draw_line_fast(screen,
                           ring_geom[r][i].x0, ring_geom[r][i].y0,
                           ring_geom[r][i].x1, ring_geom[r][i].y1, c);
        }
    }

    /* 3. Central Master CORE */
    if (current_ring == -1) {
        short vr = (victory_pulse % 18);
        c = (victory_pulse & 2) ? 7 : 6;
        for (i = 0; i < 8; i++) {
            unsigned char ca0 = (unsigned char)(i << 5);
            unsigned char ca1 = (unsigned char)((i + 1) << 5);
            short x0 = CENTER_X + (((vr + 12) * f_cos(ca0)) >> 8);
            short y0 = CENTER_Y + (((vr + 7)  * f_sin(ca0)) >> 8);
            short x1 = CENTER_X + (((vr + 12) * f_cos(ca1)) >> 8);
            short y1 = CENTER_Y + (((vr + 7)  * f_sin(ca1)) >> 8);
            draw_line_fast(screen, x0, y0, x1, y1, c);
        }
    } else {
        c = (current_ring == 0 && intercept_locked && shields > 0) ? 6 : 8;

        draw_line_fast(screen, CENTER_X, CENTER_Y - 8, CENTER_X + 13, CENTER_Y, c);
        draw_line_fast(screen, CENTER_X + 13, CENTER_Y, CENTER_X, CENTER_Y + 8, c);
        draw_line_fast(screen, CENTER_X, CENTER_Y + 8, CENTER_X - 13, CENTER_Y, c);
        draw_line_fast(screen, CENTER_X - 13, CENTER_Y, CENTER_X, CENTER_Y - 8, c);

        draw_line_fast(screen, CENTER_X - 3, CENTER_Y - 3, CENTER_X + 3, CENTER_Y - 3, 7);
        draw_line_fast(screen, CENTER_X + 3, CENTER_Y - 3, CENTER_X + 3, CENTER_Y + 3, 7);
        draw_line_fast(screen, CENTER_X + 3, CENTER_Y + 3, CENTER_X - 3, CENTER_Y + 3, 7);
        draw_line_fast(screen, CENTER_X - 3, CENTER_Y + 3, CENTER_X - 3, CENTER_Y - 3, 7);
        PLOT_PIXEL(screen, CENTER_X, CENTER_Y, 7);
    }

    /* 4. Player Avatar (Light Bit) - Hidden once derezzed */
    if (current_ring >= 0 && shields > 0) {
        short cur_rx, cur_ry;

        if (is_dropping) {
            cur_rx = drop_from_rx + (((drop_to_rx - drop_from_rx) * drop_frame) / drop_max_frames);
            cur_ry = drop_from_ry + (((drop_to_ry - drop_from_ry) * drop_frame) / drop_max_frames);
        } else {
            cur_rx = ring_rx[current_ring];
            cur_ry = ring_ry[current_ring];
        }

        px = CENTER_X + ((cur_rx * f_cos(player_ang)) >> 8);
        py = CENTER_Y + ((cur_ry * f_sin(player_ang)) >> 8);

        if (shake_frames > 0) {
            px += (st_rand() % 5) - 2;
            py += (st_rand() % 5) - 2;
        }

        target_ring = current_ring - 1;
        if (target_ring >= 0 && !is_dropping && intercept_locked) {
            short tx = CENTER_X + ((ring_rx[target_ring] * f_cos(player_ang)) >> 8);
            short ty = CENTER_Y + ((ring_ry[target_ring] * f_sin(player_ang)) >> 8);
            draw_line_fast(screen, px, py, tx, ty, (rings[0].rot & 4) ? 7 : 6);
        }

        if (is_dropping) {
            short fx = CENTER_X + ((drop_from_rx * f_cos(player_ang)) >> 8);
            short fy = CENTER_Y + ((drop_from_ry * f_sin(player_ang)) >> 8);
            draw_line_fast(screen, fx, fy, px, py, 6);
        }

        /* Trailing light streak / comet tail along current orbit */
        if (!is_dropping && current_ring >= 0) {
            signed char spd = rings[current_ring].speed;
            short mvx, mvy;

            /* Continuous fading arc (12 steps behind the player beacon) */
            for (i = 1; i <= 12; i++) {
                unsigned char t_ang = (unsigned char)(player_ang - ((spd * i * 3) >> 2));
                short tx = CENTER_X + ((cur_rx * f_cos(t_ang)) >> 8);
                short ty = CENTER_Y + ((cur_ry * f_sin(t_ang)) >> 8);
                short t_col;

                if (i <= 2) {
                    t_col = intercept_locked ? 6 : 7; /* Bright Ion Yellow or Laser White */
                    PLOT_PIXEL(screen, tx,     ty,     t_col);
                    PLOT_PIXEL(screen, tx + 1, ty,     t_col);
                    PLOT_PIXEL(screen, tx,     ty + 1, t_col);
                } else if (i <= 4) {
                    t_col = intercept_locked ? 5 : 8; /* Ion Amber or MCP Cyan */
                    PLOT_PIXEL(screen, tx,     ty,     t_col);
                    PLOT_PIXEL(screen, tx + 1, ty,     t_col);
                } else if (i <= 6) {
                    t_col = 3; /* Bright Cyan-Blue */
                    PLOT_PIXEL(screen, tx, ty, t_col);
                } else if (i <= 8) {
                    t_col = 2; /* Electric Neon Blue */
                    PLOT_PIXEL(screen, tx, ty, t_col);
                } else if (i <= 10) {
                    t_col = 9; /* Phosphor Blue Shadow */
                    PLOT_PIXEL(screen, tx, ty, t_col);
                } else {
                    t_col = 13; /* Deep Indigo / Shadow Blue */
                    PLOT_PIXEL(screen, tx, ty, t_col);
                }
            }

            /* Emit floating ion exhaust motes peeling off into space */
            mvx = ((f_sin(player_ang) * spd) >> 5) + ((st_rand() & 7) - 3);
            mvy = ((-f_cos(player_ang) * spd) >> 5) + ((st_rand() & 7) - 3);
            spawn_trail_mote(px, py, mvx, mvy, intercept_locked);
        } else if (is_dropping) {
            short dvx = (((drop_from_rx - drop_to_rx) * f_cos(player_ang)) >> 6) + ((st_rand() & 7) - 3);
            short dvy = (((drop_from_ry - drop_to_ry) * f_sin(player_ang)) >> 6) + ((st_rand() & 7) - 3);
            spawn_trail_mote(px, py, dvx, dvy, 1);
        }

        /* Prominent Multi-Layer Radiant Beacon (7x7 glowing diamond with solid core) */
        c = intercept_locked ? 6 : 8;            /* 6 = Vivid Ion Yellow, 8 = MCP Cyan Vector */
        {
            short c_outer = intercept_locked ? 5 : 2; /* 5 = Amber Glow, 2 = Electric Blue */
            short c_core  = 7;                        /* 7 = Laser White */

            /* Outer Aura tips (distance 3) */
            PLOT_PIXEL(screen, px - 3, py,     c_outer);
            PLOT_PIXEL(screen, px + 3, py,     c_outer);
            PLOT_PIXEL(screen, px,     py - 3, c_outer);
            PLOT_PIXEL(screen, px,     py + 3, c_outer);

            /* Outer diagonal diamond corners (distance 2) */
            PLOT_PIXEL(screen, px - 2, py - 2, c_outer);
            PLOT_PIXEL(screen, px + 2, py - 2, c_outer);
            PLOT_PIXEL(screen, px - 2, py + 2, c_outer);
            PLOT_PIXEL(screen, px + 2, py + 2, c_outer);

            /* Mid-layer diamond cross (distance 2) */
            PLOT_PIXEL(screen, px - 2, py,     c);
            PLOT_PIXEL(screen, px + 2, py,     c);
            PLOT_PIXEL(screen, px,     py - 2, c);
            PLOT_PIXEL(screen, px,     py + 2, c);

            /* Inner ring (distance 1 diagonals) */
            PLOT_PIXEL(screen, px - 1, py - 1, c);
            PLOT_PIXEL(screen, px + 1, py - 1, c);
            PLOT_PIXEL(screen, px - 1, py + 1, c);
            PLOT_PIXEL(screen, px + 1, py + 1, c);

            /* Solid blazing 3x3 core (laser white / yellow) */
            PLOT_PIXEL(screen, px - 1, py,     c_core);
            PLOT_PIXEL(screen, px + 1, py,     c_core);
            PLOT_PIXEL(screen, px,     py - 1, c_core);
            PLOT_PIXEL(screen, px,     py + 1, c_core);
            PLOT_PIXEL(screen, px,     py,     c_core);
        }
    }

    /* 5. Render Sparks & Floating Ion Exhaust Motes */
    update_and_render_trail_motes(screen);
    update_sparks(screen);

    /* 6. TRON Arcade Vector HUD with Persistent High Score */
    draw_text(screen, 12, 5, "SCORE", 2);
    draw_number(screen, 46, 5, score, 6, 7);

    draw_text(screen, 96, 5, "HI", 2);
    draw_number(screen, 112, 5, high_score, 6, (score >= high_score && score > 0) ? 6 : 5);

    draw_text(screen, 164, 5, "CORE", 6);
    draw_number(screen, 194, 5, level, 2, 7);

    if (current_difficulty == DIFF_NOVICE)      draw_text(screen, 216, 5, "[NOV]", 3);
    else if (current_difficulty == DIFF_PILOT)  draw_text(screen, 216, 5, "[PLT]", 6);
    else                                        draw_text(screen, 216, 5, "[MST]", 10);

    draw_text(screen, 258, 5, "SHD", 2);
    for (i = 0; i < 3; i++) {
        draw_char(screen, 282 + i * 8, 5, (i < shields) ? '#' : '-', (i < shields) ? 6 : 10);
    }

    /* Bottom Tactical Ticker - Stable display without frame flicker */
    if (current_ring == -1) {
        draw_text(screen, 46, 185, ">>> CORE OVERRIDE COMPLETE <<<", 6);
    } else if (shields <= 0) {
        if (score >= high_score && score > 0) {
            draw_text(screen, 76, 162, ">>> NEW HIGH SCORE! <<<", 6);
        }
        draw_text(screen, 44, 174, "PROGRAM DEREZZED - SYSTEM FAILURE", 10);
        draw_text(screen, 54, 186, "PRESS [SPACE] TO REBOOT CORE", (glow_tick & 16) ? 7 : 6);
    } else if (is_dropping) {
        draw_text(screen, 82, 185, ">> IN TRANSIT <<", 7);
    } else if (intercept_locked) {
        draw_text(screen, 44, 185, ">> JUMP VECTOR OPEN: PRESS [SPACE] <<", 6);
    } else {
        draw_text(screen, 62, 185, "TRACKING ROTATIONAL VECTORS...", 2);
    }
}

/* --- Buffers & Main Application --- */
char screen_buffer[32000 + 256];
char *back_buffer;
char *front_buffer;
void *orig_phys;
void *orig_log;
short orig_rez;
long  orig_ssp;

int main(void)
{
    short game_running = 1;
    short space_hit = 0;
    short title_screen = 1;
    long key;
    char ascii, scan;
    short r;

    orig_ssp = os_super(0);
    init_drive();
    has_blitter = detect_blitter();

    orig_phys = os_physbase();
    orig_log  = os_logbase();
    orig_rez  = os_getrez();

    if (orig_rez != 0) {
        os_setscreen(orig_log, orig_phys, 0);
    }

    back_buffer  = (char *)(((long)screen_buffer + 255) & ~255L);
    front_buffer = (char *)orig_phys;

    init_tables();
    init_stars();
    init_trail_motes();
    sound_init();
    load_high_score();
    init_game();

    /* Disable desktop mouse cursor */
    os_hide_mouse();

    while (game_running) {
        space_hit = 0;

        if (os_key_check()) {
            key = os_key_read();
            ascii = (char)(key & 0xFF);
            scan  = (char)((key >> 16) & 0xFF);

            if (ascii == 27 || scan == 0x01) {
                if (high_score_dirty) save_high_score();
                game_running = 0;
            }
            if (ascii == ' ' || scan == 0x39) {
                space_hit = 1;
            }
            if (ascii == 'm' || ascii == 'M') {
                music_enabled = !music_enabled;
                if (!music_enabled) {
                    os_psg_write(9, 0);
                    os_psg_write(10, 0);
                }
            }

            if (title_screen) {
                if (ascii == '1') current_difficulty = DIFF_NOVICE;
                if (ascii == '2') current_difficulty = DIFF_PILOT;
                if (ascii == '3') current_difficulty = DIFF_MASTER;
                if (ascii == 'd' || ascii == 'D') {
                    current_difficulty = (current_difficulty + 1) % 3;
                }
            }
        }

        if (title_screen) {
            clear_screen(back_buffer);
            render_stars(back_buffer);

            draw_text(back_buffer, 24,  10, "1ST    0", 2);
            draw_text(back_buffer, 160, 10, "HI-SCORE", 2);
            draw_number(back_buffer, 232, 10, high_score, 6, 6);

            draw_tron_core_logo(back_buffer, 88, 24);
            draw_text(back_buffer, 97, 66, "WRITTEN BY NINJABUFFY", 8);

            draw_text(back_buffer, 28, 80, "- ALTERNATING CIRCLES ROTATE IN COBALT BLUE", 3);
            draw_text(back_buffer, 28, 92, "- JUMP BECOMES POSSIBLE AT ION YELLOW GATES", 6);
            draw_text(back_buffer, 28, 104,"- TIME FLIGHT LEAD BEFORE SECTOR ESCAPES", 7);
            draw_text(back_buffer, 28, 116,"- PRESS [M] TO TOGGLE MUSIC", music_enabled ? 6 : 1);
            draw_text(back_buffer, 214, 116, music_enabled ? "[MUSIC ON]" : "[MUTED]", music_enabled ? 6 : 10);

            if (has_blitter) {
                draw_text(back_buffer, 28, 130, "BLITTER HARDWARE : [ONLINE / 50 FPS LOCKED]", 8);
            } else {
                draw_text(back_buffer, 28, 130, "BLITTER HARDWARE : [CPU BURST / 50 FPS LOCKED]", 5);
            }

            draw_text(back_buffer, 28, 144, "SELECT SECTOR DIFFICULTY [1, 2, 3]:", 7);
            draw_text(back_buffer, 40, 155, "1) NOVICE  (WIDE GATES / SLOW CYCLE)", (current_difficulty == DIFF_NOVICE) ? 6 : 1);
            draw_text(back_buffer, 40, 165, "2) PILOT   (STANDARD INTERCEPT)",      (current_difficulty == DIFF_PILOT)  ? 6 : 1);
            draw_text(back_buffer, 40, 175, "3) MASTER  (HYPER INFILTRATION)",     (current_difficulty == DIFF_MASTER) ? 6 : 1);

            draw_text(back_buffer, 64, 191, "PRESS [SPACE] TO ENTER GRID", (glow_tick & 16) ? 7 : 6);

            if (space_hit) {
                title_screen = 0;
                init_game();
                sound_play(0x120, 15);
            }
        } else {
            /* Active Game Session: Playing, Victory Sequence, or Derezzed */
            if (shields <= 0) {
                /* System Derezzed: Wait for Spacebar to reboot */
                if (space_hit) {
                    init_game();
                    sound_play(0x120, 15);
                }
            } else if (current_ring == -1) {
                /* Victory Pulse Sequence */
                victory_pulse++;
                if (victory_pulse == 1)  sound_play(0x060, 15);
                if (victory_pulse == 8)  sound_play(0x048, 15);
                if (victory_pulse == 16) sound_play(0x030, 15);

                if (victory_pulse > 35) {
                    score += (500 * (current_difficulty + 1)) * level;
                    if (score > high_score) {
                        high_score = score;
                        high_score_dirty = 1;
                    }
                    init_level(level + 1);
                }
            } else {
                /* Standard Flight Mechanics */
                for (r = 0; r < NUM_RINGS; r++) {
                    rings[r].rot += rings[r].speed;
                }

                if (!is_dropping) {
                    player_ang += rings[current_ring].speed;

                    if (current_ring > 0) {
                        short target = current_ring - 1;
                        unsigned char future_gate_rot = (unsigned char)(rings[target].rot + (rings[target].speed * drop_max_frames));
                        unsigned char future_gate_mid = (unsigned char)(future_gate_rot + rings[target].gate_offset);

                        unsigned char diff = (unsigned char)(player_ang - future_gate_mid);
                        if (diff > 128) diff = (unsigned char)(256 - diff);

                        intercept_locked = (diff <= rings[target].gate_width);
                    } else if (current_ring == 0) {
                        intercept_locked = 1;
                    }

                    if (space_hit) {
                        is_dropping = 1;
                        drop_frame = 0;
                        drop_from_rx = ring_rx[current_ring];
                        drop_from_ry = ring_ry[current_ring];

                        if (current_ring > 0) {
                            drop_to_rx = ring_rx[current_ring - 1];
                            drop_to_ry = ring_ry[current_ring - 1];
                        } else {
                            drop_to_rx = 0;
                            drop_to_ry = 0;
                        }

                        sound_play_sfx(SFX_DROP);
                    }
                } else {
                    drop_frame++;

                    if (drop_frame >= drop_max_frames) {
                        is_dropping = 0;

                        if (current_ring > 0) {
                            short target = current_ring - 1;
                            unsigned char land_gate_mid = (unsigned char)(rings[target].rot + rings[target].gate_offset);
                            unsigned char diff = (unsigned char)(player_ang - land_gate_mid);
                            if (diff > 128) diff = (unsigned char)(256 - diff);

                            if (diff <= rings[target].gate_width) {
                                current_ring = target;
                                score += (100 * (current_difficulty + 1)) * level;
                                if (score > high_score) {
                                    high_score = score;
                                    high_score_dirty = 1;
                                }
                                spawn_sparks(CENTER_X + ((ring_rx[current_ring] * f_cos(player_ang)) >> 8),
                                             CENTER_Y + ((ring_ry[current_ring] * f_sin(player_ang)) >> 8), 6);
                                sound_play_sfx(SFX_LAND);
                            } else {
                                shields--;
                                if (shields <= 0 && high_score_dirty) {
                                    save_high_score();
                                }
                                shake_frames = 8;
                                spawn_sparks(CENTER_X + ((ring_rx[current_ring] * f_cos(player_ang)) >> 8),
                                             CENTER_Y + ((ring_ry[current_ring] * f_sin(player_ang)) >> 8), 10);
                                sound_play_sfx(SFX_CRASH);
                            }
                        } else if (current_ring == 0) {
                            current_ring = -1;
                            victory_pulse = 0;
                            spawn_sparks(CENTER_X, CENTER_Y, 6);
                            sound_play_sfx(SFX_CORE);
                        }
                    }
                }
            }

            if (shake_frames > 0) shake_frames--;

            /* Redraw Back Buffer Every Single Frame */
            clear_screen(back_buffer);
            render_scene(back_buffer);
        }

        sound_update();
        update_stars();

        /*
         * Synchronize Both TOS Base Pointer ($44E) and Hardware Shifter
         * registers before vertical retrace to prevent address desync.
         */
        *(char **)0x44EL = back_buffer;
        *(volatile unsigned char *)0xFFFF8201UL = (unsigned char)(((unsigned long)back_buffer) >> 16);
        *(volatile unsigned char *)0xFFFF8203UL = (unsigned char)(((unsigned long)back_buffer) >> 8);

        os_vsync();
        update_neon_glow();

        {
            char *tmp = back_buffer;
            back_buffer = front_buffer;
            front_buffer = tmp;
        }
    }

    if (high_score_dirty) {
        save_high_score();
    }

    sound_stop();
    os_vsync();
    os_setscreen(orig_log, orig_phys, orig_rez);
    os_setpalette(default_tos_palette);

    /* Restore mouse cursor before returning to Desktop */
    os_show_mouse();
    os_super(orig_ssp);

    return 0;
}
