#ifndef fblib_h
#define fblib_h

// Lite version of fblib

#include <linux/fb.h>

typedef struct
{
    int                      fbfd;
    char*                    fbptr;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    int                      screensize;
} framebuffer_t;

// returns 0 if succesful, else writes lerror
int  fb_init          (framebuffer_t *fb);
// returns 0 if succesful
int  fb_update        (framebuffer_t *fb);

void fb_get_resolution(framebuffer_t *fb, size_t *width, size_t *height);
void fb_put_pixel     (framebuffer_t *fb, int x, int y, int r, int g, int b, int a);
void fb_deinit        (framebuffer_t *fb);
#endif
