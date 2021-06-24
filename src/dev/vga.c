/* 
 * This file is part of the Nautilus AeroKernel developed
 * by the Hobbes and V3VEE Projects with funding from the 
 * United States National  Science Foundation and the Department of Energy.  
 *
 * The V3VEE Project is a joint project between Northwestern University
 * and the University of New Mexico.  The Hobbes Project is a collaboration
 * led by Sandia National Laboratories that includes several national 
 * laboratories and universities. You can find out more at:
 * http://www.v3vee.org  and
 * http://xstack.sandia.gov/hobbes
 *
 * Copyright (c) 2016, Peter Dinda <pdinda@u.northwestern.edu>
 * Copyright (c) 2015, Kyle C. Hale <kh@u.northwestern.edu>
 * Copyright (c) 2015, The V3VEE Project  <http://www.v3vee.org> 
 *                     The Hobbes Project <http://xstack.sandia.gov/hobbes>
 * All rights reserved.
 *
 * Authors: Kyle C. Hale <kh@u.northwestern.edu>
 *          Peter Dinda <pdinda@northestern.edu>
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "LICENSE.txt".
 */

#include <nautilus/cpu.h>
#include <nautilus/dev.h>
#include <nautilus/mb_utils.h>
#include <nautilus/naut_string.h>
#include <dev/vga.h>

#define VGA_BASE_ADDR 0xb8000

#define CRTC_ADDR 0x3d4
#define CRTC_DATA 0x3d5
#define CURSOR_LOW 0xf
#define CURSOR_HIGH 0xe

#define ATTR_CTRL_FLIP_FLOP 0x3da
#define ATTR_CTRL_ADDR_AND_DATA_WRITE 0x3c0
#define ATTR_CTRL_DATA_READ 0x3c1
#define ATTR_MODE_CTRL 0x10

