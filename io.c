#include "io.h"

#include "defines.h"
#include "memory.h"

#include <assert.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

static WINDOW *scr = NULL, *win = NULL;

static struct {
	int xscale, yscale;
} metrics;

static uint64_t buf[RES_Y];

static void vmem_dump(void)
{
	fprintf(stderr, "\nDumping video memory:\n");
	for(int y = 0; y < RES_Y; ++y) {
		for(uint64_t x = 0x8000000000000000; x; x >>= 1)
			fprintf(stderr, "%c", buf[y] & x ? 'X' : '.');

		fprintf(stderr, "\n");
	}

	fprintf(stderr, "\n");
}

static void repaint()
{
	vmem_dump();

	for(int y = 0; y < RES_Y; ++y) {
		for(int ys = 0; ys < metrics.yscale; ++ys) {
			wmove(win, y * metrics.yscale + ys, 0);

			for(uint64_t x = 0x8000000000000000; x; x >>= 1) {
				for(int xs = 0; xs < metrics.xscale; ++xs) {
					waddch(win, buf[y] & x ? ' ' | A_REVERSE : ' ');
				}
			}
		}
	}

	wnoutrefresh(win);
}

void win_clr(void)
{
	memset(buf, 0, RES_Y * sizeof(uint64_t));
	repaint();
}

void win_refresh(void)
{
	doupdate();
}

uint8_t render_sprite(uint8_t y, uint8_t x, uint16_t mem, uint8_t n)
{
	fprintf(stderr, "Rendering at [%02x, %02x]: %04x : %02x\n", x, y, mem, n);

	assert(y * RES_X + x < RES_Y * RES_X);

	uint8_t *sprite = mem_at(mem);
	uint64_t *vmem = &buf[y];

	uint64_t collision = 0;
	while(n--) {
		uint64_t sprite_mask = ((uint64_t) *sprite) << 0x38; // 64 - 8
		sprite_mask >>= x;

		fprintf(stderr, "%08lx ", sprite_mask);

		collision |= sprite_mask & *vmem;

		*vmem ^= sprite_mask;

		++sprite;
		++vmem;
	}

	fprintf(stderr, "\n");

	repaint();

	return collision ? 0x01 : 0x00;
}

static uint16_t pressed_keys = 0x0000;

static uint8_t key_parse(int in)
{
	switch(in) {
		case '3': return 0x01;
		case '4': return 0x02;
		case '5': return 0x03;
		case '6': return 0x0c;
		case 'e': return 0x04;
		case 'r': return 0x05;
		case 't': return 0x06;
		case 'y': return 0x0d;
		case 'd': return 0x07;
		case 'f': return 0x08;
		case 'g': return 0x09;
		case 'h': return 0x0e;
		case 'c': return 0x0a;
		case 'v': return 0x00;
		case 'b': return 0x0b;
		case 'n': return 0x0f;

		default:  return 0xff;
	}
}

int key_read(void)
{
	int in = wgetch(scr);

	uint8_t key = key_parse(in);

	if(key != 0xff)
		pressed_keys |= 0x0001 << key;

	return in;
}

static int key_status(uint8_t key)
{
	uint16_t key_mask = 0x0001 << key;

	int rv = pressed_keys & key_mask;

	pressed_keys &= ~key_mask;

	return rv ? 1 : 0;
}

int key_down(uint8_t key)
{
	return key_status(key);
}

int key_up(uint8_t key)
{
	return !key_status(key);
}

uint8_t key_get(void)
{
	uint16_t key_mask = 0x0001;

	for(uint8_t key = 0x00; key < 0x10; ++key) {
		if(pressed_keys & key_mask) {
			pressed_keys &= ~key_mask;
			return key;
		}

		key_mask <<= 1;
	}

	return 0xff;
}

static void fill(WINDOW *w, chtype c)
{
	int y, x;

	getmaxyx(w, y, x);

	while(y--) {
		wmove(w, y, 0);

		for(int n = 0; n < x; ++n)
			waddch(w, c);
	}
}

static void create_win(void)
{
	int y, x;

	getmaxyx(scr, y, x);

	metrics.xscale = (x - 2) / RES_X;
	metrics.yscale = (y - 2) / RES_Y;

	fprintf(stderr, "Graphics: Screen resolution: %d, %d\n", y, x);
	fprintf(stderr, "Graphics: Window metrics: %d, %d\n", metrics.yscale, metrics.xscale);

	int w = metrics.xscale * RES_X;
	int h = metrics.yscale * RES_Y;

	fprintf(stderr, "Graphics: Window size: %d, %d\n", h, w);

	int brd_h = h + 2 * metrics.yscale > y ? 1 : metrics.yscale;
	int brd_w = w + 2 * metrics.xscale > x ? 1 : metrics.xscale;

	fprintf(stderr, "Graphics: Border size: %d, %d\n", brd_h, brd_w);

	WINDOW *brd_win = newwin(
		h + 2 * brd_h, w + 2 * brd_w,
		CENTER(h + 2 * brd_h, y),
		CENTER(w + 2 * brd_w, x)
	);

	fill(brd_win, ' ' | A_REVERSE);

	win = newwin(
		h, w, 
		CENTER(h + 2 * brd_h, y) + brd_h,
		CENTER(w + 2 * brd_w, x) + brd_w
	);

	wrefresh(brd_win);

	repaint();

	win_clr();

	wrefresh(win);
}

int io_init(void)
{
	scr = initscr();

	if(!scr) {
		fprintf(stderr, "Error: Couldn't initialize graphics\n");
		return ERROR_GRAPHICS;
	}

	if(!has_colors()) {
		fprintf(stderr, "Error: Couldn't initialize colors\n");
		return ERROR_GRAPHICS;
	}

	start_color();

	wrefresh(scr);

	create_win();

	cbreak();
	noecho();
	nodelay(scr, true);
	keypad(scr, true);

	return SUCCESS;
}

void io_end(void)
{
	nocbreak();
	echo();

	endwin();
}

