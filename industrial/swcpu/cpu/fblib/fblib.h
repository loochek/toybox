#ifndef fblib_h
#define fblib_h

#include <linux/fb.h>

struct image
{
	int width;
	int height;
	const unsigned char *data; // RGBA8888 32 * width * height
};

struct font
{
	const char *charset;
	int char_width;
	int char_height;
	const struct image *graphics; // (strlen(charset) * char_width)x(char_height) size
};

extern struct fb_var_screeninfo vinfo;
int fb_init();
void fb_close();
void update();
void clear();
void fill_rect(int x_offset, int y_offset, int w, int h, int r, int g, int b, int a);
void rect(int x_offset, int y_offset, int w, int h, int r, int g, int b, int a, int border_thickness);
void sub_image(int x_offset, int y_offset, int x_img_offset, int y_img_offset, int width, int height, const struct image img);
void label(int x_offset, int y_offset, const char* str, const struct font fnt);
void image(int x_offset, int y_offset, const struct image img);
void put_pixel(int x, int y, int r, int g, int b, int a);
#endif