const static uint64_t font[] =
{
	0x0000000000000000,
	0x0000000000000000,
	0x000000FF00000000,
	0x000000FF00FF0000,
	0x1818181818181818,
	0x6C6C6C6C6C6C6C6C,
	0x181818F800000000,
	0x6C6C6CEC0CFC0000,
	0x1818181F00000000,
	0x6C6C6C6F607F0000,
	0x000000F818181818,
	0x000000FC0CEC6C6C,
	0x0000001F18181818,
	0x0000007F606F6C6C,
	0x187E7EFFFF7E7E18,  // circle  0x00187EFFFF7E1800
	0x0081818181818100,  // square
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0008000000000000,//
	0x0000000000000000,//
	0x00180018183C3C18,//!
	0x0000000000121236,//"
	0x006C6CFE6CFE6C6C,//#
	0x00187ED07C16FC30,//$$
	0x0060660C18306606,//%
	0x00DC66B61C36361C,//&
	0x0000000000181818,//'
	0x0030180C0C0C1830,//(
	0x000C18303030180C,//)
	0x0000187E3C7E1800,//*
	0x000018187E181800,//+
	0x0C18180000000000,//,
	0x000000007E000000,//-
	0x0018180000000000,//.
	0x0000060C18306000,///
	0x003C42464A52623C,//0
	0x007E101010101C10,//1
	0x007E04081020423C,//2
	0x003C42403840423C,//3
	0x0020207E22242830,//4
	0x003C4240403E027E,//5
	0x003C42423E020438,//6
	0x000404081020407E,//7
	0x003C42423C42423C,//8
	0x001C20407C42423C,//9
	0x0018180018180000,//:
	0x0C18180018180000,//;
	0x0030180C060C1830,//<
	0x0000007E007E0000,//=
	0x000C18306030180C,//>
	0x001800181830663C,//?
	0x003C06765676663C,//@
	0x0042427E42422418,//A
	0x003E42423E42423E,//B
	0x003C42020202423C,//C
	0x001E22424242221E,//D
	0x007E02023E02027E,//E
	0x000202023E02027E,//F
	0x003C42427202423C,//G
	0x004242427E424242,//H
	0x007C10101010107C,//I
	0x001C22202020207E,//J
	0x004222120E0A1222,//K
	0x007E020202020202,//L
	0x0082828292AAC682,//M
	0x00424262524A4642,//N
	0x003C42424242423C,//O
	0x000202023E42423E,//P
	0x005C22424242423C,//Q
	0x004242423E42423E,//R
	0x003C42403C02423C,//S
	0x001010101010107C,//T
	0x003C424242424242,//U
	0x0018244242424242,//V
	0x0044AAAA92828282,//W
	0x0042422418244242,//X
	0x0010101038444444,//Y
	0x007E04081020407E,//Z
	0x003E02020202023E,//[
	0x00006030180C0600,  /* //\ */
	0x007C40404040407C,  //]
	0x000000000000663C,//^
	0xFF00000000000000,//_
	0x000000000030180C,//`
	0x007C427C403C0000,//a
	0x003E4242423E0202,//b
	0x003C4202423C0000,//c
	0x007C4242427C4040,//d
	0x003C027E423C0000,//e
	0x000404043E040438,//f
	0x3C407C42427C0000,//g
	0x00424242423E0202,//h
	0x003C1010101C0018,//i
	0x0E101010101C0018,//j
	0x0042221E22420200,//k
	0x003C101010101018,//l
	0x00829292AA440000,//m
	0x00424242423E0000,//n
	0x003C4242423C0000,//o
	0x02023E42423E0000,//p
	0xC0407C42427C0000,//q
	0x00020202463A0000,//r
	0x003E403C027C0000,//s
	0x00380404043E0404,//t
	0x003C424242420000,//u
	0x0018244242420000,//v
	0x006C929292820000,//w
	0x0042241824420000,//x
	0x3C407C4242420000,//y
	0x007E0418207E0000,//z
	0x003018180E181830,//{
	0x0018181818181818,//|
	0x000C18187018180C,//}
	0x000000000062D68C,//~
	0xFFFFFFFFFFFFFFFF,
	0x1E30181E3303331E,//€
	0x007E333333003300,//
	0x001E033F331E0038,//‚
	0x00FC667C603CC37E,//ƒ
	0x007E333E301E0033,//„
	0x007E333E301E0007,//…
	0x007E333E301E0C0C,//†
	0x3C603E03033E0000,//‡
	0x003C067E663CC37E,//ˆ
	0x001E033F331E0033,//‰
	0x001E033F331E0007,//Š
	0x001E0C0C0C0E0033,//‹
	0x003C1818181C633E,//Œ
	0x001E0C0C0C0E0007,//
	0x00333F33331E0C33,//Ž
	0x00333F331E000C0C,//
	0x003F061E063F0038,//
	0x00FE33FE30FE0000,//‘
	0x007333337F33367C,//’
	0x001E33331E00331E,//“
	0x001E33331E003300,//”
	0x001E33331E000700,//•
	0x007E33333300331E,//–
	0x007E333333000700,//—
	0x1F303F3333003300,//˜
	0x001C3E63633E1C63,//™
	0x001E333333330033,//š
	0x18187E03037E1818,//›
	0x003F67060F26361C,//œ
	0x000C3F0C3F1E3333,//
	0x70337B332F1B1B0F,//ž
	0x0E1B18187E18D870,//Ÿ
	0x007E333E301E0038,//
	0x001E0C0C0C0E001C,//¡
	0x001E33331E003800,//¢
	0x007E333333003800,//£
	0x003333331F001F00,//¤
	0x00333B3F3733003F,//¥
	0x00007E007C36363C,//¦
	0x00007E003C66663C,//§
	0x001E3303060C000C,//¨
	0x000003033F000000,//©
	0x000030303F000000,//ª
	0xF81973C67C1B3363,//«
	0xC0F9F3E6CF1B3363,//¬
	0x183C3C1818001800,//­
	0x0000CC663366CC00,//®
	0x00003366CC663300,//¯
	0x1144114411441144,//°
	0x55AA55AA55AA55AA,//±
	0xEEBBEEBBEEBBEEBB,//²
	0x1818181818181818,//³
	0x1818181F18181818,//´
	0x1818181F181F1818,//µ
	0x6C6C6C6F6C6C6C6C,//¶
	0x6C6C6C7F00000000,//·
	0x1818181F181F0000,//¸
	0x6C6C6C6F606F6C6C,//¹
	0x6C6C6C6C6C6C6C6C,//º
	0x6C6C6C6F607F0000,//»
	0x0000007F606F6C6C,//¼
	0x0000007F6C6C6C6C,//½
	0x0000001F181F1818,//¾
	0x1818181F00000000,//¿
	0x000000F818181818,//À
	0x000000FF18181818,//Á
	0x181818FF00000000,//Â
	0x181818F818181818,//Ã
	0x000000FF00000000,//Ä
	0x181818FF18181818,//Å
	0x181818F818F81818,//Æ
	0x6C6C6CEC6C6C6C6C,//Ç
	0x000000FC0CEC6C6C,//È
	0x6C6C6CEC0CFC0000,//É
	0x000000FF00EF6C6C,//Ê
	0x6C6C6CEF00FF0000,//Ë
	0x6C6C6CEC0CEC6C6C,//Ì
	0x000000FF00FF0000,//Í
	0x6C6C6CEF00EF6C6C,//Î
	0x000000FF00FF1818,//Ï
	0x000000FF6C6C6C6C,//Ð
	0x181818FF00FF0000,//Ñ
	0x6C6C6CFF00000000,//Ò
	0x000000FC6C6C6C6C,//Ó
	0x000000F818F81818,//Ô
	0x181818F818F80000,//Õ
	0x6C6C6CFC00000000,//Ö
	0x6C6C6CEF6C6C6C6C,//×
	0x181818FF00FF1818,//Ø
	0x0000001F18181818,//Ù
	0x181818F800000000,//Ú
	0xFFFFFFFFFFFFFFFF,//Û
	0xFFFFFFFF00000000,//Ü
	0x0F0F0F0F0F0F0F0F,//Ý
	0xF0F0F0F0F0F0F0F0,//Þ
	0x00000000FFFFFFFF,//ß
	0x006E3B133B6E0000,//à
	0x03031F331F331E00,//á
	0x0003030303637F00,//â
	0x0036363636367F00,//ã
	0x007F660C180C667F,//ä
	0x001E3333337E0000,//å
	0x03063E6666666600,//æ
	0x00181818183B6E00,//ç
	0x3F0C1E33331E0C3F,//è
	0x001C36637F63361C,//é
	0x007736366363361C,//ê
	0x001E33333E180C38,//ë
	0x00007EDBDB7E0000,//ì
	0x03067EDBDB7E3060,//í
	0x003C06033F03063C,//î
	0x003333333333331E,//ï
	0x00003F003F003F00,//ð
	0x003F000C0C3F0C0C,//ñ
	0x003F00060C180C06,//ò
	0x003F00180C060C18,//ó
	0x1818181818D8D870,//ô
	0x0E1B1B1818181818,//õ
	0x000C0C003F000C0C,//ö
	0x0000394E00394E00,//÷
	0x000000001C36361C,//ø
	0x0000001818000000,//ù
	0x0000001800000000,//ú
	0x383C3637303030F0,//û
	0x000000363636361E,//ü
	0x0000003E061C301E,//ý
	0x00003C3C3C3C0000,//þ
	0xFFFFFFFFFFFFFFFF //ÿ
};

