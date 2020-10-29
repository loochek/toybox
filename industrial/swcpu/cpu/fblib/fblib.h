#ifndef fblib_h
#define fblib_h

// Lite version of fblib

#include <linux/fb.h>

// returns 0 if succesful
int fb_init();
int update();

void get_resolution(size_t *width, size_t *height);
void put_pixel(int x, int y, int r, int g, int b, int a);
void fb_close();
#endif
