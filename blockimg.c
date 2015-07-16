#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#define pw 4 // aspect ratio of tty characters
#define ph 7
#define max_rows 40
#define max_cols 80
#define max2(__A__, __B__) ((__A__)>=(__B__)?(__A__):(__B__))
#define max3(__A__, __B__, __C__) max2(__A__, max2(__B__, __C__))
#define min2(__A__, __B__) ((__A__)<(__B__)?(__A__):(__B__))
#define min3(__A__, __B__, __C__) min2(__A__, min2(__B__, __C__))

static inline void ignorewhitespace(FILE *f) { /*{{{*/
	int c = 0;
	do {
		c = fgetc(f);
	} while (isspace(c));
	ungetc(c, f);
} /*}}}*/
static inline void ignorecomment(FILE *f) { /*{{{*/
	int t = fgetc(f);
	if (t == '#') {
		while (fgetc(f) != '\n');
		ignorecomment(f);
	}
	else {
		ungetc(t, f);
	}
	ignorewhitespace(f);
} /*}}}*/
int get_gray_pixel(const unsigned char* img, int x, int y, int w, int h) { /*{{{*/
	x = x < 0 ? 0 : x >= w ? w-1 : x;
	y = y < 0 ? 0 : y >= h ? h-1 : y;
	return img[x+y*w];
} /*}}}*/
void get_rgb_pixel(int *rgb, const unsigned char* img, int x, int y, int w, int h) { /*{{{*/
	x = x < 0 ? 0 : x >= w ? w-1 : x;
	y = y < 0 ? 0 : y >= h ? h-1 : y;
	rgb[0] = img[(x+y*w)*3+0];
	rgb[1] = img[(x+y*w)*3+1];
	rgb[2] = img[(x+y*w)*3+2];
} /*}}}*/
int simple_hs (const int *rgb) { /*{{{*/
	float r = rgb[0] / 255.0;
	float g = rgb[1] / 255.0;
	float b = rgb[2] / 255.0;
	char bin = 0;
	float cmax = r;
	if (cmax < g) {
		cmax = g;
		bin = 1;
	}
	if (cmax < b) {
		cmax = b;
		bin = 2;
	}
	float cmin = min3(r,g,b);
	if (cmax > 0) {
		float delta = cmax-cmin;
		float saturation = delta/cmax;
		float hue = 0;
		if (saturation < 0.125f) {
			return -1;
		}
		switch (bin) {
			case 0:
				hue = 60.0f * ((g-b)/delta);
				break;
			case 1:
				hue = 60.0f * ((b-r)/delta + 2.0f);
				break;
			case 2:
			default:
				hue = 60.0f * ((r-g)/delta + 4.0f);
				break;
		}
		while (hue > 360.0f) { hue -= 360.0f; }
		while (hue < 0.0f) { hue += 360.0f; }
		/*
			0      60        120       180      240      300
			red    yellow    green     cyan     blue     magenta
			    30        90       150      210      270         330
		*/
		if (hue <  30) { return 0; }
		if (hue <  90) { return 1; }
		if (hue < 150) { return 2; }
		if (hue < 210) { return 3; }
		if (hue < 270) { return 4; }
		if (hue < 330) { return 5; }
	}
	else {
		return -1;
	}
} /*}}}*/
void print_2x2_bitmap(FILE *fh, char bmp) { /*{{{*/
	if (bmp == 0) {
		fputc(0x20, fh);
	}
	else {
		int possible[15] = {0x98,0x9d,0x80,0x96,0x8c,0x9e,0x9b,0x97,0x9a,0x90,0x9c,0x84,0x99,0x9f,0x88};
		fputc(0xe2, fh);
		fputc(0x96, fh);
		fputc(possible[bmp-1], fh);
	}
} /*}}}*/
void print_gray_pixel(FILE *fh, int value) { /*{{{*/
	if (value < 51) {
		fputc(0x20, fh);
	}
	else {
		fputc(0xe2, fh);
		fputc(0x96, fh);
		if (value < 102) {
			fputc(0x91, fh);
		}
		else if (value < 153) {
			fputc(0x92, fh);
		}
		else if (value < 204) {
			fputc(0x93, fh);
		}
		else {
			fputc(0x88, fh);
		}
	}
} /*}}}*/
void print_hs(FILE *fh, int hs) { /*{{{*/
#ifdef MIRC
	if (hs >= 0) { fputc(3,fh); }
	switch (hs) { /*{{{*/
		case 0: // red
			fputs("4", fh);
			break;
		case 1: // yellow
			fputs("8", fh);
			break;
		case 2: // green
			fputs("9", fh);
			break;
		case 3: // cyan
			fputs("11", fh);
			break;
		case 4: // blue
			fputs("12", fh);
			break;
		case 5: // magenta
			fputs("13", fh);
			break;
		default: // reset
			fputc(15, fh);
			break;
	} /*}}}*/
#else
	switch (hs) { /*{{{*/
		case 0: // red
			fputs("\x1b[31m", fh);
			break;
		case 1: // yellow
			fputs("\x1b[33m", fh);
			break;
		case 2: // green
			fputs("\x1b[32m", fh);
			break;
		case 3: // cyan
			fputs("\x1b[36m", fh);
			break;
		case 4: // blue
			fputs("\x1b[34m", fh);
			break;
		case 5: // magenta
			fputs("\x1b[35m", fh);
			break;
		default: // reset
			fputs("\x1b[0m", fh);
			break;
	} /*}}}*/
#endif
} /*}}}*/
void output_bitmap(const unsigned char *image, int w, int h) { /*{{{*/
	for (int row=0; row<h/ph/2; ++row) {
		for (int col=0; col<w/pw/2; ++col) {
			char bitmap_char = 0;
			for (int spy=0; spy<2; ++spy) {
				for (int spx=0; spx<2; ++spx) {
					int accumulator = 0;
					for (int ay=0; ay<ph; ++ay) { /*{{{*/
						for (int ax=0; ax<pw; ++ax) {
							int x = col*pw*2 + spx*pw + ax;
							int y = row*ph*2 + spy*ph + ay;
							accumulator += get_gray_pixel(image, x, y, w, h);
						}
					} /*}}}*/
					if (accumulator >= 128*pw*ph) {
						bitmap_char |= (1<<(spx|(spy<<1)));
					}
				}
			}
			print_2x2_bitmap(stdout, bitmap_char);
		}
		fputc('\n', stdout);
	}
} /*}}}*/
void output_graymap(const unsigned char *image, int w, int h) { /*{{{*/
	for (int row=0; row<h/ph; ++row) {
		for (int col=0; col<w/pw; ++col) {
			int acc = 0;
			for (int ay=0; ay<ph; ++ay) { /*{{{*/
				for (int ax=0; ax<pw; ++ax) {
					int x = col*pw + ax;
					int y = row*ph + ay;
					acc += get_gray_pixel(image, x, y, w, h);
				}
			} /*}}}*/
			print_gray_pixel(stdout, acc/pw/ph);
		}
		fputc('\n', stdout);
	}
} /*}}}*/
void output_rgbmap(const unsigned char *image, int w, int h) { /*{{{*/
	for (int row=0; row<h/ph; ++row) {
		int ohs = -1;
		for (int col=0; col<w/pw; ++col) {
			int acc[3] = {0,0,0};
			for (int ay=0; ay<ph; ++ay) { /*{{{*/
				for (int ax=0; ax<pw; ++ax) {
					int x = col*pw + ax;
					int y = row*ph + ay;
					int accd[3];
					get_rgb_pixel(accd, image, x, y, w, h);
					acc[0] += accd[0];
					acc[1] += accd[1];
					acc[2] += accd[2];
				}
			} /*}}}*/
			int hs = simple_hs(acc);
			int gac = (acc[0]+acc[1]+acc[2])/3;
			if (ohs != hs) {
				print_hs(stdout, hs);
				ohs = hs;
			}
			print_gray_pixel(stdout, gac/pw/ph);
		}
#ifndef MIRC
		print_hs(stdout, -1);
#endif
		fputc('\n', stdout);
	}
} /*}}}*/
int main(int argc, const char **argv) {

	if (argc != 3) {
		return 0;
	}

	int mode = 0;
	if (argv[1][0] == 'g') {
		mode = 1;
	}
	else if (argv[1][0] == 'c') {
		mode = 2;
	}

	int ppc = mode == 0 ? 2 : 1; // bitmap mode can cram more pixels per char
	int psz = mode == 2 ? 3 : 1; // color mode needs three bytes per pixel

	int image_max_w = max_cols*pw*ppc;
	int image_max_h = max_rows*ph*ppc;

	const char *infile = argv[2];
	char cmdline[1024];

	sprintf(cmdline, "/usr/bin/gm convert %s -resize %dx%d p%cm:output%d", infile, image_max_w, image_max_h, mode==2?'p':'g', mode);
	system(cmdline);
	sprintf(cmdline, "/usr/bin/gm convert %s -resize %dx%d p%cm:-", infile, image_max_w, image_max_h, mode==2?'p':'g');
	FILE *fh = popen(cmdline, "r");
	int image_w;
	int image_h;
	fscanf(fh, "%*2c"); ignorewhitespace(fh); ignorecomment(fh);
	fscanf(fh, "%d", &image_w); ignorewhitespace(fh); ignorecomment(fh);
	fscanf(fh, "%d", &image_h); ignorewhitespace(fh); ignorecomment(fh);
	fscanf(fh, "%*d%*c");

	unsigned char *image = malloc(sizeof(unsigned char)*image_w*image_h*psz);
	fread(image, sizeof(unsigned char), image_w*image_h*psz, fh);
	pclose(fh);

	if (mode == 0) {
		output_bitmap(image, image_w, image_h);
	}
	else if (mode == 1) {
		output_graymap(image, image_w, image_h);
	}
	else {
		output_rgbmap(image, image_w, image_h);
	}

	free(image);
	return 0;
}