static uint16_t init_txt_buf[VGA_WIDTH*VGA_HEIGHT];

typedef enum vga_mode_type {
    VGA_TYPE_INDEXED = 0,
    VGA_TYPE_RGB,
    VGA_TYPE_EGA_TEXT,
    VGA_TYPE_NUM_TYPES,
} vga_mode_type_t;

typedef struct color_desc {
    uint8_t red_val;
    uint8_t green_val;
    uint8_t blue_val;
} color_desc_t;

struct index_color_info {
    uint32_t num_colors;
    color_desc_t * desc;
};

struct rgb_color_info {
    uint8_t red_fpos;
    uint8_t red_mask_sz;
    uint8_t green_fpos;
    uint8_t green_mask_sz;
    uint8_t blue_fpos;
    uint8_t blue_mask_sz;
};

struct color_info {
    union {
        struct index_color_info idx;
        struct rgb_color_info rgb;
    };
};

struct vga {
    void *  fb;
    uint32_t height;
    uint32_t width;
    uint32_t pitch;
    uint8_t bpp; // bits per pixel

    vga_mode_type_t type;
    struct color_info color;

};

struct term_emu {
	uint32_t height;
	uint32_t width;
	uint16_t * txt_buf;
	size_t x;
	size_t y;
	uint8_t attr;
};


