#define _POSIX_C_SOURCE 2
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define max2(__A__, __B__) ((__A__)>=(__B__)?(__A__):(__B__))
#define max3(__A__, __B__, __C__) max2(__A__, max2(__B__, __C__))
#define min2(__A__, __B__) ((__A__)<(__B__)?(__A__):(__B__))
#define min3(__A__, __B__, __C__) min2(__A__, min2(__B__, __C__))

#ifndef TPUT
#pragma message "no tput command path defined in TPUT, 15 rows and 60 cols will be the default instead"
#endif
#ifndef GRAPHICSMAGICK
#pragma message "no gm command path defined in GRAPHICSMAGICK, only pgm (for -b and -g) and ppm (for -a and -m) formats will be accepted"
#define PFCLOSE(x) fclose(x)
#else
#define PFCLOSE(x) pclose(x)
#endif

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
		if (saturation < 0.25f) {
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
	return -1;
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
void print_hs(FILE *fh, int hs, int submode) { /*{{{*/
	if (submode) { // mirc
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
	}
	else { // ansi
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
	}
} /*}}}*/
void output_bitmap(const unsigned char *image, int w, int h, int pw, int ph) { /*{{{*/
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
void output_graymap(const unsigned char *image, int w, int h, int pw, int ph) { /*{{{*/
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
void output_rgbmap(const unsigned char *image, int w, int h, int pw, int ph, int submode) { /*{{{*/
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
				print_hs(stdout, hs, submode);
				ohs = hs;
			}
			print_gray_pixel(stdout, gac/pw/ph);
		}
		if (submode == 0) {
			print_hs(stdout, -1, submode);
		}
		fputc('\n', stdout);
	}
} /*}}}*/
int main(int argc, const char **argv) { /*{{{*/

	const char *infile = NULL;
	int mode = 0;
	int pw = 1;
	int ph = 2;
	int max_rows = 0;
	int max_cols = 0;

	// command line parsing
	for (int i=1; i<argc; ++i) { /*{{{*/
		if (argv[i][0] == '-') {
			int j = i;
			for (int k=1; argv[i][k] != '\0'; ++k) {
				switch (argv[i][k]) {
					case 'b': // bitmap
						mode = 0;
						break;
					case 'g': // graymap
						mode = 1;
						break;
					case 'a': // ansi escape color
						mode = 2;
						break;
					case 'm': // mirc escape color
						mode = 3;
						break;
					case 's': // size
						if (argc <= j+2) { goto bad_command_line; }
						max_cols = atoi(argv[++j]);
						max_rows = atoi(argv[++j]);
						break;
					case 'p': // pixel aspect ratio
						if (argc <= j+2) { goto bad_command_line; }
						pw = atoi(argv[++j]);
						ph = atoi(argv[++j]);
						break;
					case 'f': // explicitly specify file
						if (argc <= j+1) { goto bad_command_line; }
						if (infile == NULL) {
							infile = argv[++j];
						}
						break;
				}
			}
			i = j;
		}
		else if (infile == NULL) {
			infile = argv[i];
		}
	} /*}}}*/

	// help text
	if (infile == NULL || pw <= 0 || ph <= 0) { /*{{{*/
		bad_command_line:
		fprintf(stderr,
			"%s [parameters] file\n" \
			"\t-b -g -a -m determines if the output is going to be in\n" \
			"\t            monochrome, gray, ansi escape or mirc escape colors\n" \
			"\t-s W H      determines the max dimensions (cols and rows)\n" \
			"\t            of the output, if not specified, %s is used\n" \
			"\t-p W H      determines the character aspect ratio, since pixels\n" \
			"\t            are usually squares, but terminal characters are not\n" \
			"\t            if not specified, default values 2 1 are used\n" \
			"\t-f FILE     explicitly specify file to read from\n" \
			"\n" \
			"parameters can be joined, eg %s -cs 80 25 SOMEFILE\n" \
			"or even %s -fsap SOMEFILE 100 30 3 7\n" \
			"\n" \
			"bitmap mode is drawn with space, and unicode characters\n" \
			"2580, 2588, 258C, 2590, and 2596-259F (hexadecimal)\n" \
			"gray and color mode is drawn with space, and unicode characters\n" \
			"2588, 2591-2593 (hexadecimal)\n%s",
			argv[0],
#ifdef TPUT
			"info from tput(1)",
#else
			"60 cols x 15 rows",
#endif
			argv[0], argv[0],
#ifndef GRAPHICSMAGICK
			"\n"
			"COMPILED TO NOT USE GRAPHICSMAGICK, that means that only binary pgm 'P5'\n" \
			"(for -b and -g) and ppm 'P6' (for -a and -m) is accepted\n"
#else
			""
#endif
		);
		return 1;
	} /*}}}*/
	int ppc = mode == 0 ? 2 : 1; // bitmap mode can cram more pixels per char
	int psz = mode == 2 ? 3 : 1; // color mode needs three bytes per pixel
	FILE *fh = NULL;

#ifdef TPUT
	// get terminal size
	if (max_rows <= 0 || max_cols <= 0) { /*{{{*/
		fh = popen(TPUT " lines", "r");
		fscanf(fh, "%d", &max_rows);
		pclose(fh);
		fh = popen(TPUT " cols", "r");
		fscanf(fh, "%d", &max_cols);
		pclose(fh);
	} /*}}}*/
#else
	// or use these defaults
	max_cols = 60;
	max_rows = 15;
#endif

#ifdef GRAPHICSMAGICK
	int image_max_w = max_cols*pw*ppc;
	int image_max_h = max_rows*ph*ppc;
	int ifnlen = strlen(infile);
	char *cmdline = malloc(sizeof(char)*(1024 + ifnlen));
	if (cmdline == NULL) { /*{{{*/
		return -1;
	} /*}}}*/
	snprintf(cmdline, 1024+ifnlen, GRAPHICSMAGICK " convert %s -resize %dx%d p%cm:-", infile, image_max_w, image_max_h, mode==2?'p':'g');
	fh = popen(cmdline, "r");
	free(cmdline);
#else
	fh = fopen(infile, "r");
#endif

	unsigned char magic[2];
	int image_w;
	int image_h;
	fscanf(fh, "%2c", magic); ignorewhitespace(fh); ignorecomment(fh);
	if (magic[0] != 0x50 || magic[1] != (mode==2?0x36:0x35)) {
		fprintf(stderr, "wrong image magic '%02x%02x': only binary pgm '5035' (for -b and -g) and ppm '5036' (for -a and -m) is accepted\n", magic[0], magic[1]);
		PFCLOSE(fh);
		return 1;
	}
	fscanf(fh, "%d", &image_w); ignorewhitespace(fh); ignorecomment(fh);
	fscanf(fh, "%d", &image_h); ignorewhitespace(fh); ignorecomment(fh);
	fscanf(fh, "%*d%*c");
	unsigned char *image = malloc(sizeof(unsigned char)*image_w*image_h*psz);
	if (image == NULL) { /*{{{*/
		PFCLOSE(fh);
		return -1;
	} /*}}}*/
	fread(image, sizeof(unsigned char), image_w*image_h*psz, fh);
	PFCLOSE(fh);
#ifndef GRAPHICSMAGICK
	int multiplier = 1;
	while (image_w > max_cols*pw*ppc*multiplier || image_h > max_rows*ph*ppc*multiplier) {
		++multiplier;
	}
	printf("%d x %d -[ %d ]-> ", pw, ph, multiplier);
	pw *= multiplier;
	ph *= multiplier;
	printf("%d x %d\n", pw, ph);
#endif
	// print image output
	if (mode == 0) { /*{{{*/
		output_bitmap(image, image_w, image_h, pw, ph);
	}
	else if (mode == 1) {
		output_graymap(image, image_w, image_h, pw, ph);
	}
	else {
		output_rgbmap(image, image_w, image_h, pw, ph, mode-2);
	} /*}}}*/
	free(image);
	return 0;
} /*}}}*/