static struct vga vga_info;
static struct term_emu term;

static uint32_t
vga_color_to_pixel (enum vga_color vga_color)
{
	uint32_t color = 0x00000000;
	switch (vga_color) {
		case COLOR_BLACK:
			color = 0x000000;
			break;
		case COLOR_WHITE:
			color = 0xFFFFFF;
			break;
		case COLOR_RED:
			color = 0xFF0000;
			break;
		case COLOR_BLUE:
			color = 0x2222FF;
			break;
		case COLOR_GREEN:
			color = 0x22FF22;
			break;
		case COLOR_CYAN:
			color = 0x11FFFF;
			break;
		case COLOR_MAGENTA:
			color = 0xFF01AA;
			break;
		case COLOR_BROWN:
			color = 0xFFEBCD;
			break;
		case COLOR_LIGHT_GREY:
			color = 0xDDDDDD;
			break;
		case COLOR_DARK_GREY:
			color = 0x555555;
			break;
		case COLOR_LIGHT_BLUE:
			color = 0x01AAFF;
			break;
		case COLOR_LIGHT_GREEN:
			color = 0x01FF01;
			break;
		case COLOR_LIGHT_CYAN:
			color = 0x01DDFF;
			break;
		case COLOR_LIGHT_RED:
			color = 0xFF2222;
			break;
		case COLOR_LIGHT_MAGENTA:
			color = 0xFF0077;
			break;
		case COLOR_LIGHT_BROWN:
			color = 0x8B4513;
			break;
	}

	return color;
}


static inline uint64_t
get_font_from_char (char c)
{
    return font[(int)c];
}

uint16_t 
vga_make_entry (char c, uint8_t color)
{
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

 
uint8_t 
vga_make_color (enum vga_color fg, enum vga_color bg) 
{
    return fg | bg << 4;
}

static inline void
vga_set_cursor_txt (size_t x, size_t y)
{
    uint16_t pos = y*VGA_WIDTH+x;

    outb(CURSOR_HIGH, CRTC_ADDR);
    outb(pos>>8, CRTC_DATA);
    outb(CURSOR_LOW, CRTC_ADDR);
    outb(pos&0xff, CRTC_DATA);
}


void
vga_set_cursor (size_t x, size_t y)
{
    term.x = x;
    term.y = y;

    if (vga_info.type == VGA_TYPE_EGA_TEXT)
        vga_set_cursor_txt(x, y);
}


void
vga_get_cursor (uint8_t *x, uint8_t *y)
{
  *x = term.x;
  *y = term.y;
}

// force attributes to be interpreted as colors instead
// of blink/bold/italic/underline/etc
static void
disable_blink()
{
    uint8_t val;

    // waits are architecturally specified at 250 us

    // Reset interaction mode back to address mode
    inb(ATTR_CTRL_FLIP_FLOP);
    udelay(300); // wait for attribute controller

    // now do a read - select our address
    // the 0x20 here indicates we want to continue to use
    // the regular VGA pallete which the bios has already setup for
    // us
    outb(ATTR_MODE_CTRL | 0x20,ATTR_CTRL_ADDR_AND_DATA_WRITE);
    // flip flop will now be in write mode (reads will not affect it)

    udelay(300); // wait for attribute controller

    // will not affect flip flop
    val = inb(ATTR_CTRL_DATA_READ);

    val &= ~0x08; // turn off bit 3, which is blinking enable

    udelay(300); // wait for attribute controller

    // write it back
    outb(val,ATTR_CTRL_ADDR_AND_DATA_WRITE);
    // FF should now be back to address mode

    udelay(300); // wait for attribute controller

}

static void
vga_init_screen_txt ()
{
    disable_blink();
    //enable cursor as a block
    outb(0x0a,CRTC_ADDR); outb((inb(CRTC_DATA) & 0xc0) | 0, CRTC_DATA);
    outb(0x0b,CRTC_ADDR); outb((inb(CRTC_DATA) & 0xe0) | 15, CRTC_DATA);
}

void
vga_init_screen()
{
	if (vga_info.type == VGA_TYPE_EGA_TEXT)
        vga_init_screen_txt();
}

static void
init_term_emu (struct vga * info) {
	if (info->type == VGA_TYPE_EGA_TEXT) {
		term.width  = VGA_WIDTH;
		term.height = VGA_HEIGHT;
	} else {
		//term.width  = vga_info.width / 8;
		//term.height = vga_info.height / 9;
        term.width = VGA_WIDTH;
        term.height = VGA_HEIGHT;
	}
	term.txt_buf = (uint16_t*)malloc(term.width*term.height*2);
	memcpy(term.txt_buf, init_txt_buf, term.width*term.height*2);

	if (info->type != VGA_TYPE_EGA_TEXT)
		memset(vga_info.fb, 0, vga_info.width*vga_info.height*vga_info.bpp/8);

	term.x    = 0;
	term.y    = 0;
	term.attr = 0x08;
}


static inline void
display_pix (uint32_t pix, size_t x, size_t y)
{
    uint32_t* pix_buf = (uint32_t*)vga_info.fb;
    pix_buf[vga_info.width * y + x] = pix;
}


static void
render_pos_video (uint16_t entry, size_t xpos, size_t ypos)
{
	uint32_t fg;
	uint32_t bg;

	uint64_t font_sel;
	uint8_t vga_fg = (entry & 0x0f00) >> 8;
	uint8_t vga_bg = entry >> 12;
	char c = entry ^ 0xff00;

	fg = vga_color_to_pixel(vga_fg);
	bg = vga_color_to_pixel(vga_bg);

    font_sel = get_font_from_char(c);

    uint8_t bits[64]; // TODO: should be sized based on BPP
    for (uint8_t i = 0; i < 64; i++)
        bits[i] = (font_sel >> i) & 1;

    for (uint32_t y = 0, yy = (ypos*9); y < 8; y++, yy++)
        for (uint32_t x = 0, xx = (xpos*8); x < 8; x++, xx++)
            display_pix(bits[(8*y) + x] ? fg : bg, xx, yy);
}

static void
wipe_video (uint32_t pix_color)
{
    uint32_t * pix_buf = (uint32_t*)vga_info.fb;
    for (uint64_t i = 0; i < vga_info.width * vga_info.height * vga_info.bpp/8; i++)  {
        pix_buf[i] = pix_color;;
    }
}

static void
render_screen (void)
{
    for (uint32_t y = 0; y < term.height; y++)
        for (uint32_t x = 0; x < term.width; x++)
            render_pos_video(term.txt_buf[y * term.width + x], x, y);
}


static void
render_pos_txt (uint16_t entry, size_t x, size_t y)
{
    uint16_t * base = (uint16_t*)vga_info.fb;
    base[(y * VGA_WIDTH) + x] = entry;
}


static void
render_pos (size_t x, size_t y)
{
	uint16_t entry = term.txt_buf[(y*term.width) + x];

    if (vga_info.type == VGA_TYPE_EGA_TEXT)
        render_pos_txt(entry, x, y);
    else  {
        render_pos_video(entry, x, y);
    }
}


void
vga_clear_screen (uint16_t val)
{
	nk_low_level_memset_word((void*)term.txt_buf, val, term.width*term.height);

    // TODO: hack to clear out the entire screen (pixel-wise) in video mode
    if (vga_info.type != VGA_TYPE_EGA_TEXT) {
        wipe_video(vga_color_to_pixel(0x09));
        render_screen();
    }
}


void
vga_write_screen (size_t x, size_t y, uint16_t val)
{
	if (x > term.width)
		return;
	if (y > term.height)
		return;

	term.txt_buf[(y*term.width) + x] = val;

	render_pos(x, y);
}


void
vga_early_init ()
{
    // these will be patched up once we parse multiboot info
    vga_info.type = VGA_TYPE_EGA_TEXT; // we assume we're in text mode initially
    vga_info.fb   = (void*)VGA_BASE_ADDR;
    vga_info.width = VGA_WIDTH;
    vga_info.height = VGA_HEIGHT;

    term.txt_buf = init_txt_buf;
    term.x = 0;
    term.y = 0;
    term.attr = 0x08;

    vga_clear_screen(vga_make_entry(' ', term.attr));
    vga_set_cursor(0, 0);
}

static struct nk_dev_int ops = {
    .open  = 0,
    .close = 0,
};


void
vga_init (struct multiboot_info * mb)
{
    nk_dev_register("vga", NK_DEV_GENERIC, 0, &ops, 0);

    switch (mb->fb_info.common.framebuffer_type) {
        case 0:
            vga_info.type = VGA_TYPE_INDEXED;
            vga_info.color.idx.num_colors = mb->fb_info.framebuffer_palette_num_colors;
            vga_info.color.idx.desc = malloc(sizeof(struct color_desc) * vga_info.color.idx.num_colors);
            memcpy(vga_info.color.idx.desc, mb->fb_info.framebuffer_palette, sizeof(struct color_desc) * vga_info.color.idx.num_colors);
            break;
        case 1:
            vga_info.type = VGA_TYPE_RGB;
            vga_info.color.rgb.red_fpos = mb->fb_info.framebuffer_red_field_position;
            vga_info.color.rgb.red_mask_sz = mb->fb_info.framebuffer_red_mask_size;
            vga_info.color.rgb.green_fpos = mb->fb_info.framebuffer_green_field_position;
            vga_info.color.rgb.green_mask_sz = mb->fb_info.framebuffer_green_mask_size;
            vga_info.color.rgb.blue_fpos = mb->fb_info.framebuffer_blue_field_position;
            vga_info.color.rgb.blue_mask_sz = mb->fb_info.framebuffer_blue_mask_size;
            break;
        case 2:
            vga_info.type = VGA_TYPE_EGA_TEXT;
            break;

    }

    vga_info.fb     = (void*)mb->fb_info.common.framebuffer_addr;
    vga_info.width  = mb->fb_info.common.framebuffer_width;
    vga_info.height = mb->fb_info.common.framebuffer_height;
    vga_info.pitch  = mb->fb_info.common.framebuffer_pitch;
    vga_info.bpp    = mb->fb_info.common.framebuffer_bpp;

	init_term_emu(&vga_info);

    term.attr = 0x09;
    vga_clear_screen(vga_make_entry(' ', term.attr));
    vga_set_cursor(0, 0);
}

static void
vga_scrollup_txt (void)
{
    uint16_t *buf = (uint16_t*) vga_info.fb;
    nk_low_level_memcpy_word(buf, buf+VGA_WIDTH, VGA_WIDTH*(VGA_HEIGHT-1));
    nk_low_level_memset_word(buf+VGA_WIDTH*(VGA_HEIGHT-1), vga_make_entry(' ', term.attr), VGA_WIDTH);
}


void
vga_scrollup (void)
{
    nk_low_level_memcpy_word(term.txt_buf, term.txt_buf + term.width, term.width*(term.height-1));
    nk_low_level_memset_word(term.txt_buf+term.width*(term.height-1), vga_make_entry(' ', term.attr), term.width);

    if (vga_info.type == VGA_TYPE_EGA_TEXT)
        vga_scrollup_txt();
    else  {
        wipe_video(vga_color_to_pixel(0x09));
        render_screen();
    }
}



void
vga_putchar (char c)
{
    if (c == '\n') {
        term.x = 0;

        if (++term.y == term.height) {
            vga_scrollup();
            term.y--;
        }
    } else {

        vga_write_screen(term.x, term.y, vga_make_entry(c, term.attr));

        if (++term.x == term.width) {
            term.x = 0;
            if (++term.y == term.height) {
                vga_scrollup();
                term.y--;
            }
        }
    }
    vga_set_cursor(term.x, term.y);
}

  
void
vga_print (char *buf)
{
    while (*buf) {
        vga_putchar(*buf);
        buf++;
    }
}


void
vga_puts (char *buf)
{
    vga_print(buf);
    vga_putchar('\n');
}


void
vga_copy_out (void * dst, uint32_t n)
{
    nk_low_level_memcpy((void*)dst, (void*)term.txt_buf, n);
}

void
vga_copy_in (void * src, uint32_t n)
{
    nk_low_level_memcpy((void*)term.txt_buf, src, n);
    wipe_video(vga_color_to_pixel(0x09));
    render_screen();
}
